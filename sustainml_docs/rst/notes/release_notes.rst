.. _release_notes:

Release notes
=============

The *SustainML Framework* is released as a whole: the *SustainML Library* (with its module nodes, the
``sustainml-wp1``, ``sustainml-wp2`` and ``sustainml-wp3`` submodules, and the back end) and the *SustainML Framework*
application share the same version.

Version 1.0.0
-------------

This release includes the following **features**:

* Support for the **Axelera Edge 130p** board: the YOLO models of the board, with the latency, power and memory
  measured on it. The model is chosen for the goal and for what the problem description asks for (accuracy, speed or
  energy), or selected by hand.
* Support for the **FPGA** ``xczu19eg-ffvb1517-2-i``: the 217 U-Net models that DFKI measured on the board, and any other
  U-Net model, with the latency and energy predicted by DFKI's predictor.
* *YOLO models* screen: the CO2 per accuracy point of every YOLO model of the Axelera board in a chosen country, and the
  curve that shows where extra CO2 stops buying accuracy.
* *U-Net models* screen, with a graph of the architecture of each model.
* Search of pre-trained **Hugging Face** models for the problem description, through an MCP server, and evaluation of a
  model found on the chosen hardware.
* **Comparison of Hugging Face models**, written by the ``llama3`` LLM from their Hugging Face facts.
* **Saving and loading** of results, Hugging Face searches and comparisons, in named save files.
* **Sustainability targets**: carbon footprint optimization (**Manual** and **Auto**), maximum memory footprint and
  location of the deployment. The carbon footprint uses the carbon intensity of the chosen country.
* Memory footprint of the suggested models.
* *Fingerprint* tab: radar charts that compare the CO2, energy, latency, memory and power of the checked iterations.
* **Stop** button to stop following a running task.
* Configuration requests between the orchestrator and the nodes over DDS RPC.

This release includes the following **improvements**:

* New layout and design of the application, with a 16:9 window.
* Knowledge graph updated with the latest data from DFKI.
* The model proposed from the knowledge graph is the nearest one to the problem description, so the same problem
  always gets the same model.
* The modality and metrics fields are removed from the problem definition.
* The hardware of the RPTU framework is removed.
* ``onnx`` updated to 1.21.0 and ``pillow`` to 12.2.0 or later.
* Continuous integration on Ubuntu 24.04, with the first tests of the Python API (``sustainml_py``).
* Improved README files.

This release includes the following **bug fixes**:

* The inputs and outputs of the problem are sent correctly, also in the re-iterations, where they were split into
  single letters.
* Long model names are no longer cut or overlapped in the lists and tables.
* The upload of a dataset no longer fails when the ``llama3`` answer is not in the expected format.
* The *YOLO models* screen starts with the country chosen in the *Problem definition* form, or with the world average
  when none is chosen.
* The filter of the lists that can be filtered is emptied each time a list is opened.
* The names of the models in the table of a Hugging Face comparison no longer show stray spaces.
* The Python examples of the *SustainML Library* (``sustainml_py/examples``) no longer fail on an ``ErrorCode`` import
  that the Python bindings do not provide.
* On the FPGA, a U-Net model whose ONNX file is not found is reported as an error, instead of another model being
  evaluated.
* A Hugging Face comparison waits up to 500 seconds for the ``llama3`` answer.

This release includes the following **documentation** updates:

* New user manual of the application, with screenshots of every screen.
* Developer manual updated: architecture, module nodes, configuration requests and data model.
* Installation with Docker updated.

This release includes the following **dependencies update**:

.. list-table::
    :header-rows: 1
    :widths: 32 38 15 15

    *   -
        - Repository
        - Old Version
        - New Version
    *   - Foonathan Memory Vendor
        - `eProsima/foonathan_memory_vendor <https://github.com/eProsima/foonathan_memory_vendor>`__
        - `v1.3.1 <https://github.com/eProsima/foonathan_memory_vendor/releases/tag/v1.3.1>`__
        - `v1.3.2 <https://github.com/eProsima/foonathan_memory_vendor/releases/tag/v1.3.2>`__
    *   - Fast CDR
        - `eProsima/Fast-CDR <https://github.com/eProsima/Fast-CDR>`__
        - `v2.3.2 <https://github.com/eProsima/Fast-CDR/releases/tag/v2.3.2>`__
        - `v2.3.6 <https://github.com/eProsima/Fast-CDR/releases/tag/v2.3.6>`__
    *   - Fast DDS
        - `eProsima/Fast-DDS <https://github.com/eProsima/Fast-DDS>`__
        - `v3.3.0 <https://github.com/eProsima/Fast-DDS/releases/tag/v3.3.0>`__
        - `v3.4.3 <https://github.com/eProsima/Fast-DDS/releases/tag/v3.4.3>`__
    *   - Dev Utils
        - `eProsima/dev-utils <https://github.com/eProsima/dev-utils>`__
        - `v1.3.0 <https://github.com/eProsima/dev-utils/releases/tag/v1.3.0>`__
        - `v1.4.0 <https://github.com/eProsima/dev-utils/releases/tag/v1.4.0>`__

Version 0.2.0
-------------

This release includes the following **features**:

* Integration of the RPTU framework.
* Model selection with DFKI's RAG.
* Knowledge graph moved from a TTL file to a Neo4j database.
* Upload of a dataset, whose metadata is extracted and used to describe the problem.
* Models downloaded from Hugging Face, and the unsupported model architectures read from a file.
* ``sustainml-framework`` command, which starts the nodes and the application.
* Several comparison tabs in the results.

This release includes the following **improvements**:

* The carbon footprint is shown again in the result screens.
* Several improvements of the application.
* Documentation with the Furo theme, and the SustainML logo.

This release includes the following **bug fixes**:

* ``max_instances`` of the baselines writer.

This release includes the following **dependencies update**:

.. list-table::
    :header-rows: 1
    :widths: 32 38 15 15

    *   -
        - Repository
        - Old Version
        - New Version
    *   - Foonathan Memory Vendor
        - `eProsima/foonathan_memory_vendor <https://github.com/eProsima/foonathan_memory_vendor>`__
        - `v1.3.1 <https://github.com/eProsima/foonathan_memory_vendor/releases/tag/v1.3.1>`__
        - `v1.3.1 <https://github.com/eProsima/foonathan_memory_vendor/releases/tag/v1.3.1>`__
    *   - Fast CDR
        - `eProsima/Fast-CDR <https://github.com/eProsima/Fast-CDR>`__
        - `v2.3.0 <https://github.com/eProsima/Fast-CDR/releases/tag/v2.3.0>`__
        - `v2.3.2 <https://github.com/eProsima/Fast-CDR/releases/tag/v2.3.2>`__
    *   - Fast DDS
        - `eProsima/Fast-DDS <https://github.com/eProsima/Fast-DDS>`__
        - `v3.2.2 <https://github.com/eProsima/Fast-DDS/releases/tag/v3.2.2>`__
        - `v3.3.0 <https://github.com/eProsima/Fast-DDS/releases/tag/v3.3.0>`__
    *   - Dev Utils
        - `eProsima/dev-utils <https://github.com/eProsima/dev-utils>`__
        - `v1.2.0 <https://github.com/eProsima/dev-utils/releases/tag/v1.2.0>`__
        - `v1.3.0 <https://github.com/eProsima/dev-utils/releases/tag/v1.3.0>`__

Version 0.1.0
-------------

This is the first release of the *SustainML Framework*.

This release includes the following **features** in the *SustainML Library*:

* C++ library of the framework nodes and their DDS data types, with Python bindings (SWIG).
* The six module nodes: *App Requirements*, *ML Model Metadata*, *ML Model Provider*, *HW Constraints*,
  *HW Resources* and *Carbon Footprint*.
* Orchestrator and back-end node, with a REST server for the application.
* Reiteration of a task, and several output models for a problem.
* Configuration service to ask the nodes for their configuration.
* DDS domain set with the ``SUSTAINML_DOMAIN_ID`` environment variable.
* Support for Fast DDS 3.

This release includes the following **features** in the *SustainML Framework* application:

* Problem definition screen.
* Results screen, with the tables of the iterations and their comparison.
* Status of the nodes, updated periodically.
* Deployment with Docker.

This release includes the following **documentation**:

* Installation manual.
* Description of the module nodes of the library.

This release uses the following **dependencies**:

.. list-table::
    :header-rows: 1
    :widths: 40 45 15

    *   -
        - Repository
        - Version
    *   - Foonathan Memory Vendor
        - `eProsima/foonathan_memory_vendor <https://github.com/eProsima/foonathan_memory_vendor>`__
        - `v1.3.1 <https://github.com/eProsima/foonathan_memory_vendor/releases/tag/v1.3.1>`__
    *   - Fast CDR
        - `eProsima/Fast-CDR <https://github.com/eProsima/Fast-CDR>`__
        - `v2.3.0 <https://github.com/eProsima/Fast-CDR/releases/tag/v2.3.0>`__
    *   - Fast DDS
        - `eProsima/Fast-DDS <https://github.com/eProsima/Fast-DDS>`__
        - `v3.2.2 <https://github.com/eProsima/Fast-DDS/releases/tag/v3.2.2>`__
    *   - Dev Utils
        - `eProsima/dev-utils <https://github.com/eProsima/dev-utils>`__
        - `v1.2.0 <https://github.com/eProsima/dev-utils/releases/tag/v1.2.0>`__
