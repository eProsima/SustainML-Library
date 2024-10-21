# Copyright 2024 SustainML Consortium
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
from kaggle.api.kaggle_api_extended import KaggleApi
import kaggle

app = Flask(__name__)

app.config['UPLOAD_FOLDER'] = 'uploads'

if not os.path.exists(app.config['UPLOAD_FOLDER']):
    os.makedirs(app.config['UPLOAD_FOLDER'])

uploaded_data = None

# Initialize Kaggle API
kaggle_api = KaggleApi()
kaggle_api.authenticate()

@app.route('/search_datasets', methods=['POST'])
def search_datasets():
    data = request.json
    modality = data.get('modality')
    if not modality:
        return jsonify({'error': 'No modality provided'}), 400

    # Map modality to search terms
    modality_mapping = {
        'image_video': 'images OR videos',
        'text': 'text data OR NLP',
        'sensor': 'sensor data OR IoT',
        'audio': 'audio data OR sound',
    }

    search_term = modality_mapping.get(modality, '')
    if not search_term:
        return jsonify({'error': 'Invalid modality'}), 400

    try:
        datasets = kaggle_api.dataset_list(search=search_term, page_size=10)
        datasets_info = []
        for dataset in datasets:
            datasets_info.append({
                'title': dataset.title,
                'url': f'https://www.kaggle.com/datasets/{dataset.ref}',
                'description': dataset.subtitle,
                'downloads': dataset.downloadCount,
                'size': f'{dataset.totalBytes // (1024 * 1024)} MB',
            })
        return jsonify({'datasets': datasets_info})
    except Exception as e:
        print(f"Error searching datasets: {e}")
        return jsonify({'error': 'Failed to fetch datasets'}), 500

@app.route('/')
def index():
    return send_from_directory('.', 'index.html')

@app.route('/data.html')
def data():
    return send_from_directory('.', 'data.html')

@app.route('/styles.css')
def styles():
    return send_from_directory('.', 'styles.css')

def handle_nan_values(data):
    # Replace NaN with None
    return data.where(pd.notnull(data), None)

@app.route('/upload', methods=['POST'])
def upload_file():
    global uploaded_data
    if 'file' not in request.files:
        return jsonify({"error": "No file part"}), 400
    file = request.files['file']
    if file.filename == '':
        return jsonify({"error": "No selected file"}), 400

    try:
        if file.filename.endswith('.csv'):
            df = pd.read_csv(file)
        elif file.filename.endswith('.json'):
            df = pd.read_json(file)
        else:
            return jsonify({"error": "Invalid file type. Please upload a CSV or JSON file."}), 400

        uploaded_data = df  # Save uploaded data globally

        first_five_rows = handle_nan_values(df.head()).to_dict(orient='records')
        empty_values_count = df.isnull().sum().sum()

        return jsonify({
            "first_five_rows": first_five_rows,
            "empty_values_count": int(empty_values_count)
        })
    except Exception as e:
        print(e)
        return jsonify({"error": "Error processing file."}), 500

@app.route('/data-info', methods=['GET'])
def data_info():
    global uploaded_data
    if uploaded_data is None:
        return jsonify({"error": "No data uploaded"}), 400

    try:
        data_size = uploaded_data.shape[0]
        num_features = uploaded_data.shape[1]
        empty_values = uploaded_data.isnull().sum().sum()

        return jsonify({
            "size": data_size,
            "features": num_features,
            "empty_values": int(empty_values)
        })
    except Exception as e:
        return jsonify({"error": str(e)}), 500

@app.route('/count-empty-values', methods=['POST'])
def count_empty_values():
    global uploaded_data
    if uploaded_data is None:
        return jsonify({'error': 'No data uploaded'})

    selected_columns = request.json.get('selectedColumns', [])
    if not selected_columns:
        return jsonify({'error': 'No columns selected'})

    selected_data = uploaded_data[selected_columns]
    empty_values_count = selected_data.isnull().sum().sum()

    return jsonify({'empty_values_count': int(empty_values_count)})

@app.route('/remove-empty', methods=['POST'])
def remove_empty():
    global uploaded_data
    if uploaded_data is None:
        return jsonify({"error": "No data uploaded"}), 400

    try:
        original_size = uploaded_data.shape[0]
        df_cleaned = uploaded_data.dropna()
        new_size = df_cleaned.shape[0]

        return jsonify({
            "original_size": original_size,
            "new_size": new_size
        })
    except Exception as e:
        print(e)
        return jsonify({"error": "Error processing data."}), 500

@app.route('/get_pd_options', methods=['GET'])
def get_pd_options():
    try:
        pd_options = []
        with open('ResearchData.csv', newline='') as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                pd_options.append(row)
        return jsonify(pd_options)
    except Exception as e:
        return jsonify({'error': str(e)}), 500

@app.route('/filter_options', methods=['POST'])
def filter_options():
    try:
        data = request.json
        modality = data.get('modality')
        problem_type = data.get('problem_type')
        evaluation_metrics = data.get('evaluation_metrics')

        filtered_options = []
        with open('ResearchData.csv', newline='') as csvfile:
            reader = csv.DictReader(csvfile)
            for row in reader:
                if modality and row['Data modality'] != modality:
                    continue
                if problem_type and row['Problem type'] != problem_type:
                    continue
                if evaluation_metrics and row['Evaluation metrics'] != evaluation_metrics:
                    continue
                filtered_options.append(row)
        return jsonify(filtered_options)
    except Exception as e:
        return jsonify({'error': str(e)}), 500

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

if __name__ == '__main__':
    app.run(debug=True)
