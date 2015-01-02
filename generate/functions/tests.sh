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

tests() {
    local I i

    for (( i = 0; i < ${#TESTS[@]}; ++i )); do
        I=$( echo "$TESTS_DIR/${TESTS[$i]}" | sed 's/\/$//g' )

        if [ ! -d $I ]; then
            error " $I is not a directory!"
            continue
        fi

        local TEST_NAME="${TESTS[$i]}"
        msg1 "Found test module $TEST_NAME"

        PRE_GEN="$(pwd)/${I}/generate.pre.sh"
        POST_GEN="$(pwd)/${I}/generate.post.sh"

        if [ -f $PRE_GEN ]; then
            if [ ! -x $PRE_GEN ]; then
                chmod +x $PRE_GEN
            fi
            CURRENT_TEMP_PATH="$(pwd)"
            msg2 "Running pre generate script $PRE_GEN"
            cd $(dirname $PRE_GEN)
            ( source $PRE_GEN )
            cd $CURRENT_TEMP_PATH
        fi

        local CMAKE_FILE="$(pwd)/${I}/$CMAKE_LISTS_NAME"
        msg2 "Generating CMakeLists.txt"
        cat > $CMAKE_FILE << EOF
# Automatically generated file; DO NOT EDIT

project( $TEST_NAME )

EOF
        local CUSTOM_FILE="$(pwd)/$I/$CMAKE_CUSTOM_FILE"

        if [ -f $CUSTOM_FILE ]; then
            cat $CUSTOM_FILE >> $CMAKE_FILE
        fi

        finSources ${I} ${TEST_NAME^^} 1>> $CMAKE_FILE

        local ENGINE_LIBS="" TTT
        for TTT in "${LIBS[@]}"; do
            ENGINE_LIBS="$ENGINE_LIBS ${PROJECT_NAME}_${TTT}"
        done

        cat >> $CMAKE_FILE <<EOF

if( EXISTS \${CMAKE_CURRENT_SOURCE_DIR}/config.in.hpp )
   configure_file("\${CMAKE_CURRENT_SOURCE_DIR}/config.in.hpp" "\${CMAKE_CURRENT_SOURCE_DIR}/config.hpp")
endif( EXISTS \${CMAKE_CURRENT_SOURCE_DIR}/config.in.hpp )

add_executable( $TEST_NAME \${${TEST_NAME^^}_SRC} \${${TEST_NAME^^}_INC} )

target_link_libraries(
 $TEST_NAME

 ${ENGINE_LIBS}
 \${ENGINE_LINK}
)

if(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
set_target_properties(
 $TEST_NAME
 PROPERTIES
  INSTALL_RPATH \${CMAKE_INSTALL_PREFIX}/lib
  INSTALL_RPATH_USE_LINK_PATH TRUE
 LINK_FLAGS	  "/LIBPATH:\${Boost_LIBRARY_DIRS} /FORCE:MULTIPLE"
  )
else()
set_target_properties(
 $TEST_NAME
 PROPERTIES
  INSTALL_RPATH \${CMAKE_INSTALL_PREFIX}/lib
  INSTALL_RPATH_USE_LINK_PATH TRUE
)
endif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)


install( TARGETS $TEST_NAME RUNTIME DESTINATION \${CMAKE_INSTALL_PREFIX}/bin )

if( EXISTS \${CMAKE_CURRENT_SOURCE_DIR}/data AND IS_DIRECTORY \${CMAKE_CURRENT_SOURCE_DIR}/data )
   install( DIRECTORY \${CMAKE_CURRENT_SOURCE_DIR}/data DESTINATION \${CMAKE_INSTALL_PREFIX}/share/engineTests/$TEST_NAME )
endif( EXISTS \${CMAKE_CURRENT_SOURCE_DIR}/data AND IS_DIRECTORY \${CMAKE_CURRENT_SOURCE_DIR}/data )

EOF


        if [ -f $POST_GEN ]; then
            if [ ! -x $POST_GEN ]; then
                chmod +x $POST_GEN
            fi
            CURRENT_TEMP_PATH="$(pwd)"
            msg2 "Running post generate script $POST_GEN"
            cd $(dirname $POST_GEN)
            ( source $POST_GEN )
            cd $CURRENT_TEMP_PATH
        fi
    done

}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
