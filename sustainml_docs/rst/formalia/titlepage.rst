.. image:: /rst/figures/sustainml-title-green.png
  :class: only-light sustainml-title-logo
  :align: center
  :alt: SustainML
  :target: http://www.sustainml.eu/

.. image:: /rst/figures/sustainml-title-white.png
  :class: only-dark sustainml-title-logo
  :align: center
  :alt: SustainML
  :target: http://www.sustainml.eu/

.. rst-class:: sustainml-title-page

SustainML
*********

Application Aware, Life-Cycle Oriented Model-Hardware Co-Design Framework for Sustainable, Energy Efficient ML
Systems project.

*SustainML* is a framework and an associated toolkit that helps AI developers take energy efficiency and carbon
footprint into account when they design a machine learning (ML) solution.
Starting from the description of the problem given by the user, the *SustainML Framework*:

* Works out the ML goal of the problem (for instance, image classification or text generation), or takes the one the
  user chooses.
* Proposes an ML model for that goal, taken from its knowledge graph of models, or evaluates the model the user
  chooses (for instance, one found in a Hugging Face search).
* Estimates the latency, power consumption and memory footprint of that model on the hardware selected by the user:
  the architectures of a GPU and processing-in-memory simulator, an Axelera AI board (running YOLO models) or an FPGA
  (running U-Net models).
* Estimates the energy consumption and the carbon footprint of the model with the carbon intensity of the electricity
  grid of the country where it is going to run.

The user can ask for several models at once, compare them, and reiterate a problem so that the framework proposes
other models, either manually or automatically until a desired carbon footprint and memory footprint are met.
The framework can also search Hugging Face models, compare them, and describe a dataset given by the user.

The framework follows a human-centric, interactive and transparent design: every intermediate result (ML goal,
model, hardware figures and carbon footprint) is shown to the user, who makes the decisions, instead of the common
black-box and fully automated optimization approaches of AutoML.

.. raw:: html

   <hr>

.. image:: /rst/figures/eu_flag.png
  :height: 45px
  :align: left

This project has received funding from the European Union's Horizon Europe research and innovation programme
(HORIZON-CL4-2021-HUMAN-01) under grant agreement No 101070408.
