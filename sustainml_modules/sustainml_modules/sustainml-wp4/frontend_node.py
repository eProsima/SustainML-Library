# Copyright 2025 SustainML Consortium
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
"""SustainML Frontend Node Implementation."""

import os
from flask import Flask, request, jsonify, send_from_directory
import pandas as pd
import csv
import requests
import json

kaggle_available = True
try:
    from kaggle.api.kaggle_api_extended import KaggleApi
    import kaggle
except IOError as error:
    print("Kaggle API not available: \n", error)
    kaggle_available = False

app = Flask(__name__)

app.config['UPLOAD_FOLDER'] = 'uploads'

if not os.path.exists(app.config['UPLOAD_FOLDER']):
    os.makedirs(app.config['UPLOAD_FOLDER'])

uploaded_data = None

# Initialize Kaggle API
if kaggle_available:
    kaggle_api = KaggleApi()
    kaggle_api.authenticate()

# @app.route('/search_datasets', methods=['POST'])  #Previous function for searching datasets
# def search_datasets():
#     data = request.json
#     modality = data.get('modality')
#     if not modality:
#         return jsonify({'error': 'No modality provided'}), 400

#     # Map modality to search terms
#     modality_mapping = {
#         'image_video': 'images OR videos',
#         'text': 'text data OR NLP',
#         'sensor': 'sensor data OR IoT',
#         'audio': 'audio data OR sound',
#     }

#     search_term = modality_mapping.get(modality, '')
#     if not search_term:
#         return jsonify({'error': 'Invalid modality'}), 400

#     if kaggle_available:
#         try:
#             datasets = kaggle_api.dataset_list(search=search_term, page_size=10)
#             datasets_info = []
#             for dataset in datasets:
#                 datasets_info.append({
#                     'title': dataset.title,
#                     'url': f'https://www.kaggle.com/datasets/{dataset.ref}',
#                     'description': dataset.subtitle,
#                     'downloads': dataset.downloadCount,
#                     'size': f'{dataset.totalBytes // (1024 * 1024)} MB',
#                 })
#             return jsonify({'datasets': datasets_info})
#         except Exception as e:
#             print(f"Error searching datasets: {e}")
#             return jsonify({'error': 'Failed to fetch datasets'}), 500
#     else:
#         return jsonify({'error': 'kaggle is not available'}), 500


@app.route('/')
def index():
    return send_from_directory('.', 'Updated_index.html')

@app.route('/upload', methods=['POST'])
def upload_file():
    # TODO: Implement dataset upload as a path to the backend
    return jsonify({'message': 'Upload endpoint is not implemented yet.'}), 501

@app.route('/get_geolocation', methods=['GET'])
def get_geolocation():
    try:
        ip_address = request.remote_addr
        response = requests.get(f'http://ipapi.co/{ip_address}/json/')
        if response.status_code == 200:
            return jsonify(response.json())
        else:
            return jsonify({'error': 'Failed to fetch geolocation data'}), 500
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/send_pd', methods=['POST'])
def send_pd():
    print("Terminal: /send_pd endpoint was called")
    try:
        data = request.json
        # pipe the information to the backend
        requests.post('http://127.0.0.1:5001/user_input', json=data)
        return jsonify({'message': 'Data successfully sent'}), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_status', methods=['GET'])
def get_status():
    try:
        # pipe the information to the backend
        response = requests.get('http://127.0.0.1:5001/status')
        return jsonify(response.json()), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_results', methods=['GET'])
def get_results():
    try:
        # pipe the information to the backend
        response = requests.get('http://127.0.0.1:5001/results')
        return jsonify(response.json()), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_results', methods=['POST'])
def get_results_args():
    print("Terminal: /result POST")
    try:
        data = request.json
        mapping = {
            "APP_REQUIREMENTS": 0,
            "CARBON_FOOTPRINT": 1,
            "HW_CONSTRAINTS": 2,
            "HW_RESOURCES": 3,
            "ML_MODEL_METADATA": 4,
            "ML_MODEL": 5,
            "ORCHESTRATOR": 7,
            "UNKNOWN": 8,
            "ALL": 9
        }
        node_id = data.get("node_id")
        if isinstance(node_id, str):
            data["node_id"] = mapping.get(node_id, node_id)
        # pipe the information to the backend
        response = requests.post('http://127.0.0.1:5001/results', json=data)
        return jsonify(response.json()), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_modality', methods=['GET'])
def get_modality():
    print("Terminal: /request modality")
    try:
        data = {
            "node_id": 4,
            "configuration": "modality"
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "modality": config.get("modalities", ""),
            "goals": config.get("goals", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_problem_from_modality', methods=['POST'])
def get_problem_from_modality():
    print("Terminal: /request problem from modality")
    try:
        in_data = request.json
        req_type_values = in_data.get('modality')
        data = {
            "node_id": 4,
            "configuration": "problem_from_modality, " + req_type_values
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "goals": config.get("goals", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_model_info', methods=['POST'])
def get_model_info():
    print("Terminal: /request model info")
    try:
        in_data = request.json
        req_type_values = in_data.get('model')
        data = {
            "node_id": 4,
            "configuration": "mode_info, " + req_type_values
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "model_uri": config.get("model_uri", ""),
            "id": config.get("id", ""),
            "name": config.get("name", ""),
            "problem": config.get("problem", ""),
            "coverTag": config.get("coverTag", ""),
            "library": config.get("library", ""),
            "downloads": config.get("downloads", ""),
            "likes": config.get("likes", ""),
            "lastModified": config.get("lastModified", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_inout_modalities', methods=['GET'])
def get_inout_modalities():
    print("Terminal: /request inputs and outputs modalities")
    try:
        data = {
            "node_id": 4,
            "configuration": "in_out_modalities"
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "inputs": config.get("inputs", ""),
            "outputs": config.get("outputs", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_metrics_from_modalities', methods=['POST'])
def get_metrics_from_modalities():
    print("Terminal: /request metrics from modalities")
    try:
        in_data = request.json
        req_type_values = in_data.get('modalities')
        data = {
            "node_id": 4,
            "configuration": (
                "metrics, " + "modality" + ": " + req_type_values
            )    # Example of req_type_values (Input modality, output modality): "Image, Label"
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "metrics": config.get("metrics", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_metrics_from_problem', methods=['POST'])
def get_metrics_from_problem():
    print("Terminal: /request metrics from problem")
    try:
        in_data = request.json
        req_type_values = in_data.get('problem')
        data = {
            "node_id": 4,
            "configuration": (
                "metrics, " + "problem" + ": " + req_type_values
            )    # Example of req_type_values (goal): "audio-text-to-text"
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "metrics": config.get("metrics", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_all_metrics', methods=['GET'])
def get_all_metrics():
    print("Terminal: /request all metrics")
    try:
        data = {
            "node_id": 4,
            "configuration": (
                "metrics, " + "all" + ": "
            )
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "metrics": config.get("metrics", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/get_hardware', methods=['GET'])
def get_hardware():
    print("Terminal: /request hardware")
    try:
        data = {
            "node_id": 3,
            "configuration": "hardwares"
        }
        # pipe the information to the backend
        url = 'http://127.0.0.1:5001/config_request'
        response = requests.post(url, json=data)
        config_str = response.json().get('response', {}).get('configuration', '{}')
        config = json.loads(config_str)
        new_config = {
            "hardwares": config.get("hardwares", "")
        }
        print(new_config)
        return jsonify(new_config), 200
    except Exception as e:
        return jsonify({'error': str(e)}), 500

if __name__ == '__main__':
    app.run(debug=True)
