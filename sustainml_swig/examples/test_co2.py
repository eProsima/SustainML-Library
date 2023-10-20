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
        print (arg2.problem_description())
        print (arg3.hw_description())
        print (arg4.node_status())
        arg5.carbon_intensity(4)
        arg5.task_id(arg1.task_id() + 1)

def run():
    listener = MyListener()
    task_node = sustainml_swig.CarbonFootprintNode(listener)
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