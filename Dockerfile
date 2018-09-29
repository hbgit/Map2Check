############################################################
# Dockerfile to build map2check build environment container images
# based on Ubuntu
# Usage:
# 
#  By gitclone https://github.com/hbgit/Map2Check:
#   $ docker build -t hrocha/mapdevel --no-cache -f Dockerfile .
#   $ docker run -it --name=mapdevel -v $(pwd):/home/map2check/devel_tool/mygitclone:Z hrocha/mapdevel /bin/bash
#
# The docker user is "map2check" and the password is "map2check"
# Docker tips:
#  You can check that the container still exists by running: $ docker ps -a
#  You can restart the container by running: docker start -ai mapdevel
############################################################
FROM ubuntu:16.04

# Metadata indicating an image maintainer.
MAINTAINER <herberthb12@gmail.com>


# Update the repository sources list
RUN apt-get update

############depois tu me manda uma cópia do teu mirrors ###### BEGIN INSTALLATION ######################
# Devel packages
RUN apt-get install -y sudo \	
    build-essential \
    libboost-all-dev \
    libboost-program-options-dev \
    cmake \
    ninja-build \
    curl \
    unionfs-fuse \
    vim \
    doxygen \
    graphviz \
    python3 \
    python3-pip \
    subversion \
    libgmp-dev \
    libmpfr-dev
    
    
# Modules Python3
RUN pip3 install pyyaml \
                 psutil

# Clean packages installation
RUN apt-get clean

RUN useradd -m map2check && \
    echo map2check:map2check | chpasswd && \
    cp /etc/sudoers /etc/sudoers.bak && \
    echo 'map2check  ALL=(root) NOPASSWD: ALL' >> /etc/sudoers

USER map2check
RUN mkdir /home/map2check/devel_tool
WORKDIR /home/map2check/devel_tool/

RUN cd /home/map2check/devel_tool/
RUN sudo chown -R map2check:map2check .

VOLUME /home/map2check/devel_tool/
# Revoke password-less sudo and Set up sudo access for the ``map2check`` user so it
# requires a password
USER root
RUN mv /etc/sudoers.bak /etc/sudoers && \
    echo 'map2check  ALL=(root) ALL' >> /etc/sudoers
USER map2check
##################### INSTALLATION END #####################
