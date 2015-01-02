#!/bin/env bash

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

_ARGC_=$#

DATA=""

tryShiftAndExit() {
    if [ ! -f $1 ]; then
        return
    fi

    echo "$DATA" | $1 $2
    exit
}

if (( _ARGC_ != 1 )); then
    echo "USAGE: $0 <filename or - for stdin>"
    exit 4
fi

DATA=""

if [[ $1 == "-" ]]; then
    DATA="$(cat)"
else
    if [ -r $1 ]; then
        DATA="$(cat $1)"
    else
        echo "Can not read $1"
        exit 2
    fi
fi

PARSE=$(echo "$DATA" | awk '{ if( $0 ~ "^f([ ]+[0-9]+){3}" ) {print $2 " " $3 " " $4  " " }}' )

for I in $PARSE; do
    if [[ -z "$X" ]]; then
        X=$I
        continue
    fi

    if (( I < X )); then
        X=$I
    fi
done

if (( X == 0 )); then
    # Nothing to do here
    echo "$DATA"
    exit
fi

(( X *= -1 ))

tryShiftAndExit "./objFaceShift.awk"                     $X
tryShiftAndExit "$(dirname $0)/objFaceShift.awk"         $X
tryShiftAndExit "$(which objFaceShift.awk 2> /dev/null)" $X

exit 3

# kate: indent-mode shell; indent-width 4; replace-tabs on;
