# Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
"""SQLite-backed save/load of task results into user-named files, on request.

Each "save" is a snapshot written to its own named .sqlite3 file under a managed
directory - there is no single always-on database and nothing is written automatically.
Failures here are expected to be logged and swallowed by callers - a save/load hiccup
must never break the live DDS pipeline.
"""

import json
import os
import re
import sqlite3
from datetime import datetime, timezone

from . import utils

_DEFAULT_SAVE_DIR = "/opt/sustainml/db"
_EXTENSION = ".sqlite3"
# '/' is allowed in a save name but must never reach the filesystem as a literal
# '/' (that would make it a path separator, i.e. a subfolder). It's swapped for
# this visually similar but filesystem-harmless character on disk, and swapped
# back whenever a name is read back out - round-tripping exactly what was typed.
_SLASH_PLACEHOLDER = "∕"  # DIVISION SLASH

_NODE_COLUMN = {
    utils.node_id.ORCHESTRATOR.value: "user_input_json",
    utils.node_id.APP_REQUIREMENTS.value: "app_requirements_json",
    utils.node_id.CARBONTRACKER.value: "carbon_footprint_json",
    utils.node_id.HW_CONSTRAINTS.value: "hw_constraints_json",
    utils.node_id.HW_PROVIDER.value: "hw_resources_json",
    utils.node_id.ML_MODEL_METADATA.value: "ml_model_metadata_json",
    utils.node_id.ML_MODEL_PROVIDER.value: "ml_model_json",
}

_RESULT_COLUMNS = [
    column for node, column in _NODE_COLUMN.items() if node != utils.node_id.ORCHESTRATOR.value
]


_save_dir_cache = None


def _resolve_save_dir():
    """Resolve (and cache) the save directory - computed only once per process, not
    on every save/load call.

    Falling back from the default system path to the per-user one is the normal,
    expected case for anyone without root (nothing is printed for it - it isn't an
    error). Only a genuine failure - the fallback itself also being unusable - is
    worth printing, since that means saving/loading can't work at all.
    """
    global _save_dir_cache
    if _save_dir_cache is not None:
        return _save_dir_cache

    env_dir = os.getenv("SUSTAINML_DB_PATH")
    if env_dir:
        os.makedirs(env_dir, exist_ok=True)
        _save_dir_cache = env_dir
        return _save_dir_cache

    try:
        os.makedirs(_DEFAULT_SAVE_DIR, exist_ok=True)
        _save_dir_cache = _DEFAULT_SAVE_DIR
    except OSError:
        fallback_dir = os.path.join(
            os.getenv("XDG_DATA_HOME", os.path.expanduser("~/.local/share")), "sustainml")
        try:
            os.makedirs(fallback_dir, exist_ok=True)
            _save_dir_cache = fallback_dir
        except OSError as fallback_error:
            print(f"[persistence] ERROR: cannot create a save directory at "
                  f"{_DEFAULT_SAVE_DIR} or fall back to {fallback_dir} ({fallback_error}); "
                  f"saving/loading will fail")
            raise
    return _save_dir_cache


def _sanitize_name(name):
    """Strip anything outside the allowed character set (letters, digits, space,
    underscore, hyphen, '/'). '/' is allowed to appear in the name itself, but -
    see _encode_filename() - never reaches the filesystem as a real path
    separator, so a name always resolves to a single file directly under the
    save directory (no subfolders), and can never escape it via '..' either
    ('.' isn't in the allowed set at all).
    """
    clean = re.sub(r"[^A-Za-z0-9 _/-]", "", str(name or "")).strip()
    return clean or "sustainml_save"


def _encode_filename(name):
    """Make a sanitized name safe to use as a single filesystem path component."""
    return name.replace("/", _SLASH_PLACEHOLDER)


def _decode_filename(name):
    """Inverse of _encode_filename() - recovers the name as it was typed."""
    return name.replace(_SLASH_PLACEHOLDER, "/")


def _path_for(name):
    filename = _encode_filename(_sanitize_name(name))
    return os.path.join(_resolve_save_dir(), filename + _EXTENSION)


def _now():
    return datetime.now(timezone.utc).isoformat()


def list_saved_files():
    """Return the names (without extension) of all save files that currently exist,
    decoded back to how they were originally typed (see _decode_filename()).
    """
    save_dir = _resolve_save_dir()
    names = [
        _decode_filename(f[:-len(_EXTENSION)]) for f in os.listdir(save_dir)
        if f.endswith(_EXTENSION) and os.path.isfile(os.path.join(save_dir, f))
    ]
    return sorted(names)


def _open(path):
    conn = sqlite3.connect(path, check_same_thread=False)
    conn.execute("PRAGMA journal_mode=WAL")
    conn.execute("""
        CREATE TABLE IF NOT EXISTS tasks (
            problem_id INTEGER NOT NULL,
            iteration_id INTEGER NOT NULL,
            user_input_json TEXT,
            app_requirements_json TEXT,
            ml_model_metadata_json TEXT,
            ml_model_json TEXT,
            hw_constraints_json TEXT,
            hw_resources_json TEXT,
            carbon_footprint_json TEXT,
            status TEXT NOT NULL DEFAULT 'pending',
            created_at TEXT NOT NULL,
            updated_at TEXT NOT NULL,
            PRIMARY KEY (problem_id, iteration_id)
        )
    """)
    conn.execute("""
        CREATE TABLE IF NOT EXISTS problems (
            problem_id INTEGER PRIMARY KEY,
            display_name TEXT,
            updated_at TEXT NOT NULL
        )
    """)
    # Holds the frontend's HF search/comparison history verbatim, as one opaque
    # JSON blob per key - the backend never looks inside these, it's the exact
    # shape the QML side already keeps in memory (searches/comparisons).
    conn.execute("""
        CREATE TABLE IF NOT EXISTS hf_state (
            key TEXT PRIMARY KEY,
            data_json TEXT NOT NULL,
            updated_at TEXT NOT NULL
        )
    """)
    return conn


def save_tasks_to_file(name, tasks):
    """Overwrite the named save file with exactly this snapshot of tasks.

    tasks: list of dicts, each {"problem_id", "iteration_id", "display_name",
           "results": {node_id_int: json_dict, ...}}. Saving under a name that already
           exists replaces its entire previous content - it does not merge with it.

    Returns the absolute path of the file that was written, so the caller can tell
    the user where their save actually landed.
    """
    path = _path_for(name)
    now = _now()
    conn = _open(path)
    try:
        conn.execute("DELETE FROM tasks")
        conn.execute("DELETE FROM problems")
        for task in tasks:
            problem_id = task["problem_id"]
            iteration_id = task["iteration_id"]
            results = task.get("results", {})

            columns = {}
            for node_id, result_json in results.items():
                column = _NODE_COLUMN.get(node_id)
                if column is not None:
                    columns[column] = json.dumps(result_json)

            if columns:
                assignments = ", ".join(f"{col} = excluded.{col}" for col in columns)
                conn.execute(f"""
                    INSERT INTO tasks (problem_id, iteration_id, {', '.join(columns)}, status, created_at, updated_at)
                    VALUES (?, ?, {', '.join('?' for _ in columns)}, 'pending', ?, ?)
                    ON CONFLICT(problem_id, iteration_id) DO UPDATE SET {assignments}, updated_at = excluded.updated_at
                """, (problem_id, iteration_id, *columns.values(), now, now))

                row = conn.execute(
                    f"SELECT {', '.join(_RESULT_COLUMNS)} FROM tasks WHERE problem_id=? AND iteration_id=?",
                    (problem_id, iteration_id)).fetchone()
                filled = sum(1 for value in row if value is not None)
                status = "complete" if filled == len(_RESULT_COLUMNS) else "partial" if filled else "pending"
                conn.execute(
                    "UPDATE tasks SET status=? WHERE problem_id=? AND iteration_id=?",
                    (status, problem_id, iteration_id))

            display_name = task.get("display_name")
            if display_name:
                conn.execute("""
                    INSERT INTO problems (problem_id, display_name, updated_at)
                    VALUES (?, ?, ?)
                    ON CONFLICT(problem_id) DO UPDATE SET
                        display_name = excluded.display_name,
                        updated_at = excluded.updated_at
                """, (problem_id, display_name, now))
        conn.commit()
    finally:
        conn.close()
    return path


def load_tasks_from_file(name):
    """Return every task stored in the named save file.

    Returns a list of dicts: {"problem_id", "iteration_id", "display_name",
    "results": {node_id_int: json_dict, ...}} - problem_id/iteration_id here are the
    file's saved ids, the caller is responsible for remapping them to fresh, live ids.
    """
    path = _path_for(name)
    if not os.path.exists(path):
        return []

    conn = _open(path)
    try:
        rows = conn.execute("""
            SELECT tasks.problem_id, tasks.iteration_id, problems.display_name,
                   tasks.user_input_json, tasks.app_requirements_json, tasks.ml_model_metadata_json,
                   tasks.ml_model_json, tasks.hw_constraints_json, tasks.hw_resources_json,
                   tasks.carbon_footprint_json
            FROM tasks
            LEFT JOIN problems USING (problem_id)
            ORDER BY tasks.problem_id, tasks.iteration_id
        """).fetchall()
    finally:
        conn.close()

    ordered_columns = [
        utils.node_id.ORCHESTRATOR.value,
        utils.node_id.APP_REQUIREMENTS.value,
        utils.node_id.ML_MODEL_METADATA.value,
        utils.node_id.ML_MODEL_PROVIDER.value,
        utils.node_id.HW_CONSTRAINTS.value,
        utils.node_id.HW_PROVIDER.value,
        utils.node_id.CARBONTRACKER.value,
    ]

    tasks = []
    for row in rows:
        problem_id, iteration_id, display_name = row[0], row[1], row[2]
        results = {}
        for node_id, payload in zip(ordered_columns, row[3:]):
            if payload is not None:
                results[node_id] = json.loads(payload)
        tasks.append({
            "problem_id": problem_id,
            "iteration_id": iteration_id,
            "display_name": display_name,
            "results": results,
        })
    return tasks


def save_hf_state(name, hf_searches, hf_comparisons):
    """Overwrite the named save file's HF search/comparison history - independent
    of, and without touching, whatever tasks that file may already hold.

    hf_searches/hf_comparisons: opaque lists as kept by the frontend - stored and
    returned verbatim, never inspected here.

    Returns the absolute path of the file that was written.
    """
    path = _path_for(name)
    now = _now()
    conn = _open(path)
    try:
        conn.execute("""
            INSERT INTO hf_state (key, data_json, updated_at) VALUES ('searches', ?, ?)
            ON CONFLICT(key) DO UPDATE SET data_json = excluded.data_json, updated_at = excluded.updated_at
        """, (json.dumps(hf_searches), now))
        conn.execute("""
            INSERT INTO hf_state (key, data_json, updated_at) VALUES ('comparisons', ?, ?)
            ON CONFLICT(key) DO UPDATE SET data_json = excluded.data_json, updated_at = excluded.updated_at
        """, (json.dumps(hf_comparisons), now))
        conn.commit()
    finally:
        conn.close()
    return path


def load_hf_state(name):
    """Return {"searches": [...], "comparisons": [...]} from the named save file -
    empty lists for either key not present (including when the file doesn't exist).
    """
    result = {"searches": [], "comparisons": []}
    path = _path_for(name)
    if not os.path.exists(path):
        return result

    conn = _open(path)
    try:
        rows = conn.execute("SELECT key, data_json FROM hf_state").fetchall()
    finally:
        conn.close()

    for key, data_json in rows:
        if key in result:
            try:
                result[key] = json.loads(data_json)
            except (TypeError, ValueError):
                pass
    return result


def delete_saved_file(name):
    """Delete a single named save file, if it exists. Does not touch live
    in-memory state or any other save file.
    """
    path = _path_for(name)
    if os.path.exists(path):
        os.remove(path)


def delete_all_saved_files():
    """Delete every save file. Does not touch live in-memory state."""
    save_dir = _resolve_save_dir()
    for f in os.listdir(save_dir):
        path = os.path.join(save_dir, f)
        if f.endswith(_EXTENSION) and os.path.isfile(path):
            os.remove(path)
