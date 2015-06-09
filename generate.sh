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


###########################
#### BEGIN Config Part ####
###########################

CONFIG_FILE="generate.cfg"

###########################
####  END Config Part  ####
###########################

STARTDIR="$PWD"

shopt -s extglob # Tells bash to do some fancy regex stuff

catch_INT() {
  error "User interrupt \x1b[1;31m[CTRL-C]" 1>&2
  exit
}

trap catch_INT INT

# Make sure we are in the engine root directory
export ENGINE_ROOT="$(readlink -m "$(dirname "$0")")"
[ -d "$ENGINE_ROOT" ] && cd "$ENGINE_ROOT"


EEC=$(awk "BEGIN {printf \"%i\", (${RANDOM} / 32767) * 6 + 31}") # COLORS!!! RANDOM!!!!! :)

EEE_C="\x1b[${EEC}m\x1b[1m"
EEE_R="\x1b[0m\x1b[1m"

#      _____ _____            _       _____
#     |  ___|  ___|          (_)     |  ___|
#     | |__ | |__ _ __   __ _ _ _ __ | |__
#     |  __||  __| '_ \ / _` | | '_ \|  __|
#     | |___| |__| | | | (_| | | | | | |___
#     \____/\____/_| |_|\__, |_|_| |_\____/
#                        __/ |
#                       |___/


eval "PAD_E=\"\$(printf '%0.1s' \" \"{1..$(( $(tput cols) / 2 - 17 ))} )\""

echo -e "\x1b[?25l"
echo -e "${PAD_E}${EEE_C} _____${EEE_R} _____           ${EEE_C} _ ${EEE_R}     ${EEE_C} _____  "
echo -e "${PAD_E}${EEE_C}|  ___${EEE_R}|  ___|          ${EEE_C}(_)${EEE_R}     ${EEE_C}|  ___| "
echo -e "${PAD_E}${EEE_C}| |__ ${EEE_R}| |__ _ __   __ _ _ _ __ ${EEE_C}| |__   "
echo -e "${PAD_E}${EEE_C}|  __|${EEE_R}|  __| '_ \ / _\` | | '_ \\\\${EEE_C}|  __|  "
echo -e "${PAD_E}${EEE_C}| |___${EEE_R}| |__| | | | (_| | | | | ${EEE_C}| |___  "
echo -e "${PAD_E}${EEE_C}\____/${EEE_R}\____/_| |_|\__, |_|_| |_${EEE_C}\____/  "
echo -e "${PAD_E}${EEE_C}      ${EEE_R}             __/ |               "
echo -e "${PAD_E}${EEE_C}      ${EEE_R}            |___/                "
echo -e ""
echo -e ""

help_text() {
  msg1 "Help Message"
cat << EOF

    You will probably be just fine with running $0 without any arguments.

      +  - Turn everything on (but only gltCcf)

      l  - Build libs
      t  - Build tests
      C  - count lines of code (needs cloc and bc)
      c  - Clean up
      f  - Reformat code with clang-format (this can only reformat code found with l and t)

     - Special flags
      q  - disable procress bar and some escape sequenzes
      Q  - disable procress bar complete
      b  - build the project
      B  - configure build options
      G  - disable pulling git submodules
      u  - check for git updates / install git (Windows only)

     - Debug stuff
      p  - Print parsed Config

      NOTE: if you have already run this script and then disable tests and/or libs, you need to enable clean

    without any options the configuration is: $0 ltf

    If you want to automaticaly build the project (on *nix systems) run
    $ $0
    $ $0 b
     - OR -
    $ $0 ltfb
EOF
    echo -e "\x1b[?25h"
    exit
}


DO_TESTS=0
DO_LIBS=0
DO_CLOC=0
DO_GLEW=0
DO_CLEAN=0
DO_FORMAT=0
DO_BUILD=0
DO_CFG_BUILD=0
SKIP_PARSING=0
SKIP_SUB_M=0

PRINT_PARSED=0
WIN_UPDATE=0

ARG_STRING=""

# Load all functions in the generate dir
for i in generate/functions/*.sh; do
  source "$i"
done

msg1 "Parsing comand line..."

for I in "$@"; do
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

ARG_STRING="${ARG_STRING//-/}"

CMD_FLAGS="Result:"

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
    l) DO_LIBS=1      ; CMD_FLAGS="$CMD_FLAGS [LIBS]";;
    t) DO_TESTS=1     ; CMD_FLAGS="$CMD_FLAGS [TESTS]";;
    C) DO_CLOC=1      ; CMD_FLAGS="$CMD_FLAGS [COUNT]";;
    c) DO_CLEAN=1     ; CMD_FLAGS="$CMD_FLAGS [CLEAN]";;
    f) DO_FORMAT=1    ; CMD_FLAGS="$CMD_FLAGS [FORMAT]";;
    b) DO_BUILD=1     ; CMD_FLAGS="$CMD_FLAGS -BUILD-";;
    B) DO_CFG_BUILD=1 ; CMD_FLAGS="$CMD_FLAGS -CFG build-";;
    q) ESC_CLEAR=""
       PB_NEWLINE=1   ; CMD_FLAGS="$CMD_FLAGS -quiet-";;
    Q) PB_ENABLE=0    ; CMD_FLAGS="$CMD_FLAGS -pb off-";;
    G) SKIP_SUB_M=1   ; CMD_FLAGS="$CMD_FLAGS -skip submodules-";;

    p) PRINT_PARSED=1 ; CMD_FLAGS="$CMD_FLAGS -print parsed-";;
    u) WIN_UPDATE=1   ; CMD_FLAGS="$CMD_FLAGS -updating-";;
    *)
      error "Unknown Argument '$I'"
      help_text
      exit
      ;;
  esac

done

CMD_FLAGS="${CMD_FLAGS//[/\\x1b[35m[\\x1b[36m}"
CMD_FLAGS="${CMD_FLAGS//]/\\x1b[35m]}"

msg2 "$CMD_FLAGS"

CONFIG_FILE="$(pwd)/$CONFIG_FILE"
parseCFG

detectOsAndSetup

if (( SKIP_SUB_M == 0 )); then
  msg1 "Initiating and updating submodules"

  if [ -z "${GIT_EXEC}" ]; then
    warning "GIT not found. Please run git init, sync and update manualy"
  else
    processBar 1 5 "init"
    "${GIT_EXEC}" submodule init                      &> /dev/null

    processBar 2 5 "sync"
    "${GIT_EXEC}" submodule sync --recursive          &> /dev/null

    processBar 3 5 "update --init"
    "${GIT_EXEC}" submodule update --init --recursive &> /dev/null

    processBar 4 5 "update"
    "${GIT_EXEC}" submodule update --recursive        &> /dev/null

    processBar 5 5 "Running make deps"
  fi
fi

makeDeps > "$DEPS_MAIN_DIR/$CMAKE_LISTS_NAME"

(( PRINT_PARSED == 1 )) && printWhatParsed
(( DO_CLEAN     == 1 )) && clean


if (( DO_LIBS == 1 )); then
  [ ! -d "${INCLUDES_DIR}" ] && mkdir "${INCLUDES_DIR}"
  generateLogMacros "$LOG_MACRO_PATH" "$LOG_TYPES" "$LOG_GEN_UNDEF"
  compilerTests
  addTarget
  msg1 "Generating main include file ${INCLUDES_DIR}/$INCLUDE_FILE"
  engineHPP 1> "${INCLUDES_DIR}/$INCLUDE_FILE"
fi

(( DO_TESTS      == 1 ))                 && tests
(( DO_TESTS      == 1 || DO_LIBS == 1 )) && rootCMake 1> "$(pwd)/$CMAKE_LISTS_NAME"
(( DO_FORMAT     == 1 ))                 && reformatSource
(( DO_CLOC       == 1 ))                 && countLines
(( DO_CFG_BUILD  == 1 ))                 && configureBuild
(( DO_BUILD      == 1 ))                 && buildProject


msg1 "DONE\x1b[?25h"

# Switch back to the start directory where $0 was called
[ -d "$STARTDIR" ] && cd "$STARTDIR"

# kate: indent-mode shell; indent-width 2; replace-tabs on; line-numbers on;
