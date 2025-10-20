# Copyright 2025 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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

"""AsyncTaskManager for handling cancellable async tasks in SustainML nodes."""

import asyncio
import threading
import logging
from typing import Dict, Callable, Optional


class AsyncTaskManager:
    """Manages async tasks with cancellation support for SustainML nodes."""

    def __init__(self):
        self.running_tasks: Dict[str, asyncio.Task] = {}
        self.stop_events: Dict[str, asyncio.Event] = {}
        self.loop: Optional[asyncio.AbstractEventLoop] = None
        self.loop_thread: Optional[threading.Thread] = None
        self._setup_event_loop()

    def _setup_event_loop(self):
        """Setup a dedicated event loop for async operations."""
        def run_loop():
            self.loop = asyncio.new_event_loop()
            asyncio.set_event_loop(self.loop)
            self.loop.run_forever()

        self.loop_thread = threading.Thread(target=run_loop, daemon=True)
        self.loop_thread.start()

        # Wait for loop to be ready
        while self.loop is None:
            threading.Event().wait(0.01)

    def _get_task_key(self, task_id) -> str:
        """Convert task_id to string key following SustainML patterns."""
        if hasattr(task_id, 'problem_id') and hasattr(task_id, 'iteration_id'):
            return f"{task_id.problem_id()}_{task_id.iteration_id()}"
        return str(task_id)

    async def _wrapped_callback(self, original_callback: Callable, stop_event: asyncio.Event, *args, **kwargs):
        """Wrapper for the original callback that can be cancelled."""
        try:
            # Check if we should stop before starting
            if stop_event.is_set():
                logging.info("Task was stopped before execution")
                return

            # If the callback is async, await it with cancellation support
            if asyncio.iscoroutinefunction(original_callback):
                # Create a task that can be cancelled
                callback_task = asyncio.create_task(original_callback(*args, **kwargs))
                stop_task = asyncio.create_task(stop_event.wait())

                # Wait for either the callback to complete or stop signal
                done, pending = await asyncio.wait(
                    [callback_task, stop_task],
                    return_when=asyncio.FIRST_COMPLETED
                )

                # Cancel any pending tasks
                for task in pending:
                    task.cancel()

                if stop_task in done:
                    logging.info("Task was stopped during execution")
                    callback_task.cancel()
                    return

                # Return the result if callback completed
                if callback_task in done:
                    return callback_task.result()
            else:
                # For sync callbacks, run in executor with periodic stop checks
                def sync_wrapper():
                    if stop_event.is_set():
                        return None
                    return original_callback(*args, **kwargs)

                return await self.loop.run_in_executor(None, sync_wrapper)

        except asyncio.CancelledError:
            logging.info("Task was cancelled")
            raise
        except Exception as e:
            logging.error(f"Error in task execution: {e}")
            raise

    def start_task(self, task_id, callback: Callable, *args, **kwargs) -> bool:
        """Start a new async task."""
        if self.loop is None:
            return False

        task_key = self._get_task_key(task_id)

        # Stop existing task with same ID if any
        self.stop_task(task_id)

        # Create stop event for this task
        stop_event = asyncio.Event()

        # Schedule the stop event creation in the event loop
        asyncio.run_coroutine_threadsafe(
            self._create_stop_event(task_key, stop_event),
            self.loop
        )

        # Schedule the task in the event loop
        future = asyncio.run_coroutine_threadsafe(
            self._wrapped_callback(callback, stop_event, *args, **kwargs),
            self.loop
        )

        # Convert future to task for easier management
        def on_done(fut):
            # Clean up when task completes
            self.running_tasks.pop(task_key, None)
            self.stop_events.pop(task_key, None)

        future.add_done_callback(on_done)
        self.running_tasks[task_key] = future

        return True

    async def _create_stop_event(self, task_key: str, stop_event: asyncio.Event):
        """Create and store stop event in the event loop."""
        self.stop_events[task_key] = stop_event

    def stop_task(self, task_id) -> bool:
        """Stop a running task."""
        task_key = self._get_task_key(task_id)

        # Signal the task to stop
        if task_key in self.stop_events:
            # Schedule the stop event to be set in the event loop
            if self.loop and not self.loop.is_closed():
                asyncio.run_coroutine_threadsafe(
                    self._set_stop_event(task_key),
                    self.loop
                )

        # Cancel the running task if it exists
        if task_key in self.running_tasks:
            task = self.running_tasks[task_key]
            task.cancel()
            return True

        return False

    async def _set_stop_event(self, task_key: str):
        """Set the stop event for a task."""
        if task_key in self.stop_events:
            self.stop_events[task_key].set()

    def is_task_running(self, task_id) -> bool:
        """Check if a task is currently running."""
        task_key = self._get_task_key(task_id)
        return task_key in self.running_tasks

    def stop_all_tasks(self):
        """Stop all running tasks."""
        for task_key in list(self.running_tasks.keys()):
            # Create a dummy task_id object following the pattern
            parts = task_key.split('_')
            problem_id = int(parts[0]) if len(parts) > 0 else 0
            iteration_id = int(parts[1]) if len(parts) > 1 else 0

            # Use sustainml_swig to create proper TaskId
            try:
                import sustainml_swig
                task_id = sustainml_swig.TaskId()
                task_id.problem_id(problem_id)
                task_id.iteration_id(iteration_id)
                self.stop_task(task_id)
            except ImportError:
                # Fallback if sustainml_swig not available
                pass

    def shutdown(self):
        """Shutdown the async task manager."""
        self.stop_all_tasks()
        if self.loop and not self.loop.is_closed():
            self.loop.call_soon_threadsafe(self.loop.stop)
        if self.loop_thread and self.loop_thread.is_alive():
            self.loop_thread.join(timeout=1.0)

# Global instance following SustainML pattern
_task_manager = None

def get_task_manager() -> AsyncTaskManager:
    """Get the global task manager instance."""
    global _task_manager
    if _task_manager is None:
        _task_manager = AsyncTaskManager()
    return _task_manager
