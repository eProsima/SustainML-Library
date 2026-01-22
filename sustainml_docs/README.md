# SustainML documentation (`sustainml_docs`)

This package contains the documentation sources for the SustainML project.
It is used to build the project documentation published on ReadTheDocs.

The documentation describes the SustainML Framework architecture, components, and usage.

## Scope

This package is intended for SustainML developers and contributors maintaining the project documentation.

End users should refer to the published documentation or the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework
https://sustainml.readthedocs.io/en/latest/

## Build

This package is a colcon package and is built automatically as part of the SustainML workspace.

For development and verification purposes, it can be built independently:

`colcon build --packages-select sustainml_docs`
