.. _user_manual_results:

Results
=======

The *Results* screen shows the model, hardware and footprint of every evaluated problem.
It opens on its own when a problem is submitted, and can be opened at any time with the **Results** button of the
*Problem definition* screen.

Each problem has its own tab, and each model evaluated for it is an *iteration* of the problem.
There is one iteration per output model, plus those of a carbon footprint optimization: the ones you add with
**Reiterate** (see :ref:`user_manual_results_iteration_info`) and the ones an **Auto** optimization adds on its own.
Results are added as the framework nodes deliver them.

.. figure:: /rst/figures/user_manual/results_running.png
    :align: center
    :alt: Results screen while a task is running, with the second iteration still without results

    The *Results* screen while the example runs: the first iteration has its results, and the model of the second one is
    known, its hardware and footprint are still being computed.

.. _user_manual_results_header:

Header buttons
--------------

.. list-table::
    :header-rows: 1
    :widths: 25 75

    * - Button
      - Action
    * - **Home**
      - Goes to the *Problem definition* screen.
    * - Back arrow
      - Returns to the previous screen. When it is the *Problem definition* screen, the form keeps the inputs of the
        last problem.
    * - **New problem**
      - Clears the *Problem definition* form and opens it, to define a new problem from scratch.
    * - **Stop**
      - Stops following the running task (see :ref:`user_manual_results_stop`). Enabled only while a task is running.
    * - **Save**
      - Saves only the results of the open tabs into a named file. Enabled once a problem has the results of all six
        nodes. To save the Hugging Face searches and comparisons too, use **Save** on the *Node status* screen
        (activity icon). See :ref:`user_manual_node_status_save_load`.
    * - **Load**
      - Loads only the results of a saved file. Each loaded problem opens in a new tab.

While a task is running, the header shows **Working on task, please wait**, and **Home**, the back arrow,
**New problem** and the Node status icon are disabled.

.. _user_manual_results_tabs:

Problem tabs
------------

A tab named *Problem N* is opened for each problem.

* Double-click the name of a tab to rename it. Press Enter to keep the new name, or Escape to cancel.
  The name is kept when the results are saved.
* Click the cross of a tab to close it. A closed problem is no longer part of the results that **Save** stores.
  When every tab is closed, an empty *New Tab* remains.

Inside a problem tab there are further tabs: **Overview**, and, once iterations are selected in it,
**Iteration**, **Fingerprint** (from two selected iterations) and one tab per metric comparison.

.. _user_manual_results_overview:

Overview
--------

The **Overview** tab has one row per iteration of the problem, with these columns:

.. list-table::
    :header-rows: 1
    :widths: 30 70

    * - Column
      - Content
    * - Info icon
      - Opens the iteration info view (see :ref:`user_manual_results_iteration_info`).
    * - Check box
      - Selects the iteration for the **Iteration**, **Fingerprint** (at least two) and comparison tabs.
        Disabled while a task is running.
    * - **Iteration**
      - Number of the iteration.
    * - **Problem**
      - The goal of the problem, as chosen or inferred. Hidden when no iteration has one (U-Net models on the FPGA).
    * - **ML Model**
      - The suggested model. Hover the cell to see its full name.
    * - **Hardware**
      - The hardware the model was evaluated on.
    * - **Power Consumption [W]**
      - Power of the model on the hardware.
    * - **Carbon Footprint [gCO2e]**
      - Carbon emissions of the use of the model on the hardware.
    * - **Carbon Intensity [gCO2/kWh]**
      - Carbon intensity of the electricity grid of the chosen (or detected) country, in grams of CO2 per kWh.
    * - **Memory Footprint [MB]**
      - Memory the model needs: its weights plus the working memory to process the longest input it accepts.
        Empty when it could not be measured.

Drag the separators of the header to resize the columns.

When the problem has a desired carbon footprint or a maximum memory footprint (with any carbon footprint
optimization, or with none), the rows are ranked: first the models within the limits (lowest carbon footprint first), then the others by
how far they are over the limits. A line above the table names the result:
*Best model within the limits (...)* or, when no model meets them, *No model has met the limits (...) yet. Closest: ...*.

.. figure:: /rst/figures/user_manual/results_overview.png
    :align: center
    :alt: Overview tab with three summarization models ranked against the limits, and the best one named above the table

    The **Overview** of the example, in a tab renamed *Support emails*: the three models are within the limits, so they
    are ranked by carbon footprint, the lowest first.

.. _user_manual_results_iteration_info:

Iteration info view
^^^^^^^^^^^^^^^^^^^

The info icon of a row opens the *Iteration inputs* screen: the *Problem definition* form, read-only, with the values
that iteration was run with. It shows only the fields that applied to it:

* for a Hugging Face model evaluated with **Analyze**, only the problem description used for the search;
* for a selected model (always the case for U-Net models), the type, goal, hardware, model and location.

The *Previous Results* panel on the right repeats the results of the iteration: iteration number, problem kind,
suggested model, suggested hardware, power consumption, carbon footprint, carbon intensity and memory footprint.
Click **Results** in the header to go back to the *Results* screen.

When the iteration searched its model with the **Manual** or **Auto** carbon footprint optimization, the **Reiterate**
button at the bottom of the form runs the same inputs again as one new iteration of the problem, and the *Results*
screen opens to follow it:

* the new iteration suggests a model that was not tried yet in any iteration of the problem;
* with **Auto**, when that model misses the desired carbon footprint or the maximum memory footprint, the framework
  goes on searching on its own, up to three models;
* one click adds one iteration, even when the problem asked for several output models.

**Reiterate** is not available without an optimization, or for a selected model or a U-Net model, because the same model
would be evaluated again. It is not available for a problem loaded from a save file either: the framework keeps only
its results, so it cannot run new iterations of it.

.. figure:: /rst/figures/user_manual/results_iteration_info.png
    :align: center
    :alt: Iteration inputs screen with the read-only form on the left and the Previous Results panel on the right

    The iteration info view of the second iteration of the example.

.. _user_manual_results_iteration:

Iteration
---------

The **Iteration** tab opens when the first iteration is checked in the **Overview**, and closes when none is.
It shows the checked iterations with more metrics than the **Overview**:

.. list-table::
    :header-rows: 1
    :widths: 30 70

    * - Column
      - Content
    * - **Latency [s]**
      - Time of a complete inference of the model on the hardware.
    * - **Power Consumption [W]**
      - Power consumed while the model runs on the hardware.
    * - **Carbon Footprint [gCO2e]**
      - Carbon emissions of the use of the model on the hardware.
    * - **Carbon Intensity [gCO2/kWh]**
      - Emissions per unit of energy of the electricity grid.
    * - **Energy Consumption [kWh]**
      - Energy used by the execution.
    * - **Memory Footprint [MB]**
      - Memory the model needs: weights plus working memory.

The columns **Iteration**, **Problem**, **ML Model** and **Hardware** are shown too.

.. figure:: /rst/figures/user_manual/results_iteration.png
    :align: center
    :alt: Iteration tab with the three iterations of the example and all their metrics

    The **Iteration** tab with the three iterations of the example checked.

Right-click the table to open a context menu:

* On a metric column header: **Compare** opens a comparison tab for that metric
  (see :ref:`user_manual_results_comparison`), and **More info** explains the metric.
* On an iteration number: **More info** opens a panel with every output of the six nodes for that iteration.
* On the name of a Hugging Face model: **More on HF** opens the page of the model on the Hugging Face website.
  The U-Net and YOLO models have no such menu.

.. figure:: /rst/figures/user_manual/results_compare_menu.png
    :align: center
    :alt: Menu with Compare and More info, opened on the Carbon Footprint column header

    Right-click on the **Carbon Footprint** header.

.. figure:: /rst/figures/user_manual/results_more_info.png
    :align: center
    :alt: Panel with the outputs of the six nodes for iteration 2, with the Carbon Footprint node expanded

    **More info** on iteration 2, with the outputs of the *Carbon Footprint* node expanded.

The tab on the right edge of the table opens the **VISIBLE COLUMNS** panel, where columns can be hidden or shown:
**Problem**, **ML Model**, **Hardware**, **Latency**, **Power Consumption**, **Carbon Footprint**, **Carbon Intensity**,
**Energy Consumption** and **Memory Footprint**. Uncheck a column to hide it; the **Iteration** column is always shown.

.. figure:: /rst/figures/user_manual/results_visible_columns.png
    :align: center
    :alt: Visible columns panel with a check box for each metric column of the Iteration tab

    The **VISIBLE COLUMNS** panel, opened with the tab on the right edge of the table.

.. _user_manual_results_comparison:

Metric comparison
-----------------

A comparison tab, named after its metric (for example *Carbon Footprint*), draws a bar chart of that metric with one
bar per checked iteration. Right-click a bar and choose **More info** to see the outputs of the six nodes for that
iteration.

.. figure:: /rst/figures/user_manual/results_comparison.png
    :align: center
    :alt: Bar chart of the carbon footprint of the three iterations of the example

    The *Carbon Footprint* comparison of the example.

The comparison tabs can be closed with their cross, and they all close when fewer than two iterations remain checked.

.. _user_manual_results_fingerprint:

Fingerprint
-----------

The **Fingerprint** tab opens when at least two iterations are checked, and closes when fewer than two are: a single
model has no other one to compare it with.
It draws the *Model fingerprint* of the checked iterations as radar charts: on every axis the best of them sits near
the centre and the worst on the outer ring, so a smaller shape is a greener model.

* The axes are **CO2**, **Energy**, **Latency**, **Memory** (only when every model has it) and **Power**.
* YOLO models of the Axelera board are drawn on their own radar, *Axelera YOLO models*, with one more axis:
  **Accuracy gap**, the accuracy points below the most accurate YOLO model of the same task (0 is the best).
* The other models (Hugging Face, ONNX) are drawn on the *Other models* radar, without accuracy.
* Each radar draws up to six models. Click a model in the legend to hide or show it.
  Hover a point to see its value.
* Checked iterations without a result (an error, or still running) are left out, and listed below the radars.

With a single model on a radar, the tab asks you to select another iteration to compare with.

.. figure:: /rst/figures/user_manual/results_fingerprint.png
    :align: center
    :alt: Radar chart of the three Hugging Face models of the example

    *Other models*: the three summarization models of the example.

.. figure:: /rst/figures/user_manual/results_fingerprint_yolo.png
    :align: center
    :alt: Radar chart of three YOLO models of the Axelera board, with an Accuracy gap axis

    *Axelera YOLO models*: three object detection models, with the **Accuracy gap** axis.

.. _user_manual_results_stop:

Stopping a task
---------------

**Stop** makes the application stop following the running task: it waits for the iteration in progress and does not
follow any further iteration of the task (more output models, or the extra searches of an **Auto** optimization).
The framework nodes themselves are not interrupted.

.. _user_manual_results_errors:

Errors
------

When a node fails, an error dialog explains it:

.. list-table::
    :header-rows: 1
    :widths: 50 50

    * - Message
      - Meaning
    * - *No suitable model found for this task. Please refine the problem or constraints.*
      - The *ML Model Provider* node found no model for the problem.
    * - *Error in node ML Model Provider. Please check the logs for more details.*
      - The model search failed.
    * - *Error in node HW Resource. Please check the logs for more details.*
      - The model could not be evaluated on the hardware (for example, it could not be loaded, or the ONNX file of a
        U-Net model for the FPGA was not found).
    * - *Error in node Carbon Footprint. Please check the logs for more details.*
      - The carbon footprint could not be computed (no carbon intensity).

The details are in the output of the framework nodes, in the terminal where the framework was started.

.. figure:: /rst/figures/user_manual/results_error.png
    :align: center
    :alt: Error dialog saying that no suitable model was found, over an iteration with NO_MODEL

    The error shown when no model is found: the iteration of the problem shows ``NO_MODEL``.
