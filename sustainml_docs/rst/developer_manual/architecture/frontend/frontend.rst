.. _frontend:

Front-End Node
==============

.. raw:: html

   <style>
        .module_node:not(#front-end) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#front-end) {
            opacity: 0.6;
        }

        .module_node:hover:not(#module_node) {
            opacity: 1 !important;
        }

    </style>

.. raw:: html
   :file: ../../../figures/svg_href_loader.html

.. raw:: html
   :file: ../../../figures/sustainml_framework_arch.svg

The Front-End component within the software architecture of the *SustainML Design Framework* , is the user-facing software component.
In addition, is responsible for displaying information, receiving user input for defining a new task, and providing a
means for users to interact with the underlying functionality of the *SustainML Design Framework* in synchronization with the :ref:`orchestrator`.

The Front-End includes all the visual elements conforming the user interface, such as windows, dialogs,
buttons, text fields, menus, icons, and other graphical elements.
The UI of the SustainML Design Framework is designed to be intuitive and user-friendly to reach all kind of end-users: from novel to experts in AI.

