# Copyright 2023 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
"""SustainML SWIG Example Node API."""

import sustainml_swig
import signal
import threading
import time

global running
running = False

def signal_handler(sig, frame):
    print("\nExiting")
    sustainml_swig.TaskEncoderNode.terminate()
    running = False

class MyListener(sustainml_swig.CarbonFootprintTaskListener):
    def __init__(
            self):
        """
        """

        # Parent class constructor
        super().__init__()

    def on_new_task_available(self, arg1, arg2, arg3, arg4, arg5):
        print (arg1.model())
        print (arg2.problem_definition())
        print (arg3.hw_description())
        print (arg4.node_status())
        arg5.carbon_intensity(4)
        arg5.task_id(arg1.task_id())

class MyServiceListener(sustainml_swig.RequestReplyListener):
    def __init__(
            self):
        """
        """

        # Parent class constructor
        super().__init__()

    def on_configuration_request(self, arg1, arg2):
        print("Configuration request:", arg1.configuration())
        arg2.node_id(arg1.node_id())
        arg2.transaction_id(arg1.transaction_id())
        arg2.configuration(arg1.configuration())
        arg2.success(True)
        arg2.err_code(sustainml_swig.ErrorCode.NO_ERROR)

def run():
    listener = MyListener()
    listener_service = MyServiceListener()
    task_node = sustainml_swig.CarbonFootprintNode(listener, listener_service)
    running = True
    task_node.spin()

# Call main in program execution
if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)

    runner = threading.Thread(target=run)
    runner.start()

    while running:
        time.sleep(1)

    runner.join()
