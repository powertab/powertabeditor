#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

sudo add-apt-repository --yes ppa:kalakris/cmake # For cmake 2.8.9+
sudo add-apt-repository --yes ppa:beineri/opt-qt521 # For Qt 5.2.1
sudo add-apt-repository --yes ppa:boost-latest/ppa # Updated Boost
sudo add-apt-repository --yes ppa:ubuntu-toolchain-r/test # gcc 4.8
sudo apt-get update -yqq
sudo apt-get install -yqq cmake qt52base libboost1.55-dev libboost-date-time1.55-dev libboost-iostreams1.55-dev libboost-program-options1.55-dev libasound2-dev g++-4.8 binutils-dev
sudo update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-4.8 90
sudo update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-4.8 90
sudo pip install cpp-coveralls pyyaml # code coverage
