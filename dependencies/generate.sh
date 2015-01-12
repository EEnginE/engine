#!/bin/bash

# Creating symlink for Open3DGC
if [ -e o3dgc ]; then
   rm -rf o3dgc
fi

ln -s $(pwd)/glTF/converter/COLLADA2GLTF/dependencies/o3dgc $(pwd)

# Disabling Warnings and install prefix
find -L "$(pwd)/o3dgc" -name "CMakeLists.txt" -exec sed -i 's/^set(CMAKE_CXX_FLAGS/# set(CMAKE_CXX_FLAGS/g' "{}" \;
find -L "$(pwd)/o3dgc" -name "CMakeLists.txt" -exec sed -i 's/^set(CMAKE_INSTALL_P/# set(CMAKE_INSTALL_P/g' "{}" \;
