#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

sudo add-apt-repository --yes ppa:beineri/opt-qt542-trusty # For Qt 5.4.2
sudo apt-get update -yqq
sudo apt-get install -yqq cmake qt54base libboost-dev libboost-date-time-dev libboost-filesystem-dev libboost-iostreams-dev libboost-program-options-dev libboost-regex-dev libasound2-dev binutils-dev