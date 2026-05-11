# SustainML Python API (`sustainml_py`)

This package provides the Python API for SustainML.
It is generated from the public C++ API defined in `sustainml_cpp` using SWIG (see `sustainml_swig`).

## Scope

This package is intended for SustainML developers and contributors working on the Python bindings and API integration.

End users should follow the installation and deployment instructions in the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework

## Build

This package is a colcon package and is built automatically as part of the SustainML workspace.

For development and verification purposes, it can be built independently:

`colcon build --packages-select sustainml_py`
