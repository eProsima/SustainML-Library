# SustainML – Work Package 4 (WP4)

This directory contains the **legacy web front end** of the SustainML project, developed as part of **Work Package 4 (WP4)**.

It is a Flask web application (`frontend_node.py`, with `index.html`, `data.html` and `styles.css`) that allowed users to:

- Enter the user input that defines the ML problem and constraints
- Upload datasets and inspect basic dataset properties
- Forward the user input to the backend and display the execution status and results

> [!NOTE]
> The SustainML Framework no longer uses this web front end: its user interface is the Qt/QML application of the
> [SustainML Framework](https://github.com/eProsima/SustainML-Framework) repository, which talks to the backend node of WP5.
> This code is kept for reference, and is still installed with the `sustainml_modules` package and run by the `front_end`
> container of the Docker deployment.

## Scope

WP4 components are integrated into the SustainML Framework through the `sustainml_modules` package and are **not intended to be used as standalone applications**.

End users should follow the installation and deployment instructions in the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework
