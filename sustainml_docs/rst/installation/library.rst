.. _installation_library:

SustainML Stand-alone library installation
==========================================

The instructions for installing the :ref:`SustainML Library <index_introduction>` from sources, without the GUI of the *SustainML Framework*, are provided in this page.
It is organized as follows:

* :ref:`installation_library_dependencies`
* :ref:`installation_library_build`
* :ref:`installation_library_run`

The *SustainML Library* is composed of different software modules, each one related to a specific task, which are specialized in solving the different parts of the framework.
Each of the modules conforms a **Node**, which imports its corresponding Python library, so that each node can be abstracted from the communication between other nodes.

The communication is performed through `DDS (Data Distribution Service) protocol <https://www.omg.org/omg-dds-portal/>`_, using the `eProsima Fast DDS <https://fast-dds.docs.eprosima.com/>`_ library.
During the installation process, some of the `Fast DDS requirements <https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html#requirements>`_ will need to be addressed.

The following sections describe the steps to install the *SustainML Library* on **Ubuntu** (the continuous integration of the library builds it on Ubuntu 24.04).
The Python dependencies (for example, ``torch==2.4.1``) require Python 3.10, 3.11 or 3.12.

.. _installation_library_dependencies:

SustainML Library dependencies
------------------------------

The following packages provide the tools required to install SustainML and its dependencies from command line.
General and build tools such as `curl <https://curl.se/>`_, `wget <https://www.gnu.org/software/wget/>`_, `git <https://git-scm.com/>`_, `CMake <https://cmake.org/>`_, `g++ <https://gcc.gnu.org/>`_, `Python3 <https://www.python.org/>`_ and `SWIG <https://www.swig.org/>`_ are required.
`Fast DDS dependencies <https://fast-dds.docs.eprosima.com/en/latest/notes/versions.html#library-dependencies>`_ such as `OpenSSL <https://www.openssl.org/>`_, `Asio <https://think-async.com/Asio/>`_, `TinyXML2 <https://github.com/leethomason/tinyxml2>`_, `LibP11 <https://github.com/OpenSC/libp11/>`_, and `SoftHSM2 <https://www.opendnssec.org/softhsm/>`_ are also needed.

.. code-block:: bash

    sudo apt update && \
    sudo apt install --yes --no-install-recommends \
        curl wget git cmake g++ build-essential python3 python3-pip python3-venv libpython3-dev swig \
        libssl-dev libasio-dev libtinyxml2-dev libp11-dev libengine-pkcs11-openssl softhsm2

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

.. _installation_library_build:

Build SustainML Library sources
-------------------------------

Create a SustainML workspace and a Python virtual environment, and download the repositories file of the *SustainML Library*, which also fetches its repository dependencies (such as `Fast DDS <https://github.com/eProsima/Fast-DDS>`_ or `Fast CDR <https://github.com/eProsima/Fast-CDR>`_):

.. code-block:: bash

    mkdir -p ~/SustainML/SustainML_ws/src && cd ~/SustainML && \
    python3 -m venv SustainML_venv && source SustainML_venv/bin/activate && \
    pip3 install -U colcon-common-extensions vcstool gdown && \
    cd ~/SustainML/SustainML_ws && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Library/main/sustainml.repos && \
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

.. _installation_library_run:

Run SustainML Modules and Backend
---------------------------------

Before running the backend, set the ``HF_TOKEN`` environment variable to your personal `Hugging Face access token <https://huggingface.co/settings/tokens>`_.
Optionally, set ``SUSTAINML_DOMAIN_ID`` to use a DDS domain other than the default ``0``.

.. code-block:: bash

    export HF_TOKEN=<your_huggingface_token>
    export SUSTAINML_DOMAIN_ID=<domain_id>

The following commands start Neo4j, the backend node (which hosts the orchestrator and serves a REST API on ``http://127.0.0.1:5001``) and the module nodes.
The backend node is started first, and the module nodes once it listens on port ``5001``, so that it does not miss the status that each node publishes when it becomes ready:

.. code-block:: bash

    source ~/SustainML/SustainML_venv/bin/activate
    source ~/SustainML/SustainML_ws/install/setup.bash
    sudo systemctl start neo4j
    until (exec 3<>/dev/tcp/127.0.0.1/7687) 2>/dev/null; do sleep 1; done
    cd ~/SustainML/SustainML_ws/build/sustainml_modules/lib/sustainml_modules
    python3 sustainml-wp5/backend_node.py &
    until (exec 3<>/dev/tcp/127.0.0.1/5001) 2>/dev/null; do sleep 1; done
    python3 sustainml-wp1/app_requirements_node.py &
    python3 sustainml-wp1/ml_model_metadata_node.py &
    python3 sustainml-wp1/ml_model_provider_node.py &
    python3 sustainml-wp2/hw_constraints_node.py &
    python3 sustainml-wp2/hw_resources_provider_node.py &
    python3 sustainml-wp3/carbon_footprint_node.py &

Internet access is needed while the framework runs: the nodes download the ``BAAI/bge-large-en`` embedding model the first time and each Hugging Face model they evaluate (the ``HF_TOKEN`` token lets them load gated models), the Hugging Face search and comparison query the Hugging Face Hub, and the Carbon Footprint node detects the country of the machine from its public IP address (with `geocoder <https://geocoder.readthedocs.io/>`_) when the user does not choose one.

These are the same steps that the ``sustainml-framework`` launcher of the *SustainML Framework* runs before opening its GUI (see :ref:`installation_framework_deployment`).
Clients, such as that GUI, interact with the nodes through the REST API of the backend node.
