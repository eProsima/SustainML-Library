.. _installation_docker:

SustainML Framework using Dockers
=================================

The instructions for installing the :ref:`SustainML Framework <index_introduction>` using Dockers are provided in this page.
It is organized as follows:

* :ref:`installation_framework_docker_dependencies`
* :ref:`installation_framework_docker_build`
* :ref:`installation_framework_docker_deployment`

The *SustainML Framework* is composed of different software modules, each one related to a specific task, which are specialized in solving the different parts of the framework.
Each of the modules conforms a **Node**, which imports its corresponding Python library, so that each node can be abstracted from the communication between other nodes.
With Docker, every node and the GUI run in their own container, all of them created from the same *SustainML* Docker image.

The following sections describe the steps to install the *SustainML Framework* using Dockers on an **Ubuntu** host.

.. _installation_framework_docker_dependencies:

SustainML Framework dependencies
--------------------------------

`Docker <https://www.docker.com/>`_ is a platform that allows to develop and run applications in virtual containers.
**Install Docker Engine and its Compose plugin** (the ``docker compose`` command) following the installation instructions of the `Docker documentation <https://docs.docker.com/engine/install/ubuntu/>`_.

To run the *SustainML Framework* using Docker, three files from the ``docker`` folder of the `SustainML Framework repository <https://github.com/eProsima/SustainML-Framework>`_ are used:

* ``Dockerfile``: builds an Ubuntu 24.04 image with all the dependencies (including Java 21 and Neo4j), the *SustainML* workspace built with colcon, the ANN index of the models knowledge base and the Neo4j database already loaded.
* ``run.bash``: entrypoint of the image, which starts the node selected by the ``SUSTAINML_NODE`` environment variable.
* ``docker-compose.yaml``: deploys one container per node, plus the GUI, using the previous Docker image.

The following command downloads the three files:

.. code-block:: bash

    mkdir -p ~/SustainML && cd ~/SustainML && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/Dockerfile && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/docker-compose.yaml && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/docker/run.bash

The containers do not include `Ollama <https://ollama.com/>`_.
The nodes reach the Ollama server of the host at ``http://localhost:11434`` (all the containers use the host network), and use its ``llama3`` model.
Install Ollama on the host, which also starts it as a service, and pull the model:

.. code-block:: bash

    curl -fsSL https://ollama.com/install.sh | sh && \
    ollama pull llama3

.. _installation_framework_docker_build:

Build the SustainML Framework Docker image
------------------------------------------

The Dockerfile defines the set of dependencies and configurations required to build an image with the *SustainML Framework*.
It clones the repositories listed in the ``sustainml.repos`` file of the ``main`` branch of the *SustainML Framework*.
The following command builds the Docker image from the ``~/SustainML`` folder (which must contain ``run.bash``), setting the image name as *sustainml* and the tag as **v0.2.0**, which is the image that ``docker-compose.yaml`` uses:

.. code-block:: bash

    cd ~/SustainML && \
    docker build -f Dockerfile -t sustainml:v0.2.0 .

.. note::

    The branch of each repository can be changed with the build arguments ``fastcdr_branch``, ``fastdds_branch``, ``devutils_branch``, ``sustainml_lib_branch`` and ``sustainml_framework_branch``.
    For example:

    .. code-block:: bash

        docker build -f Dockerfile -t sustainml:v0.2.0 --build-arg sustainml_lib_branch=<branch> .

.. _installation_framework_docker_deployment:

SustainML Framework Docker deployment
-------------------------------------

.. important::

    Before running the framework, set the ``HF_TOKEN`` environment variable on the host to your personal `Hugging Face access token <https://huggingface.co/settings/tokens>`_.
    ``docker-compose.yaml`` passes it to the backend container, which forwards it to the nodes that download models from the Hugging Face Hub, so that gated models can be loaded.

    .. code-block:: bash

        export HF_TOKEN=<your_huggingface_token>

.. note::

    All the nodes use the DDS domain ``0`` by default.
    To change it, set the environment variable ``SUSTAINML_DOMAIN_ID`` on the host before launching the containers; ``docker-compose.yaml`` passes it to the backend and module node containers.

    .. code-block:: bash

        export SUSTAINML_DOMAIN_ID=<domain_id>

The ``docker-compose.yaml`` file deploys the following containers:

* ``desktop``: the *SustainML Framework* GUI, shown on the X server of the host.
* ``back_end``: the backend node, which hosts the orchestrator and the REST API used by the GUI (``http://127.0.0.1:5001``).
* ``app_requirements``, ``ml_model_metadata``, ``ml_model``, ``hw_constraints``, ``hw_resources`` and ``carbon_tracker``: the module nodes.
  The ``ml_model_metadata`` container also starts the Neo4j database used by the ML Model Metadata and ML Model Provider nodes.
  The ``carbon_tracker`` container mounts ``/sys`` read-only to measure the energy consumption.
* ``front_end``: the web front end of work package 4, which the GUI does not use.

To run your own version of a node instead, comment its entry in ``docker-compose.yaml``.

Provide privileges to the X localhost server and deploy the *SustainML Framework* using Docker compose, from the folder that contains ``docker-compose.yaml``:

.. code-block:: bash

    cd ~/SustainML && \
    xhost local:root && \
    docker compose up
