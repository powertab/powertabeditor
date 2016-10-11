#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

brew update
# Install Qt. Boost and cmake are already installed on the travis environment.
brew install qt5
