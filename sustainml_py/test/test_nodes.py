# Copyright 2026 Proyectos y Sistemas de Mantenimiento SL (eProsima).
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
"""Smoke tests of the SustainML Python API: the node wrappers import, and refuse missing callbacks."""

import importlib
import unittest

TASK_NODES = [
    'AppRequirementsNode',
    'CarbonFootprintNode',
    'HardwareConstraintsNode',
    'HardwareResourcesNode',
    'MLModelMetadataNode',
    'MLModelNode',
]


def _node_class(name):
    return getattr(importlib.import_module('sustainml_py.nodes.' + name), name)


def _callback(*args):
    pass


class TestNodes(unittest.TestCase):

    def test_import(self):
        for name in TASK_NODES + ['OrchestratorNode']:
            with self.subTest(node=name):
                self.assertTrue(callable(_node_class(name)))

    def test_task_nodes_require_callbacks(self):
        for name in TASK_NODES:
            node_class = _node_class(name)
            with self.subTest(node=name, missing='callback'):
                with self.assertRaises(ValueError):
                    node_class(callback=None, service_callback=_callback)
            with self.subTest(node=name, missing='service_callback'):
                with self.assertRaises(ValueError):
                    node_class(callback=_callback, service_callback=None)

    def test_orchestrator_requires_callbacks(self):
        node_class = _node_class('OrchestratorNode')
        with self.assertRaises(ValueError):
            node_class(new_node_output=None, node_status_change=_callback)
        with self.assertRaises(ValueError):
            node_class(new_node_output=_callback, node_status_change=None)


if __name__ == '__main__':
    unittest.main()
