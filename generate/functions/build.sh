#!/bin/env bash

# Copyright (C) 2015 EEnginE project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

buildProject() {
   msg1 "Start building..."
   local ENGINE_ROOT=$(pwd)

   if [ -z "$CMAKE_EXECUTABLE" ]; then
      CMAKE_EXECUTABLE=$(which cmake)
      if (( $? != 0 )); then
         error "Unable to find cmake!"
         exit 1
      fi
   fi

   if [ -z "$BUILD_COMMAND" ]; then
      BUILD_COMMAND=$(which make)
      if (( $? != 0 )); then
         error "Unable to find default build command make!"
         exit 1
      fi
   fi

   # Testing the directory
   if [ -z "$BUILD_DIR" ]; then
      BUILD_DIR="build"
   fi

   if [ -e $BUILD_DIR ]; then
      if [ ! -d $BUILD_DIR ]; then
         error "$BUILD_DIR exists but is not a directory"
         exit 1
      fi
   else
      mkdir $BUILD_DIR
   fi

   cd $BUILD_DIR

   if [ -n "$CPP_COMPILER" ]; then
      CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_CXX_COMPILER=$CPP_COMPILER"
   fi

   if [ -n "$C_COMPILER" ]; then
      CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_C_COMPILER=$C_COMPILER"
   fi

   if [ -n "$INST_PREFIX" ]; then
      CMAKE_FLAGS="$CMAKE_FLAGS -DCMAKE_INSTALL_PREFIX=$INST_PREFIX"
   fi

   local RET=0

   msg2 "Running '$CMAKE_EXECUTABLE $CMAKE_FLAGS $ENGINE_ROOT'"
   $CMAKE_EXECUTABLE $CMAKE_FLAGS $ENGINE_ROOT
   RET=$?

   if (( RET != 0 )); then
      error "Cmake failed!"
      exit $RET
   fi

   msg2 "Running '$BUILD_COMMAND $BUILD_FLAGS'"
   $BUILD_COMMAND $BUILD_FLAGS

   RET=$?

   if (( RET != 0 )); then
      error "$BUILD_COMMAND failed!"
      exit $RET
   fi
}




generateAtomBuild() {
   msg1 "Generating .atom-build.json"
   cat << EOF > .atom-build.json
{
   "cmd": "./generate.sh",
   "args": [ "qSb" ],
   "sh": true,
   "cwd": "{PROJECT_PATH}",
   "env": {
      "CMAKE_EXECUTABLE": "$CMAKE_EXECUTABLE",
      "BUILD_COMMAND": "$BUILD_COMMAND",
      "BUILD_DIR": "$BUILD_DIR",

      "CMAKE_FLAGS": "$CMAKE_FLAGS",
      "CPP_COMPILER": "$CPP_COMPILER",
      "C_COMPILER": "$C_COMPILER",
      "INST_PREFIX": "$INST_PREFIX",

      "BUILD_FLAGS": "$BUILD_FLAGS"
   }
}
EOF
}
