#!/usr/bin/awk -f
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
