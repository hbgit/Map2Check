############################################################
# Dockerfile to build map2check container images
# based on Ubuntu
# Usage: 
#  $ docker build -t hrocha/mapdevel .
#  $ docker run -it --name=mapdevel -v $(pwd):/home/map2check/devel_tool/ hrocha/mapdevel /bin/bash
############################################################
FROM ubuntu:16.10

# Metadata indicating an image maintainer.
MAINTAINER <herberthb12@gmail.com>

ENV BUILD_DIR=/home/map2check/devel_tool/build \
    MAP_SRC=/home/map2check/devel_tool/map_src

# Update the repository sources list
RUN apt-get update

################## BEGIN INSTALLATION ######################
# Devel packages
RUN apt-get install -y sudo \
	apt-utils \
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
VOLUME /home/map2check/devel_tool/


### Buildind Map2Check tool

# Copy across source files needed for build
RUN mkdir ${MAP_SRC}
ADD / ${MAP_SRC}

# Create build directory
RUN mkdir ${BUILD_DIR}

# Set klee user to be owner
RUN sudo chown --recursive map2check: ${MAP_SRC}

RUN ls -alh ${MAP_SRC}

# Build KLEE (use TravisCI script)
RUN ${MAP_SRC}/map2check-build.sh

# Revoke password-less sudo and Set up sudo access for the ``map2check`` user so it
# requires a password
USER root
RUN mv /etc/sudoers.bak /etc/sudoers && \
    echo 'map2check  ALL=(root) ALL' >> /etc/sudoers
USER map2check
            


##################### INSTALLATION END #####################
