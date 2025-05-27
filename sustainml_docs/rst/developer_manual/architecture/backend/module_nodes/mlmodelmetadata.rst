.. _mlmodelmetadata_node:

ML-Model-Metadata Node
======================

.. raw:: html

   <style>
        .module_node:not(#ml-model-metadata) rect {
            fill: #aaa;
            stroke: #888 !important;
        }

        .module_node {
            opacity: 1;
            transition: opacity 0.1s;
        }

        svg:hover .module_node:not(#ml-model-metadata) {
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

The :ref:`mlmodelmetadata_node` processes the problem description given by the user and generates the required metadata for generating the machine learning model.

Inputs and Outputs
------------------

The following table summarizes the inputs and outputs of the :ref:`mlmodelmetadata_node`:

+----------------------+-------------------+--------------------------------+---------------------------+
| Input                | From Node         | Output                         | To Node                   |
+----------------------+-------------------+--------------------------------+---------------------------+
|:ref:`user_input_type`|:ref:`orchestrator`|:ref:`mlmodelmetadata_type`     |:ref:`mlmodelprovider_node`|
+----------------------+-------------------+--------------------------------+---------------------------+

Node Template
-------------

Following is the Python API provided for the :ref:`mlmodelmetadata_node`.
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
    """SustainML Task Encoder Node Implementation."""

    from sustainml_py.nodes.MLModelMetadataNode import MLModelMetadataNode

    # Manage signaling
    import os
    import signal
    import threading
    import time
    import json

    from rdftool.rdfCode import (
        load_graph, get_problems, get_cover_tags, search_metrics_by_modalities, get_models_for_problem, get_models_for_problem_and_tag,
        find_metrics_by_model, get_model_details, get_problems_for_cover_tag, get_all_metrics, get_modalities_input,
        get_modalities_output, search_metrics_by_cover_tag
    )
    from ollama import Client

    # Whether to go on spinning or interrupt
    running = False

    # Global variable of the graph
    graph = None

    unsupported_goals = [
                    "any-to-any",
                    "audio-classification",
                    "audio-text-to-text",
                    "audio-to-audio",
                    "automatic-speech-recognition",
                    "depth-estimation",
                    "document-question-answering",
                    "feature-extraction",
                    "fill-mask",
                    "graph-ml",
                    "image-classification",
                    "image-feature-extraction",
                    "image-segmentation",
                    "image-text-to-text",
                    "image-to-3d",
                    "image-to-image",
                    "image-to-text",
                    "image-to-video",
                    "keypoint-detection",
                    "mask-generation",
                    "object-detection",
                    "question-answering",
                    "reinforcement-learning",
                    "robotics",
                    "sentence-similarity",
                    "table-question-answering",
                    "tabular-classification",
                    "tabular-regression",
                    "text-classification",
                    "text-to-3d",
                    "text-to-audio",
                    "text-to-image",
                    "text-to-speech",
                    "text-to-video",
                    "text2text-generation",
                    "time-series-forecasting",
                    "token-classification",
                    "unconditional-image-generation",
                    "video-classification",
                    "video-text-to-text",
                    "visual-question-answering",
                    "voice-activity-detection",
                    "zero-shot-classification",
                    "zero-shot-image-classification",
                    "zero-shot-object-detection"
                ]

    # Signal handler
    def signal_handler(sig, frame):
        print("\nExiting")
        MLModelMetadataNode.terminate()
        global running
        running = False

    def get_llm_response(client, model_version, problem_definition, prompt):
        """Get a response from the Ollama API."""
        prompt = f"Given the following Information: \"{problem_definition}\". {prompt}"
        try:
            response = client.chat(model=model_version, messages=[
                {
                    'role': 'user',
                    'content': prompt,
                }
            ])
            return response['message']['content']
        except Exception as e:
            print(f"Error in getting response from Ollama: {e}")
            return None

    # User Callback implementation
    # Inputs: user_input
    # Outputs: node_status, ml_model_metadata
    def task_callback(user_input, node_status, ml_model_metadata):

        # Callback implementation here
        global graph
        print (f"Received Task: {user_input.task_id().problem_id()},{user_input.task_id().iteration_id()}")

        try:
            extra_data_bytes = user_input.extra_data()
            extra_data_str = ''.join(chr(b) for b in extra_data_bytes)
            extra_data_dict = json.loads(extra_data_str)
            accumulated_data = {}

            if "model_restrains" in extra_data_dict:
                accumulated_data["model_restrains"] = extra_data_dict["model_restrains"]

            if "model_selected" in extra_data_dict and extra_data_dict["model_selected"] != "":
                accumulated_data["model_selected"] = extra_data_dict["model_selected"]

            if "type" in extra_data_dict and extra_data_dict["type"] != "":
                accumulated_data["type"] = extra_data_dict["type"]

            encoded_data = json.dumps(accumulated_data).encode("utf-8")
            ml_model_metadata.extra_data(encoded_data)

            if "goal" in extra_data_dict and extra_data_dict["goal"] != "":
                goal = extra_data_dict["goal"]
                ml_model_metadata.ml_model_metadata().append(goal)
                print(f"Skipped ML Model Metadata. ML Goal selected as input: {goal}")
                return
        except Exception as e:
            print(f"No extra data was found: {e}")

        client = Client(host='http://localhost:11434')

        # Retrieve Possible Ml Goals from graph
        try:
            raw_goals = get_problems(graph)
            inputs = [str(g) for g in raw_goals]
            goals = [goal for goal in inputs if goal not in unsupported_goals]
        except Exception as e:
            print(f"Error in getting problems from MLModel graph: {e}")
            return

        # Select MLGoal Using Ollama llama 3
        prompt = f"Which of the following machine learning Goals can be used to solve this problem: {goals}?. Answer with only one of the Machine learning goals and nothing more, just the goal name without "" or ''. If you are not sure, answer with 'None'."
        if(user_input.modality() != ""):
            prompt = f"{prompt} Using the modality {user_input.modality()}."
        # Add metrics to the prompt
        if(user_input.inputs()):
            prompt = f"{prompt} The user inputs known are {', '.join(user_input.inputs())}."
        if(user_input.outputs()):
            prompt = f"{prompt} The user outputs known are {', '.join(user_input.outputs())}."
        if isinstance(user_input.minimum_samples(), int) and user_input.minimum_samples() > 0:
            prompt = f"{prompt} Have into account that needs to have {user_input.minimum_samples()} minimum samples."
        if isinstance(user_input.maximum_samples(), int) and user_input.maximum_samples() > 0:
            prompt = f"{prompt} Have into account that needs to have {user_input.maximum_samples()} maximum samples."

        problem = user_input.problem_short_description()
        if(user_input.problem_definition() != ""):
            problem = f"{problem}. {user_input.problem_definition()}."

        print (f"Complete problem defined: {problem}")
        print (f"Complete prompt use: {prompt}")

        mlgoal = None
        max_attempts = 3
        attempt = 0
        while attempt < max_attempts:
            mlgoal = get_llm_response(client, "llama3", problem, prompt).strip().lower()
            if mlgoal is not None and mlgoal in goals:
                break
            attempt += 1
            prompt = f"Your previous answer '{mlgoal}' was not valid. {prompt}"
            print(f"Retry {attempt}: Response '{mlgoal}' is not among available goals. Retrying...")
            print(f"Using new prompt: {prompt}")

        if mlgoal is not None and mlgoal in goals:
            ml_model_metadata.ml_model_metadata().append(mlgoal)
            print(f"Selected ML Goal: {mlgoal}")
        else:
            print(f"Failed to determine ML goal for task {user_input.task_id()}.")
            ml_model_metadata.ml_model_metadata().clear()
            error_message = "Failed to extract metadata due to invalid or incomplete input parameters."
            error_info = {"error": error_message}
            encoded_error = json.dumps(error_info).encode("utf-8")
            ml_model_metadata.extra_data(encoded_error)

    # User Configuration Callback implementation
    # Inputs: req
    # Outputs: res
    def configuration_callback(req, res):

        # Callback for configuration implementation here
        global graph
        if req.configuration() == "modality":
            res.node_id(req.node_id())
            res.transaction_id(req.transaction_id())
            try:
                # Retrieve Possible Ml Goals from graph
                raw_modality = get_cover_tags(graph)
                inputs = [str(m) for m in raw_modality]
                unsupported_modality = [
                    "audio",
                    "cv",
                    "multimodal",
                    "other",
                    "rl",
                    "tabular"
                ]
                supported_modality = [modality for modality in inputs if modality not in unsupported_modality]
                sorted_modalities = ', '.join(sorted(supported_modality))

                if sorted_modalities == "":
                    res.success(False)
                    res.err_code(1) # 0: No error || 1: Error
                else:
                    res.success(True)
                    res.err_code(0) # 0: No error || 1: Error
                print(f"Available Modalities: {sorted_modalities}") #debug

                raw_goals = get_problems(graph)
                inputs = [str(g) for g in raw_goals]
                supported_goals = [goal for goal in inputs if goal not in unsupported_goals]
                sorted_goals = ', '.join(sorted(supported_goals))  # TODO: fix overflow bug sending goals response to request

                if sorted_goals == "":
                    res.success(False)
                    res.err_code(1) # 0: No error || 1: Error
                else:
                    res.success(True)
                    res.err_code(0) # 0: No error || 1: Error
                print(f"Available Goals: {sorted_goals}")   #debug

                # json_str = json.dumps(dict(modalities=sorted_modalities, goals=sorted_goals))
                # print(len(json_str))    #debug

                res.configuration(json.dumps(dict(modalities=sorted_modalities, goals=sorted_goals)))
            except Exception as e:
                print(f"Error getting goals and modalities from request: {e}")
                res.success(False)
                res.err_code(1) # 0: No error || 1: Error

        elif "in_out_modalities" in req.configuration():
            res.node_id(req.node_id())
            res.transaction_id(req.transaction_id())
            try:
                # Retrieve Possible Ml Inputs and Outputs modalities
                inputs = get_modalities_input(graph)
                sorted_inputs = ', '.join(sorted(inputs))
                outputs = get_modalities_output(graph)
                sorted_outputs = ', '.join(sorted(outputs))

                if sorted_inputs == "" or sorted_outputs == "":
                    res.success(False)
                    res.err_code(1) # 0: No error || 1: Error
                else:
                    res.success(True)
                    res.err_code(0) # 0: No error || 1: Error
                print(f"Available Input Modalities: {sorted_inputs}") #debug
                print(f"Available Output Modalities: {sorted_outputs}") #debug

                res.configuration(json.dumps(dict(inputs=sorted_inputs, outputs=sorted_outputs)))

            except Exception as e:
                print(f"Error getting inputs and outputs modalities from request: {e}")
                res.success(False)
                res.err_code(1) # 0: No error || 1: Error

        elif "metrics" in req.configuration():
            res.node_id(req.node_id())
            res.transaction_id(req.transaction_id())

            # Extracts datas for metrics reception
            config_content = req.configuration()[len("metrics, "):]  # "metrics, <metric_req_type>: <req_type_values>"

            try:
                metric_req_type, req_type_values = config_content.split(":", 1)
                metric_req_type = metric_req_type.strip()
                req_type_values = req_type_values.strip()

                if metric_req_type == "cover_tag":
                    parts = req_type_values.split(',')
                    if len(parts) >= 2:
                        cover_tag = parts[0].strip()
                        tag = parts[1].strip()
                        metrics = search_metrics_by_cover_tag(graph, cover_tag)
                    else:
                        cover_tag = req_type_values.strip()
                        metrics = search_metrics_by_cover_tag(graph, cover_tag)

                    all_metrics = []
                    for problem, metrics_list in metrics.items():
                        for model, m in metrics_list.items():
                            if isinstance(m, list):
                                for metric in m:
                                    if metric not in all_metrics:
                                        all_metrics.append(metric)
                            else:
                                if m not in all_metrics:
                                    all_metrics.append(m)
                    sorted_metrics = ', '.join(sorted(all_metrics))

                elif metric_req_type == "modality":
                    input_modality, output_modality = req_type_values.split(",", 1)
                    input_modality = input_modality.strip()
                    output_modality = output_modality.strip()
                    metrics = search_metrics_by_modalities(graph, input_modality, output_modality)
                    all_metrics = []
                    for problem, metrics_list in metrics.items():
                        for model, m in metrics_list.items():
                            if isinstance(m, list):
                                for metric in m:
                                    if metric not in all_metrics:
                                        all_metrics.append(metric)
                            else:
                                if m not in all_metrics:
                                    all_metrics.append(m)
                    sorted_metrics = ', '.join(sorted(all_metrics))

                elif metric_req_type == "problem":
                    parts = req_type_values.split(',')
                    if len(parts) >= 2:
                        problem_name = parts[0].strip()
                        tag = parts[1].strip()
                        models = get_models_for_problem_and_tag(graph, problem_name, tag)
                    else:
                        problem_name = req_type_values.strip()
                        models = get_models_for_problem(graph, problem_name)

                    # Pass problem name and tag
                    all_metrics = []

                    for model,downloads in models:
                        metrics = find_metrics_by_model(graph, model)
                        if isinstance(metrics, list):
                            all_metrics.extend(metrics)
                        else:
                            all_metrics.append(metrics)

                    sorted_metrics = ', '.join(sorted(all_metrics))

                elif metric_req_type == "all":
                    metrics = get_all_metrics(graph)
                    sorted_metrics = ', '.join(sorted(metrics))

                else:
                    res.success(False)
                    res.err_code(1) # 0: No error || 1: Error

            except Exception as e:
                print(f"Error getting metrics from request: {e}")
                res.success(False)
                res.err_code(1) # 0: No error || 1: Error

            if sorted_metrics == "":
                res.success(False)
                res.err_code(1) # 0: No error || 1: Error
            else:
                res.success(True)
                res.err_code(0) # 0: No error || 1: Error
            print(f"Available Metrics: {sorted_metrics}")   #debug

            res.configuration(json.dumps(dict(metrics=sorted_metrics)))

        elif 'mode_info' in req.configuration():
            res.node_id(req.node_id())
            res.transaction_id(req.transaction_id())

            try:
                model = req.configuration()[len("mode_info, "):]
                details = get_model_details(graph, model)

                if not details:
                    res.success(False)
                    res.err_code(1)  # 0: No error || 1: Error
                else:
                    res.success(True)
                    res.err_code(0)  # 0: No error || 1: Error

                print(f"Model details for {model}: {details}")  #debug
                res.configuration(json.dumps(details))
            except Exception as e:
                print(f"Error getting model details from request: {e}")
                res.success(False)
                res.err_code(1)

        elif 'problem_from_modality' in req.configuration():
            res.node_id(req.node_id())
            res.transaction_id(req.transaction_id())

            try:
                modality = req.configuration()[len("problem_from_modality, "):]
                goals = get_problems_for_cover_tag(graph, modality)
                sorted_goals = ', '.join(sorted(goals))

                if not sorted_goals:
                    res.success(False)
                    res.err_code(1)  # 0: No error || 1: Error
                else:
                    res.success(True)
                    res.err_code(0)  # 0: No error || 1: Error

                print(f"Problems for {modality}: {goals}")  #debug
                res.configuration(json.dumps(dict(goals=sorted_goals)))

            except Exception as e:
                print(f"Error getting problems for the modality from request: {e}")
                res.success(False)
                res.err_code(1)

        else:
            res.node_id(req.node_id())
            res.transaction_id(req.transaction_id())
            error_msg = f"Unsupported configuration request: {req.configuration()}"
            res.configuration(json.dumps({"error": error_msg}))
            res.success(False)
            res.err_code(1) # 0: No error || 1: Error
            print(error_msg)


    # Main workflow routine
    def run():
        global graph
        graph = load_graph(os.path.dirname(__file__)+'/graph_v2.ttl')
        node = MLModelMetadataNode(callback=task_callback, service_callback=configuration_callback)
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
