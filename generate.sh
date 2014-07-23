#!/bin/bash

ARGC=$#
ARGV=$*

###########################
#### BEGIN Config Part ####
###########################

SOURCE_FILE="sources.cmake"
INCLUDE_FILE="engine.hpp"

UTILS_LIB="utils"
UTILS_SRC="utils.cmake"
BASENAME_UTILS="UTILS"

INIT_LIB="init"
INIT_SRC="init.cmake"
BASENAME_INIT="INIT"

RENDER_LIB="render"
RENDER_SRC="render.cmake"
BASENAME_RENDER="RENDER"

TESTS_DIR="tests"

VAR_FOR_CMAKE_ROOT="PROJECT_SOURCE_DIR"


## The platform dirs ##
X11="x11"
WAYLAND="wayland"
MIR="mir"
WINDOWS="windows"


## Log macro config ##
LOG_MACRO_PATH="utils/log/uMacros.hpp"
LOG_TYPES="a b c d e f g h i j k l m n o p q r s t u v w x y z"
LOG_GEN_UNDEF=1

###########################
####  END Config Part  ####
###########################


STARTDIR="$PWD"


########################################################################################
############################################################################################################################
#####           #####################################################################################################################
###  Where am I?  ######################################################################################################################
#####           #####################################################################################################################
############################################################################################################################
########################################################################################


# Make sure we are in the engine root directory
CD_TO_THIS_DIR="$( dirname $0 )"

if [ -d $CD_TO_THIS_DIR ]; then
    cd $CD_TO_THIS_DIR
fi

rm_save() {
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
    
    rm_save $INIT_SRC
    rm_save $RENDER_SRC
    rm_save $UTILS_SRC
    
    rm_save $INCLUDE_FILE
    rm_save $LOG_MACRO_PATH
    rm_save tests/CMakeLists.txt
    rm_save defines.hpp
    rm_save Doxyfile
    
    TEMP=$( ls -d tests/*/ )
    
    for I in $TEMP; do
       rm_save $I/CMakeLists.txt
       rm_save $I/$SOURCE_FILE
    done
    
    cd GLEW
    
    make clean &> /dev/null
    
    cd ..
}


standard() {
    generateLogMacros $LOG_MACRO_PATH "$LOG_TYPES" $LOG_GEN_UNDEF
    finSources        $INIT_LIB   $INIT_SRC   $X11 $WAYLAND $MIR $WINDOWS $BASENAME_INIT
    finSources        $RENDER_LIB $RENDER_SRC $X11 $WAYLAND $MIR $WINDOWS $BASENAME_RENDER
    finSources        $UTILS_LIB  $UTILS_SRC  $X11 $WAYLAND $MIR $WINDOWS $BASENAME_UTILS
    engineHPP         $INCLUDE_FILE
    tests
}

help_text() {
    echo "You will probably be just fine with running $0 without any arguments."
    echo "In fact, the standard scenario will be selected."
    echo ""
    echo "  all      - Do everything: clean, GLEW, generate sources"
    echo "  glew     - Compile GLEW and generate sources"
    echo "  standard - Generate GLEW (if needed) and the sources"
    echo "  count    - Count the lines of code (needs cloc and bc) and run the standard scenario"
    echo "  clean    - Clean up"
    echo "  help     - Print this help message and exit"
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

source ${PWD}/generate/functions/logMacros.sh
source ${PWD}/generate/functions/findSources.sh
source ${PWD}/generate/functions/engineHPP.sh
source ${PWD}/generate/functions/glew.sh
source ${PWD}/generate/functions/tests.sh

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

if (( ARGC == 1 )); then
    case $1 in
        all) 
            clean
            doGlew 1
            standard
            ;;
        help)
            help_text
            ;;
        glew)
            doGlew 1
            standard
            ;;
        standard)
            doGlew 0
            standard
            ;;
        clean)
            clean
            ;;
        count)
            doGlew 0
            standard
            countLines
            ;;
        *)
            help_text
            ;;
    esac
else
    doGlew 0
    standard
fi



echo "INFO: DONE"

echo ""
echo ""



# Switch back to the start directory where $0 was called
if [ -d $STARTDIR ]; then
    cd $STARTDIR
fi
