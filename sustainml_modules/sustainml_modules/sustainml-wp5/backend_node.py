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

from flask import Flask, request
import threading
import time
import signal
import sys
from orchestrator_node import orchestrator_node
from werkzeug.serving import make_server

running = True
orchestrator = orchestrator_node.Orchestrator()
server = Flask(__name__)
server_ip_address = '127.0.0.1'
server_port = 5000

# Flask server default route
@server.route('/')
def hello_world():
    return 'Hello world! Use "/terminate" route to stop Back-end node.\n'

@server.route('/status', methods=['GET'])
def status():
    return orchestrator.get_all_status()

# Flask server shutdown route
@server.route('/shutdown', methods=['GET'])
def shutdown():
    shutdown_func = request.environ.get('werkzeug.server.shutdown')
    if shutdown_func is None:
        return 'Use "/terminate" route to stop Back-end node.\n'
    shutdown_func()
    return 'Terminating...\n'

# Flask server terminate route
@server.route('/terminate', methods=['GET'])
def terminate():
    signal.raise_signal(signal.SIGINT)
    return 'Terminating...\n'

class ServerThread(threading.Thread):
    def __init__(self):
        # Create orchestrator node
        self.orchestrator_thread = threading.Thread(target=orchestrator.run)
        # Create Flask server
        threading.Thread.__init__(self)
        self.srv = make_server(server_ip_address, server_port, server)
        self.ctx = server.app_context()
        self.ctx.push()

    def run(self):
        # Start the orchestrator thread
        self.orchestrator_thread.start()
        # Start the Flask server
        self.srv.serve_forever()

    def shutdown(self):
        # Terminate the orchestrator and its thread
        orchestrator.terminate()
        # Terminate the Flask server
        self.srv.shutdown()
        # Wait for the orchestrator thread to finish
        self.orchestrator_thread.join()

# Process signal handler
def signal_handler(sig, frame):
    print("\nBack-end node terminating...")
    flask_server_thread.shutdown()
    global running
    running = False

# Main program execution
if __name__ == '__main__':
    signal.signal(signal.SIGINT, signal_handler)
    print("Back-end Node running, use Ctrl+C to terminate. Server listening at http://" + server_ip_address + ":" + str(server_port) + "/")
    flask_server_thread = ServerThread()
    flask_server_thread.start()  # Start the Flask server with the orchestrator
    while running:
        time.sleep(1)
    flask_server_thread.join()
    sys.exit(0)
