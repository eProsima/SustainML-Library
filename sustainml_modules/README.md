# SustainML modules (`sustainml_modules`)

This package contains the SustainML Python modules that implement the framework pipeline nodes.

These modules use the SustainML Python API (`sustainml_py`) and are executed as part of the SustainML Framework deployment.

## Scope

This package is intended for SustainML developers and contributors working on the framework modules.

End users should follow the installation and deployment instructions in the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework

## Build

This package is a colcon package and is built automatically as part of the SustainML workspace.

For development and verification purposes, it can be built independently:

`colcon build --packages-select sustainml_modules`
