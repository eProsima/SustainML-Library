.. _user_manual_models:

Exploring models
================

Besides the problem evaluation, the application has screens to explore three families of models:
pre-trained Hugging Face models, the YOLO models of the Axelera board, and the U-Net models of the FPGA.
They are opened from the header of the *Problem definition* screen.

.. _user_manual_models_hf:

Hugging Face models
-------------------

The **HF models** button of the *Problem definition* screen opens a menu:

* **Search**: searches Hugging Face models for the problem description and opens the *Hugging Face models* screen.
  It uses the **Problem definition**, or the **Problem short description** when the definition is empty.
  Disabled for CNNs (Hugging Face models are Transformers) and when both descriptions are empty.
* **Results**: opens the saved searches. Disabled until a search has returned models.
* **Comparisons**: opens the saved comparisons. Disabled until a comparison has been made.

.. _user_manual_models_hf_search:

Searching models
^^^^^^^^^^^^^^^^

The *Hugging Face models* screen lists the *Hugging Face suggestions* for the query, shown below the card title.
**Searching Hugging Face...** is shown while the search runs.
Up to ten models are listed, best first, each with its **score** (how close the model is to the description).
If the search fails or finds nothing, the screen shows **No suggestions yet**.

* Hover a model to see a tooltip with its details (task, library, license, languages, datasets...) and its
  configuration (architecture, hidden size, layers, heads...).
* Click a model to open its page on the Hugging Face website.
* Click **Analyze** to evaluate that model for the problem (see :ref:`user_manual_models_hf_analyze`).
* Check the box of up to five models to compare them (see :ref:`user_manual_models_hf_compare`).

The header has the **Compare**, **Results** and **Comparisons** buttons, and shows how many models are selected
(for example *2 / 5 selected*).

Every search that returns models is kept as a saved search.

.. _user_manual_models_hf_analyze:

Evaluating a model
^^^^^^^^^^^^^^^^^^

**Analyze** submits a task that evaluates the chosen Hugging Face model, as if it had been selected in the
*Problem definition* screen: it is evaluated once, with no search to optimize, on the hardware and location set in the
*Problem definition* form. Its goal is the task of the model on Hugging Face.
The *Results* screen opens with the new problem.
Its iteration info view shows only the problem description the model was found for.

.. _user_manual_models_hf_compare:

Comparing models
^^^^^^^^^^^^^^^^

#. Check between two and five models on the *Hugging Face models* screen.
   Once five are checked, the other boxes are disabled.
#. Click **Compare**. The *Model comparison* screen opens and shows **Comparing N models...** while the comparison is
   prepared (the models' information is read from Hugging Face and summarized by the ``llama3`` model).
#. The comparison is added to the **Saved comparisons** list. Click it to open it.

An open comparison has:

* a **Comparison table** with the **MODEL**, **LICENSE**, **MODEL FAMILY**, **ARCHITECTURE** and **LANGUAGES** of each
  model;
* for each model, its **Purpose**, what it is **Best for**, and what to **Watch out for**;
* a **Comparison summary**.

If the comparison cannot be made, the screen shows **Comparison failed**
(with the reason, when there are no saved comparisons yet).

Saved searches and comparisons
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The *Search results* screen (**Results**) lists the saved searches, named after their query, with the number of
models and the time of the search. Click a search to show its models, with the same **score**, link and
**Analyze** button as in a new search; click it again to hide them.

The *Model comparison* screen (**Comparisons**) lists the saved comparisons, named after their models.
Click one to open it; click it again to close it. Up to ten comparisons are kept: the oldest is dropped when a new one
is added.

In both lists, every entry has two icons:

* **Rename**: type a new name and press Enter (or Escape to cancel).
* **Delete**: asks for confirmation, then deletes the entry.

Saved searches and comparisons live only while the application is open.
Keep them with the **Save** button of each screen, which saves only the searches or only the comparisons into a named
file, and add them back later with **Load**. See :ref:`user_manual_node_status_save_load`.

.. _user_manual_models_yolo:

YOLO models (Axelera)
---------------------

The **YOLO models** button of the *Problem definition* screen opens the *YOLO models* screen.
It compares the YOLO models measured on the Axelera Edge 130p board: how much CO2 each one emits per accuracy point,
and where extra CO2 stops buying accuracy.
The CO2 of a model is its energy per inference (measured latency times measured power) times the carbon intensity of
the electricity grid of the chosen country.

While the *HW Resources* and *Carbon Footprint* nodes are not ready, the screen shows **Waiting for the YOLO models**.

CO2 per accuracy point
^^^^^^^^^^^^^^^^^^^^^^

The first card is a matrix of the models:

* one row per task (object detection, oriented object detection, instance segmentation, semantic segmentation and
  keypoint (pose) detection) and architecture (YOLO11 and YOLO26);
* one column per model size: **n**, **s**, **m**, **l** and **x**.

Each cell shows the CO2 per accuracy point (``g/pt``), the CO2 per million inferences and the accuracy of the model
after int8 quantization, and is coloured from light (less CO2 per accuracy point, better) to dark.
A ``×N`` marker shows where the CO2 is multiplied by 1.8 or more from the previous size.
Hover a cell to see the details of the model, including its accuracy before quantization (fp32) and the change from the
previous size.

The **Country** list (it can be filtered by typing) sets the carbon intensity. It starts with the country chosen in the
*Problem definition* form; when none is chosen there, it keeps the last country used on this screen
(the world average at first).

Click the cells to select models for the curve below; **Select all** selects every model and **Clear** unselects them.

Where extra CO2 stops buying accuracy
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The second card needs at least ten selected models.
It plots each selected model against its own task: the CO2 per inference as a multiple of the smallest model of the
same task (log scale), against the share of the best accuracy of that task.
The curve is split in zones by the accuracy points gained per doubling of CO2:

* **Steep**: 10 or more points; accuracy climbs fast.
* **Flattening**: between 3 and 10 points.
* **Saturated**: under 3 points; little gain.

A summary under the chart describes the zones found for the selected models.
The zones need neighbouring sizes (n and s, s and m, m and l, or l and x) of the same task and architecture.

To evaluate a YOLO model for a problem, choose **CNNs** and **Axelera Edge 130p** in the *Problem definition* form
(see :ref:`user_manual_problem_definition_modes`).

.. _user_manual_models_unet:

U-Net models (FPGA)
-------------------

The **U-Net models** button of the *Problem definition* screen opens the *U-Net models* screen: the CNN
architectures available for the FPGA. Each row shows the model name and a description: input size and channels, depth,
initial feature channels, kernel sizes, MFLOPs and millions of parameters.
**No U-Net models yet** is shown until the framework sends the list.

Click a model to open its graph on the *U-Net graphs* screen.
The **U-Net graphs** button of the header (with the number of open models) goes to that screen too.

U-Net graphs
^^^^^^^^^^^^

Every opened model has a card at the top of the *U-Net graphs* screen. Click a card to show its model, and its cross to
close it. The opened cards are kept while the application is open.

The graph of a model has:

* its input, depth, initial channels and kernels;
* the **Architecture** diagram: each box is a feature map, its height following the spatial resolution and its width the
  number of channels. Hover a box for its shape, memory and parameters;
* summary tiles: parameters, compute (MACs), receptive field, largest feature map and layers;
* **Where the compute goes**: the share of the operations per stage;
* **Compared with the original U-Net** (Ronneberger et al., 2015);
* **What this model is for, and what its settings mean**;
* **All layers**: the full layer table, which can be expanded.

**Open in browser** writes the same visualization as a web page and opens it in the system browser, for example to
share or print it. The page is written to a ``sustainml`` folder in the temporary directory of the system.

To evaluate a U-Net model, choose **CNNs** in the *Problem definition* form (the hardware is then the FPGA),
select the model in **Model selection** and click **Submit** (see :ref:`user_manual_problem_definition_modes`).
