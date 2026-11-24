.. _installation_framework:

SustainML Framework installation
================================

The instructions for installing the :ref:`SustainML Framework <index_introduction>` from sources are provided in this page.
It is organized as follows:

* :ref:`installation_framework_dependencies`
* :ref:`installation_framework_build`
* :ref:`installation_framework_deployment`

The *SustainML Framework* is composed of different software modules, each one related to a specific task, which are specialized in solving the different parts of the framework.
Each of the modules conforms a **Node**, which imports its corresponding Python library, so that each node can be abstracted from the communication between other nodes.

The communication is performed through `DDS (Data Distribution Service) protocol <https://www.omg.org/omg-dds-portal/>`_, using the `eProsima Fast DDS <https://fast-dds.docs.eprosima.com/>`_ library.
During the installation process, some of the `Fast DDS requirements <https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html#requirements>`_ will need to be addressed.

The following sections describe the steps to install the *SustainML Framework* on **Ubuntu** (the continuous integration of the SustainML Library builds on Ubuntu 24.04, and the Docker image is based on Ubuntu 24.04).
The Python dependencies (for example, ``torch==2.4.1``) require Python 3.10, 3.11 or 3.12.

.. important::

    The ``sustainml-framework`` launcher looks for the workspace in ``~/SustainML/SustainML_ws``.
    Use that exact path, as in the commands below.

.. _installation_framework_dependencies:

SustainML Framework dependencies
--------------------------------

The following packages provide the tools required to install *SustainML* and its dependencies from command line.
General and build tools such as `curl <https://curl.se/>`_, `wget <https://www.gnu.org/software/wget/>`_, `git <https://git-scm.com/>`_, `CMake <https://cmake.org/>`_, `g++ <https://gcc.gnu.org/>`_, `Python3 <https://www.python.org/>`_ and `SWIG <https://www.swig.org/>`_ are required.
`Fast DDS dependencies <https://fast-dds.docs.eprosima.com/en/latest/notes/versions.html#library-dependencies>`_ such as `OpenSSL <https://www.openssl.org/>`_, `Asio <https://think-async.com/Asio/>`_, `TinyXML2 <https://github.com/leethomason/tinyxml2>`_, `LibP11 <https://github.com/OpenSC/libp11/>`_, and `SoftHSM2 <https://www.opendnssec.org/softhsm/>`_ are also needed.
`Qt 5.15 <https://www.qt.io/>`_ libraries and QML modules (Qt Quick, Qt Quick Controls 1 and 2, Qt Quick Dialogs, Qt Charts, Qt Labs QML Models and Qt SVG) are necessary for the GUI of the *SustainML Framework*.

.. code-block:: bash

    sudo apt update && \
    sudo apt install --yes --no-install-recommends \
        curl wget git cmake g++ build-essential python3 python3-pip python3-venv libpython3-dev swig \
        libssl-dev libasio-dev libtinyxml2-dev libp11-dev libengine-pkcs11-openssl softhsm2 \
        qtdeclarative5-dev libqt5charts5-dev qml-module-qtcharts qtquickcontrols2-5-dev libqt5svg5 \
        qml-module-qtquick-controls qml-module-qtquick-controls2 qml-module-qt-labs-qmlmodels qml-module-qtquick-dialogs

The ML Model Metadata and ML Model Provider nodes query a `Neo4j <https://neo4j.com/>`_ graph database with the models knowledge base.
Install Java 21, required by Neo4j, and add it to the path (the ``JAVA_HOME`` below is the one of ``amd64`` machines):

.. code-block:: bash

    sudo apt install -y openjdk-21-jdk && \
    echo 'export JAVA_HOME=/usr/lib/jvm/java-21-openjdk-amd64' >> ~/.bashrc && \
    echo 'export PATH=$JAVA_HOME/bin:$PATH' >> ~/.bashrc && \
    source ~/.bashrc

Install the latest Neo4j from the Neo4j Debian repository:

.. code-block:: bash

    sudo apt install -y gnupg ca-certificates wget && \
    wget -O - https://debian.neo4j.com/neotechnology.gpg.key | gpg --dearmor | sudo tee /usr/share/keyrings/neo4j.gpg > /dev/null && \
    echo "deb [signed-by=/usr/share/keyrings/neo4j.gpg] https://debian.neo4j.com stable latest" | sudo tee /etc/apt/sources.list.d/neo4j.list && \
    sudo apt update && \
    sudo apt install -y neo4j

The ML Model Metadata node, the Hugging Face MCP server used by the ML Model Provider node, and the model comparison of the backend run the ``llama3`` model through a local `Ollama <https://ollama.com/>`_ server (``http://localhost:11434``).
Install Ollama, which also starts it as a service, and pull the model:

.. code-block:: bash

    curl -fsSL https://ollama.com/install.sh | sh && \
    ollama pull llama3

.. _installation_framework_build:

Build SustainML Framework sources
---------------------------------

Create the SustainML workspace and a Python virtual environment, and download the repositories file that will be used to install *SustainML Framework* and all its repository dependencies (such as the `SustainML Library <https://github.com/eProsima/SustainML-Library>`_, `Fast DDS <https://github.com/eProsima/Fast-DDS>`_, or `Fast CDR <https://github.com/eProsima/Fast-CDR>`_):

.. code-block:: bash

    mkdir -p ~/SustainML/SustainML_ws/src && cd ~/SustainML && \
    python3 -m venv SustainML_venv && source SustainML_venv/bin/activate && \
    pip3 install -U colcon-common-extensions vcstool gdown && \
    cd ~/SustainML/SustainML_ws && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/sustainml.repos && \
    vcs import src < sustainml.repos

The nodes of work packages 1, 2 and 3 are git submodules of the *SustainML Library*.
Initialize them and install the Python dependencies of the nodes:

.. code-block:: bash

    cd ~/SustainML/SustainML_ws/src/sustainml_lib && \
    git submodule update --init --recursive && \
    pip3 install -r ~/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/requirements.txt

Download the ANN index of the models knowledge base (``rag/models_index.ann``) with `gdown <https://github.com/wkentaro/gdown>`_.
If this file is missing, the ML Model Provider node downloads it to ``~/.cache/sustainml`` on its first start.

.. code-block:: bash

    cd ~/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1 && \
    gdown 1TQvt1bSXares-I9l7Wki0Jge3oubRkOJ -O rag/models_index.ann

Build and install the workspace with `colcon <https://colcon.readthedocs.io/>`_, and source the generated environment:

.. code-block:: bash

    cd ~/SustainML/SustainML_ws && \
    colcon build && \
    source ~/SustainML/SustainML_ws/install/setup.bash

Finally, load the Neo4j database dumps shipped in ``sustainml-wp1/rag/neo4j_backup``.
Both the ``system`` database (which holds the user credentials the nodes connect with) and the ``neo4j`` database (the knowledge base) are loaded, with Neo4j stopped:

.. code-block:: bash

    NEO4J_BACKUP=$HOME/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/rag/neo4j_backup && \
    sudo systemctl stop neo4j && \
    sudo neo4j-admin database load system --from-path=$NEO4J_BACKUP --overwrite-destination=true && \
    sudo neo4j-admin database load neo4j --from-path=$NEO4J_BACKUP --overwrite-destination=true && \
    sudo chown -R neo4j:neo4j /var/lib/neo4j/data

.. _installation_framework_deployment:

SustainML Framework deployment
------------------------------

.. important::

    Before running the framework, set the ``HF_TOKEN`` environment variable to your personal `Hugging Face access token <https://huggingface.co/settings/tokens>`_.
    The backend reads it at start up (and warns if it is missing) and forwards it to the nodes that download models from the Hugging Face Hub, so that gated models can be loaded.

    .. code-block:: bash

        export HF_TOKEN=<your_huggingface_token>

.. note::

    All the nodes use the DDS domain ``0`` by default.
    To isolate several deployments, set the environment variable ``SUSTAINML_DOMAIN_ID`` to another domain ID before launching the framework.

    .. code-block:: bash

        export SUSTAINML_DOMAIN_ID=<domain_id>

The *SustainML Framework* GUI retrieves the user inputs and sends them, through the REST API of the backend node (``http://127.0.0.1:5001``), to the remaining nodes that conform the framework.
To run the complete framework, both the GUI application and the framework nodes need to be executed.
With the Python virtual environment activated and the workspace sourced, the ``sustainml-framework`` command runs all of them:

.. code-block:: bash

    source ~/SustainML/SustainML_venv/bin/activate && \
    source ~/SustainML/SustainML_ws/install/setup.bash && \
    sustainml-framework

This command:

#. Starts Neo4j with ``sudo systemctl start neo4j`` (so it may ask for the ``sudo`` password) and waits until it accepts connections on port ``7687``.
#. Starts the backend node (``sustainml-wp5/backend_node.py``), which hosts the orchestrator and the REST API, and waits until it listens on port ``5001``.
#. Starts the App Requirements, ML Model Metadata, ML Model Provider, Hardware Constraints, Hardware Resources Provider and Carbon Footprint nodes.
#. Runs the GUI (``sustainml``). When the GUI is closed, all the nodes are stopped, and so is Neo4j.

.. note::

    The *SustainML Framework* GUI application alone is run with the ``sustainml`` command.

Internet access is needed while the framework runs: the nodes download the ``BAAI/bge-large-en`` embedding model the first time and each Hugging Face model they evaluate (the ``HF_TOKEN`` token lets them load gated models), the Hugging Face search and comparison query the Hugging Face Hub, and the Carbon Footprint node detects the country of the machine from its public IP address (with `geocoder <https://geocoder.readthedocs.io/>`_) when the user does not choose one.

The Axelera YOLO models catalog, the FPGA U-Net models and predictor and the UPMEM simulator are part of the ``sustainml-wp2`` sources, and the Hugging Face MCP server is part of the ``sustainml-wp1`` sources (the ML Model Provider node starts it as a subprocess).
They are installed with the steps above and need no extra setup, hardware or token of their own.
