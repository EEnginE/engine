#!/bin/bash

compilerTests() {
    # generate tests?
    if (( COMPILER_TESTS != 1 )); then
        return
    fi

    local I i PRE_GEN POST_GEN

    for (( i = 0; i < ${#C_TESTS[@]}; ++i )); do
        local I=$( echo "$COMPILER_TESTS_DIR/${C_TESTS[$i]}" | sed 's/\/$//g' )

        if [ ! -d $I ]; then
            error " $I is not a directory! (Compiler tests)"
            continue
        fi

        local TEST_NAME="${C_TESTS[$i]}"
        msg2 "Found compiler test module $TEST_NAME"

        local CMAKE_FILE="$(pwd)/${I}/$CMAKE_LISTS_NAME"
        msg2 "Generating CMakeLists.txt"

        TEST_NAME=${TEST_NAME}

        cat > $CMAKE_FILE << EOF
# Automatically generated file; DO NOT EDIT

cmake_minimum_required(VERSION ${CMAKE_VERSION})

project( ${TEST_NAME} )

set( CMAKE_MODULE_PATH \${CMAKE_MODULE_PATH} $(pwd)/generate/cmake )
set( CMAKE_BUILD_TYPE DEBUG )

EOF

        cat $(pwd)/generate/CMakeLists.txt.flags >> $CMAKE_FILE

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

EOF
    done
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
