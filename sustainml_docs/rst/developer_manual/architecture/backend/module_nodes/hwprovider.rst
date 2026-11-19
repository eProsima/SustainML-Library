.. _hwprovider_node:

Hardware-Resource-Provider Node
===============================

.. raw:: html

   <style>
        .module_node:not(#hardware-resource-provider) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#hardware-resource-provider) {
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

The :ref:`hwprovider_node` evaluates the machine learning model proposed by the :ref:`mlmodelprovider_node` on the hardware chosen by the user.
It returns the latency of one inference of the model on that hardware, the power that the model draws on it and the memory footprint of the model,
which the :ref:`carbontracker_node` turns into energy and carbon footprint.
It is implemented in ``hw_resources_provider_node.py`` of the ``sustainml-wp2`` module.

The node does not choose the hardware: it evaluates the model on the hardware set in the :ref:`hardware_constraints_type` (the first element of ``hardware_required``).

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`hwprovider_node`:

+--------------------------------+---------------------------+-----------------------------+---------------------------+
| Input                          | From Node                 | Output                      | To Node                   |
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`mlmodel_type`             |:ref:`mlmodelprovider_node`|:ref:`hardware_resource_type`|:ref:`carbontracker_node`  |
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`hardware_constraints_type`|:ref:`hwconstraints_node`  |:ref:`hardware_resource_type`|:ref:`orchestrator`        |
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`apprequirements_type`     |:ref:`appreqs_node`        |:ref:`hardware_resource_type`|:ref:`mlmodelprovider_node`|
+--------------------------------+---------------------------+-----------------------------+---------------------------+

The :ref:`apprequirements_type` input is required to start the task, but the current implementation does not use its content.
The :ref:`mlmodelprovider_node` does not receive the output directly: the :ref:`orchestrator` publishes it again on the ``/sustainml/hw_resources/baseline`` topic when it starts a new task or iteration.

Supported Hardware
------------------

The node evaluates the model in one of three ways, depending on the model family chosen by the user (``model_family`` in the ``extra_data`` forwarded by the :ref:`hwconstraints_node`, ``Transformers`` by default) and on the hardware:

* ``CNNs`` on ``FPGA (xczu19eg-ffvb1517-2-i)``: DFKI's U-Net latency and energy predictor.
* ``CNNs`` on ``Axelera Edge 130p``: the YOLO models measured on the Axelera board.
* Any other combination: the UPMEM LLM simulator.

The frontend only offers the FPGA and the Axelera board for ``CNNs``, and the simulated hardware for ``Transformers``.

UPMEM LLM simulator
^^^^^^^^^^^^^^^^^^^

Hugging Face models are evaluated with a copy of the UPMEM LLM Framework vendored in ``sustainml-wp2/upmem_llm_framework``
(taken from eProsima's fork https://github.com/eProsima/upmem_llm_framework, commit ``f4ed113``, MIT License, Copyright (c) 2024 UPMEM).
Each simulated hardware is described in ``upmem_llm_framework/sim_architectures.yaml`` by its host-device and memory bandwidths, its compute throughput (TFLOPS) and the energy per transferred bit and per TFLOP.
The node:

#. Downloads the model from Hugging Face with ``transformers`` in ``float16``, using the Hugging Face token of the task (``hf_token``) when there is one.
   Models whose class matches one of the ``unsupported_models`` listed in the ``extra_data`` of the :ref:`mlmodel_type` are rejected.
#. Builds a dummy input with the first processor that loads for the model (tokenizer, image processor, feature extractor or multimodal processor):
   the text ``How to prepare coffee?``, a white 224x224 image, or 16 such frames for video models.
#. Maps every leaf layer of the model to the selected hardware and runs the model under the simulator's profiler
   (``generate()`` with greedy decoding and ``max_length=64`` when the model supports it, a forward pass otherwise).
#. Takes the simulated latency of the inference, and computes the power as the simulated energy of the whole inference (prompt and reply) divided by that latency.

The hardware that can be simulated is:
``A17Pro``, ``A6000``, ``A800``, ``CXL_PIM_BC``, ``CXL_PIM_nBC``, ``DGX100``, ``Dimensity9300``, ``H100``, ``H100_x2``, ``H100_x3``, ``H100_x4``, ``H100_x5``, ``H100_x8``, ``H20``, ``H200``,
``PIM_AI_1chip``, ``PIM_AI_4chip``, ``PIM_AI_1dimm``, ``PIM_AI_2dimm``, ``PIM_AI_4dimm``, ``PIM_AI_6dimm``, ``PIM_AI_8dimm``, ``PIM_AI_10dimm``, ``PIM_AI_12dimm``, ``PIM_AI_16dimm``, ``PIM_AI_24dimm``,
``SAM_LPDDR5PIM``, ``Snapdragon8gen3`` and ``V100``.

FPGA with DFKI's U-Net predictor
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For ``CNNs`` on ``FPGA (xczu19eg-ffvb1517-2-i)``, the FPGA device on which DFKI measured its models, the node uses the latency and energy predictor developed by DFKI,
vendored in ``sustainml-wp2/hw_provider_fpga/vendor/sustain_ml_predictor`` and wrapped by ``hw_provider_fpga/fpga_predictor_adapter.py``.
It works on U-Net models in ONNX format:

* The model evaluated is the ONNX file given in ``model_path`` of the :ref:`mlmodel_type`.
  If ``model_path`` is not an existing ``.onnx`` file, the node falls back to the first ``.onnx`` file found in the vendored ``unet_models`` folder.
* ``unet_models`` holds the 217 U-Nets that DFKI measured on the board (``xczu19eg-ffvb1517-2-i/measurements.jsonl``).
  When the model is one of them (same file name and same SHA-256 hash), the node returns DFKI's measurement.
* Otherwise, the latency, the dynamic energy and the board runtime energy are predicted with DFKI's predictor models
  (``predictor_model_latency.onnx``, ``predictor_model_energy_dynamic.onnx`` and ``predictor_model_energy_board_runtime.onnx``), run with ``onnxruntime``
  on the graph of the U-Net extracted from its ONNX file.
* The power of the model is its dynamic energy (the board's energy while running the model minus its idle energy) divided by the latency.
  The idle power of the board, about 18 to 21 W in DFKI's measurements, is left out because it does not depend on the model.
  A negative predicted dynamic energy is counted as 0.

The whole result of the predictor is attached to the ``extra_data`` of the output (see below).

Axelera board with YOLO models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

For ``CNNs`` on ``Axelera Edge 130p`` (a PCIe card with one Metis AIPU, INT8 only), the model is one of the YOLO models of the Axelera Voyager SDK model zoo that SustainML offers for this board.
They are listed in ``sustainml-wp2/hw_provider_axelera/yolo_models.json``: 36 models (YOLO11 and YOLO26) for five goals,
``object-detection``, ``oriented-object-detection``, ``instance-segmentation``, ``semantic-segmentation`` and ``keypoint-detection``.
The board's specifications, taken from Axelera's datasheet, are in ``hw_provider_axelera/axelera_devices.yaml``.

Nothing is loaded or run for these models.
For each one, ``yolo_models.json`` holds the accuracy published by Axelera (FP32, and INT8 after the quantization loss), the frames per second published by Axelera,
and the values measured on an Axelera Edge 130p board or computed from the model's ONNX file:

* ``latency_s``: time of one inference, with one input processed alone.
* ``full_load_fps``: throughput of the board with its four AIPU cores busy.
* ``power_w``: extra power of the card while it runs one input alone, measured at the wall with the share of the host CPU removed (the card's idle power is not included).
* ``weights_mb``, ``working_mb`` and ``memory_mb``: memory footprint computed from the ONNX file, as for any other ONNX model.

``hw_provider_axelera/import_measurements.py`` loads new measurements into ``yolo_models.json``.
The ``hw_provider_axelera`` package also provides the ranking of these models (``rank_models()``) that the :ref:`mlmodelprovider_node` uses to suggest one of them.

Output
------

The node fills the :ref:`hardware_resource_type` output as follows:

* ``hw_description``: the name of the hardware evaluated, or ``Error`` when the UPMEM simulator could not evaluate the model.
* ``latency``: latency of one inference, in **hours** (the :ref:`carbontracker_node` multiplies it by the power).
* ``power_consumption``: power that the model draws on the hardware, in W, without the idle power of the hardware.
* ``memory_footprint_of_ml_model``: memory footprint of the model, in MB, as its weights plus the working memory of one input
  (the peak of the tensors alive at the same time, for the largest input the model accepts), every floating-point value counted at 32 bits.
  It is computed by ``sustainml-wp2/model_memory.py`` from the ONNX file of the model when there is one, and otherwise from its Hugging Face configuration.
  For the YOLO models of the Axelera board it is the value stored in ``yolo_models.json``.
  It is ``0`` when it cannot be computed.
* ``max_hw_memory_footprint``: the ``max_memory_footprint`` of the :ref:`hardware_constraints_type`, in MB (``0`` means no limit).
* ``extra_data``: a JSON object that depends on the evaluation:

  * FPGA: the result of DFKI's predictor, with the keys ``device``, ``latency_s``, ``latency_h``, ``power_w``, ``energy_J``, ``energy_Wh``, ``energy_dynamic_Wh``,
    ``power_board_runtime_w``, ``energy_board_runtime_Wh`` and ``provenance`` (whether the values were measured or predicted, the predictor files used and the SHA-256 hash of the model).
  * Axelera board: ``{"axelera": {...}, "accuracy": {...}}``, with the latency, full-load FPS, power, energy per inference and memory of the model and where each value comes from,
    and the accuracy of the model as listed in ``yolo_models.json``.
  * UPMEM simulator: nothing on success.
  * Errors: ``{"error_code": "NO_MODEL", "error": "..."}`` when the :ref:`mlmodelprovider_node` returned no model (``NO_MODEL``),
    ``{"error_code": "UNKNOWN_MODEL", "error": "..."}`` when the model is not one of the YOLO models of the Axelera board,
    and ``{"error": "Failed to test model on hardware: ..."}`` when the UPMEM simulator could not evaluate the model.
    In all these cases the latency and power are ``0``.
    When DFKI's predictor fails, the latency and power are ``0`` too and the error is only printed.

Configuration Requests
----------------------

The :ref:`hwprovider_node` answers the following configuration requests.
The frontend sends them to the backend, and the Python orchestrator (``sustainml-wp5/orchestrator_node/orchestrator_node.py``) addresses to this node (``node_id`` ``3``, ``HW_PROVIDER``)
any request whose ``configuration`` string contains ``hardwares`` or ``yolo_models``.
See :ref:`node_config_service` for the request and response types.

``hardwares``
^^^^^^^^^^^^^

Returns the hardware that can be chosen in the problem definition, as a single string of names separated by ``,`` followed by a space:
the hardware simulated by the UPMEM LLM simulator (sorted by name), then the Axelera board and the FPGA.
The frontend splits this string to fill its hardware list.

.. code-block:: json

    {"hardwares": "A17Pro, A6000, A800, CXL_PIM_BC, CXL_PIM_nBC, DGX100, Dimensity9300, H100, H100_x2, H100_x3, H100_x4, H100_x5, H100_x8, H20, H200, PIM_AI_10dimm, PIM_AI_12dimm, PIM_AI_16dimm, PIM_AI_1chip, PIM_AI_1dimm, PIM_AI_24dimm, PIM_AI_2dimm, PIM_AI_4chip, PIM_AI_4dimm, PIM_AI_6dimm, PIM_AI_8dimm, SAM_LPDDR5PIM, Snapdragon8gen3, V100, Axelera Edge 130p, FPGA (xczu19eg-ffvb1517-2-i)"}

If the list cannot be built, ``success`` is ``false`` and ``err_code`` is ``1``.

``yolo_models``
^^^^^^^^^^^^^^^

Returns the YOLO models of the Axelera board, with their accuracy and the resources they use on the board.
The frontend shows them in its YOLO models screen and in the model fingerprint view.
Each element of the list has the following keys:

.. code-block:: json

    {"yolo_models": [
        {
            "name": "yolo11n-coco-onnx",
            "display_name": "YOLO11n",
            "architecture": "YOLO11",
            "size": "n",
            "goal": "object-detection",
            "metric": "mAP",
            "accuracy_fp32": 39.17,
            "accuracy_int8": 38.46,
            "latency_s": 0.006656,
            "power_w": 1.17,
            "energy_per_inference_j": 0.007787519999999999,
            "memory_mb": 28.73,
            "power_source": "measured"
        }
    ]}

``latency_s`` is in seconds, ``power_w`` in W, ``energy_per_inference_j`` (power times latency) in J and ``memory_mb`` in MB.
On failure, ``success`` is ``false``, ``err_code`` is ``1`` and the ``configuration`` is ``{"error": "<message>"}``.

Any other request is answered with ``success`` set to ``false``, ``err_code`` set to ``1`` and the ``configuration`` ``{"error": "Unsupported configuration request: <request>"}``.

Dependencies
------------

Besides ``sustainml_py``, the node uses:

* ``transformers``, ``torch`` and ``Pillow`` to load the Hugging Face models and build their inputs, and access to Hugging Face to download them.
* ``PyYAML`` to read the simulated hardware and the Axelera board specifications.
* ``onnx``, ``onnxruntime``, ``networkx`` and ``numpy`` for DFKI's predictor and to compute the memory footprint of ONNX models.
* The vendored UPMEM LLM Framework (``upmem_llm_framework``) and DFKI's predictor (``hw_provider_fpga/vendor/sustain_ml_predictor``), shipped with the module.

Node Template
-------------

Following is the Python API provided for the :ref:`hwprovider_node`.
The functionality of the node is implemented within ``task_callback()``, which receives the :ref:`mlmodel_type`, the :ref:`apprequirements_type` and the :ref:`hardware_constraints_type`, and fills the :ref:`hardware_resource_type` output.
The response to the configuration requests forwarded by the orchestrator is implemented within ``configuration_callback()``.
A new hardware evaluation can be added as another branch of ``task_callback()``, and its hardware name added to the reply to the ``hardwares`` request.

The template below is a reduced version of the actual ``sustainml-wp2/hw_resources_provider_node.py``: it keeps its structure and the way it reads and writes the data, while the three evaluation paths (UPMEM simulator, FPGA predictor and Axelera measurements) are replaced by a single ``evaluate()`` function to implement.

.. code-block:: python

    """SustainML HW Resources Provider Node Implementation."""

    from sustainml_py.nodes.HardwareResourcesNode import HardwareResourcesNode

    # Manage signaling
    import json
    import signal
    import threading
    import time

    # Hardware this node can evaluate (reply to the "hardwares" configuration request)
    HARDWARES = ["My_board"]

    # Whether to go on spinning or interrupt
    running = False

    # Evaluate a model on a hardware: latency (s), power (W) and memory (MB)
    def evaluate(model_name, model_path, hardware):
        raise NotImplementedError

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        HardwareResourcesNode.terminate()
        global running
        running = False

    # User Callback implementation
    # Inputs: ml_model, app_requirements, hw_constraints
    # Outputs: node_status, hw
    def task_callback(ml_model, app_requirements, hw_constraints, node_status, hw):

        hw_selected = hw_constraints.hardware_required()[0]
        hw.hw_description(hw_selected)
        hw.max_hw_memory_footprint(float(hw_constraints.max_memory_footprint()))

        try:
            latency_s, power_w, memory_mb = evaluate(ml_model.model(), ml_model.model_path(), hw_selected)
            hw.latency(latency_s / 3600.0)  # The Carbon Footprint node expects hours
            hw.power_consumption(power_w)
            hw.memory_footprint_of_ml_model(memory_mb)
        except Exception as e:
            hw.latency(0.0)
            hw.power_consumption(0.0)
            hw.extra_data(json.dumps({"error": str(e)}).encode("utf-8"))

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())

        if req.configuration() == "hardwares":
            res.success(True)
            res.err_code(0)  # 0: No error || 1: Error
            res.configuration(json.dumps(dict(hardwares=', '.join(HARDWARES))))
            return

        error_msg = f"Unsupported configuration request: {req.configuration()}"
        res.configuration(json.dumps({"error": error_msg}))
        res.success(False)
        res.err_code(1)  # 0: No error || 1: Error

    # Main workflow routine
    def run():
        node = HardwareResourcesNode(callback=task_callback, service_callback=configuration_callback)
        global running
        running = True
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
