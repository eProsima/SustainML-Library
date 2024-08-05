.. _installation_library:

SustainML Stand-alone library installation
==========================================

The instructions for installing the :ref:`SustainML Library <index_introduction>` from sources are provided in this page.
It is organized as follows:

.. contents::
    :local:
    :backlinks: none
    :depth: 2

The *SustainML Library* is composed of different software modules, each one related to specific task, which are specialized in solving the different parts of the framework.
Each of the modules conforms a **Node**, which shall import its corresponding Python library, so that each node can be abstracted from the communication between other nodes.

The communication is performed through `DDS (Data Distribution Service) protocol <https://www.omg.org/omg-dds-portal/>`_, using the `eProsima Fast DDS <https://fast-dds.docs.eprosima.com/>`_ library.
During the installation process, some of the `Fast DDS requirements <https://fast-dds.docs.eprosima.com/en/latest/installation/sources/sources_linux.html#requirements>`_ will need to be addressed.

The following sections describe the steps to install the *SustainML Library* on **Ubuntu** and **MacOS**.

.. _installation_library_dependencies:

SustainML Library dependencies
------------------------------

The following packages provide the tools required to install SustainML and its dependencies from command line.
General and build tools such as `wget <https://www.gnu.org/software/wget/>`_, `git <https://git-scm.com/>`_, `CMake <https://cmake.org/>`_, `g++ <https://gcc.gnu.org/>`_, `Python3 <https://www.python.org/>`_ and `SWIG <https://www.swig.org/>`_ are required.
`Fast DDS dependencies <https://fast-dds.docs.eprosima.com/en/latest/notes/versions.html#library-dependencies>`_ such as `OpenSSL <https://www.openssl.org/>`_, `Asio <https://think-async.com/Asio/>`_, `TinyXML2 <https://github.com/leethomason/tinyxml2>`_, `LibP11 <https://github.com/OpenSC/libp11/>`_, and `SoftHSM2 <https://www.opendnssec.org/softhsm/>`_ are also needed.

Install them using the package manager of the appropriate OS distribution.

.. tabs::

    .. group-tab:: Ubuntu

        On **Ubuntu** use the following command to install all the dependencies:

        .. code-block:: bash

            apt install --yes --no-install-recommends \
                wget git cmake g++ build-essential python3 python3-pip python3.10-venv libpython3-dev swig \
                libssl-dev libasio-dev libtinyxml2-dev libp11-dev libengine-pkcs11-openssl softhsm2

    .. group-tab:: MacOS

        On **MacOS** use the following command to install all the dependencies:

        .. code-block:: bash

            brew install \
                wget git llvm cmake gcc python swig openssl@3.0 asio tinyxml2 libp11 softhsm

.. _installation_library_build:

Build SustainML Library sources
-------------------------------

Create a SustainML directory and download the repositories file that will be used to install *SustainML Library*, and all its repository dependencies (such as the `Fast DDS <https://github.com/eProsima/Fast-DDS>`_ or `Fast CDR <https://github.com/eProsima/Fast-CDR>`_).
The following command builds and installs the *SustainML library* and its dependencies.


.. tabs::

    .. group-tab:: Ubuntu

        .. code-block:: bash

            mkdir -p ~/SustainML/SustainML_ws/src && cd ~/SustainML && \
            python3 -m venv SustainML_venv && source SustainML_venv/bin/activate && cd ~/SustainML/SustainML_ws && \
            pip3 install -U colcon-common-extensions vcstool && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Library/main/sustainml.repos && \
            vcs import src < sustainml.repos && \
            git submodule update --init --recursive && \
            pip3 install -r ~/SustainML/SustainML_ws/src/sustainml_docs/requirements.txt && \
            colcon build

    .. group-tab:: MacOS

        .. code-block:: bash

            mkdir -p ~/SustainML/SustainML_ws/src && cd ~/SustainML && \
            python3 -m venv SustainML_venv && source SustainML_venv/bin/activate && cd ~/SustainML/SustainML_ws && \
            pip3 install -U colcon-common-extensions vcstool && \
            wget https://raw.githubusercontent.com/eProsima/SustainML-Library/macos-compilation/sustainml.repos && \
            vcs import src < sustainml.repos && \
            git submodule update --init --recursive && \
            pip3 install -r ~/SustainML/SustainML_ws/src/sustainml_docs/requirements.txt && \
            colcon build --cmake-args -DCMAKE_CXX_STANDARD=17
