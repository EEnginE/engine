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

# Variables:

cat << EOF >/dev/null

CMAKE_LISTS_NAME
CMAKE_VERSION
CLANG_COMPLETE
PROJECT_NAME
DISPLAY_SERVER
OS
LIBS
LIBS_DEP
TESTS
INCLUDE_FILE
TESTS_DIR
C_TESTS
COMPILER_TESTS
COMPILER_TESTS_DIR
TOOLS_DIRECTORY
LOG_MACRO_PATH
LOG_GEN_UNDEF
LOG_TYPES
DEPS_MAIN_DIR
DEPS
DEPS_DIR
DEPS_DEFAULT
DEBUG_DEF_FILE
CLANG_FORMAT_VERSION
BUILD_CFG_NAME
WINDOWS_SETUP_DIR

EOF


parseCFG() {
    if [ ! -f "$CONFIG_FILE" ]; then
        error " Cannot find config file $CONFIG_FILE"
        exit 2
    fi

    msg1 "Parsing Config file $CONFIG_FILE"

    local LINE
    while read LINE; do
        LINE="${LINE/\#*/}" # Remove comments
        LINE="${LINE/$*( )/}" # Remove ' ' at the end of line ( Needs shopt extglob on )

        if [ -z "$LINE" ]; then
            continue
        fi

        local TEMP="${LINE/#*:*( )/}" # Needs shopt extglob on
        local VARIABLE="${LINE/%:*/}"

        case $VARIABLE in
            CM)   CMAKE_LISTS_NAME="$TEMP"    ;;
            CM_V) CMAKE_VERSION="$TEMP"       ;;
            CLA)  CLANG_COMPLETE="$TEMP"      ;;
            PRO)  PROJECT_NAME=$TEMP          ;;
            P)    DISPLAY_SERVER+=( "$TEMP" ) ;;
            OS)
                local T_OS="${TEMP/%*( );*/}"
                local T_DS="${TEMP/#*;*( )/}"
                OS+=( $T_OS )
                eval "DS_${T_OS}=( $T_DS )"
                ;;
            L)
                local T_LIB_NAME="${TEMP/%*( );*/}"
                local T_LIB_DEP="${TEMP/#*;*( )/}"
                LIBS+=( "$T_LIB_NAME" )
                LIBS_DEP+=( "$T_LIB_DEP" )
                ;;
            T)         TESTS+=( "$TEMP" )           ;;
            E_INC)     INCLUDE_FILE="$TEMP"         ;;
            T_DIR)     TESTS_DIR="$TEMP"            ;;
            CT)        C_TESTS+=( "$TEMP" )         ;;
            CT_DO)     COMPILER_TESTS=$TEMP         ;;
            CT_DIR)    COMPILER_TESTS_DIR=$TEMP     ;;
            TOOLS_D)   TOOLS_DIRECTORY="$TEMP"      ;;
            LOG_PATH)  LOG_MACRO_PATH="$TEMP"       ;;
            LOG_UNDEF) LOG_GEN_UNDEF=$TEMP          ;;
            LOG)       LOG_TYPES="$TEMP"            ;;
            CLANG_F)   CLANG_FORMAT_CONFIG="$TEMP"  ;;
            DDIR)      DEPS_MAIN_DIR="$TEMP"        ;;
            DEP)       DEPS+=( "$TEMP" )            ;;
            DEP_DIR)   DEPS_DIR+=( "$TEMP" )        ;;
            DEP_DEF)   DEPS_DEFAULT+=( "$TEMP" )    ;;
            DEBUG_F)   DEBUG_DEF_FILE="${TEMP}"     ;;
            CLANG_V)   CLANG_FORMAT_VERSION=$TEMP   ;;
            BUILD_CFG) BUILD_CFG_NAME="$TEMP"       ;;
            WINSETUP)  WINDOWS_SETUP_DIR="$TEMP"    ;;
            *)
                warning "Unknown option '$VARIABLE' with argumet(s) $TEMP"
                ;;
        esac

    done < "$CONFIG_FILE"
}

printWhatParsed() {
    msg1 "Printing parsed config"
    msg2 "Name of CMake files to generate: '$CMAKE_LISTS_NAME'"
    msg2 "CMake version to use:            '$CMAKE_VERSION'"
    msg2 "clang autocomplete file:         '$CLANG_COMPLETE'"
    msg2 "Project Name:                     $PROJECT_NAME"
    msg2 "Added target platforms:           ${DISPLAY_SERVER[*]}"
    msg2 "Added Operating Systems:          ${OS[*]}"
    msg2 "Added libs:                       ${LIBS[*]}"
    msg2 "Added Tests:                      ${TESTS[*]}"
    msg2 "Main engine include file:        '$INCLUDE_FILE'"
    msg2 "Tests dir:                       '$TESTS_DIR'"
    msg2 "Added Compiler Test:              ${C_TESTS[*]}"
    msg2 "Using compiler tests:            '$COMPILER_TESTS' (1 -- true; 0 -- false)"
    msg2 "Using compiler tests directory:  '$COMPILER_TESTS_DIR'"
    msg2 "Tools dir:                       '$TOOLS_DIRECTORY'"
    msg2 "Log macros file:                 '$LOG_MACRO_PATH'"
    msg2 "Create undefs for log macros:     $LOG_GEN_UNDEF (1 -- true; 0 -- false)"
    msg2 "Log types:                        $LOG_TYPES"
    msg2 "Clang format config:              $CLANG_FORMAT_CONFIG"
    msg2 "Dependencies main dir:            $DEPS_MAIN_DIR"
    msg2 "Dependencies:                     ${DEPS[*]}"
    msg2 "Dependencies (dirs):              ${DEPS_DIR[*]}"
    msg2 "Dependencies (default on):        ${DEPS_DEFAULT[*]}"
    msg2 "Debug define file:                ${DEBUG_DEF_FILE}"
    msg2 "Required clang-format version:    ${CLANG_FORMAT_VERSION}"
    msg2 "Build config file name:           ${BUILD_CFG_NAME}"
    msg2 "Windows installer directory       ${WINDOWS_SETUP_DIR}"
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
