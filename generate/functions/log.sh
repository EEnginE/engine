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

ESC_CLEAR="\x1b[2K\x1b[0G"

msg1() {
    echo -e "${ESC_CLEAR}\x1b[1;32m==>\x1b[1;37m $@\x1b[0m"
}

msg2() {
    echo -e "${ESC_CLEAR}   \x1b[1;34m--\x1b[1;37m $@\x1b[0m"
}

msg3() {
    echo -e "${ESC_CLEAR}     \x1b[1;34m--\x1b[1;37m $@\x1b[0m"
}

msg4() {
    echo -e "${ESC_CLEAR}       \x1b[1;34m--\x1b[1;37m $@\x1b[0m"
}

found() {
   echo -e "${ESC_CLEAR} \x1b[1;33m--> \x1b[1;37mFound $@\x1b[0m"
}

error() {
    echo -e "${ESC_CLEAR}\x1b[1;31m==> ERROR:\x1b[1;37m $@\x1b[0m"
}

warning() {
    echo -e "${ESC_CLEAR}\x1b[1;33m==> WARNING:\x1b[1;37m $@\x1b[0m"
}

PB_COLLS=$(tput cols)
PAD0=$(printf '%0.1s' " "{1..1000} )
PAD1=$(printf '%0.1s' "#"{1..1000} )
PB_NEWLINE=0
PB_ENABLE=1

if [ -z "$PB_COLLS" ]; then
    PB_COLLS=150
fi

processBar() {
    if (( PB_ENABLE != 1 )); then
       return
    fi

    PB_FRAC=$( awk "BEGIN {printf \"%.2f\", ${1} / ${2}}" )
    PB_WIDTH=$(( $PB_COLLS / 2 ))


    PB_STR1="${1}/${2}$(printf '%*.*s' 0 $((${#2}-${#1})) "$PAD0" )  \x1b[1;37m-\x1b[0m $3"
    PB_STR1="${PB_STR1:0:$PB_WIDTH}$(
        printf '%*.*s' 0 $((${PB_WIDTH} - (${#PB_STR1}>$PB_WIDTH ? $PB_WIDTH : ${#PB_STR1}))) "$PAD0" )"

    PB_LENGTH=$(( $PB_COLLS - ${#PB_STR1} + 8 ))
    PB_L1=$(awk "BEGIN {printf \"%i\", (${1}/${2})*${PB_LENGTH}}")

    PB_STR2="$(printf '%*.*s' 0 $PB_L1 "$PAD1" )$(printf '%*.*s' 0 $((PB_LENGTH - PB_L1)) "$PAD0" )"
    PB_STR3=$(awk "BEGIN {printf \"%i\", (${1}/${2})*100}")

    if (( PB_NEWLINE == 0 )); then
       echo -ne "${ESC_CLEAR}\x1b[1;34m${PB_STR1}\x1b[1;37m [${PB_STR2}] \x1b[1;33m${PB_STR3}%\x1b[0m"
    else
       echo -e  "${ESC_CLEAR}\x1b[1;34m${PB_STR1}\x1b[1;37m [${PB_STR2}] \x1b[1;33m${PB_STR3}%\x1b[0m"
    fi
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
