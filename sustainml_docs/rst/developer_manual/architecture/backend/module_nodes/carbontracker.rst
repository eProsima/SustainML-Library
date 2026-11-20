.. _carbontracker_node:

Carbontracker Node
==================

.. raw:: html

   <style>
        .module_node:not(#carbontracker) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#carbontracker) {
            opacity: 0.6;
        }

        .module_node:hover:not(#module_node) {
            opacity: 1 !important;
        }

    </style>

.. raw:: html
   :file: ../../../../figures/svg_href_loader.html

.. raw:: html
   :file: ../../../../figures/sustainml_framework_arch.svg

The :ref:`carbontracker_node` estimates the energy and the carbon footprint of one inference of the machine learning model on the selected hardware,
using the grid carbon intensity of the country where the model will be deployed.
It is implemented in ``carbon_footprint_node.py`` of the ``sustainml-wp3`` module.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`carbontracker_node`:

+--------------------------------+---------------------------+---------------------------+---------------------------+
| Input                          | From Node                 | Output                    | To Node                   |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`user_input_type`          |:ref:`orchestrator`        |:ref:`carbonfootprint_type`|:ref:`orchestrator`        |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`mlmodel_type`             |:ref:`mlmodelprovider_node`|:ref:`carbonfootprint_type`|:ref:`mlmodelprovider_node`|
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`hardware_resource_type`   |:ref:`hwprovider_node`     |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+

The :ref:`mlmodelprovider_node` does not receive the output directly: the :ref:`orchestrator` publishes it again on the ``/sustainml/carbon_tracker/baseline`` topic when it starts a new task or iteration.

Energy and Carbon Footprint
---------------------------

The energy of one inference is computed in the same way for every model and hardware, from the :ref:`hardware_resource_type` given by the :ref:`hwprovider_node`:

.. code-block:: text

    energy (kWh)           = power_consumption (W) x latency (h) / 1000
    carbon footprint (gCO2e) = energy (kWh) x carbon intensity (gCO2/kWh)

The power is the power that the model draws on the selected hardware, without the idle power of the hardware:
measured on the Axelera board, measured by DFKI (or estimated with its predictor) on the FPGA, and from the UPMEM LLM simulator for Hugging Face models (see :ref:`hwprovider_node`).

The node fills the :ref:`carbonfootprint_type` output as follows:

* ``carbon_footprint``: carbon footprint of one inference, in gCO2e.
* ``energy_consumption``: energy of one inference, in kWh.
* ``carbon_intensity``: grid carbon intensity used, in gCO2/kWh.

The result is ``0`` in the following cases, with the reason in the ``extra_data`` of the output:

* The :ref:`mlmodelprovider_node` returned no model (``NO_MODEL``).
* The carbon intensity table cannot be read.
* The latency given by the :ref:`hwprovider_node` is ``0``, which means that it could not evaluate the model.
* The CarbonTracker fallback (see below) fails or measures no energy.

CarbonTracker fallback
^^^^^^^^^^^^^^^^^^^^^^

When a Hugging Face model has a latency but no power from the :ref:`hwprovider_node`, the node falls back to `CarbonTracker <https://github.com/lfwa/carbontracker>`_.
In a child process, it builds the model from its Hugging Face configuration and runs it repeatedly for ``CT_TARGET_SECONDS`` seconds (environment variable, 3 seconds by default)
while CarbonTracker measures the energy of the machine that runs the node, with a timeout of 75 seconds.
The energy of one inference is the measured energy divided by the number of inferences of the given latency that fit in the measured time.
CarbonTracker is made to use the same grid carbon intensity as the rest of the node instead of looking up its own.

This energy is that of the machine running the node, not of the selected hardware, and it includes the idle power of that machine and CarbonTracker's data center factor (PUE),
so it cannot be compared with the other results.
The ``mode`` key of the ``extra_data`` marks these results as ``tracker``.
CarbonTracker writes its logs to ``/tmp/logs/carbontracker/<timestamp>_<pid>``.

Carbon Intensity per Country
----------------------------

The grid carbon intensities come from ``sustainml-wp3/carbon_intensities.csv``, shipped with the node.
It holds the latest yearly carbon intensity of electricity (gCO2/kWh) of each country, from Our World in Data (`Carbon intensity of electricity <https://ourworldindata.org/grapher/carbon-intensity-electricity>`_, data from Ember),
indexed by the ISO 3166-1 alpha-2 code of the country.
It covers 185 countries (the UN member states and Palestine that have data, except Israel) and ends with a ``WORLD`` row with the world average.
``sustainml-wp3/update_carbon_intensities.py`` downloads the data again and regenerates the file.

The intensity is chosen as follows:

#. The country chosen by the user, read from ``geo_location_region`` of the :ref:`user_input_type` as an ISO alpha-2 code (the frontend sends the code of the country selected in the problem definition).
#. If no country is chosen, or it is not in the table, the country of the public IP address of the machine running the node, detected with ``geocoder`` (``geocoder.ip("me")``).
   The detected country is kept for the next tasks; a failed detection is retried in the next task.
#. If the country cannot be detected, or it is not in the table, the world average (``WORLD``).

The ``geo_location_continent`` field of the :ref:`user_input_type` is not used.

Output ``extra_data``
---------------------

The ``extra_data`` of the :ref:`carbonfootprint_type` output is a JSON object that the orchestrator and the frontend use to drive further iterations:

* ``num_outputs``: number of output models still requested by the user (from the ``extra_data`` of the :ref:`user_input_type`, ``1`` by default).
  While it is greater than ``1``, the orchestrator starts a new iteration to look for another model.
* ``model_restrains``: models already suggested, the current one first, so that the next iterations do not suggest them again.
* ``grid_ci_source``: the intensity used and how it was chosen, as ``<country> (<year>), <reason>``,
  where the reason is ``chosen in SustainML``, ``detected from the IP address`` or ``country unknown or not in the list``.
* ``mode``: how the result was obtained:
  ``axelera_measured`` (YOLO model on the Axelera board), ``onnx_hw_only`` (ONNX model, as the U-Nets on the FPGA), ``hw_simulated`` (Hugging Face model on the UPMEM LLM simulator),
  ``tracker`` (CarbonTracker fallback), ``no_model`` or ``error``.
* ``auto_reiterate``: whether the orchestrator must discard the model and look for another one (automatic carbon footprint optimization, see below).
* ``error``: the reason why the carbon footprint was not computed, when it was not.

When the automatic carbon footprint optimization is enabled (``optimize_carbon_footprint_auto`` of the :ref:`user_input_type`) and the user did not select a model,
``auto_reiterate`` is ``true`` when the model misses the desired carbon footprint (``desired_carbon_footprint``, if greater than ``0``),
exceeds the maximum memory footprint (``max_memory_footprint`` in the ``extra_data`` of the :ref:`user_input_type`, if greater than ``0``), or could not be evaluated.
It is ``false`` from the third model tried for the same output on (``auto_attempt``), so the search stops there.

Configuration Requests
----------------------

The :ref:`carbontracker_node` answers the ``carbon_intensities`` configuration request.
The frontend sends it to the backend, and the Python orchestrator (``sustainml-wp5/orchestrator_node/orchestrator_node.py``) addresses to this node (``node_id`` ``1``, ``CARBONTRACKER``)
any request whose ``configuration`` string contains ``carbon_intensities``.
See :ref:`node_config_service` for the request and response types.

``carbon_intensities``
^^^^^^^^^^^^^^^^^^^^^^

Returns the whole carbon intensity table, indexed by ISO alpha-2 code, including the ``WORLD`` average.
The frontend uses it in its YOLO models screen to turn the energy of the models into carbon footprint for the chosen country.
The ``year`` is a string and ``g_per_kwh`` is in gCO2/kWh:

.. code-block:: json

    {"carbon_intensities": {
        "DE": {"country": "Germany", "year": "2025", "g_per_kwh": 329.65},
        "ES": {"country": "Spain", "year": "2025", "g_per_kwh": 153.6},
        "WORLD": {"country": "World average", "year": "2025", "g_per_kwh": 458.29}
    }}

If the table cannot be read, ``success`` is ``false``, ``err_code`` is ``1`` and the ``configuration`` is ``{"error": "carbon_intensities.csv could not be read"}``.

Any other request is answered with ``success`` set to ``false``, ``err_code`` set to ``1`` and the ``configuration`` ``{"error": "Unsupported configuration request: <request>"}``.

Dependencies
------------

Besides ``sustainml_py``, the node uses:

* ``geocoder``, to detect the country from the public IP address (it needs Internet access).
* ``carbontracker``, ``torch`` and ``transformers``, for the CarbonTracker fallback.
  The node imports them when it starts, so they are needed even when the fallback is not used.

Node Template
-------------

Following is the Python API provided for the :ref:`carbontracker_node`.
The functionality of the node is implemented within ``task_callback()``, which receives the :ref:`mlmodel_type`, the :ref:`user_input_type` and the :ref:`hardware_resource_type`, and fills the :ref:`carbonfootprint_type` output.
The response to the configuration requests forwarded by the orchestrator is implemented within ``configuration_callback()``.

The template below is a shortened version of ``sustainml-wp3/carbon_footprint_node.py`` (`full file on GitHub <https://github.com/eProsima/sustainml-wp3/blob/main/carbon_footprint_node.py>`__).
It keeps the structure of the node and the way it reads and writes its data, with the carbon intensity of the chosen country read from ``carbon_intensities.csv``. It leaves out, as described above:

* the other sources of the carbon intensity (the country detected from the IP address, then the world average) and the ``grid_ci_source`` key of the ``extra_data``;
* the early exit for ``NO_MODEL`` and the error outputs;
* the ``num_outputs`` and ``model_restrains`` of the user input merged into the ``extra_data``, and the ``auto_reiterate`` flag of the **Auto** carbon footprint optimization;
* the CarbonTracker fallback, for a Hugging Face model without energy from the HW Resources node;
* the ``carbon_intensities`` request, which returns the intensity of every country for the *YOLO models* screen.

.. code-block:: python

    """SustainML Carbon Footprint Node Implementation."""

    from sustainml_py.nodes.CarbonFootprintNode import CarbonFootprintNode

    # Manage signaling
    import csv
    import json
    import os
    import signal
    import threading
    import time

    # Whether to go on spinning or interrupt
    running = False

    # Grid carbon intensity (gCO2/kWh) of a country, by ISO alpha-2 code ("WORLD" for the average)
    def country_intensity(country_code):
        path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "carbon_intensities.csv")
        with open(path, newline="") as f:
            for row in csv.DictReader(f):
                if row["alpha-2"].upper() == (country_code or "").strip().upper():
                    return float(row["Carbon intensity of electricity (gCO2/kWh)"])
        return None

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        CarbonFootprintNode.terminate()
        global running
        running = False

    # User Callback implementation
    # Inputs: ml_model, user_input, hw
    # Outputs: node_status, co2
    def task_callback(ml_model, user_input, hw, node_status, co2):

        output_extra_data = {"num_outputs": 1, "model_restrains": [ml_model.model()]}

        latency_h = float(hw.latency())            # h
        power_w = float(hw.power_consumption())    # W
        energy_kwh = (power_w * latency_h) / 1000.0

        grid_ci = country_intensity(user_input.geo_location_region()) or country_intensity("WORLD") or 0.0

        co2.energy_consumption(energy_kwh)              # kWh per inference
        co2.carbon_intensity(grid_ci)                   # gCO2/kWh
        co2.carbon_footprint(energy_kwh * grid_ci)      # gCO2e
        output_extra_data["mode"] = "hw_simulated"
        co2.extra_data(json.dumps(output_extra_data).encode("utf-8"))

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())

        error_msg = f"Unsupported configuration request: {req.configuration()}"
        res.configuration(json.dumps({"error": error_msg}))
        res.success(False)
        res.err_code(1) # 0: No error || 1: Error

    # Main workflow routine
    def run():
        global running
        running = True
        node = CarbonFootprintNode(callback=task_callback, service_callback=configuration_callback)
        node.spin()

    # Call main in program execution
    if __name__ == '__main__':
        signal.signal(signal.SIGINT, signal_handler)

        """Python does not process signals async if
        the main thread is blocked (spin()) so, run
        user work flow in another thread """
        runner = threading.Thread(target=run)
        runner.start()

        while running:
            time.sleep(1)

        runner.join()
