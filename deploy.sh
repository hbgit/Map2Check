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

git config --global user.email "${GIT_EMAIL}"
git config --global user.name "${GIT_NAME}"
git config --global push.default simple
#git remote set-url origin https://${GH_TOKEN}@github.com/${TRAVIS_REPO_SLUG}.git
git remote set-url origin git@github.com:${TRAVIS_REPO_SLUG}.git
#add

# ssh-keyscan github.com >> ~/.ssh/known_hosts
# ssh-keyscan -t rsa github.com >> /etc/ssh/known_hosts
ssh-keyscan -t rsa github.com >> ~/.ssh/known_hosts

echo -e "Host github.com\n\tStrictHostKeyChecking no\n" >> /etc/ssh/ssh_config
echo -e "Host github.com\n\tStrictHostKeyChecking no\n" >> ~/.ssh/config


cat /etc/ssh/ssh_config
cat ~/.ssh/config
cat ~/.ssh/known_hosts

export GIT_TAG=v7.3.1.$YEAR-$MONTH.$TRAVIS_BUILD_NUMBER
echo "${bold}${green}DEPLOYING $GIT_TAG"

cd ${TRAVIS_BUILD_DIR}

sudo git remote -v
git fetch --tags

echo "${bold}${green}Creating TAG ..."
git tag ${GIT_TAG} -a -m "Deploying from Travis ${GIT_TAG}"
echo "${bold}${green}Pushing develop ..."
#sudo 
git push origin develop && git push origin develop --tags

echo "${bold}${green}-------------------------------"
echo "${bold}${green}Deploying ${GIT_TAG}"
echo "${bold}${green}-------------------------------"
echo "\n\n"
