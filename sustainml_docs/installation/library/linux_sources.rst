.. _installation_library_linux:

SustainML Stand-alone library on Linux
======================================

The instructions for installing the :ref:`SustainML Library <index_introduction>` from sources are provided in this page.
It is organized as follows:

.. contents::
    :local:
    :backlinks: none
    :depth: 2

The *SustainML Library* is composed of different software modules, each one related to specific task, which are specialized in solving the different parts of the framework.
Each of the modules conforms a **Node**, which shall import its corresponding Python library, so that each Node can be abstracted from the communication between other nodes.

The communication is performed through DDS (Data Distribution Service) protocol, using the eProsima Fast DDS library.
During the installation process, some of the Fast DDS requirements will need to be addressed.

The following sections describe the steps to install the SustainML Library on **Ubuntu**.

.. _installation_library_linux_dependencies:

SustainML Library dependencies
--------------------------------

The following packages provide the tools required to install SustainML and its dependencies from command line.
General and build tools such as wget, git, cmake, g++, python3 and swig are required.
Fast DDS dependencies such as OpenSSL, Asio, TinyXML2, PKCS11, and SoftHSM2 are also needed.

Install them using the package manager of the appropriate Linux distribution.
On **Ubuntu** use the following command to install all the dependencies:

.. code-block:: bash

    apt install --yes --no-install-recommends \
        wget git cmake g++ build-essential python3 python3-pip libpython3-dev swig \
        libssl-dev libasio-dev libtinyxml2-dev libp11-dev libengine-pkcs11-openssl softhsm2 && \
    pip3 install -U \
        colcon-common-extensions vcstool

.. _installation_library_linux_build:

Build SustainML Library sources
-------------------------------

Create a SustainML directory and download the repositories file that will be used to install SustainML Library, and all its repository dependencies (such as the SustainML library, Fast DDS, or Fast CDR).
The following command also builds and installs the SustainML library.

.. code-block:: bash

    mkdir -p ~/SustainML/src && cd ~/SustainML && \
    wget https://raw.githubusercontent.com/eProsima/SustainML-Library/main/sustainml.repos && \
    vcs import src < sustainml.repos && \
    colcon build
