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
                wget git cmake g++ build-essential python3 python3-pip libpython3-dev swig \
                libssl-dev libasio-dev libtinyxml2-dev libp11-dev libengine-pkcs11-openssl softhsm2 \
                qtdeclarative5-dev libqt5charts5-dev qtquickcontrols2-5-dev libqt5svg5 qml-module-qtquick-controls \
                qml-module-qtquick-controls2 && \
            pip3 install -U --user \
                colcon-common-extensions vcstool

    .. group-tab:: MacOS

        `Homebrew <https://brew.sh/>`_ is a package manager for MacOS that simplifies the installation of software.
        Install Homebrew following it installation instructions.

        On **MacOS** use the following command to install all the dependencies:

        .. code-block:: bash

            brew install \
                wget git cmake clang python3 python3-pip libpython3-dev swig \
                libssl-dev libasio-dev libtinyxml2-dev libp11-dev libengine-pkcs11-openssl softhsm2 qt5 && \
            pip3 install -U --user \
                colcon-common-extensions vcstool

.. _installation_framework_build:

Build SustainML Framework sources
---------------------------------

Create a SustainML directory and download the repositories file that will be used to install *SustainML Framework*, and all its repository dependencies (such as the `SustainML Library <https://github.com/eProsima/SustainML-Library>`_, `Fast DDS <https://github.com/eProsima/Fast-DDS>`_, or `Fast CDR <https://github.com/eProsima/Fast-CDR>`_).
The following command also builds and installs the SustainML framework, and the generated libraries and applications are sourced.

.. tabs::

    .. group-tab:: Ubuntu

        .. code-block:: bash

            mkdir -p ~/SustainML/src && cd ~/SustainML && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/sustainml.repos && \
            vcs import src < sustainml.repos && \
            colcon build && \
            source ~/SustainML/install/setup.bash

    .. group-tab:: MacOS

        .. code-block:: bash

            mkdir -p ~/SustainML/src && cd ~/SustainML && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Framework/main/sustainml.repos && \
            vcs import src < sustainml.repos && \
            colcon build && \
            source ~/SustainML/install/setup.bash

.. _installation_framework_deployment:

SustainML Framework deployment
------------------------------

The *SustainML Framework* application retrieves the user inputs and deliver the information to the remain nodes that conform the framework.
To run the complete framework, both GUI application and framework nodes need to be executed.
The following script runs all of them:

.. tabs::

    .. group-tab:: Ubuntu

        .. code-block:: bash

            bash -c " \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/app_requirements_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_metadata_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_provider_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_constraints_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_resources_provider_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp3/carbon_footprint_node.py & \
                sustainml"

    .. group-tab:: MacOS

        .. code-block:: bash

            bash -c " \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/app_requirements_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_metadata_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp1/ml_model_provider_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_constraints_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp2/hw_resources_provider_node.py & \
                python3 ~/SustainML/src/sustainml_lib/sustainml_modules/sustainml_modules/sustainml-wp3/carbon_footprint_node.py & \
                sustainml"

.. note::

    The *SustainML Framework* GUI application is run with the ``sustainml`` command.
