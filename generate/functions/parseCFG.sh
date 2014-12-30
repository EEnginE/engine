#!/bin/bash

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

EOF


parseCFG() {
    if [ ! -f "$CONFIG_FILE" ]; then
        error " Cannot find config file $CONFIG_FILE"
        exit 2
    fi

    msg1 "Parsing Config file $CONFIG_FILE"

    local NUM_LINES=$(wc -l $CONFIG_FILE | awk '{print $1}')
    local COUNTER=1

    local LINE
    while read LINE; do
        processBar $COUNTER $NUM_LINES $LINE
        ((COUNTER++))

        LINE="$(echo "$LINE" | sed 's/#.*//g')"   # Remove comments
        LINE="$(echo "$LINE" | sed 's/[ ]*$//g')" # Remove ' ' at the end of line

        if [ -z "$LINE" ]; then
            continue
        fi

        local TEMP="$(echo "$LINE" | sed 's/^[a-zA-Z 0-9_\.]*:[ ]*//g' )"
        local VARIABLE="$(echo "$LINE" | sed 's/:[a-zA-Z 0-9_;\.\/\-]*$//g' )"

        case $VARIABLE in
            CM)
                CMAKE_LISTS_NAME="$TEMP"
                ;;
            CM_V)
                CMAKE_VERSION="$TEMP"
                ;;
            CLA)
                CLANG_COMPLETE="$TEMP"
                ;;
            PRO)
                PROJECT_NAME=$TEMP
                ;;
            P)
                DISPLAY_SERVER+=( "$TEMP" )
                ;;
            OS)
                local T_OS=$(echo $TEMP | sed 's/;[a-zA-Z 0-9_\/\.]*$//g' )
                local T_DS=$(echo $TEMP | sed 's/^[a-zA-Z 0-9_\/\.]*;//g' )
                OS+=( $T_OS )
                eval "DS_${T_OS}=( $T_DS )"
                ;;
            L)
                local T_LIB_NAME="$(echo "$TEMP" | sed 's/;[a-z A-Z 0-9]*$//g')"
                local T_LIB_DEP="$(echo "$TEMP"  | sed 's/^[a-zA-Z0-g]*;[ ]*//g')"
                LIBS+=( "$T_LIB_NAME" )
                LIBS_DEP+=( "$T_LIB_DEP" )
                ;;
            T)
                TESTS+=( "$TEMP" )
                ;;
            E_INC)
                INCLUDE_FILE="$TEMP"
                ;;
            T_DIR)
                TESTS_DIR="$TEMP"
                ;;
            CT)
                C_TESTS+=( "$TEMP" )
                ;;
            CT_DO)
                COMPILER_TESTS=$TEMP
                ;;
            CT_DIR)
                COMPILER_TESTS_DIR=$TEMP
                ;;
            TOOLS_D)
                TOOLS_DIRECTORY="$TEMP"
                ;;
            LOG_PATH)
                LOG_MACRO_PATH="$TEMP"
                ;;
            LOG_UNDEF)
                LOG_GEN_UNDEF=$TEMP
                ;;
            LOG)
                LOG_TYPES="$TEMP"
                ;;
            CLANG_F)
                CLANG_FORMAT_CONFIG="$TEMP"
                ;;
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
    msg2 "Added target platforms:           ${DISPLAY_SERVER[@]}"
    msg2 "Added Operating Systems:          ${OS[@]}"
    msg2 "Added libs:                       ${LIBS[@]}"
    msg2 "Added Testa:                      ${TESTS[@]}"
    msg2 "Main engine include file:        '$INCLUDE_FILE'"
    msg2 "Tests dir:                       '$TESTS_DIR'"
    msg2 "Added Compiler Test:              ${C_TESTS[@]}"
    msg2 "Using compiler tests:            '$COMPILER_TESTS' (1 -- true; 0 -- false)"
    msg2 "Using compiler tests directory:  '$COMPILER_TESTS_DIR'"
    msg2 "Tools dir:                       '$TOOLS_DIRECTORY'"
    msg2 "Log macros file:                 '$LOG_MACRO_PATH'"
    msg2 "Create undefs for log macros:     $LOG_GEN_UNDEF (1 -- true; 0 -- false)"
    msg2 "Log types:                        $LOG_TYPES"
    msg2 "Clang format config:              $CLANG_FORMAT_CONFIG"
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
