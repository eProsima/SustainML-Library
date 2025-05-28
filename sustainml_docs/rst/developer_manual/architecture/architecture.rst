.. _architecture:

SustainML Framework Architecture
================================

.. raw:: html

   <style>
        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node {
            opacity: 0.6;
        }

        .module_node:hover:not(#module_node) {
            opacity: 1 !important;
        }

    </style>

.. raw:: html
   :file: ../../figures/svg_href_loader.html

.. raw:: html
   :file: ../../figures/sustainml_framework_arch.svg

The *SustainML Framework* aims to provide an energy optimized hardware solution and the corresponding ML model for solving a desired
machine learning problem formulated by the user, considering the carbon footprint it would take to train the resulting model.

Within the *SustainML Architecture*, a ``Node`` refers to each one of the software modules that performs
a particular task, conforms a single executable unit and can be locally or remotely deployed.
This decoupling and modularity can be achieved thanks to the underlying `eProsima Fast DDS middleware <https://www.eprosima.com/index.php/products-all/eprosima-fast-dds>`_.

In the architecture, the ``Front-End`` acts as the interface between the user and the *SustainML Framework*.
The Front-End is responsible for collecting the user requirements and constraints, sending them to the Back-End and give feedback of the intermediate and final results when available:

* :ref:`frontend`

The ``Back-End`` is composed of two main entities:

* The :ref:`orchestrator` that controls the execution of the task and nourishes data to the Front-End.
* The :ref:`module_nodes` that specialize in particular tasks of the solution.

.. _module_nodes:

Module Nodes
""""""""""""

And the different ``Module Nodes`` that specializes in a particular task of the solution:

* :ref:`appreqs_node`
* :ref:`carbontracker_node`
* :ref:`hwconstraints_node`
* :ref:`mlmodelmetadata_node`
* :ref:`mlmodelprovider_node`
* :ref:`hwprovider_node`


.. toctree::
   :maxdepth: 1
   :hidden:

   frontend/frontend
   backend/orchestrator
   backend/module_nodes/apprequirements
   backend/module_nodes/carbontracker
   backend/module_nodes/hwconstraints
   backend/module_nodes/mlmodelmetadata
   backend/module_nodes/mlmodelprovider
   backend/module_nodes/hwprovider
