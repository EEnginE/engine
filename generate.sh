#!/bin/bash

ARGC=$#
ARGV=$*

###########################
#### BEGIN Config Part ####
###########################

CONFIG_FILE="generate.cfg"

###########################
####  END Config Part  ####
###########################

STARTDIR="$PWD"


########################################################################################
############################################################################################################################
#####                   #############################################################################################################
###  Directory Locations  ######################################################################################################################
#####                   #############################################################################################################
############################################################################################################################
########################################################################################


# Make sure we are in the engine root directory
export ENGINE_ROOT=$(readlink -m $(dirname $0))
if [ -d $ENGINE_ROOT ]; then
    cd $ENGINE_ROOT
fi

parseCFG() {
    if [ ! -f "$CONFIG_FILE" ]; then
        echo "ERROR: Cannot find config file $CONFIG_FILE"
        exit 2
    fi

    echo "INFO: Parsing Config file $CONFIG_FILE"

    local LINE
    while read LINE; do
        LINE="$(echo "$LINE" | sed 's/#.*//g')"   # Remove comments
        LINE="$(echo "$LINE" | sed 's/[ ]*$//g')" # Remove ' ' at the end of line

        if [ -z "$LINE" ]; then
            continue
        fi

        local TEMP="$(echo "$LINE" | sed 's/^[a-zA-Z 0-9_\.]*:[ ]*//g' )"

        case "$(echo "$LINE" | sed 's/:[a-zA-Z 0-9_;\.\/]*$//g' )" in
            CM)
                echo "INFO:   -- Name of CMake files to generate: '$TEMP'"
                CMAKE_LISTS_NAME="$TEMP"
                ;;
            CM_V)
                echo "INFO:   -- CMake version to use: '$TEMP'"
                CMAKE_VERSION="$TEMP"
                ;;
            CLA)
                echo "INFO:   -- Name of the clang autocomplete file: '$TEMP'"
                CLANG_COMPLETE="$TEMP"
                ;;
            PRO)
                echo "INFO:   -- Project Name: $TEMP"
                PROJECT_NAME=$TEMP
                ;;
            P)
                echo "INFO:   -- Added target platform: $TEMP"
                DISPLAY_SERVER+=( "$TEMP" )
                ;;
            OS)
                local T_OS=$(echo $TEMP | sed 's/;[a-zA-Z 0-9_\/\.]*$//g' )
                local T_DS=$(echo $TEMP | sed 's/^[a-zA-Z 0-9_\/\.]*;//g' )
                echo "INFO:   -- Added Operating System $T_OS"
                OS+=( $T_OS )
                eval "DS_${T_OS}=( $T_DS )"
                ;;
            L)
                local T_LIB_NAME="$(echo "$TEMP" | sed 's/;[a-z A-Z 0-9]*$//g')"
                local T_LIB_DEP="$(echo "$TEMP"  | sed 's/^[a-zA-Z0-g]*;[ ]*//g')"
                echo "INFO:   -- Added lib: $T_LIB_NAME [$T_LIB_DEP]"
                LIBS+=( "$T_LIB_NAME" )
                LIBS_DEP+=( "$T_LIB_DEP" )
                ;;
            T)
                echo "INFO:   -- Added Test: $TEMP"
                TESTS+=( "$TEMP" )
                ;;
            E_INC)
                echo "INFO:   -- Main engine include file: '$TEMP'"
                INCLUDE_FILE="$TEMP"
                ;;
            T_DIR)
                echo "INFO:   -- Tests dir: '$TEMP'"
                TESTS_DIR="$TEMP"
                ;;
            CT)
                echo "INFO:   -- Added Compiler Test: $TEMP"
                C_TESTS+=( "$TEMP" )
                ;;
            CT_DO)
                echo "INFO:   -- Using compiler tests: '$TEMP' (1 -- true; 0 -- false)"
                COMPILER_TESTS=$TEMP
                ;;
            CT_DIR)
                echo "INFO:   -- Using compiler tests directory: '$TEMP'"
                COMPILER_TESTS_DIR=$TEMP
                ;;
            TOOLS_D)
                echo "INFO:   -- Tools dir: '$TEMP'"
                TOOLS_DIRECTORY="$TEMP"
                ;;
            LOG_PATH)
                echo "INFO:   -- Log macros file: '$TEMP'"
                LOG_MACRO_PATH="$TEMP"
                ;;
            LOG_UNDEF)
                echo "INFO:   -- Create undefs for log macros: $TEMP (1 -- true; 0 -- false)"
                LOG_GEN_UNDEF=$TEMP
                ;;
            LOG)
                echo "INFO:   -- Log types: $TEMP"
                LOG_TYPES="$TEMP"
                ;;
            *)
                echo "ERROR: Unknown option"
                exit
                ;;
        esac

    done < "$CONFIG_FILE"
}


rm_save() {
    if [[ $1 == *'*'* ]]; then
        echo "INFO: Removing $1: Files removed"
        rm -rf $1
        exit 0
    fi
    if [ -e $1 ]; then
        if [ -d $1 ]; then
            echo "INFO: Removing $1: Directory removed"
            rm -rf $1
        else
            echo "INFO: Removing $1: File removed"
            rm $1
        fi
    else
        echo "INFO: Removing $1: Nothing to do"
    fi
}

clean() {
    echo "INFO: Cleaning"

    local I

    for (( I = 0; I < ${#LIBS[@]}; ++I )); do
        rm_save ${LIBS[$I]}/CMakeLists.txt
    done

    rm_save $INCLUDE_FILE
    rm_save $LOG_MACRO_PATH
    rm_save CMakeLists.txt
    rm_save defines.hpp
    rm_save Doxyfile

    local TEMP

    TEMP=$( ls -d ${COMPILER_TESTS_DIR}/*/ )

    for I in $TEMP; do
       rm_save $I/CMakeLists.txt
       if [ -f $I/.gitignore ]; then
          for J in $( cat $I/.gitignore ); do
             rm_save "$I/$J"
          done
       fi
    done

    TEMP=$( ls -d ${TESTS_DIR}/*/ )

    for I in $TEMP; do
       rm_save $I/CMakeLists.txt
       if [ -f $I/.gitignore ]; then
          for J in $( cat $I/.gitignore ); do
             rm_save "$I/$J"
          done
       fi
    done

    cd GLEW

    make clean &> /dev/null

    cd ..
}


help_text() {
    echo "You will probably be just fine with running $0 without any arguments."
    echo ""
    echo "  all      - Turn everything on"
    echo "  none     - Turn everything off"
    echo ""
    echo "  glew     - Force recompile GLEW"
    echo "  noGlew   - Do not force recompile GLEW"
    echo ""
    echo "  libs     - Build libs"
    echo "  noLibs   - Do not build libs"
    echo ""
    echo "  tests    - Build tests"
    echo "  noTests  - Do not build tests"
    echo ""
    echo "  count    - Enable counting lines of code (needs cloc and bc)"
    echo "  noCount  - Do not count lines of code"
    echo ""
    echo "  clean    - Clean up"
    echo "  noClean  - Do not clean up"
    echo ""
    echo "  help     - Print this help message and exit"
    echo ""
    echo "  NOTE: if you have already run this script and then disable tests and/or libs, you need to enable clean"
    echo ""
    echo "DEFAULT:"
    echo "  - glew:  no"
    echo "  - libs:  yes"
    echo "  - tests: yes"
    echo "  - clean: no"
    echo ""
}

countLines() {
    CLOC_EXEC="$(which cloc 2> /dev/null)"
    which bc &> /dev/null
    RET_BC=$?

    echo ""
    echo ""
    echo ""

    if [ -n "$CLOC_EXEC" -a $RET_BC -eq 0 ]; then
        $CLOC_EXEC --not-match-d='([a-zA-Z_/]*\.[a-zA-Z\._]+|build|doxygen|GLEW)' ./ | tee temp_cloc.txt;
        CALC_ALL="$(cat temp_cloc.txt | grep SUM)"
        CALC_ALL="$(echo $CALC_ALL    | sed 's/SUM: [0-9]*//g')"
        CALC_ALL="$(echo $CALC_ALL    | sed 's/ /\+/g')"
        echo ""
        echo "TOTAL: $(echo $CALC_ALL | bc )"

        if [ -e temp_cloc.txt ]; then
            rm temp_cloc.txt
        fi
    fi
}


########################################################################################
############################################################################################################################
#####                  ##############################################################################################################
###  Load the Functions  ###############################################################################################################
#####                  ##############################################################################################################
############################################################################################################################
########################################################################################

for i in $(find ${PWD}/generate/functions -name "*.sh" -type f -print); do
    source $i
done


echo ""
echo "               CMake generator"
echo "               ==============="
echo ""
echo ""
echo ""
echo "INFO: Initiating and updating submodules"

git submodule init    > /dev/null
git submodule sync    > /dev/null
git submodule update  > /dev/null

CONFIG_FILE="$(pwd)/$CONFIG_FILE"

DO_TESTS=1
DO_LIBS=1
DO_CLOC=0
DO_GLEW=0
DO_CLEAN=0

for I in $@; do

    case $I in
        all)
            DO_TESTS=1
            DO_LIBS=1
            DO_CLOC=1
            DO_GLEW=1
            DO_CLEAN=1
            ;;
        none)
            DO_TESTS=0
            DO_LIBS=0
            DO_CLOC=0
            DO_GLEW=0
            DO_CLEAN=0
            ;;
        help)
            help_text
            exit
            ;;
        glew)       DO_GLEW=1  ;;
        noGlew)     DO_GLEW=0  ;;
        libs)       DO_LIBS=1  ;;
        noLibs)     DO_LIBS=0  ;;
        tests)      DO_TESTS=1 ;;
        noTests)    DO_TESTS=0 ;;
        count)      DO_CLOC=1  ;;
        noCount)    DO_CLOC=0  ;;
        clean)      DO_CLEAN=1 ;;
        noClean)    DO_CLEAN=0 ;;
        *)
            echo "ERROR: Unknown Argument $I"
            help_text
            exit
            ;;
    esac

done

parseCFG
doGlew $DO_GLEW

if (( DO_CLEAN == 1 )); then
    clean
fi

if (( DO_LIBS == 1 )); then
    generateLogMacros $LOG_MACRO_PATH "$LOG_TYPES" $LOG_GEN_UNDEF
    compilerTests
    addTarget
    echo "INFO: Generating main include file $INCLUDE_FILE"
    engineHPP         1> $INCLUDE_FILE
fi

if (( DO_TESTS == 1 )); then
    tests
fi

if (( DO_TESTS == 1 || DO_LIBS == 1 )); then
    rootCMake 1> $(pwd)/$CMAKE_LISTS_NAME
fi


if (( DO_CLOC == 1 )); then
    countLines
fi



echo "INFO: DONE"



# Switch back to the start directory where $0 was called
if [ -d $STARTDIR ]; then
    cd $STARTDIR
fi

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
