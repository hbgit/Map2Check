#!/bin/bash

# ------------------------
# Set Terminal Font Colors
# ------------------------

bold=$(tput bold)
red=$(tput setaf 1)
green=$(tput setaf 2)
yellow=$(tput setaf 3)
blue=$(tput setaf 4)
magenta=$(tput setaf 5)
cyan=$(tput setaf 6)
white=$(tput setaf 7)
defaultcolor=$(tput setaf default)

YEAR=$(date +"%Y")
MONTH=$(date +"%m")
DAY=$(date +%d)
shortHash=$(git rev-parse --short HEAD)

GIT_TAG=v7.3.1.$YEAR-$MONTH.$DAY.$shortHash
echo "${bold}${green}Tag id =  $GIT_TAG"

git fetch --tags

echo "${bold}${green}Creating TAG ..."
git tag ${GIT_TAG} -a -m "Deploying ${GIT_TAG}"
git push origin develop --tags

echo "${defaultcolor}"
