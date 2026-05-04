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

The :ref:`hwprovider_node` selects the hardware resource that optimally fits the proposed machine learning model.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`hwprovider_node`:

+--------------------------------+---------------------------+-----------------------------+---------------------------+
| Input                          | From Node                 | Output                      | To Node                   |
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`apprequirements_type`     |:ref:`appreqs_node`        |:ref:`hardware_resource_type`|:ref:`mlmodelprovider_node`|
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`mlmodel_type`             |:ref:`mlmodelprovider_node`|:ref:`hardware_resource_type`|:ref:`orchestrator`        |
+--------------------------------+---------------------------+-----------------------------+---------------------------+
|:ref:`hardware_constraints_type`|:ref:`hwconstraints_node`  |:ref:`hardware_resource_type`|:ref:`carbontracker_node`  |
+--------------------------------+---------------------------+-----------------------------+---------------------------+

Node Template
-------------

Following is the Python API provided for the :ref:`hwprovider_node`.
User is meant to implement the funcionality of the node within the ``test:callback()``.
And inside ``configuration_callback()`` implement the response to the configuration request from the orchestrator.

.. code-block:: python

    # Copyright 2023 SustainML Consortium
    #
    # Licensed under the Apache License, Version 2.0 (the "License");
    # you may not use this file except in compliance with the License.
    # You may obtain a copy of the License at
    #
    #     http://www.apache.org/licenses/LICENSE-2.0
    #
    # Unless required by applicable law or agreed to in writing, software
    # distributed under the License is distributed on an "AS IS" BASIS,
    # WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    # See the License for the specific language governing permissions and
    # limitations under the License.
    """SustainML HW Resources Provider Node Implementation."""

    from sustainml_py.nodes.HardwareResourcesNode import HardwareResourcesNode
    from rptu_framework import integration as rptu_integration
    from transformers import (AutoConfig, AutoModel, AutoModelForCausalLM, AutoModelForSeq2SeqLM)

    import onnx
    import sys, os
    HERE = os.path.dirname(__file__)

    if HERE not in sys.path:
        sys.path.insert(0, HERE)

    import hw_provider_fpga
    from hw_provider_fpga import predict_latency_energy

    # Managing UPMEMEM LLM
    import upmem_llm_framework as upmem_layers
    import transformers
    import signal
    import threading
    import time
    import json
    import torch
    import yaml

    # Whether to go on spinning or interrupt
    running = False


    # Load generic ml model and generate its input
    def load_any_model(model_name, hf_token=None, unsupported_models=None, **kwargs):

        model = None

        # Skip hardware test if there is no model
        if model_name.upper() == "NO_MODEL":
            print("[INFO] Skipping HW evaluation: no model selected for this task.")
            return "NO_MODEL", None, None

        try:
            config = AutoConfig.from_pretrained(model_name, trust_remote_code=True, token=hf_token)
            # print(f"Model configuration loaded: {config}")
            is_seq2seq = getattr(config, "is_encoder_decoder", False)

            if is_seq2seq:
                model = AutoModelForSeq2SeqLM.from_pretrained(
                    model_name, trust_remote_code=True, token=hf_token, **kwargs
                )
            else:
                try:
                    model = AutoModelForCausalLM.from_pretrained(
                        model_name, trust_remote_code=True, token=hf_token, **kwargs
                    )
                except Exception:
                    model = AutoModel.from_pretrained(
                        model_name, trust_remote_code=True, token=hf_token, **kwargs
                    )

            # Guard unsupported list AFTER we have the actual class
            if unsupported_models is not None:
                cls_name = type(model).__name__.lower()
                for unsupported in unsupported_models:
                    if unsupported.lower() in cls_name:
                        raise ValueError(f"[WARNING] Models that use '{unsupported}' are not supported.")

        except Exception as e:
            raise Exception(f"[ERROR_LOAD] Could not load model {model_name}: {e}")

        if model is None:
            raise Exception(f"Model {model_name} is not currently supported")

        tokenizer = None
        available_token_classes = [
            ("Token", transformers.AutoTokenizer, {}),
            ("Image", transformers.AutoImageProcessor, {"use_fast": True}),
            ("FeatureExtractor", transformers.AutoFeatureExtractor, {}),
            ("Processor", transformers.AutoProcessor, {})
        ]

        for label, token_class, extra_args in available_token_classes:
            try:
                tokenizer = token_class.from_pretrained(
                    model_name,
                    token=hf_token,
                    trust_remote_code=True,
                    **{**extra_args, **kwargs}
                )
                break
            except Exception as e:
                print(f"[WARN] Could not load token as {label}: {e}")

        if tokenizer is None:
            raise Exception(f"Error initializing tokenizer for model {model_name}: {e}")

        input = None
        try:
            print(f"Try input created as a {label}")
            # Text
            if label == "Token":
                if tokenizer.eos_token is None:
                    tokenizer.eos_token = "<|endoftext|>"
                if tokenizer.pad_token is None:
                    tokenizer.pad_token = tokenizer.eos_token
                text = "How to prepare coffee?"
                input = tokenizer(
                    text,
                    return_tensors="pt",
                    padding=True,
                    truncation=True
                )

            # Image or Video
            elif label == "Image" or label == "FeatureExtractor" or "image" in tokenizer.__class__.__name__.lower():
                from PIL import Image
                import numpy as np

                # Check for video case based on tokenizer class name containing "video"
                if "video" in tokenizer.__class__.__name__.lower():
                    # Video case: create a list of 16 frames (all white images)
                    arr = np.ones((224, 224, 3), dtype=np.uint8) * 255
                    img = Image.fromarray(arr)
                    video_frames = [img for _ in range(16)]
                    input = tokenizer(
                        images=video_frames,
                        return_tensors="pt",
                    )
                else:
                    # Image case: create a single white image
                    arr = np.ones((224, 224, 3), dtype=np.uint8) * 255
                    img = Image.fromarray(arr)
                    input = tokenizer(
                        images=img,
                        return_tensors="pt",
                    )
                input = {k: v.to(torch.float16) if v.dtype == torch.float32 else v for k, v in input.items()}

            # Multimodal
            elif label == "Processor":
                from PIL import Image
                import numpy as np
                # Create a dummy white image
                arr = np.ones((224, 224, 3), dtype=np.uint8) * 255
                img = Image.fromarray(arr)
                text = "How to prepare coffee?"
                # Combine text and image to create input for the processor
                input = tokenizer(text=text, images=img, return_tensors="pt")

            print(f"[OK] Input created correctly as a {label}")

        except Exception as e:
            raise Exception(f"Error creating input for model {model_name}, tokenizer {tokenizer} : {e}")

        return model, tokenizer, input


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

        # Variable to store RPTU default model
        rptu_model = os.path.dirname(__file__)+'/rptu_framework/model.onnx'

        latency = 0.0
        power_consumption = 0.0

        global hf_token

        upmem_layers.initialize_profiling_options(simulation=True)
        upmem_layers.profiler_init()

        hw_selected = hw_constraints.hardware_required()[0]

        model_path = ml_model.model_path()
        if isinstance(model_path, (list, tuple)):
            try:
                model_path = ''.join(chr(b) for b in model_path)
            except Exception:
                model_path = ""

        model_family = "Transformers"
        # Try to read model_family from hw_constraints.extra_data
        try:
            b = hw_constraints.extra_data()
            if b:
                d = json.loads(bytes(b).decode('utf-8'))
                model_family = d.get("model_family", model_family)
        except Exception as e:
            print(f"[WARN] extra_data parse error: {e}")

        # Optional fallback: if still default, try ml_model.extra_data
        if model_family == "Transformers":
            try:
                b2 = ml_model.extra_data()
                if b2:
                    d2 = json.loads(bytes(b2).decode('utf-8'))
                    model_family = d2.get("model_family", model_family)
            except Exception:
                pass

        print(f"[INFO] model_family selected by user: {model_family}")

        mf = (model_family or '').strip().lower()
        is_cnn = mf.lower() == 'cnns'

        # Use RPTU hw predictor for their devices
        if hw_selected in ["Zynq UltraScale+ ZCU102", "Zynq UltraScale+ ZCU104", "Ultra96-V2", "TySOM-3A-ZU19EG"]:
            print("Using ONNX model path")
            try:
                # Use RPTU
                results = rptu_integration.onnx_ml_resource_estimation(rptu_model, hw_selected) # TODO: hw_selected should affect predictor
                print(f"RPTU latency results: {results['Latency']}")
                print(f"RPTU power consumption results: {results['Run_power']}")
                latency = results['Latency']
                power_consumption = results['Run_power']

            except Exception as e:
                print(f"[ERROR] Failed to load/run ONNX at '{model_path}': {e}.")

        # Use DFKI predictor for xczu19eg target (only when user selected CNNs)
        elif is_cnn and hw_selected == "FPGA (xczu19eg-ffvb1517-2-i)":
            try:
                # 1) Pick an ONNX to use
                candidates = []
                if isinstance(model_path, str) and model_path.endswith(".onnx") and os.path.isfile(model_path):
                    candidates.append(model_path)

                # Vendored test models (if any)
                vendored_dir = os.path.join(os.path.dirname(hw_provider_fpga.__file__), "vendor", "sustain_ml_predictor", "unet_models")
                vendored_dir = os.path.abspath(vendored_dir)
                if os.path.isdir(vendored_dir):
                    for f in os.listdir(vendored_dir):
                        if f.endswith(".onnx"):
                            candidates.append(os.path.join(vendored_dir, f))

                # Fallback to the existing rptu sample ONNX if available
                if os.path.isfile(rptu_model):
                    candidates.append(rptu_model)

                if not candidates:
                    raise FileNotFoundError("No ONNX file available for FPGA prediction. "
                                            "Provide an ONNX CNN (U-Net) in the model step or vendor one into unet_models/.")

                onnx_to_use = candidates[0]

                # 2) Quick CNN check: must contain Conv or ConvTranspose
                m = onnx.load(onnx_to_use)
                has_conv = any(n.op_type in ("Conv", "ConvTranspose") for n in m.graph.node)
                if not has_conv:
                    raise ValueError(f"Selected ONNX '{onnx_to_use}' is not a CNN (no Conv/ConvTranspose). "
                                    "DFKI predictor is for U-Net-like CNNs.")

                # 3) Run predictor
                pred = predict_latency_energy(onnx_to_use)
                latency = float(pred.get("latency_h", 0.0))
                power_consumption = float(pred.get("power_w", 0.0))

                # Attach full payload for backend/UI
                try:
                    hw.extra_data(json.dumps(pred).encode("utf-8"))
                except Exception:
                    pass

                print(f"[DFKI FPGA] using {onnx_to_use}")

            except Exception as e:
                print(f"[ERROR][DFKI FPGA] {e}")
                latency = 0.0
                power_consumption = 0.0

        # Use UPMEM hw simulator
        else:
            try:
                print("Using Hugging Face model")
                hf_token = None
                extra_data_bytes = hw_constraints.extra_data()
                if extra_data_bytes:
                    extra_data_str = ''.join(chr(b) for b in extra_data_bytes)
                    if extra_data_str:
                        try:
                            extra_data_dict = json.loads(extra_data_str)
                        except json.JSONDecodeError:
                            print("[WARN] In hw_provider node extra_data JSON is not valid.")
                            extra_data_dict = {}
                    if "hf_token" in extra_data_dict:
                        hf_token = extra_data_dict["hf_token"]
                if hf_token is None:
                    raise Exception("HF token was not provided. Please set the HF_TOKEN environment variable.")

                unsupported_models = None
                extra_data_bytes = ml_model.extra_data()
                if extra_data_bytes:
                    extra_data_str = ''.join(chr(b) for b in extra_data_bytes)
                    if extra_data_str:
                        try:
                            extra_data_dict = json.loads(extra_data_str)
                        except json.JSONDecodeError:
                            print("[WARN] In ml_model node extra_data JSON is not valid.")
                            extra_data_dict = {}
                        if "unsupported_models" in extra_data_dict:
                            unsupported_models = extra_data_dict["unsupported_models"]

                model, tokenizer, input = load_any_model(
                    ml_model.model(),
                    hf_token=hf_token,
                    unsupported_models=unsupported_models,
                    low_cpu_mem_usage=True,
                    torch_dtype=torch.float16
                )
                print("Model, Tokenizer and Input loaded successfully")
                print(f"Model: {model}")
                print(f"Tokenizer: {tokenizer}")
                print(f"Input: {input}")

                layer_mapping = {}
                for name, module in model.named_modules():
                    if not name:
                        continue
                    if len(list(module.children())) == 0:
                        layer_mapping[name.split('.')[-1]] = hw_selected

                raw_last = list(layer_mapping.keys())[-1]
                last_layer = raw_last.split('.')[-1]
                print(f"Last layer for profiling: {last_layer}")  # debug

                model.eval()  # Put model in evaluation / inference mode

                # Noinspection PyUnresolvedReferences
                upmem_layers.profiler_start(
                    layer_mapping=layer_mapping,
                    last_layer=last_layer,
                )
                # In case we want to time the original execution (comment out profiler_start)
                # start = time.time_ns()

                # Safe, minimal workload for HW profiling
                try:
                    # Attempt generation if supported
                    if hasattr(model, "generate"):
                        model.generate(**input, do_sample=False, max_length=64)
                    else:
                        model(**input)
                except Exception as e_gen:
                    print(f"Error generating output with generate(): {e_gen}. Trying forward instead.")

                    # Convert input to mutable dict
                    input_dict = dict(input)

                    # Add decoder inputs for seq2seq models
                    if "decoder_input_ids" not in input_dict and "input_ids" in input_dict:
                        input_dict["decoder_input_ids"] = input_dict["input_ids"]

                    try:
                        model(**input_dict)
                    except Exception as e_model:
                        raise Exception(f"[ERROR_MODEL_FORWARD] {e_model}")

                # Noinspection PyUnresolvedReferences
                upmem_layers.profiler_end()

                latency = upmem_layers.profiler_get_latency()
                power_consumption = upmem_layers.profiler_get_power_consumption()

            except Exception as e:
                import traceback
                traceback.print_exc()
                print(f"Error testing model on hardware: {e}")
                print("Please provide different model")
                hw.hw_description("Error")
                hw.power_consumption(0.0)
                hw.latency(0.0)
                error_message = "Failed to test model on hardware: " + str(e)
                error_info = {"error": error_message}
                encoded_error = json.dumps(error_info).encode("utf-8")
                hw.extra_data(encoded_error)
                return

        hw.hw_description(hw_selected)
        hw.power_consumption(power_consumption)
        hw.latency(latency)
        print(f"Power Consumption: {power_consumption:.8f} W")
        print(f"Latency: {latency}")


    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        # Callback for configuration implementation here
        if req.configuration() == "hardwares":
            try:
                res.node_id(req.node_id())
                res.transaction_id(req.transaction_id())

                # Retrieve Hardwares from sim_architectures.yaml
                with open(os.path.dirname(__file__)+'/upmem_llm_framework/sim_architectures.yaml', 'r') as file:
                    upmem_devices = yaml.safe_load(file)
                with open(os.path.dirname(__file__)+'/rptu_framework/rptu_devices.yaml', 'r') as file:
                    rptu_devices = yaml.safe_load(file)

                # Extract the hardware names
                hardware_names = list(upmem_devices.keys()) + list(rptu_devices.keys())
                hardware_names.append("FPGA (xczu19eg-ffvb1517-2-i)")  # Expose the DFKI FPGA predictor device

                if not hardware_names:
                    res.success(False)
                    res.err_code(1)  # 0: No error || 1: Error
                else:
                    res.success(True)
                    res.err_code(0)  # 0: No error || 1: Error
                sorted_architectures = sorted(list(upmem_devices.keys()))
                sorted_rptu_devices = sorted(list(rptu_devices.keys()))
                sorted_hardware_names = ', '.join(sorted_architectures + sorted_rptu_devices + ["FPGA (xczu19eg-ffvb1517-2-i)"])
                print(f"Available Hardwares: {sorted_hardware_names}")
                res.configuration(json.dumps(dict(hardwares=sorted_hardware_names)))

            except Exception as e:
                print(f"Error getting types of hardwares from request: {e}")
                res.success(False)
                res.err_code(1)  # 0: No error || 1: Error

        else:
            res.node_id(req.node_id())
            res.transaction_id(req.transaction_id())
            error_msg = f"Unsupported configuration request: {req.configuration()}"
            res.configuration(json.dumps({"error": error_msg}))
            res.success(False)
            res.err_code(1)  # 0: No error || 1: Error
            print(error_msg)


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
        the main thread is blocked (spin()) so, tun
        user work flow in another thread """
        runner = threading.Thread(target=run)
        runner.start()

        while running:
            time.sleep(1)

        runner.join()
