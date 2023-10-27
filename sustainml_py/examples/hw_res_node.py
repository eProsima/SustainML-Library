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

from sustainml_py.nodes.HardwareResourcesNode import HardwareResourcesNode

# Manage signaling
import signal
import threading
import time

# Whether to go on spinning or interrupt
global running
running = False

# Signal handler
def signal_handler(sig, frame):
    print("\nExiting")
    HardwareResourcesNode.terminate()
    running = False

# User Callback implementation
# Inputs: ml_model
# Outputs: node_status, hw
def task_callback(ml_model, node_status, hw):
    print (ml_model.model())
    hw.hw_description("This is a HW description")

# Main workflow routine
def run():
    node = HardwareResourcesNode(callback=task_callback)
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
