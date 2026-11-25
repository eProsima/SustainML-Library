.. _user_manual_overview:

Getting started
===============

The *SustainML Framework* application is the graphical interface of the framework.
You describe a machine learning problem in it, and the framework nodes suggest a model and the hardware to run it on,
with the power, energy, carbon footprint and memory that the model needs on that hardware.
This manual describes every screen of the application and how to use it.

.. _user_manual_launch:

Launching the application
-------------------------

Run the complete framework (the nodes and the application) with:

.. code-block:: bash

    sustainml-framework

This command, installed from the ``framework_run.sh`` script of the *SustainML Framework* repository, does the following:

#. Starts the Neo4j database that holds the knowledge graph of ML problems and models, and waits until it accepts connections.
#. Starts the back-end node, which hosts the orchestrator and the REST API the application talks to (``127.0.0.1:5001``),
   and waits until it is ready.
#. Starts the six framework nodes: *App Requirements*, *ML Model Metadata*, *ML Model Provider*, *HW Constraints*,
   *HW Resources* and *Carbon Footprint*.
#. Opens the application (the ``sustainml`` command).

Closing the application stops every node and the Neo4j database.
See :ref:`installation_framework` for the installation and the requirements of the framework.

Some features need more than the nodes:

* The ``llama3`` model served by `Ollama <https://ollama.com/>`_ on the same machine is used to find the ML goal of a
  Transformers problem when the user does not choose one, to search and compare Hugging Face models, and to describe an
  uploaded dataset.
* Evaluating Transformers models and searching and comparing Hugging Face models need an Internet connection.
  The Hugging Face access token (the ``HF_TOKEN`` environment variable) is used to load gated models and to read the
  information of the models to compare; the search does not need it.

.. _user_manual_start_screen:

Start screen
------------

The application opens on the start screen, which presents SustainML and the project partners
(click a partner logo to open its website).

.. figure:: /rst/figures/user_manual/start_screen.png
    :align: center
    :alt: Start screen of the SustainML Framework application

    The start screen.

Click **Start now** to go to the :ref:`Problem definition <user_manual_problem_definition>` screen.
The start screen is shown only once, at launch.

.. _user_manual_navigation:

Navigation
----------

Every screen has a header with its title, a short description and its buttons.

* **Home** goes to the *Problem definition* screen, which is the starting point of every task (not to the start screen).
* The back arrow returns to the previous screen.
  When it leads to a fixed screen, its tooltip names that screen (for example, *Go to Problem Definition screen*).
* The activity icon (a pulse line) at the right end of every header opens the :ref:`Node status <user_manual_node_status>`
  screen. It is disabled while a task is running, and hidden in the iteration info view.

The window can be resized down to 1050 x 560 pixels.

.. _user_manual_workflow:

Workflow
--------

A typical session goes as follows:

#. Wait until the *Problem definition* screen no longer shows **Initializing...**: the framework nodes are then ready
   and the goal and hardware lists are filled in.
#. Describe the problem, choose the kind of model and the hardware, and set the sustainability targets
   (see :ref:`user_manual_problem_definition`).
   Optionally, upload a dataset to fill in its metadata automatically.
#. Click **Submit**.
   The application opens the *Results* screen, where every evaluated model (an *iteration* of the problem) is added as it
   arrives (see :ref:`user_manual_results`).
#. Compare the iterations in tables, bar charts and radar charts, and save the results to load them in a later session
   (see :ref:`user_manual_node_status_save_load`).

The application also has screens to explore models directly (see :ref:`user_manual_models`):

* **HF models**: search pre-trained Hugging Face models for the problem description, compare them, and evaluate one.
* **YOLO models**: the CO2 and accuracy of the YOLO models measured on the Axelera board.
* **U-Net models**: the U-Net architectures available for the FPGA, with a graph of each one.
