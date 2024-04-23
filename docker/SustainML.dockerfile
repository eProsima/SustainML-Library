# Use the vulcanexus image to avoid the need to install dependencies
FROM eprosima/vulcanexus:iron-desktop
LABEL author=raul@eprosima.com

# Avoid interactuation with installation of some package that needs the locale.
ENV TZ=Europe/Madrid
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Avoids using interactions during building
ENV DEBIAN_FRONTEND=noninteractive

# Use a bash shell so it is possigle to run things like `source` (required for colcon builds)
SHELL ["/bin/bash", "-c"]

# Install apt dependencies
RUN apt-get update && apt-get install --yes --no-install-recommends qtdeclarative5-dev libqt5charts5-dev qtquickcontrols2-5-dev

WORKDIR /sustainml

ARG fastcdr_branch=v2.2.0
ARG fastdds_branch=v2.14.0
ARG devutils_branch=main
ARG sustainml_branch=feature/architecture_update
ARG sustainml_lib_branch=feature/baseline_topics

# default run node is the orchestrator
ENV node orchestrator

RUN echo "fastcdr branch: <${fastcdr_branch}>" && \
    echo "fastdds branch: <${fastdds_branch}>" && \
    echo "devutils branch: <${devutils_branch}>" && \
    echo "sustainml branch: <${sustainml_branch}>" && \
    echo "sustainml_lib branch: <${sustainml_lib_branch}>"

RUN wget https://raw.githubusercontent.com/eProsima/SustainML-Frontend/main/sustainml.repos && \
    mkdir src && vcs import src < sustainml.repos && \
    cd src/fastcdr && git checkout ${fastcdr_branch} && cd ../.. && \
    cd src/fastdds && git checkout ${fastdds_branch} && cd ../.. && \
    cd src/dev-utils && git checkout ${devutils_branch} && cd ../.. && \
    cd src/sustainml && git checkout ${sustainml_branch} && cd ../.. && \
    cd src/sustainml_lib && git checkout ${sustainml_lib_branch} && cd ../..

RUN colcon build --event-handlers console_direct+ --cmake-args -DTHIRDPARTY_Asio=FORCE -DTHIRDPARTY_TinyXML2=FORCE

# Examples (piped) run (uncomment this if necessary)
#RUN printf '#!/bin/bash\n set -e\n \n source "/sustainml/install/setup.bash" \n if [[ ${node} == "orchestrator" ]]; then /sustainml/build/sustainml/sustainml \n elif [[ ${node} == "app_requirements" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_py/examples/app_requirements_node.py \n elif [[ ${node} == "carbon_tracker" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_py/examples/co2_node.py \n elif [[ ${node} == "hw_constraints" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_py/examples/hw_constratins_node.py \n elif [[ ${node} == "hw_resources" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_py/examples/hw_resources_node.py \n elif [[ ${node} == "ml_model_metadata" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_py/examples/ml_metadata_node.py \n elif [[ ${node} == "ml_model" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_py/examples/ml_model_node.py \n else echo "Unknown node: ${node}" \n exit 1 \n fi \n exec "$@"' > /run.bash && chmod +x /run.bash

# WP submodules run (uncomment this if necessary)
RUN printf '#!/bin/bash\n set -e\n \n source "/sustainml/install/setup.bash" \n if [[ ${node} == "orchestrator" ]]; then /sustainml/build/sustainml/sustainml \n elif [[ ${node} == "app_requirements" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/app_requirements_node.py \n elif [[ ${node} == "ml_model_metadata" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_metadata_node.py \n elif [[ ${node} == "ml_model" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_provider_node.py \n elif [[ ${node} == "hw_constraints" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_constraints_node.py \n elif [[ ${node} == "hw_resources" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_resources_provider_node.py \n elif [[ ${node} == "carbon_tracker" ]]; then python3 /sustainml/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp3/carbon_footprint_node.py \n else echo "Unknown node: ${node}" \n exit 1 \n fi \n exec "$@"' > /run.bash && chmod +x /run.bash


ENTRYPOINT ["/run.bash" ]
