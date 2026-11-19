# SustainML Python API (`sustainml_py`)

This package provides the Python API for SustainML.
It is a set of Python classes, one per node, that wrap the SWIG bindings of the public C++ API of `sustainml_cpp`
(the `sustainml_swig` module, see `sustainml_swig`).

The `examples` folder has a minimal example of each node, and the `test` folder the tests of the package.

## Scope

This package is intended for SustainML developers and contributors working on the Python bindings and API integration.

End users should follow the installation and deployment instructions in the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework

## Build

This package is a colcon package and is built automatically as part of the SustainML workspace.

For development and verification purposes, it can be built independently:

`colcon build --packages-select sustainml_py`

The tests are run with:

`colcon test --packages-select sustainml_py`
