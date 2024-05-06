.. _installation_docker:

SustainML Framework using Dockers
=================================

The instructions for installing the :ref:`SustainML Framework <index_introduction>` using Dockers are provided in this page.
It is organized as follows:

.. contents::
    :local:
    :backlinks: none
    :depth: 2

The *SustainML Framework* is composed of different software modules, each one related to specific task, which are specialized in solving the different parts of the framework.
Each of the modules conforms a **Node**, which shall import its corresponding Python library, so that each Node can be abstracted from the communication between other nodes.

The following sections describe the steps to install the SustainML Framework using Dockers.

.. _installation_framework_docker_dependencies:

SustainML Framework dependencies
--------------------------------

`Docker <https://www.docker.com/>`_ is a platform that allows to develop and run applications in virtual containers.
The Docker installation instructions can be found in the `Docker documentation <https://docs.docker.com/get-docker/>`_.

To run the *SustainML Framework* using Docker, a Dockerfile is required to build the Docker image with all the required configuration.
Also, a Docker compose file is required to deploy all the nodes that conform the *SustainML Framework*, using the previous Docker image.
Finally, a bash script file is required to run the selected node based on an input argument.

The following command downloads both Dockerfile, Docker compose and script files:

.. code-block:: bash

    mkdir -p ~/SustainML && cd ~/SustainML && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/Dockerfile && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/docker-compose.yaml && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/run.bash

.. _installation_framework_docker_build:

Build the SustainML Framework Docker image
------------------------------------------

The Dockerfile defines the set of dependencies and configurations required to build a image with the *SustainML Framework*.
The following command builds the Docker image using the downloaded Dockerfile, setting the image name as *sustainml*, and tagged as version **v0.1.0**.

.. code-block:: bash

    docker build -f ~/SustainML/Dockerfile -t sustainml:v0.1.0 .

.. _installation_framework_docker_deployment:

SustainML Framework Docker deployment
-------------------------------------

To run the *SustainML Framework* using Dockers, execute the following command:

.. code-block:: bash

    docker compose up
