#!/bin/bash
use_sustainml_modules=true

# source sustainml environment
source "/sustainml/install/setup.bash"

if [[ ${node} == "orchestrator" ]]; then
    /sustainml/build/sustainml/sustainml
elif [[ ${node} == "app_requirements" ]]; then
    if [[ $use_sustainml_modules ]]; then
        python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/app_requirements_node.py
    else
        python3 /sustainml/src/sustainml_lib/sustainml_py/examples/app_requirements_node.py
    fi
elif [[ ${node} == "carbon_tracker" ]]; then
    if [[ $use_sustainml_modules ]]; then
        python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp3/carbon_footprint_node.py
    else
        python3 /sustainml/src/sustainml_lib/sustainml_py/examples/co2_node.py
    fi
elif [[ ${node} == "hw_constraints" ]]; then
    if [[ $use_sustainml_modules ]]; then
        python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_constraints_node.py
    else
        python3 /sustainml/src/sustainml_lib/sustainml_py/examples/hw_constratins_node.py
    fi
elif [[ ${node} == "hw_resources" ]]; then
    if [[ $use_sustainml_modules ]]; then
        python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_resources_provider_node.py
    else
        python3 /sustainml/src/sustainml_lib/sustainml_py/examples/hw_resources_node.py
    fi
elif [[ ${node} == "ml_model_metadata" ]]; then
    if [[ $use_sustainml_modules ]]; then
        python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_metadata_node.py
    else
        python3 /sustainml/src/sustainml_lib/sustainml_py/examples/ml_metadata_node.py
    fi
elif [[ ${node} == "ml_model" ]]; then
    if [[ $use_sustainml_modules ]]; then
        python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_provider_node.py
    else
        python3 /sustainml/src/sustainml_lib/sustainml_py/examples/ml_model_node.py
    fi
else
    echo "Unknown node: ${node}"
    exit 1
fi

