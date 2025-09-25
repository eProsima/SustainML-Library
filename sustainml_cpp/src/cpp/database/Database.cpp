// Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

/**
 * @file Database.cpp
 */

#include <sustainml_cpp/database/Database.hpp>
#include <iostream>
#include <fstream>

namespace sustainml {
namespace database {

Database::Database(const std::string& db_file)
    : db_file_(db_file)
    , db_(nullptr)
{
}


Database::~Database()
{
    close();
}


int Database::close()
{
    if (!db_)
    {
        return SQLITE_OK;
    }
    int rc = sqlite3_close(db_);
    if (rc == SQLITE_OK)
    {
        db_ = nullptr;
    }
    return rc;
}


int Database::initialize()
{
    if (db_)
    {
        return SQLITE_OK;
    }

    auto file_exists = [](const std::string& p) -> bool {
        std::ifstream f(p);
        return f.good();
    };
    created_on_open_ = !file_exists(db_file_);

    int rc = sqlite3_open(db_file_.c_str(), &db_);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_open failed: " << rc << "\n";
        return rc;
    }

    if (created_on_open_)
    {
        rc = create_schema();
        if (rc != SQLITE_OK)
        {
            std::cerr << "create_schema failed: " << rc << "\n";
            return rc;
        }
    }

    return SQLITE_OK;
}


int Database::exec(const std::string& sql)
{
    if (!db_)
    {
        return SQLITE_MISUSE;
    }

    char* errmsg = nullptr;
    int rc = sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errmsg);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_exec error (" << rc << "): "
                  << (errmsg ? errmsg : "unknown") << "\n";
        if (errmsg) sqlite3_free(errmsg);
    }
    return rc;
}


int Database::create_schema()
{
    const char* ddl =
        "CREATE TABLE IF NOT EXISTS task_results ("
        "  problem_id INTEGER NOT NULL,"
        "  iteration_id INTEGER NOT NULL,"
        "  UserInputJson TEXT NOT NULL,"
        "  AppRequirementsJson TEXT NOT NULL,"
        "  MLModelMetadataJson TEXT NOT NULL,"
        "  MLModelJson TEXT NOT NULL,"
        "  HWConstraintsJson TEXT NOT NULL,"
        "  HWResourcesJson TEXT NOT NULL,"
        "  CarbonFootprintJson TEXT NOT NULL,"
        "  CHECK (typeof(UserInputJson)='text'),"
        "  CHECK (typeof(AppRequirementsJson)='text'),"
        "  CHECK (typeof(MLModelMetadataJson)='text'),"
        "  CHECK (typeof(MLModelJson)='text'),"
        "  CHECK (typeof(HWConstraintsJson)='text'),"
        "  CHECK (typeof(HWResourcesJson)='text'),"
        "  CHECK (typeof(CarbonFootprintJson)='text')"
    #ifdef SQLITE_ENABLE_JSON1
        "  , CHECK (json_valid(UserInputJson))"
        "  , CHECK (json_valid(AppRequirementsJson))"
        "  , CHECK (json_valid(MLModelMetadataJson))"
        "  , CHECK (json_valid(MLModelJson))"
        "  , CHECK (json_valid(HWConstraintsJson))"
        "  , CHECK (json_valid(HWResourcesJson))"
        "  , CHECK (json_valid(CarbonFootprintJson))"
    #endif
        "  , UNIQUE(problem_id, iteration_id)"
        ");"
        "CREATE INDEX IF NOT EXISTS idx_task_results_problem_iter "
        "ON task_results(problem_id, iteration_id);";

    int rc = exec(ddl);
    if (rc != SQLITE_OK)
    {
        return rc;
    }

    return SQLITE_OK;
}


int Database::replace_all_rows(const std::vector<Row>& rows)
{
    if (!db_)
    {
        return SQLITE_MISUSE;
    }

    int rc = exec("BEGIN;");
    if (rc != SQLITE_OK) return rc;

    rc = exec("DELETE FROM task_results;");
    if (rc != SQLITE_OK)
    {
        exec("ROLLBACK;");
        return rc;
    }

    const char* insert_sql =
        "INSERT INTO task_results ("
        "  problem_id, iteration_id,"
        "  UserInputJson, AppRequirementsJson, MLModelMetadataJson, MLModelJson,"
        "  HWConstraintsJson, HWResourcesJson, CarbonFootprintJson)"
        "VALUES (?,?,?,?,?,?,?,?,?);";

    sqlite3_stmt* stmt = nullptr;
    rc = sqlite3_prepare_v2(db_, insert_sql, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_prepare_v2(replace_all_rows) failed: " << rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        exec("ROLLBACK;");
        return rc;
    }

    auto bind_txt = [](sqlite3_stmt* s, int idx, const std::string& v) -> int
    {
        return sqlite3_bind_text(s, idx, v.c_str(), static_cast<int>(v.size()), SQLITE_TRANSIENT);
    };

    for (const auto& r : rows)
    {
        sqlite3_reset(stmt);
        sqlite3_clear_bindings(stmt);

        sqlite3_bind_int64(stmt, 1, r.problem_id);
        sqlite3_bind_int64(stmt, 2, r.iteration_id);

        bind_txt(stmt, 3,  r.user_input_json);
        bind_txt(stmt, 4,  r.app_requirements_json);
        bind_txt(stmt, 5,  r.ml_model_metadata_json);
        bind_txt(stmt, 6,  r.ml_model_json);
        bind_txt(stmt, 7,  r.hw_constraints_json);
        bind_txt(stmt, 8,  r.hw_resources_json);
        bind_txt(stmt, 9,  r.carbon_footprint_json);

        rc = sqlite3_step(stmt);
        if (rc != SQLITE_DONE)
        {
            std::cerr << "sqlite3_step(replace_all_rows) failed: " << rc
                      << " msg=" << sqlite3_errmsg(db_) << "\n";
            sqlite3_finalize(stmt);
            exec("ROLLBACK;");
            return rc;
        }
    }

    sqlite3_finalize(stmt);

    rc = exec("COMMIT;");
    if (rc != SQLITE_OK)
    {
        exec("ROLLBACK;");
    }
    return rc;
}


int Database::read_all_rows(std::vector<Row>& out_rows)
{
    if (!db_)
    {
        return SQLITE_MISUSE;
    }

    const char* q =
        "SELECT problem_id, iteration_id,"
        "       UserInputJson, AppRequirementsJson, MLModelMetadataJson, MLModelJson,"
        "       HWConstraintsJson, HWResourcesJson, CarbonFootprintJson "
        "FROM task_results "
        "ORDER BY problem_id, iteration_id;";

    sqlite3_stmt* stmt = nullptr;
    int rc = sqlite3_prepare_v2(db_, q, -1, &stmt, nullptr);
    if (rc != SQLITE_OK)
    {
        std::cerr << "sqlite3_prepare_v2(read_all_rows) failed: " << rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        return rc;
    }

    auto to_str = [](sqlite3_stmt* s, int idx) -> std::string
    {
        const unsigned char* t = sqlite3_column_text(s, idx);
        return t ? reinterpret_cast<const char*>(t) : std::string();
    };

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
    {
        Row r;
        r.problem_id            = sqlite3_column_int64(stmt, 0);
        r.iteration_id          = sqlite3_column_int64(stmt, 1);
        r.user_input_json       = to_str(stmt, 2);
        r.app_requirements_json = to_str(stmt, 3);
        r.ml_model_metadata_json= to_str(stmt, 4);
        r.ml_model_json         = to_str(stmt, 5);
        r.hw_constraints_json   = to_str(stmt, 6);
        r.hw_resources_json     = to_str(stmt, 7);
        r.carbon_footprint_json = to_str(stmt, 8);
        out_rows.emplace_back(std::move(r));
    }

    if (rc != SQLITE_DONE)
    {
        std::cerr << "sqlite3_step(read_all_rows) failed: " << rc
                  << " msg=" << sqlite3_errmsg(db_) << "\n";
        sqlite3_finalize(stmt);
        return rc;
    }

    sqlite3_finalize(stmt);
    return SQLITE_OK;
}


sqlite3* Database::getDB() const
{
    return db_;
}

} // database
} // sustainml
