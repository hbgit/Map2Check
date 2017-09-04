############################################################
# Dockerfile to build map2check container images
# based on Ubuntu
# Usage:
#  By docker https://hub.docker.com:
#   $ docker pull hrocha/mapdevel
#   $ mkdir $(pwd)/mapdevel
#   $ docker run -it --name=mapdevel -v $(pwd):/home/map2check/devel_tool/host:Z hrocha/mapdevel /bin/bash
#  By gitclone https://github.com/hbgit/Map2Check:
#   $ docker build -t hrocha/mapdevel .
#   $ docker run -it --name=mapdevel -v $(pwd):/home/map2check/devel_tool/mygitclone:Z hrocha/mapdevel /bin/bash
#
# The docker user is "map2check" and the password is "map2check"
# Docker tips:
#  You can check that the container still exists by running: $ docker ps -a
#  You can restart the container by running: docker start -ai mapdevel
#  You can run any command in running container just knowing its ID (or name): docker exec <container_id_or_name> echo "Hello from container!"
############################################################
FROM ubuntu:16.10

# Metadata indicating an image maintainer.
MAINTAINER <herberthb12@gmail.com>

ENV BUILD_DIR=/home/map2check/devel_tool/build \
    MAP_SRC=/home/map2check/devel_tool/map_src_on_docker

# Fix for docker for windows
RUN echo 'debconf debconf/frontend select Noninteractive' | debconf-set-selections

# Update the repository sources list
RUN apt-get update

############depois tu me manda uma cópia do teu mirrors ###### BEGIN INSTALLATION ######################
# Devel packages
RUN apt-get install -y sudo \
	python \
	python-minimal \
	python-pip \
	git \
	libboost-all-dev \
	libgtest-dev \
	wget \
	build-essential \
	cmake \
	pkg-config \
	libz-dev \
	curl \
	bison \
	flex \
	bc \
	libcap-dev \
	libncurses5-dev \
	unzip

# Packages to Run tool
RUN apt-get install -y python-pycparser \
	python-pyparsing \
	python-networkx \
	uncrustify

# Clean packages installation
RUN apt-get clean


# Setting entrypoint
# Create ``map2check`` user for container with password ``map2check``.
# and give it password-less sudo access (temporarily so we can use the TravisCI scripts)
RUN useradd -m map2check && \
    echo map2check:map2check | chpasswd && \
    cp /etc/sudoers /etc/sudoers.bak && \
    echo 'map2check  ALL=(root) NOPASSWD: ALL' >> /etc/sudoers

USER map2check
RUN mkdir /home/map2check/devel_tool
WORKDIR /home/map2check/devel_tool/


### Buildind Map2Check tool

# Copy across source files needed for build
RUN mkdir ${MAP_SRC}
ADD / ${MAP_SRC}

# Set map2check user to be owner
RUN sudo chown -R map2check:map2check ${MAP_SRC}/*

# Build Map2Check
RUN cd ${MAP_SRC} && sudo ./build-map2check.sh

RUN sudo chown -R map2check:map2check ${MAP_SRC}/*
VOLUME /home/map2check/devel_tool/

# Revoke password-less sudo and Set up sudo access for the ``map2check`` user so it
# requires a password
USER root
RUN mv /etc/sudoers.bak /etc/sudoers && \
    echo 'map2check  ALL=(root) ALL' >> /etc/sudoers
USER map2check



##################### INSTALLATION END #####################
