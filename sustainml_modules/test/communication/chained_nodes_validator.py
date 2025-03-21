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
"""Script to test the communication among different SustainML Python Nodes"""

import argparse
import os
import subprocess
import sys
from time import sleep

class ParseOptions():
    """Parse arguments."""

    def __init__(self):
        """Object constructor."""
        self.args = self.__parse_args()

    def __parse_args(self):
        """
        Parse the input arguments.

        :return: A dictionary containing the arguments parsed.
        """
        parser = argparse.ArgumentParser(
            formatter_class=argparse.ArgumentDefaultsHelpFormatter,
            add_help=True,
            description=(
                'Script to test the communication among different SustainML Python Nodes'),
        )
        parser.add_argument(
            '-ps',
            '--pub-sub',
            type=str,
            help='Path to the SimpleTaskPubSub executable.'
        )
        parser.add_argument(
            '-tp',
            '--topic-pub',
            type=str,
            help='String identifying the publisher topic.'
        )
        parser.add_argument(
            '-ttp',
            '--topic-type-pub',
            type=str,
            help='String identifying the publisher topic type [ui, task, ml, hw, co2].'
        )
        parser.add_argument(
            '-ts',
            '--topic-sub',
            type=str,
            help='String identifying the subscriber topic.'
        )
        parser.add_argument(
            '-tts',
            '--topic-type-sub',
            type=str,
            help='String identifying the publisher topic type [ui, task, ml, hw, co2].'
        )
        parser.add_argument(
            '-a',
            '--samples',
            type=int,
            help='Number of samples sent by the publisher and received by the subscriber.'
        )
        parser.add_argument(
            '-mlm',
            '--ml-metadata',
            type=str,
            help='Executable name of a MLModelMetadata in the sustainml-wp1 directory'
        )
        parser.add_argument(
            '-ml',
            '--machine-learning',
            type=str,
            help='Executable name of a MLModelNode in the sustainml-wp1 directory'
        )
        parser.add_argument(
            '-hw',
            '--hardware',
            type=str,
            help='Executable name of a HWResourcesNode in the sustainml-wp2 directory'
        )
        parser.add_argument(
            '-co2',
            '--co2-footprint',
            type=str,
            help='Executable name of a CarbonFootprintNode in the sustainml-wp3 directory'
        )
        parser.add_argument(
            '-hwc',
            '--hw-constraints',
            type=str,
            help='Executable name of a HWConstraints in the sustainml-wp2 directory'
        )
        parser.add_argument(
            '-ap',
            '--app-requirements',
            type=str,
            help='Executable name of a AppRequirements in the sustainml-wp1 directory'
        )
        parser.add_argument(
            '-bt',
            '--baseline-topics',
            nargs='+',
            action='store',
            required=False,
            help='List of baseline topic names.'
        )
        parser.add_argument(
            '-btt',
            '--baseline-topic-types',
            nargs='+',
            action='store',
            required=False,
            help='List of baseline topic types.'
        )
        parser.add_argument(
            '-orc',
            '--orchestrator',
            type=str,
            help='Executable name of the OrchestratorNode'
        )
        parser.add_argument(
            '-py-orc',
            '--python-orchestrator',
            type=str,
            help='Executable name of the python OrchestratorNode'
        )

        return parser.parse_args()


def run(args):
    """
    Run the required nodes

    :param args: The input parameters.

    :return: The return code resulting. It is the number of failed processes.
    """
    pub_commands = []
    sub_command = []
    node_commands = []

    script_dir = os.path.dirname(os.path.realpath(__file__))
    nodes_base_dir = script_dir+'/../../lib/sustainml_modules/'
    print (nodes_base_dir)

    if args.pub_sub and not os.path.isfile(args.pub_sub):
        print(f'PublisherSubscriber executable file does not exists: {args.pub}')
        sys.exit(1)

    if args.pub_sub and not os.access(args.pub_sub, os.X_OK):
        print(
            'PublisherSubscriber executable does not have execution permissions:'
            f'{args.pub_sub}')

    pub_command = []
    if args.pub_sub:
        pub_command.append(os.path.join(script_dir, args.pub_sub))
        sub_command.append(os.path.join(script_dir, args.pub_sub))

    if args.topic_pub and args.topic_sub:
        pub_command.append('publisher')
        pub_command.extend(['--topic', str(args.topic_pub)])
        sub_command.append('subscriber')
        sub_command.extend(['--topic', str(args.topic_sub)])

    if not args.topic_pub and not args.topic_sub and not (args.orchestrator or args.python_orchestrator):
        print('Not provided basic executable names.')
        sys.exit(1)

    if args.topic_type_pub and args.topic_type_sub:
        pub_command.append(str('--'+ str(args.topic_type_pub)))
        sub_command.append(str('--'+ str(args.topic_type_sub)))

    if not args.topic_type_pub and not args.topic_type_sub and not (args.orchestrator or args.python_orchestrator):
        print('Not provided basic topic types.')
        sys.exit(1)

    if args.samples:
        pub_command.extend(['--samples', str(args.samples)])
        sub_command.extend(['--samples', str(args.samples)])

    if args.topic_pub:
        pub_commands.append(pub_command)

    if args.ml_metadata or args.machine_learning or args.hardware or args.co2_footprint or args.app_requirements or args.hw_constraints:

        if args.ml_metadata:
            te_command = ['python3']
            te_exec_file = os.path.join(nodes_base_dir, 'sustainml-wp1/' + args.ml_metadata)

            if not os.path.isfile(te_exec_file):
                print(f'TaskEncoder executable file does not exists: {te_exec_file}')
                sys.exit(1)

            te_command.extend([te_exec_file])
            node_commands.append(te_command)

        if args.machine_learning:
            ml_command = ['python3']
            ml_exec_file = os.path.join(nodes_base_dir, 'sustainml-wp1/' + args.machine_learning)

            if not os.path.isfile(ml_exec_file):
                print(f'MachineLearning executable file does not exists: {ml_exec_file}')
                sys.exit(1)

            ml_command.extend([ml_exec_file])
            node_commands.append(ml_command)

        if args.hardware:
            hw_command = ['python3']
            hw_exec_file = os.path.join(nodes_base_dir, 'sustainml-wp2/' + args.hardware)

            if not os.path.isfile(hw_exec_file):
                print(f'HardwareResources executable file does not exists: {hw_exec_file}')
                sys.exit(1)

            hw_command.extend([hw_exec_file])
            node_commands.append(hw_command)

        if args.co2_footprint:
            co2_command = ['python3']
            co2_exec_file = os.path.join(nodes_base_dir, 'sustainml-wp3/' + args.co2_footprint)

            if not os.path.isfile(co2_exec_file):
                print(f'CarbonFootprint executable file does not exists: {co2_exec_file}')
                sys.exit(1)

            co2_command.extend([co2_exec_file])
            node_commands.append(co2_command)

        if args.hw_constraints:
            hwcons_command = ['python3']
            hwcons_exec_file = os.path.join(nodes_base_dir, 'sustainml-wp2/' + args.hw_constraints)

            if not os.path.isfile(hwcons_exec_file):
                print(f'HardwareConstraints executable file does not exists: {hwcons_exec_file}')
                sys.exit(1)

            hwcons_command.extend([hwcons_exec_file])
            node_commands.append(hwcons_command)

        if args.app_requirements:
            app_reqs_command = ['python3']
            app_reqs_exec_file = os.path.join(nodes_base_dir, 'sustainml-wp1/' + args.app_requirements)

            if not os.path.isfile(app_reqs_exec_file):
                print(f'AppRequirements executable file does not exists: {app_reqs_exec_file}')
                sys.exit(1)

            app_reqs_command.extend([app_reqs_exec_file])
            node_commands.append(app_reqs_command)

    if args.baseline_topics and args.baseline_topic_types and len(args.baseline_topics) == len(args.baseline_topic_types):
        for i_baseline in range(0, len(args.baseline_topics)):
            baseline_cmd = []
            baseline_cmd.append(os.path.join(script_dir, args.pub_sub))
            baseline_cmd.append('publisher')
            baseline_cmd.extend(['--topic', str(args.baseline_topics[i_baseline])])
            baseline_cmd.append(str('--') + str(args.baseline_topic_types[i_baseline]))
            baseline_cmd.extend(['--samples', str(args.samples)])
            pub_commands.append(baseline_cmd)
    elif args.baseline_topics and (not args.baseline_topic_types and not len(args.baseline_topics) == len(args.baseline_topic_types)):
        print('Not provided simple task pubsub topic names.')
        sys.exit(1)

    node_procs = []
    for node_cmd in node_commands:

        node_proc = subprocess.Popen(node_cmd)
        print(
           'Running Node - commmand:  ' + str(node_cmd))

        node_procs.append(node_proc)
        sleep(1)

    if args.topic_sub:
        listener_proc = subprocess.Popen(sub_command)
        print(
            f'Running Subscriber - commmand:  ' + str(sub_command))
    elif args.orchestrator:
        orch_exec = os.path.join(script_dir, args.orchestrator)

        if not os.path.isfile(orch_exec):
            print(f'OrchestratorNode executable file does not exists: {orch_exec}')
            sys.exit(1)

        listener_proc = subprocess.Popen(orch_exec)
        print(
            f'Running Orchestrator - commmand:  ' + str(orch_exec))
        sleep(1)
    elif args.python_orchestrator:
        py_orch_exec = os.path.join(script_dir, args.python_orchestrator)

        if not os.path.isfile(py_orch_exec):
            print(f'OrchestratorNode python file does not exists: {py_orch_exec}')
            sys.exit(1)

        listener_proc = subprocess.Popen(['python3', py_orch_exec])
        print(
            f'Running Python Orchestrator - commmand:  ' + str(py_orch_exec))
        sleep(1)

    pub_procs = []
    for pub_cmd in pub_commands:

        pub_proc = subprocess.Popen(pub_cmd)
        print(
           'Running Publisher - commmand:  ' + str(pub_cmd))

        pub_procs.append(pub_proc)
        sleep(1)

    try:
        outs, errs = listener_proc.communicate(timeout=200)
    except subprocess.TimeoutExpired:
        print('Subscriber process timed out, terminating...')
        listener_proc.kill()
        [pub_proc.kill() for pub_proc in pub_procs]
        [node_proc.kill() for node_proc in node_procs]
        try:
            sys.exit(os.EX_SOFTWARE)
        except AttributeError:
            sys.exit(1)


    [pub_proc.kill() for pub_proc in pub_procs]
    listener_proc.kill()
    [node_proc.kill() for node_proc in node_procs]
    try:
        sys.exit(os.EX_OK)
    except AttributeError:
        sys.exit(0)

if __name__ == '__main__':

    # Parse arguments
    args = ParseOptions()
    run(args.args)
