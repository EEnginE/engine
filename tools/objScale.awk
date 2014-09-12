#!/usr/bin/awk -f
BEGIN {
    if( ARGC < 2 ) {
        print "ERROR: Usage: <scale> [<files>]"
        print "  if no file is not given, use stdin"
        exit
    }

    scale=ARGV[1]
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
    if( $0 ~ "^v([ ]+(-)?[0-9]+(.[0-9]+)*){3}" ) {
        print $1 " " ( $2 * scale ) " " ( $3 * scale ) " " ( $4 * scale )
    } else {
        print $0
    }
}
