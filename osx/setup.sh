#!/bin/bash
set -e # Exit immediately if an error occurs.
set -x # Echo commands.

brew update
brew install boost qt5 gettext external/withershins/osx/binutils.rb
brew link --force gettext
