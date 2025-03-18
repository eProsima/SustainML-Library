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
"""SustainML Example Python Node API."""

from sustainml_py.nodes.CarbonFootprintNode import CarbonFootprintNode
from sustainml_swig import ErrorCode

# Manage signaling
import signal
import threading
import time

# Whether to go on spinning or interrupt
running = False

# Signal handler
def signal_handler(sig, frame):
    print("\nExiting")
    CarbonFootprintNode.terminate()
    global running
    running = False

# User Callback implementation
# Inputs: ml_model, user_input, hw
# Outputs: node_status, co2
def task_callback(ml_model, user_input, hw, node_status, co2):
    print (ml_model.model())
    print (user_input.problem_definition())
    print (hw.hw_description())
    print (node_status.node_status())
    co2.carbon_intensity(4)

# User Configuration Callback implementation
# Inputs: req
# Outputs: res
def configuration_callback(req, res):
    print (req.configuration())
    res.node_id(req.node_id())
    res.transaction_id(req.transaction_id())
    res.configuration(req.configuration())
    res.success(True)
    res.err_code(ErrorCode.NO_ERROR)


# Main workflow routine
def run():
    global running
    running = True
    node = CarbonFootprintNode(callback=task_callback, service_callback=configuration_callback)
    node.spin()

# Call main in program execution
if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)

    """Python does not process signals async if
    the main thread is blocked (spin()) so, tun
    user work flow in another thread """
    runner = threading.Thread(target=run)
    runner.start()

    while running:
        time.sleep(1)

    runner.join()
