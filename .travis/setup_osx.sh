#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

brew update
# Install Qt. Boost is already installed on the travis environment.
brew install qt5
