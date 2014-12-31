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


# Make sure we are in the engine root directory
export ENGINE_ROOT=$(readlink -m $(dirname $0))
if [ -d $ENGINE_ROOT ]; then
    cd $ENGINE_ROOT
fi


echo -e "\x1b[1m"
echo "               CMake generator"
echo "               ==============="
echo ""
echo -e "\x1b[0m"

help_text() {
    msg1 "Help Message"
    echo ""
    echo "You will probably be just fine with running $0 without any arguments."
    echo ""
    echo "  +  - Turn everything on"
    echo ""
    echo "  g  - Force recompile GLEW"
    echo "  l  - Build libs"
    echo "  t  - Build tests"
    echo "  C  - count lines of code (needs cloc and bc)"
    echo "  c  - Clean up"
    echo "  f  - Reformat code with clang-format (this can only reformat code found with l and t)"
    echo ""
    echo " - Debug stuff"
    echo "  p  - Print parsed Config"
    echo ""
    echo "  NOTE: if you have already run this script and then disable tests and/or libs, you need to enable clean"
    echo ""
    echo "without any options the configuration is: $0 ltf"
    echo ""
    exit
}


DO_TESTS=0
DO_LIBS=0
DO_CLOC=0
DO_GLEW=0
DO_CLEAN=0
DO_FORMAT=0

ARG_STRING=""

# Load all functions in the generate dir
for i in $(find ${PWD}/generate/functions -name "*.sh" -type f -print); do
    source $i
done

CONFIG_FILE="$(pwd)/$CONFIG_FILE"
parseCFG

msg1 "Parsing comand line..."

for I in $@; do
    case $I in
        help|--help|-h)
            help_text
            ;;
    esac

    ARG_STRING="${ARG_STRING}${I}"
done

if [ -z "$ARG_STRING" ]; then
    ARG_STRING="ltf"
fi

for (( i=0; i<${#ARG_STRING}; ++i )); do
    I=${ARG_STRING:$i:1}

    case $I in
        +)
            DO_TESTS=1
            DO_LIBS=1
            DO_CLOC=1
            DO_GLEW=1
            DO_CLEAN=1
            DO_FORMAT=1
            ;;
        g) DO_GLEW=1  ; msg2 "Force rebuild GLEW";;
        l) DO_LIBS=1  ; msg2 "Generating libs";;
        t) DO_TESTS=1 ; msg2 "Generating tests";;
        C) DO_CLOC=1  ; msg2 "Enabling code counter";;
        c) DO_CLEAN=1 ; msg2 "Enabling clean";;
        f) DO_FORMAT=1; msg2 "Enabling automatic code formating";;

        p) printWhatParsed ;;
        *)
            error "Unknown Argument '$I'"
            help_text
            exit
            ;;
    esac

done

msg1 "Initiating and updating submodules"

GIT_EXEC=$(which git 2>/dev/null)

if [ -z ${GIT_EXEC} ]; then
    warning "Unable to find git. Please run git init, sync and update manualy"
else
    git submodule init    &> /dev/null
    git submodule sync    &> /dev/null
    git submodule update  &> /dev/null
fi

doGlew $DO_GLEW


if (( DO_CLEAN == 1 )); then
    clean
fi

if (( DO_LIBS == 1 )); then
    generateLogMacros $LOG_MACRO_PATH "$LOG_TYPES" $LOG_GEN_UNDEF
    compilerTests
    addTarget
    msg1 "Generating main include file $INCLUDE_FILE"
    engineHPP         1> $INCLUDE_FILE
fi

if (( DO_TESTS == 1 )); then
    tests
fi

if (( DO_TESTS == 1 || DO_LIBS == 1 )); then
    rootCMake 1> $(pwd)/$CMAKE_LISTS_NAME
fi

if (( DO_FORMAT == 1 )); then
    reformatSource
fi

if (( DO_CLOC == 1 )); then
    countLines
fi

msg1 "DONE"

# Switch back to the start directory where $0 was called
if [ -d $STARTDIR ]; then
    cd $STARTDIR
fi

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
