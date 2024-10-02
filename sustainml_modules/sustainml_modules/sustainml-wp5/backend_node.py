# Copyright 2024 SustainML Consortium
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
"""SustainML Backend Node Implementation."""

from sustainml_py.nodes.OrchestratorNode import OrchestratorNode

# Manage signaling
import signal
import threading
import time

# Whether to go on spinning or interrupt
running = False

# Signal handler
def signal_handler(sig, frame):
    print("\nExiting")
    OrchestratorNode.terminate()
    global running
    running = False

def new_output(id, data):
    print("New output")

def status_change(id, status):
    # NodeID_ID_APP_REQUIREMENTS are the kind of ids
    print("Status change")

# Main workflow routine
def run():
    global running
    running = True

    node = OrchestratorNode(
        new_node_output=new_output,
        node_status_change=status_change)
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
