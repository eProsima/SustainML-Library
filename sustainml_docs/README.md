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

This package is a colcon package of the SustainML workspace. The documentation is only built when `BUILD_DOCS` is
enabled, with the Python requirements of this folder installed:

```bash
pip install -r sustainml_docs/requirements.txt
colcon build --packages-select sustainml_docs --cmake-args -DBUILD_DOCS=ON
```

To build the HTML pages directly with Sphinx, from this folder: `sphinx-build -b html . _build/html`.
