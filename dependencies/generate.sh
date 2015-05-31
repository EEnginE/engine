#!/bin/bash

# Disabled Open3DGC because it is not used

# Creating symlink for Open3DGC
#if [ -e o3dgc ]; then
#   rm -rf o3dgc
#fi

#ln -s $(pwd)/glTF/converter/COLLADA2GLTF/dependencies/o3dgc $(pwd)

# Disabling Warnings and install prefix
#find -L "$(pwd)/o3dgc" -name "CMakeLists.txt" -exec sed -i 's/^set(CMAKE_CXX_FLAGS/# set(CMAKE_CXX_FLAGS/g' "{}" \;
#find -L "$(pwd)/o3dgc" -name "CMakeLists.txt" -exec sed -i 's/^set(CMAKE_INSTALL_P/# set(CMAKE_INSTALL_P/g' "{}" \;

dependency_GLEW() {
   if [ ! -d GLEW ]; then
      error "Directory GLEW does NOT exist!"
      return
   fi

   if [ -z "$(ls -A GLEW)" ]; then
      warning "Directory GLEW is empty. Please init and update the submodules"
      return
   fi

   cd GLEW
   msg2 "Generating GLEW extensions. This can take a while"

   export GIT_CLONE="\"${GIT_EXEC}\" clone --branch glew https://github.com/nigels-com/glfixes.git"

   make extensions &> ../make_extensions.log

   (( $? != 0 )) && warning "An error occured while running 'make extensions' in $PWD"
}

dependency_GLEW_custom() {
   echo "   set( BUILD_UTILS OFF CACHE BOOL \"Disabling BUILD_UTILS\" )"
}
