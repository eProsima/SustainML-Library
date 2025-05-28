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

    # Managing UPMEMEM LLM
    import upmem_llm_framework as upmem_layers
    import transformers
    import onnxruntime
    import os
    import signal
    import threading
    import time
    import json
    import torch
    import yaml

    # Whether to go on spinning or interrupt
    running = False

    # ONNX Model-based testing class
    class ONNXModel(torch.nn.Module):
        def __init__(self, onnx_model_path):
            super(ONNXModel, self).__init__()
            self.onnx_session = onnxruntime.InferenceSession(onnx_model_path)

        def forward(self, inputs: torch.Tensor):
            input_name = self.onnx_session.get_inputs()[0].name
            np_input = inputs.detach().cpu().numpy()
            outputs = self.onnx_session.run(None, {input_name: np_input})

            if len(outputs) == 1:
                return torch.from_numpy(outputs[0])

            elif len(outputs) == 2:
                bounding_boxes = torch.from_numpy(outputs[0])
                class_scores = torch.from_numpy(outputs[1])
                return bounding_boxes, class_scores

            else:
                return tuple(torch.from_numpy(out) for out in outputs)

        # def forward(self, inputs):
        #     # TODO - Make something intelligent to determine the forward method
        #     return torch.nn.functional.softmax(inputs, dim=0)

    def load_any_model(model_name, hf_token=None, **kwargs):

        model = None
        available_model_classes = [
            ("CausalLM", transformers.AutoModelForCausalLM, {"ignore_mismatched_sizes": True}),
            ("Seq2Seq", transformers.AutoModelForSeq2SeqLM, {}),
            ("TokenClassification", transformers.AutoModelForTokenClassification, {}),
            ("SequenceClassification", transformers.AutoModelForSequenceClassification, {}),
            ("PreTraining", transformers.AutoModelForPreTraining, {}),
            ("MaskedLM", transformers.AutoModelForMaskedLM, {}),
            ("MaskGeneration", transformers.AutoModelForMaskGeneration, {}),
            ("MultipleChoice", transformers.AutoModelForMultipleChoice, {}),
            ("NextSentencePrediction", transformers.AutoModelForNextSentencePrediction, {}),
            ("QuestionAnswering", transformers.AutoModelForQuestionAnswering, {}),
            ("TextEncoding", transformers.AutoModelForTextEncoding, {}),
            ("DepthEstimation", transformers.AutoModelForDepthEstimation, {}),
            ("ImageClassification", transformers.AutoModelForImageClassification, {}),
            ("VideoClassification", transformers.AutoModelForVideoClassification, {}),
            ("KeypointDetection", transformers.AutoModelForKeypointDetection, {}),
            ("MaskedImageModeling", transformers.AutoModelForMaskedImageModeling, {}),
            ("ObjectDetection", transformers.AutoModelForObjectDetection, {}),
            ("ImageSegmentation", transformers.AutoModelForImageSegmentation, {}),
            ("ImageToImage", transformers.AutoModelForImageToImage, {}),
            ("SemanticSegmentation", transformers.AutoModelForSemanticSegmentation, {}),
            ("InstanceSegmentation", transformers.AutoModelForInstanceSegmentation, {}),
            ("UniversalSegmentation", transformers.AutoModelForUniversalSegmentation, {}),
            ("ZeroShotImageClassification", transformers.AutoModelForZeroShotImageClassification, {}),
            ("ZeroShotObjectDetection", transformers.AutoModelForZeroShotObjectDetection, {}),
            ("AudioClassification", transformers.AutoModelForAudioClassification, {}),
            ("AudioFrameClassification", transformers.AutoModelForAudioFrameClassification, {}),
            ("CTC", transformers.AutoModelForCTC, {}),
            ("SpeechSeq2Seq", transformers.AutoModelForSpeechSeq2Seq, {}),
            ("AudioXVector", transformers.AutoModelForAudioXVector, {}),
            ("TextToSpectrogram", transformers.AutoModelForTextToSpectrogram, {}),
            ("TextToWaveform", transformers.AutoModelForTextToWaveform, {}),
            ("TableQuestionAnswering", transformers.AutoModelForTableQuestionAnswering, {}),
            ("DocumentQuestionAnswering", transformers.AutoModelForDocumentQuestionAnswering, {}),
            ("VisualQuestionAnswering", transformers.AutoModelForVisualQuestionAnswering, {}),
            ("Vision2Seq", transformers.AutoModelForVision2Seq, {}),
            ("ImageTextToText", transformers.AutoModelForImageTextToText, {}),
            ("VitPose", transformers.VitPoseForPoseEstimation, {}),
            ("Generic", transformers.AutoModel, {})
        ]

        for label, model_class, extra_args in available_model_classes:
            try:
                model = model_class.from_pretrained(
                    model_name,
                    token=hf_token,
                    trust_remote_code=True,
                    **{**extra_args, **kwargs}
                )
                print(f"[OK] Model loaded as {label}")
                break
            except Exception as e:
                print(f"[WARN] Could not load model as {label}: {e}")

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
                print(f"[OK] Token loaded as {label}")
                break
            except Exception as e:
                print(f"[WARN] Could not load token as {label}: {e}")

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

        global hf_token

        upmem_layers.initialize_profiling_options(simulation=True)
        upmem_layers.profiler_init()

        hw_selected = hw_constraints.hardware_required()[0]

        layer_mapping = {
            "input_layernorm": hw_selected,
            "q_proj": hw_selected,
            "k_proj": hw_selected,
            "rotary_emb": hw_selected,
            "v_proj": hw_selected,
            "o_proj": hw_selected,
            "output_layernorm": hw_selected,
            "gate_proj": hw_selected,
            "up_proj": hw_selected,
            "down_proj": hw_selected,
            "norm": hw_selected,
            "lm_head": hw_selected,
        }

        model_path = ml_model.model_path()
        if isinstance(model_path, (list, tuple)):
            try:
                model_path = ''.join(chr(b) for b in model_path)
            except Exception:
                model_path = ""

        # Use model path if available
        if model_path and model_path != "Error":
            print(f"Using ONNX model path: {model_path}")
            try:
                onnx_model = ONNXModel(model_path)
                my_tensor = torch.rand(1,3,640,640, dtype=torch.float32)
                upmem_layers.profiler_start(layer_mapping)
                onnx_model.forward(my_tensor)
                upmem_layers.profiler_end()

            except Exception as e:
                print(f"[WARN] Failed to load/run ONNX at '{model_path}': {e}. Falling back to HF model.")
                model_path = ""

        # Use Hugging Face model
        else:
            try:
                print(f"Using Hugging Face model")
                hf_token = None
                extra_data_bytes = hw_constraints.extra_data()
                if extra_data_bytes:
                    extra_data_str = ''.join(chr(b) for b in extra_data_bytes)
                    extra_data_dict = json.loads(extra_data_str)
                    if "hf_token" in extra_data_dict:
                        hf_token = extra_data_dict["hf_token"]
                if hf_token is None:
                    raise Exception("HF token was not provided. Please set the HF_TOKEN environment variable.")

                model, tokenizer, input = load_any_model(
                    ml_model.model(),
                    hf_token=hf_token,
                    low_cpu_mem_usage=True,
                    torch_dtype=torch.float16
                )
                print(f"Model, Tokenizer and Input loaded successfully")
                print(f"Model: {model}")
                print(f"Tokenizer: {tokenizer}")
                print(f"Input: {input}")

                model.eval()  # Put model in evaluation / inference mode

                # noinspection PyUnresolvedReferences
                upmem_layers.profiler_start(layer_mapping)
                # In case we want to time the original execution (comment out profiler_start)
                # start = time.time_ns()

                try:
                    output = model.generate(
                        **input, do_sample=True, temperature=0.9, min_length=64, max_length=64
                    )
                except Exception as e_gen:
                    print(f"Error generating output with generate: {e_gen}. Trying forward instead.")
                    try:
                        output = model(**input, bool_masked_pos=False)
                    except Exception as e_model:
                        print(f"Error generating output using model: {e_model}")
                        raise Exception from e_model

                # noinspection PyUnresolvedReferences
                upmem_layers.profiler_end()

            except Exception as e:
                import traceback
                traceback.print_exc()
                print(f"Error testing model on hardware: {e}")
                print(f"Please provide different model")
                hw.hw_description("Error")
                hw.power_consumption(0.0)
                hw.latency(0.0)
                error_message = "Failed to test model on hardware: " + str(e)
                error_info = {"error": error_message}
                encoded_error = json.dumps(error_info).encode("utf-8")
                hw.extra_data(encoded_error)
                return

        hw.hw_description(hw_selected)
        hw.power_consumption(upmem_layers.profiler_get_power_consumption())
        hw.latency(upmem_layers.profiler_get_latency())
        print(f"Power Consumption: {upmem_layers.profiler_get_power_consumption():.8f} W")
        print(f"Latency: {upmem_layers.profiler_get_latency()} ms")

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        # Callback for configuration implementation here

        # Dummy JSON configuration and implementation
        dummy_config = {
            "param1": "value1",
            "param2": "value2",
            "param3": "value3"
        }
        res.configuration(json.dumps(dummy_config))
        res.node_id(req.node_id())
        res.transaction_id(req.transaction_id())
        res.success(True)
        res.err_code(0) # 0: No error || 1: Error

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
