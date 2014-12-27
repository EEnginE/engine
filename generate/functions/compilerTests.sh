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
            echo "ERROR: $I is not a directory! (Compiler tests)"
            continue
        fi

        local TEST_NAME="${C_TESTS[$i]}"
        echo "INFO: Found compiler test module $TEST_NAME"

        PRE_GEN="$(pwd)/${I}/generate.pre.sh"
        POST_GEN="$(pwd)/${I}/generate.post.sh"

        if [ -f $PRE_GEN ]; then
            if [ ! -x $PRE_GEN ]; then
                chmod +x $PRE_GEN
            fi
            CURRENT_TEMP_PATH="$(pwd)"
            echo "INFO:    -- Running pre generate script $PRE_GEN"
            cd $(dirname $PRE_GEN)
            $PRE_GEN
            cd $CURRENT_TEMP_PATH
        fi

        local CMAKE_FILE="$(pwd)/${I}/$CMAKE_LISTS_NAME"
        echo "INFO:    -- Generating CMakeLists.txt"

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
        if [ -f $POST_GEN ]; then
            if [ ! -x $POST_GEN ]; then
                chmod +x $POST_GEN
            fi
            CURRENT_TEMP_PATH="$(pwd)"
            echo "INFO:    -- Running post generate script $POST_GEN"
            cd $(dirname $POST_GEN)
            $POST_GEN
            cd $CURRENT_TEMP_PATH
        fi
    done
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
