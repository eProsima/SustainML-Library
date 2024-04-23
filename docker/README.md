# SustainML docker

This section describes the docker tools to deploy the SustainML framework.

In this folder, there are three different files:

1. ``SustainML.Dockerfile``: dockerfile that, based on the latest Vulcanexus iron image, downloads and installs the SustainML framework dependencies.
2. ``run.bash``: script used as an entrypoint in the SustainML dockerfile to run the selected node based on an input argument.
3. ``docker-compos.yaml``: compose file that uses both docker image and entrypoint to deploy all the SustainML Framework nodes.

To run the SustainML Framework using docker, please follow these steps:

1. Create docker image based on the Dockerfile: ``docker build -f <path_to_dockerfile>/SustainML.Dockerfile -t sustainml:v0.1.0``
2. If you want to run your own SustainML node apart, comment the corresponding container entry in the ``docker-compose.yaml`` file
3. Deploy the compose: ``docker compose up``
