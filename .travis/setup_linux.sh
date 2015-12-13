#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

sudo add-apt-repository --yes ppa:george-edison55/precise-backports # For updated CMake
sudo add-apt-repository --yes ppa:beineri/opt-qt542 # For Qt 5.4.2
sudo add-apt-repository --yes ppa:boost-latest/ppa # Updated Boost
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test # gcc 4.8
sudo apt-get update -yqq
sudo apt-get purge cmake -yqq
sudo apt-get install -yqq cmake qt54base libboost1.55-dev libboost-date-time1.55-dev libboost-filesystem1.55-dev libboost-iostreams1.55-dev libboost-program-options1.55-dev libboost-regex1.55-dev libasound2-dev g++-4.8 binutils-dev
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 90
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90
