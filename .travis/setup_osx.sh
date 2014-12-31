#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

brew update
# Install Qt, gettex, and binutils. Boost is already installed on the travis environment.
brew install qt5 gettext external/withershins/osx/binutils.rb
brew link --force gettext
