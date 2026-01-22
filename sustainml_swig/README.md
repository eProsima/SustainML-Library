# SustainML SWIG bindings (`sustainml_swig`)

This package contains the SWIG interface files and CMake configuration used to generate the Python bindings for the SustainML C++ library.

SWIG is used to expose the public C++ API defined in `sustainml_cpp` to Python, producing the `sustainml_py` package.

## Scope

This package is intended for SustainML developers and contributors working on the C++–Python bindings.

End users should follow the installation and deployment instructions in the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework

## Design notes

Only DDS-independent public C++ types are exposed to SWIG.
Implementation types generated from IDL files and dependent on Fast DDS are intentionally hidden from the Python interface.

## Build

This package is a colcon package and is built automatically as part of the SustainML Framework build process.
