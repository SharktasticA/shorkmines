#!/bin/bash

######################################################
##         SHORK ENTERTAINMENT - SHORKMINES         ##
######################################################
## shorkmines install script                        ##
######################################################
## Licence: MIT                                     ##
######################################################
## Kali (links.sharktastica.co.uk)                  ##
######################################################



set -e



BLUE='\033[0;34m'
GREEN='\033[0;32m'
RED='\033[0;31m'
YELLOW='\033[1;33m'
RESET='\033[0m'



if ! gcc --version >/dev/null 2>&1; then
    echo -e "${RED}ERROR: GCC is required for compiling shorkmines${RESET}"
    exit 1
fi

if ! git --version >/dev/null 2>&1; then
    echo -e "${RED}ERROR: git is required for compiling shorkmines${RESET}"
    exit 1
fi

if ! make --version >/dev/null 2>&1; then
    echo -e "${RED}ERROR: make is required for compiling shorkmines${RESET}"
    exit 1
fi



echo -e "${YELLOW}Cloning shorkmines repo...${RESET}"
git clone https://github.com/SharktasticA/shorkmines
cd shorkmines

echo -e "${YELLOW}Installing shorkmines (you may be asked for sudo)...${RESET}"
git submodule update --init
sudo make install

echo -e "${YELLOW}Cleaning up...${RESET}"
cd ..
rm -rf shorkmines

shorkmines --version
echo -e "${GREEN}All done! :) Run: shorkmines${RESET}"
