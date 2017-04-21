#!/bin/sh

# WARNING : this is not production code! should NOT be merged into master branch

# hppav-14
#cmake -D SWIG_DIR="C:\Program Files (x86)\swigwin-3.0.12" -D SWIG_EXECUTABLE="C:\Program Files (x86)\swigwin-3.0.12\swig.exe" -D NUMPY_INCLUDE="C:\Python\x64\py35\Lib\site-packages\numpy\core\include" . -G"Visual Studio 14 2015 Win64"

# limbo-iv
cmake -D SWIG_DIR="/d/Programs/swigwin-3.0.12" -D SWIG_EXECUTABLE="/d/Programs/swigwin-3.0.12/swig.exe" -D NUMPY_INCLUDE="/d/Python/x64/Python35/Lib/site-packages/numpy/core/include" . -G"Visual Studio 14 2015 Win64"
