#!/bin/bash

msg1() {
    echo -e "\x1b[2K\x1b[0G\x1b[1;32m==>\x1b[1;37m $@\x1b[0m"
}

msg2() {
    echo -e "\x1b[2K\x1b[0G   \x1b[1;34m--\x1b[1;37m $@\x1b[0m"
}

msg3() {
    echo -e "\x1b[2K\x1b[0G     \x1b[1;34m--\x1b[1;37m $@\x1b[0m"
}

msg4() {
    echo -e "\x1b[2K\x1b[0G       \x1b[1;34m--\x1b[1;37m $@\x1b[0m"
}

error() {
    echo -e "\x1b[2K\x1b[0G\x1b[1;31m==> ERROR:\x1b[1;37m $@\x1b[0m"
}

warning() {
    echo -e "\x1b[2K\x1b[0G\x1b[1;33m==> WARNING:\x1b[1;37m $@\x1b[0m"
}

PB_COLLS=$(tput cols)
PAD0=$(printf '%0.1s' " "{1..1000} )
PAD1=$(printf '%0.1s' "#"{1..1000} )

processBar() {
    PB_FRAC=$( awk "BEGIN {printf \"%.2f\", ${1} / ${2}}" )
    PB_WIDTH=$(( $PB_COLLS / 2 ))

    PB_STR1="${1}/${2}$(printf '%*.*s' 0 $((${#2}-${#1})) "$PAD0" )  \x1b[1;37m-\x1b[0m $3"
    PB_STR1="${PB_STR1:0:$PB_WIDTH}$(printf '%*.*s' 0 $((${PB_WIDTH}-${#PB_STR1})) "$PAD0" )"

    PB_LENGTH=$(( $PB_COLLS - ${#PB_STR1} + 8 ))
    PB_L1=$(awk "BEGIN {printf \"%i\", (${1}/${2})*${PB_LENGTH}}")

    PB_STR2="$(printf '%*.*s' 0 $PB_L1 "$PAD1" )$(printf '%*.*s' 0 $((PB_LENGTH - PB_L1)) "$PAD0" )"
    PB_STR3=$(awk "BEGIN {printf \"%i\", (${1}/${2})*100}")

    echo -ne "\x1b[2K\x1b[0G\x1b[1;34m${PB_STR1}\x1b[1;37m [${PB_STR2}] \x1b[1;33m${PB_STR3}%\x1b[0m"
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
