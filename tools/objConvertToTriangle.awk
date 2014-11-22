#!/usr/bin/gawk -f
{
    if( $0 ~ "^f([ ]+(-)?[0-9]+){" NF-1 "}" && NF >= 4) {
        for(i = 1; i < NF - 2; i++) {
            print $1 " " $2 " " $(i+2) " " $(i+3)
        }
    } else {
        print $0
    }
}
