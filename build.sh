#!/bin/bash

rm -rf CMakeFiles
rm -f service-session
rm -f CMakeCache.txt

cmake .
make
