#!/bin/bash

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

makeDeps() {
   msg1 "Generating dependencies" >&2
   local GEN_SCRIPT="${PWD}/$DEPS_MAIN_DIR/generate.sh"
   local CURRENT_TEMP_PATH="$PWD"

   if [ -f "$GEN_SCRIPT" ]; then
      if [ ! -x "$GEN_SCRIPT" ]; then
         chmod +x "$GEN_SCRIPT"
      fi

      source "$GEN_SCRIPT"
   fi

   cat <<EOF
#
# This is an automatically gnererated file
# !!! DO NOT EDIT !!!
#

cmake_minimum_required(VERSION ${CMAKE_VERSION})

set( CMAKE_CXX_FLAGS_DEBUG   "" )
set( CMAKE_CXX_FLAGS_RELEASE "" )
set( CMAKE_BUILD_TYPE RELEASE )

EOF

   local i varName name DEFAULT FUNC

   for (( i = 0; i < ${#DEPS[@]}; ++i )); do
      name="${DEPS[$i]}"
      varName="BUILD_${name^^}"
      FUNC="dependency_${name}"

      found "dependency $name" >&2

      if [[ "$(type -t "${FUNC}")" == "function" ]]; then
         cd "${PWD}/$DEPS_MAIN_DIR"
         eval "${FUNC} >&2" # Calling function from GEN_SCRIPT
         cd "$CURRENT_TEMP_PATH"
      fi

      if [[ "${DEPS_DEFAULT[$i]}" == "true" ]]; then
         DEFAULT="ON"
      else
         DEFAULT="OFF"
      fi

      ALL_DIRS+=( "$DEPS_MAIN_DIR/${DEPS_DIR[$i]}" )

      cat <<EOF

option( $varName "Build dependency ${name}?" $DEFAULT )

if( $varName )
EOF

   [[ "$(type -t "${FUNC}_custom")" == "function" ]] && eval "${FUNC}_custom" # Calling function from GEN_SCRIPT

cat <<EOF
   include_directories( ${DEPS_DIR[$i]} )
   add_subdirectory( ${DEPS_DIR[$i]} )
   message( STATUS "Building dependency $name (use -D${varName}=OFF to disable) [DEFAULT: $DEFAULT]" )
else()
   message( STATUS "NOT building dependency $name (use -D${varName}=ON to enable) [DEFAULT: $DEFAULT]" )
endif( $varName )

EOF

   done

   ALL_DIRS+=( "" )
}
