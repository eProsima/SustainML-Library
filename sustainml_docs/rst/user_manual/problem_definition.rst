.. _user_manual_problem_definition:

Problem definition
==================

The *Problem definition* screen is where every task starts.
You describe the machine learning problem, choose the kind of model and the hardware, and set the sustainability targets.
On **Submit**, the framework looks for a suitable model (or evaluates the one you chose), measures or estimates it on the
hardware, and computes its carbon footprint.

While the framework nodes start, the header shows **Initializing...** with a spinning icon, and **Submit** is disabled.
They disappear once all six nodes are ready; the **Model goal** and **Required hardware** lists are then filled in.

.. _user_manual_problem_definition_header:

Header buttons
--------------

.. list-table::
    :header-rows: 1
    :widths: 25 75

    * - Button
      - Action
    * - **Results**
      - Opens the :ref:`Results <user_manual_results>` screen.
    * - **Upload dataset**
      - Opens the *Dataset selector* screen (see :ref:`user_manual_problem_definition_dataset`).
        Disabled for CNNs on the FPGA, which do not use the dataset.
    * - **U-Net models**
      - Opens the list of U-Net models for the FPGA (see :ref:`user_manual_models_unet`).
    * - **YOLO models**
      - Opens the YOLO models of the Axelera board, in the country chosen in this form (see :ref:`user_manual_models_yolo`).
    * - **HF models**
      - Opens a menu with **Search** (search Hugging Face models for the problem description), **Results**
        (saved searches) and **Comparisons** (saved comparisons). See :ref:`user_manual_models_hf`.

.. _user_manual_problem_definition_fields:

Form fields
-----------

The form has three cards.
The screenshots of this manual follow an example: summarizing the emails that customers send to the support team of
an online shop, run in Spain.

Problem
^^^^^^^

.. figure:: /rst/figures/user_manual/problem_definition_problem.png
    :align: center
    :alt: Problem card of the Problem definition screen, filled in for a summarization problem

    The *Problem* card of the example.

.. list-table::
    :header-rows: 1
    :widths: 25 75

    * - Field
      - What to enter
    * - **Problem short description**
      - A short summary of the objective of the problem.
    * - **Problem definition**
      - The machine learning problem, described as precisely as possible.
        The framework infers the goal of the problem from the descriptions when no goal is chosen.
    * - **Type limiter**
      - The kind of model: **Transformers** (default) or **CNNs**.
        See :ref:`user_manual_problem_definition_modes`.
    * - **Inputs** / **Outputs**
      - A description of the input and output data of the model.

Model and data
^^^^^^^^^^^^^^

.. list-table::
    :header-rows: 1
    :widths: 25 75

    * - Field
      - What to enter
    * - **Minimum samples** / **Maximum samples**
      - Whole numbers from 1. Leave them empty when they do not apply.
    * - **Model goal**
      - The goal (kind of problem) of the model, from the list of goals of the framework.
        Leave it empty (or choose **(empty)**) to let the framework infer it from the descriptions.
        Choosing a goal fills the **Model selection** list with the models for that goal.
    * - **Required hardware**
      - The hardware the model will run on. The list can be filtered by typing.
        Choosing **(empty)** leaves no hardware, and the problem cannot be submitted.
    * - **Model selection**
      - Optionally, a specific model to evaluate, instead of letting the framework search one.
        The list can be filtered by typing. Choose **(empty)** to go back to no model.
    * - **Nº output models**
      - How many models the framework suggests for the problem, one iteration each. Default: 1.

When a dataset has been uploaded, an **UPLOADED DATASET** section is added to this card with the
**Dataset description**, **Dataset topic**, **Dataset keywords**, **Dataset applications** and **Dataset profile**
extracted from it (only the ones that are not empty). You can edit them before submitting.

Sustainability targets
^^^^^^^^^^^^^^^^^^^^^^

.. figure:: /rst/figures/user_manual/problem_definition_targets.png
    :align: center
    :alt: Model and data and Sustainability targets cards, with Manual optimization, a memory limit and Spain as the country

    The *Model and data* and *Sustainability targets* cards of the example: no goal (the framework infers it),
    three output models, **Manual** optimization with a target of 0.00001 gCO2e per inference, a memory limit of
    3000 MB, and Spain as the country.

.. list-table::
    :header-rows: 1
    :widths: 25 75

    * - Field
      - What to enter
    * - **Carbon footprint optimization**
      - **(empty)** (no optimization), **Manual** or **Auto**. See :ref:`user_manual_problem_definition_optimization`.
    * - **Desired carbon footprint**
      - The target carbon footprint per inference, in gCO2e. Leave it empty for no target.
        The results are ranked against it with any **Carbon footprint optimization**, or with none.
    * - **Max memory footprint**
      - The maximum memory of the model, in MB. Leave it empty for no limit.
    * - **Geo location: continent**
      - The continent where the model will run.
    * - **Geo location: country**
      - The country where the model will run. The list can be filtered by typing.
        Enabled once a continent is chosen; choosing another continent clears it.

The country sets the carbon intensity of the electricity grid used to compute the carbon footprint
(the latest yearly value from Our World in Data).
When no country is chosen, the *Carbon Footprint* node uses the country detected from the IP address of the machine,
or the world average when that country is unknown or not in its list.

The lists that can be filtered show a search field: typing *Spa* leaves *Spain*.

.. _user_manual_problem_definition_modes:

Transformers, CNNs and hardware
-------------------------------

The **Type limiter** and the **Required hardware** together decide how the problem is evaluated.
The hardware list offers only the hardware that runs the chosen type.

.. list-table::
    :header-rows: 1
    :widths: 15 30 55

    * - Type limiter
      - Required hardware
      - Evaluation
    * - **Transformers**
      - The architectures simulated by the *HW Resources* node (UPMEM LLM framework simulator): GPUs (such as
        ``H100`` or ``A6000``), mobile SoCs (such as ``Snapdragon8gen3``) and processing-in-memory devices (such as
        ``SAM_LPDDR5PIM``, ``PIM_AI_1chip`` or ``CXL_PIM_BC``). Default: ``PIM-AI-1chip``.
      - The framework searches a Transformers model for the problem (or evaluates the selected one), and simulates
        its latency and power on the chosen hardware.
    * - **CNNs**
      - ``FPGA (xczu19eg-ffvb1517-2-i)`` (default for CNNs)
      - A U-Net model chosen by hand in **Model selection** (required), with the latency and power predicted for it on the FPGA
        by DFKI's predictor. There is no search: the goal, description and optimization fields do not apply.
    * - **CNNs**
      - ``Axelera Edge 130p``
      - One of the YOLO models of the Axelera board, with the latency and power measured on it.
        The **Model goal** list offers its vision tasks (object detection, oriented object detection,
        instance segmentation, semantic segmentation and keypoint detection), and **Model selection** its YOLO models
        (optional: without one, the framework picks the best model for the goal).

With YOLO models, the problem descriptions stay editable after choosing a goal: the framework reads in them what the
model has to favour (accuracy, speed or energy) to pick it.
**Nº output models** also stays editable: more output models are the next ones of that ranking.
Models over the **Max memory footprint** are not suggested.

.. figure:: /rst/figures/user_manual/problem_definition_axelera.png
    :align: center
    :alt: Model and data card with CNNs on the Axelera Edge 130p, object detection as the goal and three output models

    CNNs on the Axelera board, for a problem that counts the vehicles of a traffic camera: the **object-detection** goal,
    no model selected (the framework picks it) and three output models.

.. _user_manual_problem_definition_disabled:

Disabled fields
^^^^^^^^^^^^^^^

Some fields are disabled (greyed out) when they no longer apply:

* **A goal is chosen** (Transformers): the goal is no longer inferred, so the descriptions, **Inputs**, **Outputs**,
  **Minimum samples**, **Maximum samples**, **Nº output models** and the dataset fields are disabled.
  With YOLO models, only the samples and the dataset fields are disabled.
* **A model is selected, or CNNs run on the FPGA**: the model is evaluated as is, once, with no search.
  Every field that feeds the search is disabled: descriptions, inputs, outputs, samples, **Model goal**,
  **Nº output models**, dataset fields, **Carbon footprint optimization**, **Desired carbon footprint** and
  **Max memory footprint**.
  What you typed in them is kept but not sent: the task is sent without optimization or memory limit, and with one
  output model.
* **Model selection** is disabled until a goal is chosen (except for CNNs on the FPGA), and for CNNs until a hardware
  is chosen. Its placeholder says what is missing.
* **Geo location: country** is disabled until a continent is chosen.

To change a disabled goal, first set **Model selection** back to **(empty)**.

.. figure:: /rst/figures/user_manual/problem_definition_fpga.png
    :align: center
    :alt: Problem definition form with CNNs on the FPGA, where most fields are disabled

    CNNs on the FPGA: only the hardware, the U-Net model and the location can be set, and **Submit** stays disabled until
    a U-Net model is selected.

Values kept when switching
^^^^^^^^^^^^^^^^^^^^^^^^^^

* Changing the **Type limiter** clears the selected model and resets the hardware to the default of the new type.
  Switching to CNNs also clears the goal.
* Changing the hardware with CNNs clears the goal and the selected model, as each board has its own models.
* Changing the goal clears the selected model and loads the models of the new goal.
* The other fields keep their values.

.. _user_manual_problem_definition_optimization:

Carbon footprint optimization
-----------------------------

The **Desired carbon footprint** and the **Max memory footprint** are limits: whatever the optimization, the *Overview*
of the results ranks the iterations against them and names the best one (see :ref:`user_manual_results`).
The optimization decides who runs further iterations of the problem:

* **(empty)**: no optimization. The framework suggests the models it finds for the problem, and the problem cannot be
  reiterated.
* **Manual**: you decide. The **Reiterate** button of the *Iteration inputs* screen
  (see :ref:`user_manual_results_iteration_info`) runs one more iteration of the problem, with a model that was not
  tried yet in it.
* **Auto**: as **Manual**, and in addition, when a model misses the desired carbon footprint or the maximum memory
  footprint (or cannot be measured), the framework discards it and searches another model automatically,
  up to three models for each output model.

Models already tried in earlier iterations of the problem are not suggested again while an optimization is chosen.
The optimization does not apply when a model is selected.

.. _user_manual_problem_definition_submit:

Submitting the problem
----------------------

The footer of the form has two buttons:

* **Clear all** resets every field to its default (Transformers, ``PIM-AI-1chip``, one output model)
  and clears the uploaded dataset.
* **Submit** sends the problem to the framework and opens the :ref:`Results <user_manual_results>` screen.
  It is disabled:

  * while the framework nodes are initializing, or while a list is being loaded;
  * when no hardware is chosen;
  * for CNNs on the FPGA, until a U-Net model is selected;
  * when there is nothing to evaluate: no problem definition, no short description, no selected model and no
    uploaded dataset.

When the *Results* screen was opened from this form, its back arrow returns here with the inputs of the last problem,
except the goal and the selected model, which are left empty so that every field can be edited.

.. _user_manual_problem_definition_dataset:

Uploading a dataset
-------------------

A dataset helps the framework understand the problem: its metadata is used to infer the goal.

#. Click **Upload dataset** in the header of the *Problem definition* screen.
   The *Dataset selector* screen opens.

   .. figure:: /rst/figures/user_manual/dataset_selector.png
       :align: center
       :alt: Dataset selector screen with an empty Dataset path field and the Browse button

       The *Dataset selector* screen.

#. Click **Browse** and choose a CSV (``.csv``) or JSON (``.json``) file.
   Its path is shown in the **Dataset path** field.
#. Wait while **Extracting metadata, please wait** is shown.
   The *ML Model Metadata* node profiles the data and describes it with the ``llama3`` model (served by Ollama).
#. The application returns to the *Problem definition* screen, with the dataset metadata filled in
   (see the **UPLOADED DATASET** section above).

   .. figure:: /rst/figures/user_manual/problem_definition_uploaded_dataset.png
       :align: center
       :alt: Uploaded dataset section of the form, with the description, topic, keywords and applications of a dataset of customer emails

       The metadata of a small CSV file of customer support emails, filled in by the framework.

Only CSV and JSON files are processed. **Clear all** removes the dataset metadata.
