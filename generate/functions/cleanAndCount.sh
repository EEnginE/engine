#!/bin/bash

rm_save() {
    if [[ $1 == *'*'* ]]; then
        msg2 "Removing $1: Files removed"
        rm -rf $1
        exit 0
    fi
    if [ -e $1 ]; then
        if [ -d $1 ]; then
            msg2 "Removing $1: Directory removed"
            rm -rf $1
        else
            msg2 "Removing $1: File removed"
            rm $1
        fi
    else
        msg2 "Removing $1: Nothing to do"
    fi
}

clean() {
    msg1 "Cleaning"

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


# Count lines of code with cloc
countLines() {
    CLOC_EXEC="$(which cloc 2> /dev/null)"
    which bc &> /dev/null
    RET_BC=$?

    if [ -n "$CLOC_EXEC" -a $RET_BC -eq 0 ]; then
        echo ""
        echo ""
        echo ""
        $CLOC_EXEC --not-match-d='([a-zA-Z_/]*\.[a-zA-Z\._]+|build|doxygen|GLEW)' ./ | tee temp_cloc.txt;
        CALC_ALL="$(cat temp_cloc.txt | grep SUM)"
        CALC_ALL="$(echo $CALC_ALL    | sed 's/SUM: [0-9]*//g')"
        CALC_ALL="$(echo $CALC_ALL    | sed 's/ /\+/g')"
        echo ""
        msg1 "TOTAL: $(echo $CALC_ALL | bc )"

        if [ -e temp_cloc.txt ]; then
            rm temp_cloc.txt
        fi
    else
        error "Can not count lines of code because bc and or cloc are not installed"
    fi
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
