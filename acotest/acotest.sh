#!/bin/bash
cd libaco-master
make
sudo make install
cd ..
mkdir build
cd build
cmake ..
make
cd ..