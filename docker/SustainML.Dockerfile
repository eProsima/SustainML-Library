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

# Set the SustainML working directory
WORKDIR /sustainml

# Set the branch per each repository
ARG fastcdr_branch=v2.2.0
ARG fastdds_branch=v2.14.0
ARG devutils_branch=main
ARG sustainml_branch=feature/architecture_update
ARG sustainml_lib_branch=feature/baseline_topics

# default run node is the orchestrator
ENV node orchestrator

# Copy the bash script entrypoint file
COPY run.bash /run.bash

# Provide permissions to the run.bash file
RUN chmod +x /run.bash

# Print the branches that are going to be used
RUN echo "fastcdr branch: <${fastcdr_branch}>" && \
    echo "fastdds branch: <${fastdds_branch}>" && \
    echo "devutils branch: <${devutils_branch}>" && \
    echo "sustainml branch: <${sustainml_branch}>" && \
    echo "sustainml_lib branch: <${sustainml_lib_branch}>"

# Clone the repositories and checkout the branches
RUN wget https://raw.githubusercontent.com/eProsima/SustainML-Frontend/main/sustainml.repos && \
    mkdir src && vcs import src < sustainml.repos && \
    cd src/fastcdr && git checkout ${fastcdr_branch} && cd ../.. && \
    cd src/fastdds && git checkout ${fastdds_branch} && cd ../.. && \
    cd src/dev-utils && git checkout ${devutils_branch} && cd ../.. && \
    cd src/sustainml && git checkout ${sustainml_branch} && cd ../.. && \
    cd src/sustainml_lib && git checkout ${sustainml_lib_branch} && cd ../..

# Build the projects
RUN colcon build --event-handlers console_direct+ --cmake-args -DTHIRDPARTY_Asio=FORCE -DTHIRDPARTY_TinyXML2=FORCE

ENTRYPOINT ["/run.bash" ]
