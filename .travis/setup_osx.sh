#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

brew update
# Install Qt and binutils. Boost is already installed on the travis environment.
brew install qt5 external/withershins/osx/binutils.rb

# Debugging.
ls /usr/local/Cellar/binutils/2.24
