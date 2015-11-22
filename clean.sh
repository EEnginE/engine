#!/bin/bash

LIBS=( utils init render )

ESC_CLEAR="\x1b[2K\x1b[0G\x1b[?25l"

msg1() {
    wait
    echo -e "${ESC_CLEAR}\x1b[1;32m==>\x1b[1;37m $*\x1b[0m"
}

msg2() {
    wait
    echo -e "${ESC_CLEAR}   \x1b[1;34m--\x1b[1;37m $*\x1b[0m"
}

rm_save() {
    if [[ "$1" == *'*'* ]]; then
        msg2 "Removing $1: Files removed"
        rm -rf "$1"
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
rm_save Doxyfile

TEMP=$( ls -d tests/*/ )

for I in $TEMP; do
   rm_save "$I/CMakeLists.txt"
   if [ -f "$I/.gitignore" ]; then
      for J in $( cat "$I/.gitignore" ); do
         rm_save "$I/$J"
      done
   fi
done

