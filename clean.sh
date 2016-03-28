#!/bin/bash

LIBS=( utils init render loader )

ESC_CLEAR="\x1b[2K\x1b[0G"

msg1() {
    wait
    echo -e "${ESC_CLEAR}\x1b[1;32m==>\x1b[1;37m $*\x1b[0m"
}

msg2() {
    wait
    echo -e "${ESC_CLEAR}   \x1b[1;34m--\x1b[1;37m $*\x1b[0m"
}

rm_save() {
    local I TMP
    if [[ "$1" == *'*'* ]]; then
        TMP=( $1 )
        for I in "${TMP[@]}"; do
            [[ "$I" == "$1" ]] && return # Nothing
            rm_save "$I"
        done
        return
    fi
    if [ -e "$1" ]; then
        if [ -d "$1" ]; then
            msg2 "Removing \x1b[35m[\x1b[36mDirectory\x1b[35m]\x1b[37m $1"
            rm -rf "$1"
        else
            msg2 "Removing \x1b[35m[\x1b[36mFile\x1b[35m]\x1b[37m $1"
            rm "$1"
        fi
    else
        msg2 "Removing \x1b[35m[\x1b[36mNothing to do\x1b[35m]\x1b[37m $1"
    fi
}

cd "$(dirname "$0")"

msg1 "Cleaning"

for (( I = 0; I < ${#LIBS[@]}; ++I )); do
   rm_save "${LIBS[$I]}/CMakeLists.txt"
   rm_save "${LIBS[$I]}/${LIBS[$I]}_export.hpp"
done

rm_save "include"
rm_save "utils/log/uMacros.hpp"
rm_save "utils/uEnum2Str.hpp"
rm_save "utils/uEnum2Str.cpp"
rm_save Doxyfile

for I in tests/*/; do
   rm_save "${I}CMakeLists.txt"
   if [ -f "${I}.gitignore" ]; then
      for J in $( cat "${I}.gitignore" ); do
         rm_save "${I}$J"
      done
   fi
done

