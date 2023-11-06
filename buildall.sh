#!/bin/zsh
rm -r build
cd lib
rm -r build
cmake -B build
cd build
make
cd ../..
cmake -B build
cd build
make