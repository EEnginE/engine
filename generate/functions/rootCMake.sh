#!/usr/bin/env bash

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

rootCMake() {

    local FILE i I

    msg1 "Generating root CMake file" >&2

    cat <<EOF
#
# This is an automatically gnererated file
# !!! DO NOT EDIT !!!
#

cmake_minimum_required(VERSION ${CMAKE_VERSION})

project(${PROJECT_NAME})

EOF

    cat "$(pwd)/generate/CMakeLists.txt.pre"

    cat <<'EOF'

#     _____ _                                                  _     _         ___  ______ _____
#    /  __ \ |                                                | |   (_)       / _ \ | ___ \_   _|
#    | /  \/ |__   ___   ___  ___  ___    __ _ _ __ __ _ _ __ | |__  _  ___  / /_\ \| |_/ / | |
#    | |   | '_ \ / _ \ / _ \/ __|/ _ \  / _` | '__/ _` | '_ \| '_ \| |/ __| |  _  ||  __/  | |
#    | \__/\ | | | (_) | (_) \__ \  __/ | (_| | | | (_| | |_) | | | | | (__  | | | || |    _| |_
#     \____/_| |_|\___/ \___/|___/\___|  \__, |_|  \__,_| .__/|_| |_|_|\___| \_| |_/\_|    \___/
#                                         __/ |         | |
#                                        |___/          |_|


if( NOT DISPLAY_SERVER )
EOF

    msg2 "Generating Display server switches" >&2

    local DS_AND_OS=()

    local DEFAULT_DS
    for i in "${OS[@]}"; do
        eval "DEFAULT_DS=\${DS_${i}[0]}"
        cat <<EOF
   if( $i )
      set( DISPLAY_SERVER ${i^^}_${DEFAULT_DS^^} )
   endif( $i )

EOF
        eval "DEFAULT_DS=( \"\${DS_${i}[@]}\" )"
        for I in "${DEFAULT_DS[@]}"; do
            DS_AND_OS+=( "${i^^}_${I^^}" )
        done
    done

    cat <<EOF
endif( NOT DISPLAY_SERVER )


EOF

    local IF_STRING=""

    for i in "${DS_AND_OS[@]}"; do
        IF_STRING="$IF_STRING NOT DISPLAY_SERVER MATCHES ${i} AND"
    done

    IF_STRING="$(echo -n "$IF_STRING" | sed 's/AND$//g' )"

    cat <<EOF
if( $IF_STRING )
    message( FATAL_ERROR "Display Server '\${DISPLAY_SERVER}' is not supported!!! You can use: ${DS_AND_OS[@]}" )
endif( $IF_STRING )


EOF

    for i in "${DS_AND_OS[@]}"; do
        cat <<EOF
# ${i}:
set( CM_${i} 0 )

if( DISPLAY_SERVER MATCHES ${i} )
    set( CM_${i} 1 )
    message( STATUS "Using Display Server ${i}" )
endif( DISPLAY_SERVER MATCHES ${i} )


EOF
    done


    cat "$(pwd)/generate/CMakeLists.txt.flags"
    cat "$(pwd)/generate/CMakeLists.txt.dep"
    cat "$(pwd)/generate/CMakeLists.txt.git"


    cat <<'EOF'

#     _____           _           _       ______ _
#    |_   _|         | |         | |      |  _  (_)
#      | | _ __   ___| |_   _  __| | ___  | | | |_ _ __ ___
#      | || '_ \ / __| | | | |/ _` |/ _ \ | | | | | '__/ __|
#     _| || | | | (__| | |_| | (_| |  __/ | |/ /| | |  \__ \
#     \___/_| |_|\___|_|\__,_|\__,_|\___| |___/ |_|_|  |___/
#

EOF

    local TEMP_SIZE=0
    local TEMP_A=()

    echo "-I$(pwd)/$INCLUDES_DIR" > "$CLANG_COMPLETE"

    for i in "${DISPLAY_SERVER[@]}"; do
        eval "TEMP_SIZE=\${#${i^^}_DIRS[@]}"

        if (( TEMP_SIZE != 0 )); then
            echo "# ${i} dirs:"
            echo "if( DISPLAY_SERVER MATCHES ${i^^} )"
            echo ""
            echo "include_directories("
            eval "TEMP_A=( \${${i^^}_DIRS[@]} )"
            for I in "${TEMP_A[@]}"; do
                echo "  $I"
                echo "-I$(pwd)/${I}" >> "$CLANG_COMPLETE"
            done
            echo ")"
            echo ""
            echo "endif( DISPLAY_SERVER MATCHES ${i^^} )"
            echo ""
            echo ""
        fi

    done

    echo ""
    echo "# All Display_Servers"
    echo "include_directories("
    echo "  ${INCLUDES_DIR}"
    for I in "${ALL_DIRS[@]}"; do
        echo "  $I"
        echo "-I$(pwd)/${I}" >> "$CLANG_COMPLETE"
    done
    echo ")"

    cat <<'EOF'


#     _____         _                               _ _
#    |_   _|       | |                             (_) |
#      | | ___  ___| |_    ___ ___  _ __ ___  _ __  _| | ___ _ __ ___
#      | |/ _ \/ __| __|  / __/ _ \| '_ ` _ \| '_ \| | |/ _ \ '__/ __|
#      | |  __/\__ \ |_  | (_| (_) | | | | | | |_) | | |  __/ |  \__ \
#      \_/\___||___/\__|  \___\___/|_| |_| |_| .__/|_|_|\___|_|  |___/
#                                            | |
#                                            |_|

EOF

    if (( COMPILER_TESTS == 1 )); then
        local TEST_TO_DO
        for i in "${C_TESTS[@]}"; do
            TEST_TO_DO=${i^^}

            local TEST_PATH=$( echo "$(pwd)/$COMPILER_TESTS_DIR/$i" | sed 's/^\/cygdrive\///g' | sed 's/^\([a-zA-Z]\)/\U\1:/g' )
            cat << EOF


try_compile( ${TEST_TO_DO}_RET "\${PROJECT_BINARY_DIR}/bin/compilerTests" "$TEST_PATH" $i OUTPUT_VARIABLE ${TEST_TO_DO}_OUTPUT )

if( ${TEST_TO_DO}_RET )
    set( COMPILER_TEST_${TEST_TO_DO}_PASSED 1 )
    message( STATUS "Compiler test '${i}' passed" )
else( ${TEST_TO_DO}_RET )
    set( COMPILER_TEST_${TEST_TO_DO}_PASSED 0 )
    message( WARNING "Compiler test '${i}' failed!" )
endif( ${TEST_TO_DO}_RET )

EOF
        done
    fi

    cat <<'EOF'

#    ___  ___      _                                         _       __ _
#    |  \/  |     | |                                       | |     / _(_)
#    | .  . | __ _| | _____   ___  ___  _ __ ___   ___    __| | ___| |_ _ _ __   ___  ___
#    | |\/| |/ _` | |/ / _ \ / __|/ _ \| '_ ` _ \ / _ \  / _` |/ _ \  _| | '_ \ / _ \/ __|
#    | |  | | (_| |   <  __/ \__ \ (_) | | | | | |  __/ | (_| |  __/ | | | | | |  __/\__ \
#    \_|  |_/\__,_|_|\_\___| |___/\___/|_| |_| |_|\___|  \__,_|\___|_| |_|_| |_|\___||___/
#

EOF

    makeDebugDefines

    cat <<'EOF'


#    ______       _ _     _                   _   _           _        _ _
#    | ___ \     (_) |   | |                 | | (_)         | |      | | |
#    | |_/ /_   _ _| | __| |   __ _ _ __   __| |  _ _ __  ___| |_ __ _| | |
#    | ___ \ | | | | |/ _` |  / _` | '_ \ / _` | | | '_ \/ __| __/ _` | | |
#    | |_/ / |_| | | | (_| | | (_| | | | | (_| | | | | | \__ \ || (_| | | |
#    \____/ \__,_|_|_|\__,_|  \__,_|_| |_|\__,_| |_|_| |_|___/\__\__,_|_|_|
#


EOF

    for i in "${LIBS[@]}"; do
        echo "add_subdirectory( $i )"
    done

    if (( DO_TESTS == 1 )); then
        echo ""
        for i in "${TESTS[@]}"; do
            echo "add_subdirectory( $TESTS_DIR/$i )"
        done
    fi


cat << EOF

# Installing tools

install(
   PROGRAMS
EOF

    for I in $(ls "$TOOLS_DIRECTORY"); do
        if [ ! -f "$TOOLS_DIRECTORY/$I" ]; then
            continue
        fi
        echo "      $TOOLS_DIRECTORY/$I"
    done

cat << EOF
   DESTINATION \${CMAKE_INSTALL_PREFIX}/bin
)

# Generate defines.hpp
configure_file(
 "\${PROJECT_SOURCE_DIR}/${INCLUDES_DIR}/defines.in.hpp"
 "\${PROJECT_SOURCE_DIR}/${INCLUDES_DIR}/defines.hpp"
)

EOF
    msg2 "Generating main defines file" >&2
    generateDefinesHPP 1> "${INCLUDES_DIR}/defines.in.hpp"

    cat "$(pwd)/generate/CMakeLists.txt.post"

}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
