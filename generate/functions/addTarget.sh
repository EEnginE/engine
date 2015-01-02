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

addTarget() {
    local I T DEP CMAKE_FILE
    for (( I = 0; I < ${#LIBS[@]}; ++I )); do
        T="${LIBS[$I]}"
        DEP="${LIBS_DEP[$I]}"
        CMAKE_FILE="$T/${CMAKE_LISTS_NAME}"

        msg1 "Generating ${CMAKE_LISTS_NAME} for target $T [$DEP]"

        cat > $CMAKE_FILE <<EOF
# Automatically generated file; DO NOT EDIT

EOF

        finSources $T ${T^^} 1>> $CMAKE_FILE
        local TMP_NAME="${PROJECT_NAME}_${T}"

        local LINK_LIBS=""
        for i in $DEP; do
            LINK_LIBS="$LINK_LIBS ${PROJECT_NAME}_${i}"
        done

        cat >> $CMAKE_FILE <<EOF

if( NOT DEFINED ENGINE_BUILD_SHARED )
  set( ENGINE_BUILD_SHARED 1 )          # DEFAULT: We create a shared library ( .so or a .dll )
endif( NOT DEFINED ENGINE_BUILD_SHARED )

set( LIBS_TO_LINK $LINK_LIBS )

if( ENGINE_BUILD_SHARED )
  add_library( ${TMP_NAME} SHARED \${${T^^}_SRC} \${${T^^}_INC} ) # Create a shared library
  set( _BUILD_TYPE "shared" )
else( ENGINE_BUILD_SHARED )
  add_library( ${TMP_NAME} STATIC \${${T^^}_SRC} \${${T^^}_INC} ) # Create a static library
  set( _BUILD_TYPE "static" )
endif( ENGINE_BUILD_SHARED )

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
  set_target_properties(
    ${TMP_NAME}
    PROPERTIES
      VERSION       \${CM_VERSION_MAJOR}.\${CM_VERSION_MINOR}.\${CM_VERSION_SUBMINOR}
      SOVERSION     \${CM_VERSION_MAJOR}
    LINK_FLAGS	  "/LIBPATH:\${Boost_LIBRARY_DIRS} /FORCE:MULTIPLE"
  )
else()
# Set some variables so that Cmake can do some fancy stuff with versions and filenames
  set_target_properties(
    ${TMP_NAME}
      PROPERTIES
        VERSION       \${CM_VERSION_MAJOR}.\${CM_VERSION_MINOR}.\${CM_VERSION_SUBMINOR}
        SOVERSION     \${CM_VERSION_MAJOR}
  )
endif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)

# Windows also needs some linking...
if( WIN32 )
  target_link_libraries(${TMP_NAME} \${ENGINE_LINK} \${LIBS_TO_LINK} )
endif( WIN32 )

install( FILES \${${T^^}_INC} DESTINATION \${CMAKE_INSTALL_PREFIX}/include/engine)

install(
  TARGETS  ${TMP_NAME}
  RUNTIME  DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/
  LIBRARY  DESTINATION \${CMAKE_INSTALL_PREFIX}/lib/
  ARCHIVE  DESTINATION \${CMAKE_INSTALL_PREFIX}/lib/
)

if( "\${LIBS_TO_LINK}" STREQUAL "" )
  set( LIBS_TO_LINK "NOTHING" )
endif( "\${LIBS_TO_LINK}" STREQUAL "" )

message( STATUS "Added \${_BUILD_TYPE} Library ${TMP_NAME} (Depends on: \${LIBS_TO_LINK})" )

EOF
    done

}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
