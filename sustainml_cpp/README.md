# SustainML C++ library (`sustainml_cpp`)

This package contains the core C++ implementation of SustainML logic and the public C++ API used by other SustainML components.

The public C++ types are designed to avoid exposing Fast DDS types in public headers to keep the SWIG Python wrapper generation simple. Implementation types generated from IDL use the `Impl` suffix and are wrapped by public types.

## Scope

This package is primarily intended for SustainML developers and contributors. End users should follow the installation and deployment instructions in the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework

## Build

This package is a colcon package and is built automatically as part of the SustainML workspace.

For development and verification purposes, it can be built independently:

`colcon build --packages-select sustainml_cpp`
