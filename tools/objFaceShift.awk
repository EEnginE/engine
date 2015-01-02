#!/bin/env gawk -f

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
    if( ARGC < 2 ) {
        print "ERROR: Usage: <shift> [<files>]"
        print "  if no file is not given, use stdin"
        exit
    }

    shift=ARGV[1]
    for( i = 1; i < ARGC; i++ ) {
        ARGV[i] = ARGV[i+1]
    }
    ARGC--

    if (ARGC == 1) { # the shift value was the only thing, so force read from standard input (used by book)
        ARGC = 2
        ARGV[1] = "-"
    }
}
{
    if( $0 ~ "^f([ ]+[0-9]+){3}" ) {
        print $1 " " ( $2 + shift ) " " ( $3 + shift ) " " ( $4 + shift )
    } else if( $0 ~ "^f([ ]+[0-9]+//[0-9]+){3}" ) {
        f1=$2; sub( "//[0-9]+", "", f1 )
        f2=$3; sub( "//[0-9]+", "", f2 )
        f3=$4; sub( "//[0-9]+", "", f3 )

        n1=$2; sub( "[0-9]+//", "", n1 )
        n2=$3; sub( "[0-9]+//", "", n2 )
        n3=$4; sub( "[0-9]+//", "", n3 )
        print $1 " " ( f1 + shift ) "//" ( n1  + shift ) " " ( f2 + shift ) "//" ( n2  + shift ) " " ( f3 + shift ) "//" ( n3 + shift )
    } else {
        print $0
    }
}
