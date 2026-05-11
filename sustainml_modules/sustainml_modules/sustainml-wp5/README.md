# SustainML – Work Package 5 (WP5)

This directory contains the implementation of **Work Package 5 (WP5)** components of the SustainML project.

WP5 provides the **backend and orchestration functionality** of the SustainML Framework. It is responsible for:

- Receiving user input from the frontend
- Running the Orchestrator node and coordinating task execution
- Providing execution status and collecting results produced by the framework

WP5 acts as the central coordination layer between the frontend and the remaining SustainML components.

## Scope

WP5 components are integrated into the SustainML Framework through the `sustainml_modules` package and are **not intended to be used as standalone services**.

This code is intended for SustainML developers and contributors working on framework orchestration and backend logic.

End users should follow the installation and deployment instructions in the SustainML Framework repository:

https://github.com/eProsima/SustainML-Framework
