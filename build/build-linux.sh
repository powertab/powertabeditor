#!/bin/sh

mkdir -p build-linux
cd build-linux

echo "Running qmake ..."
qmake ../../source/source.pro

echo "Building ..."
make -j2

echo "Packaging ..."
cd build

mkdir pte
cp powertabeditor pte/
cp -r skins pte/

tar cfa powertabeditor-linux.tar.gz pte/
mv powertabeditor-linux.tar.gz ../../

rm -r pte/

cd ../../
