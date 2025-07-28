.. _installation_framework:

SustainML Framework installation
================================

The instructions for installing the :ref:`SustainML Framework <index_introduction>` from sources are provided in this page.
It is organized as follows:

.. contents::
    :local:
    :backlinks: none
    :depth: 2

The *SustainML Framework* is composed of different software modules, each one related to specific task, which are specialized in solving the different parts of the framework.
Each of the modules conforms a **Node**, which shall import its corresponding Python library, so that each node can be abstracted from the communication between other nodes.

The communication is performed through `DDS (Data Distribution Service) protocol <https://www.omg.org/omg-dds-portal/>`_, using the `eProsima Fast DDS <https://fast-dds.docs.eprosima.com/>`_ library.
During the installation process, some of the `Fast DDS requirements <https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html#requirements>`_ will need to be addressed.

The following sections describe the steps to install the *SustainML Framework* on **Ubuntu** and **MacOS**.

.. _installation_framework_dependencies:

SustainML Framework dependencies
--------------------------------

The following packages provide the tools required to install *SustainML* and its dependencies from command line.
General and build tools such as `wget <https://www.gnu.org/software/wget/>`_, `git <https://git-scm.com/>`_, `CMake <https://cmake.org/>`_, `g++ <https://gcc.gnu.org/>`_, `Python3 <https://www.python.org/>`_ and `SWIG <https://www.swig.org/>`_ are required.
`Fast DDS dependencies <https://fast-dds.docs.eprosima.com/en/latest/notes/versions.html#library-dependencies>`_ such as `OpenSSL <https://www.openssl.org/>`_, `Asio <https://think-async.com/Asio/>`_, `TinyXML2 <https://github.com/leethomason/tinyxml2>`_, `LibP11 <https://github.com/OpenSC/libp11/>`_, and `SoftHSM2 <https://www.opendnssec.org/softhsm/>`_ are also needed.
`Qt <https://www.qt.io/>`_ libraries and modules are necessary for the GUI components of the *SustainML Framework*.

Install them using the package manager of the appropriate Linux distribution.

.. tabs::

    .. group-tab:: Ubuntu

        On **Ubuntu** use the following command to install all the dependencies:

        .. code-block:: bash

            apt install --yes --no-install-recommends \
                curl wget git cmake g++ build-essential python3 python3-pip python3-venv libpython3-dev swig \
                libssl-dev libasio-dev libtinyxml2-dev libp11-dev libengine-pkcs11-openssl softhsm2 \
                qtdeclarative5-dev libqt5charts5-dev qml-module-qtcharts qtquickcontrols2-5-dev libqt5svg5 \
                qml-module-qtquick-controls qml-module-qtquick-controls2 qml-module-qt-labs-qmlmodels

    .. group-tab:: MacOS

        `Homebrew <https://brew.sh/>`_ is a package manager for MacOS that simplifies the installation of software.
        Install Homebrew following it installation instructions.

        On **MacOS** use the following command to install all the dependencies:

        .. code-block:: bash

            brew install \
                curl wget git llvm cmake gcc python swig openssl@3.0 asio tinyxml2 libp11 softhsm qt@5

.. _installation_framework_build:

Build SustainML Framework sources
---------------------------------

Create a SustainML directory and download the repositories file that will be used to install *SustainML Framework*, and all its repository dependencies (such as the `SustainML Library <https://github.com/eProsima/SustainML-Library>`_, `Fast DDS <https://github.com/eProsima/Fast-DDS>`_, or `Fast CDR <https://github.com/eProsima/Fast-CDR>`_).
The following command also builds and installs the SustainML framework and all its dependencies, and the generated libraries and applications are sourced.

.. tabs::

    .. group-tab:: Ubuntu

        .. code-block:: bash

            mkdir -p ~/SustainML/SustainML_ws/src && cd ~/SustainML && \
            python3 -m venv SustainML_venv && source SustainML_venv/bin/activate && \
            pip3 install -U colcon-common-extensions vcstool && \
            curl -fsSL https://ollama.com/install.sh | sh && ollama pull llama3 && cd ~/SustainML/SustainML_ws && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/sustainml.repos && \
            vcs import src < sustainml.repos && cd ~/SustainML/SustainML_ws/src/sustainml_lib && \
            git submodule update --init --recursive && \
            pip3 install -r ~/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/requirements.txt && \
            cd ~/SustainML/SustainML_ws && colcon build && \
            source ~/SustainML/SustainML_ws/install/setup.bash && \
            sudo neo4j-admin database load system \
              --from-path=/home/eprosima/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/rag/neo4j_backup \
              --overwrite-destination=true && \
            sudo neo4j-admin database load neo4j \
              --from-path=/home/eprosima/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/rag/neo4j_backup \
              --overwrite-destination=true && \
            sudo chown -R neo4j:neo4j /var/lib/neo4j/data

    .. group-tab:: MacOS

        .. code-block:: bash

            mkdir -p ~/SustainML/SustainML_ws/src && cd ~/SustainML && \
            python3 -m venv SustainML_venv && source SustainML_venv/bin/activate && \
            pip3 install -U colcon-common-extensions vcstool && \
            curl -fsSL https://ollama.com/install.sh | sh && ollama pull llama3 && cd ~/SustainML/SustainML_ws && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/macos-compilation/sustainml.repos && \
            vcs import src < sustainml.repos && cd ~/SustainML/SustainML_ws/src/sustainml_lib && \
            git submodule update --init --recursive && \
            pip3 install -r ~/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/requirements.txt && \
            cd ~/SustainML/SustainML_ws && colcon build --packages-up-to sustainml --cmake-args -DCMAKE_CXX_STANDARD=17 \
                    -DQt5_DIR=/usr/local/opt/qt5/lib/cmake/Qt5 && \
            cd ~/SustainML/SustainML_ws/install && source setup.bash && \
            sudo neo4j-admin database load system \
              --from-path=/home/eprosima/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/rag/neo4j_backup \
              --overwrite-destination=true && \
            sudo neo4j-admin database load neo4j \
              --from-path=/home/eprosima/SustainML/SustainML_ws/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/rag/neo4j_backup \
              --overwrite-destination=true && \
            sudo chown -R neo4j:neo4j /var/lib/neo4j/data

.. _installation_framework_deployment:

SustainML Framework deployment
------------------------------

.. important::
    Before running the framework, make sure you have set the ``HF_TOKEN`` environment variable on your host to your personal Hugging Face access token.

.. note::
    You can also override the default DDS domain ID for all nodes by setting the environment variable ``SUSTAINML_DOMAIN_ID`` on the host before launching the containers.

The *SustainML Framework* application retrieves the user inputs and deliver the information to the remain nodes that conform the framework.
To run the complete framework, both GUI application and framework nodes need to be executed.
The following command run each module, the backend orchestrator and the frontend application.

.. tabs::

    .. group-tab:: Ubuntu

        .. code-block:: bash

            sustainml-framework

    .. group-tab:: MacOS

        .. code-block:: bash

            sustainml-framework

.. note::

    The *SustainML Framework* GUI application alone is run with the ``sustainml`` command.
