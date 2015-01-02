#!/bin/gawk -f

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

BEGIN {
    if( ARGC < 4 ) {
        print "ERROR: Usage: <moveX> <moveY> <moveZ> [<files>]"
        print "  if no file is not given, use stdin"
        exit
    }

    moveX=ARGV[1]
    moveY=ARGV[2]
    moveZ=ARGV[3]
    for( i = 1; i < ARGC; i++ ) {
        ARGV[i] = ARGV[i+3]
    }
    ARGC -= 3

    if (ARGC == 1) { # the shift value was the only thing, so force read from standard input (used by book)
        ARGC = 2
        ARGV[1] = "-"
    }
}
{
    if( $0 ~ "^v([ ]+(-)?[0-9]+(.[0-9]+)*){3}" ) {
        print $1 " " ( $2 + moveX ) " " ( $3 + moveY ) " " ( $4 + moveZ )
    } else {
        print $0
    }
}
