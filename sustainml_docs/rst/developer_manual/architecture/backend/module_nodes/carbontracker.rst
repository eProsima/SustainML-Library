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

The :ref:`carbontracker_node` is responsible for estimating the carbon footprint of the machine learning model on the selected hardware.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`carbontracker_node`:

+--------------------------------+---------------------------+---------------------------+---------------------------+
| Input                          | From Node                 | Output                    | To Node                   |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`user_input_type`          |:ref:`orchestrator`        |:ref:`carbonfootprint_type`|:ref:`mlmodelprovider_node`|
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`mlmodel_type`             |:ref:`mlmodelprovider_node`|:ref:`carbonfootprint_type`|:ref:`orchestrator`        |
+--------------------------------+---------------------------+---------------------------+---------------------------+
|:ref:`hardware_resource_type`   |:ref:`hwprovider_node`     |                           |                           |
+--------------------------------+---------------------------+---------------------------+---------------------------+

Node Template
-------------

Following is the Python API provided for the :ref:`carbontracker_node`.
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
    """SustainML Carbon Footprint Node Implementation."""

    from sustainml_py.nodes.CarbonFootprintNode import CarbonFootprintNode

    from carbontracker.tracker import CarbonTracker
    from carbontracker import parser

    # Manage signaling
    import json
    import multiprocessing
    import os
    import signal
    import threading
    import time
    import torch
    import transformers

    # Whether to go on spinning or interrupt
    running = False
    GRID_CARBON_INTENSITY = float(os.getenv("SUSTAINML_GRID_CI", "174.05"))


    # CarbonTracker log parser helper
    def _parse_tracker_logs(log_dir):
        carbon_g = None
        energy_kwh = None
        ci_g_per_kwh = None

        try:
            logs = parser.parse_all_logs(log_dir=log_dir)
        except Exception as e:
            # print("[CT DEBUG] parse_all_logs error:", e)
            return carbon_g, energy_kwh, ci_g_per_kwh

        if not logs:
            # print("[CT DEBUG] no logs parsed")
            return carbon_g, energy_kwh, ci_g_per_kwh

        # Take the last entry (the one CarbonTracker just wrote)
        entry = logs[-1]
        actual = entry.get("actual") or {}
        pred   = entry.get("pred") or {}

        # Prefer ACTUAL, fall back to PRED if needed
        def get_first(keys, d):
            for k in keys:
                if k in d:
                    v = float(d[k])
                    if v > 0.0:
                        return v
            return None

        carbon_g   = get_first(("co2eq (g)",), actual) or get_first(("co2eq (g)",), pred)
        energy_kwh = get_first(("energy (kWh)",), actual) or get_first(("energy (kWh)",), pred)

        if energy_kwh is not None and energy_kwh > 0.0 and carbon_g is not None:
            ci_g_per_kwh = carbon_g / energy_kwh

        return carbon_g, energy_kwh, ci_g_per_kwh


    # Load generic ml model and generate its input
    def load_any_model(model_name, hf_token=None, unsupported_models=None, **kwargs):

        model = None

        try:
            config = transformers.AutoConfig.from_pretrained(model_name, trust_remote_code=True)
            print(f"Model configuration loaded: {config}")
            model_class = transformers.AutoModel._model_mapping.get(type(config), None)

            if unsupported_models is not None:
                for unsupported in unsupported_models:
                    if unsupported.lower() in model_class.__name__.lower():
                        raise ValueError(f"[WARNING] Models that use '{unsupported}' are not supported.")

        except Exception as e:
            raise Exception(f"[ERROR] Could not load model {model_name}: {e}")

        try:
            if model_class is None:
                model = transformers.AutoModel.from_config(config)

            else:
                model = model_class(config)
        except Exception as e:
            raise Exception(f"[ERROR] Could not load model {model_name}: {e}")

        if model is None:
            raise Exception(f"Model {model_name} is not currently supported")

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
                tokenizer = None

        if tokenizer is None:
            raise Exception(f"Error initializing tokenizer for model {model_name}: {e}")

        input = None
        try:
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

        except Exception as e:
            raise Exception(f"Error creating input for model {model_name}, tokenizer {tokenizer} : {e}")

        return model, tokenizer, input


    # Create tracker on different proccess
    def create_tracker(log_dir, epochs, queue, ml_model=None, unsupported_models=None):
        try:
            model, tokenizer, input = load_any_model(
                ml_model.model(),
                hf_token=None,
                unsupported_models=unsupported_models,
                low_cpu_mem_usage=True,
                torch_dtype=torch.float16
            )
            print(f"[CT] HF model loaded for tracker: {ml_model.model()}")

            # No-grad / eval for consistent forwards
            if hasattr(model, "eval"):
                model.eval()

            tracker = CarbonTracker(log_dir=log_dir, epochs=epochs)

            # Window of real work per epoch (seconds). Tunable via env.
            target_s = float(os.getenv("CT_TARGET_SECONDS", "1.0"))

            for epoch in range(epochs):
                tracker.epoch_start()
                start = time.time()
                iters = 0
                with torch.inference_mode():
                    while (time.time() - start) < target_s:
                        try:
                            model(**input)
                        except Exception:
                            # fallback for encoder-decoder models
                            if "decoder_input_ids" not in input and "input_ids" in input:
                                input["decoder_input_ids"] = input["input_ids"]
                            model(**input)
                        iters += 1
                elapsed = time.time() - start
                tracker.epoch_end()

            tracker.stop()
            time.sleep(0.3)  # allow flush

            # One parse + one result returned
            carbon_g, _, _ = _parse_tracker_logs(log_dir)
            carbon = float(carbon_g or 0.0)
            queue.put(carbon)
        except Exception as e:
            print("[CT ERROR] create_tracker:", e)
            queue.put(e)


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

        global GRID_CARBON_INTENSITY
        run_tag = f"{int(time.time())}_{os.getpid()}"
        log_directory = f"/tmp/logs/carbontracker/{run_tag}"
        os.makedirs(log_directory, exist_ok=True)

        output_extra_data = {}
        # adding number of output request to extra data
        extra_data_bytes = user_input.extra_data()
        extra_data_str = ''.join(chr(b) for b in extra_data_bytes)
        try:
            extra_data_dict = json.loads(extra_data_str)
        except json.JSONDecodeError:
            print("[WARN] In carbon node extra_data JSON is not valid.")
            extra_data_dict = {}

        if "num_outputs" in extra_data_dict and extra_data_dict["num_outputs"] != "":
            num_outputs = extra_data_dict["num_outputs"]
            model_restrains_list = [ml_model.model()]
            if "model_restrains" in extra_data_dict:
                model_restrains_list.extend(extra_data_dict["model_restrains"])

            output_extra_data["num_outputs"]     = num_outputs
            output_extra_data["model_restrains"] = model_restrains_list

        model_path = ml_model.model_path()
        is_onnx = isinstance(model_path, str) and model_path.endswith(".onnx")

        if is_onnx:
            raw_latency = float(hw.latency())            # h
            raw_power   = float(hw.power_consumption())  # W

            energy_kwh = (raw_power * raw_latency) / 1000.0
            carbon_g   = energy_kwh * GRID_CARBON_INTENSITY

            co2.carbon_footprint(carbon_g)
            co2.energy_consumption(energy_kwh)
            co2.carbon_intensity(GRID_CARBON_INTENSITY)

            output_extra_data["mode"] = "onnx_hw_only"
            # Make sure num_outputs exists (fallback)
            output_extra_data.setdefault("num_outputs", 1)
            output_extra_data.setdefault("model_restrains", [ml_model.model()])

            co2.extra_data(json.dumps(output_extra_data).encode("utf-8"))
            return

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

        #log_directory = "/tmp/logs/carbontracker"               # temp log dir for reading carbon data results

        # Define CarbonTracker with fallback for no available components
        try:
            queue = multiprocessing.Queue()
            ### proc = multiprocessing.Process(target=create_tracker, args=(log_directory, 1, queue, ml_model, unsupported_models))

            proc = multiprocessing.Process(target=create_tracker, args=(log_directory, 1, queue, ml_model, unsupported_models))
            proc.start()
            proc.join(timeout=60)
            if proc.is_alive():
                print("Child process did not finish within the timeout period. Terminating...")
                proc.terminate()
                proc.join()
                raise Exception("tracker child process did not finish within the timeout period. Terminating...")

            if proc.exitcode == 70:
                raise Exception("No hardware components available; failed to obtain carbon footprint value.")
            else:
                if not queue.empty():
                    result = queue.get()
                    if isinstance(result, Exception):
                        raise Exception("Error creating tracker: " + str(result))
                    else:
                        print("Tracker created successfully.")
                        carbon = float(result or 0.0)

                        # Prefer tracker energy if present, else fall back to HW-based estimate
                        tracker_energy_kwh = None
                        try:
                            _, ekwh, _ = _parse_tracker_logs(log_directory)
                            tracker_energy_kwh = ekwh
                        except Exception as e:
                            print("[CT DEBUG] could not re-parse tracker logs in task_callback:", e)

                        try:
                            latency_h = float(hw.latency())
                            power_w   = float(hw.power_consumption())   # W

                            # W * h = Wh → /1000 = kWh
                            energy_consump_hw_kwh = (power_w * latency_h) / 1000.0
                        except Exception as e:
                            print("[CT DEBUG] HW energy compute failed:", e)
                            energy_consump_hw_kwh = 0.0

                        # Choose energy source (per-epoch/second energy)
                        if tracker_energy_kwh is not None and tracker_energy_kwh > 0:
                            energy_consump = tracker_energy_kwh
                        else:
                            energy_consump = energy_consump_hw_kwh

                        # Convert epoch-based values to per-inference values (approximate)
                        try:
                            epoch_s = float(os.getenv("CT_TARGET_SECONDS", "1.0"))
                            latency_h = float(hw.latency())        # hours per inference
                            latency_s = latency_h * 3600.0         # convert to seconds

                            if epoch_s > 0.0 and latency_s > 0.0:
                                inf_per_epoch = epoch_s / latency_s
                                if inf_per_epoch > 0.0:
                                    carbon = carbon / inf_per_epoch
                                    energy_consump = energy_consump / inf_per_epoch
                                    # print(f"[CT DEBUG] per-inference metrics: "f"carbon_g={carbon:.9f}, energy_kwh={energy_consump:.9f}, "f"approx_inf_per_epoch={inf_per_epoch:.2f}")
                                else:
                                    print("[CT DEBUG] inf_per_epoch <= 0, skipping per-inference scaling")
                            else:
                                print("[CT DEBUG] epoch_s or latency_s <= 0, skipping per-inference scaling")
                        except Exception as e:
                            print("[CT DEBUG] per-inference scaling failed:", e)
                else:
                    raise Exception("No result obtained from the tracker process; failed to obtain carbon footprint value.")

            intensity = 0.0
            if energy_consump > 0:
                intensity = carbon/energy_consump
                GRID_CARBON_INTENSITY = intensity  # Update global for consistency

            # populate carbon footprint information
            co2.carbon_footprint(carbon)
            co2.energy_consumption(energy_consump)
            co2.carbon_intensity(intensity)
            co2.extra_data(json.dumps(output_extra_data).encode("utf-8"))

        except Exception as e:
            print(f"Error getting carbon footprint information: {e}")
            co2.carbon_footprint(0.0)
            co2.energy_consumption(0.0)
            co2.carbon_intensity(0.0)
            output_extra_data["error"] = f"Failed to obtain carbon footprint information: {e}"
            co2.extra_data(json.dumps(output_extra_data).encode("utf-8"))


    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        # Callback for configuration implementation here

        # Case not supported
        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())
        error_msg = f"Unsupported configuration request: {req.configuration()}"
        res.configuration(json.dumps({"error": error_msg}))
        res.success(False)
        res.err_code(1) # 0: No error || 1: Error
        print(error_msg)


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
        the main thread is blocked (spin()) so, tun
        user work flow in another thread """
        runner = threading.Thread(target=run)
        runner.start()

        while running:
            time.sleep(1)

        runner.join()
