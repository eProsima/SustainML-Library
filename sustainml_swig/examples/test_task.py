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

class MyListener(sustainml_swig.TaskEncoderTaskListener):
    def __init__(
            self):
        """
        """

        # Parent class constructor
        super().__init__()

    def on_new_task_available(self, arg1, arg2, arg3):
        print (arg1.problem_description())
        arg3.keywords().append("HEYYY")
        arg3.keywords().append("IM")
        arg3.keywords().append("A")
        arg3.keywords().append("KEYWORD")

def run():
    listener = MyListener()
    task_node = sustainml_swig.TaskEncoderNode(listener)
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