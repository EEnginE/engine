#!/usr/bin/gawk -f
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
