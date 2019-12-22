#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

sudo apt-get update -yqq
sudo apt-get install -yqq cmake qtbase5-dev libboost-dev libboost-date-time-dev libboost-filesystem-dev libboost-iostreams-dev libboost-program-options-dev libboost-regex-dev libasound2-dev librapidjson-dev ninja-build
