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
Each of the modules conforms a **Node**, which shall import its corresponding Python library, so that each node can be abstracted from the communication between other nodes.

The following sections describe the steps to install the SustainML Framework using Dockers.

.. _installation_framework_docker_dependencies:

SustainML Framework dependencies
--------------------------------

`Docker <https://www.docker.com/>`_ is a platform that allows to develop and run applications in virtual containers.
**Install Docker** following the installation instructions of the `Docker documentation <https://docs.docker.com/get-docker/>`_.

To run the *SustainML Framework* using Docker, a Dockerfile is required to build the Docker image with all the required configuration.
Also, a Docker compose file is required to deploy all the nodes that conform the *SustainML Framework*, using the previous Docker image.
Finally, a bash script file is required to run the selected node based on an input argument.

The following command downloads both Dockerfile, Docker compose and script files:

.. tabs::

    .. group-tab:: Ubuntu

        .. code-block:: bash

            mkdir -p ~/SustainML && cd ~/SustainML && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/Dockerfile && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/docker-compose.yaml && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/run.bash

    .. group-tab:: MacOS

        .. code-block:: bash

            mkdir -p ~/SustainML && cd ~/SustainML && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/Dockerfile && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/docker-compose.yaml && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/run.bash

        The *SustainML Framework* GUI application requires special privileges to access the X server.
        To provide them, `XQuartz <https://www.xquartz.org/>`_ needs to be installed.

.. _installation_framework_docker_build:

Build the SustainML Framework Docker image
------------------------------------------

The Dockerfile defines the set of dependencies and configurations required to build a image with the *SustainML Framework*.
The following command builds the Docker image using the downloaded Dockerfile, setting the image name as *sustainml*, and tagged as version **v0.1.0**.

.. tabs::

    .. group-tab:: Ubuntu

        .. code-block:: bash

            docker build -f ~/SustainML/Dockerfile -t sustainml:v0.1.0 .

    .. group-tab:: MacOS

        .. code-block:: bash

            docker build -f ~/SustainML/Dockerfile -t sustainml:v0.1.0 .

.. _installation_framework_docker_deployment:

SustainML Framework Docker deployment
-------------------------------------

To run the *SustainML Framework* using Dockers, execute the following command:

.. tabs::

    .. group-tab:: Ubuntu

        Provide privileges to the X localhost server and deploy the *SustainML Framework* using Docker compose:

        .. code-block:: bash

            xhost local:root && \
            docker compose up

    .. group-tab:: MacOS

        Open a XQuart terminal (required in the **MacOS** section of the :ref:`installation_framework_docker_dependencies`).

        .. code-block:: bash

            open -a XQuartz

        Ensure that the security setting *Allow connections from network clients* is enabled in the XQuartz preferences.

        .. image:: /rst/figures/xquartz_settings.png
            :width: 45%

        .. image:: /rst/figures/xquartz_security.png
            :width: 45%

        .. note::

            The XQuartz terminal may require to be restarted to apply the changes.

        In the XQuartz terminal, provide privileges to the X localhost server and set the display environment variable.
        Then, deploy the *SustainML Framework* using Docker compose:

        .. code-block:: bash

            xhost + 127.0.0.1 && \
            export DISPLAY=`127.0.0.1:0` && \
            docker compose up
