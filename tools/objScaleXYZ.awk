#!/usr/bin/gawk -f
BEGIN {
    if( ARGC < 4 ) {
        print "ERROR: Usage: <scaleX> <scaleY> <scaleZ> [<files>]"
        print "  if no file is not given, use stdin"
        exit
    }

    scaleX=ARGV[1]
    scaleY=ARGV[2]
    scaleZ=ARGV[3]
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
        print $1 " " ( $2 * scaleX ) " " ( $3 * scaleY ) " " ( $4 * scaleZ )
    } else {
        print $0
    }
}
