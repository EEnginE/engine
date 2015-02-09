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

rm_save() {
    if [[ "$1" == *'*'* ]]; then
        msg2 "Removing $1: Files removed"
        rm -rf "$1"
        return
    fi
    if [ -e "$1" ]; then
        if [ -d "$1" ]; then
            msg2 "Removing \x1b[35m[\x1b[36mDirectory\x1b[35m]\x1b[37m $1"
            rm [rf "$1"
        else
            msg2 "Removing \x1b[35m[\x1b[36mFile\x1b[35m]\x1b[37m $1"
            rm "$1"
        fi
    else
        msg2 "Removing \x1b[35m[\x1b[36mNothing to do\x1b[35m]\x1b[37m $1"
    fi
}

clean() {
    msg1 "Cleaning"

    local I

    for (( I = 0; I < ${#LIBS[@]}; ++I )); do
        rm_save "${LIBS[$I]}/CMakeLists.txt"
    done

    rm_save "$INCLUDE_FILE"
    rm_save "$LOG_MACRO_PATH"
    rm_save CMakeLists.txt
    rm_save defines.hpp
    rm_save "${DEBUG_DEF_FILE}.in.hpp"
    rm_save "${DEBUG_DEF_FILE}.hpp"
    rm_save "${CLANG_COMPLETE}"
    rm_save .clang-format
    rm_save Doxyfile

    local TEMP

    TEMP=$( ls -d ${COMPILER_TESTS_DIR}/*/ )

    for I in $TEMP; do
       rm_save "$I/CMakeLists.txt"
       if [ -f "$I/.gitignore" ]; then
          for J in $( cat "$I/.gitignore" ); do
             rm_save "$I/$J"
          done
       fi
    done

    TEMP=$( ls -d ${TESTS_DIR}/*/ )

    for I in $TEMP; do
       rm_save "$I/CMakeLists.txt"
       if [ -f "$I/.gitignore" ]; then
          for J in $( cat "$I/.gitignore" ); do
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
        $CLOC_EXEC --not-match-d='([a-zA-Z_/]*\.[a-zA-Z\._]+|build|doxygen|dependencies|GLEW)' ./ | tee temp_cloc.txt;
        CALC_ALL="$(grep SUM temp_cloc.txt )"
        CALC_ALL="$(echo "$CALC_ALL"  | sed 's/SUM: *[0-9]* *//g')"
        CALC_ALL="$(echo "$CALC_ALL"  | sed 's/ \+/\+/g')"
        echo ""
        msg1 "TOTAL: $(echo "$CALC_ALL" | bc )"

        if [ -e temp_cloc.txt ]; then
            rm temp_cloc.txt
        fi
    else
        error "Can not count lines of code because bc and or cloc are not installed"
    fi
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
