.. _orchestrator:

Orchestrator Node
=================

.. raw:: html

   <style>
        .module_node:not(#orchestrator) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#orchestrator) {
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

The :ref:`orchestrator` within the *SustainML Framework* is the entity in charge of commanding and gathering the generated data by each of the :ref:`module_nodes` as well as acting as the data-source for the :ref:`frontend`.

The Orchestrator Node has three main resposibilities:

Manage nodes' lifecycle
^^^^^^^^^^^^^^^^^^^^^^^

It is the application's ``Lifecycle Manager`` meaning that it controls the lifecycle of the rest of the nodes by sending control commands and receiving the feedback status.

Monitor tasks
^^^^^^^^^^^^^

Each user task has an associated ``task_id``. The ``Orchestrator Node`` is in charge of generating a unique ``task_id`` and keeping track of the status of every initiated task, which allows for serving multiple tasks at a time.
The act of tracking a particular task, involves subscribing to all the intermediate data outputs from each of the :ref:`module_nodes` as well as always monitoring the node statuses to be able to react, for instance, in case that a ``module node`` is taking too much to respond, or simply, that enters in and error state.
Each of the ``task_ids`` along with the corresponding data, has a reserved storage within the orchestrator as it is explained next.

Host database
^^^^^^^^^^^^^

The ``Orchestrator Node`` is where all the data reside. It can be considered similar to a ``Database``.
All the data is indexed by a ``problem_id``.
A ``probelm_id``, in turn, has a sorted vector of ``iteration_id`` where each element stores a single iteration for that problem.
All the information is made available to the Front-end to be displayed so the user can receive online feedback on the status of each task.

