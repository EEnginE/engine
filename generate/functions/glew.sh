#!/bin/bash

doGlew() {
    local ARGC=$#
   
    if (( $ARGC != 1 )); then
        error " doGlew needs 1 argument"
        return
    fi
    
    cd GLEW
    
    git checkout master &> /dev/null
    git pull            &> /dev/null
    
    if [ -d lib -a ! $1 -eq 1 ]; then
        msg1 "GLEW: Nothing to do here"
        cd ..
        return
    fi
    
    msg1 "GLEW: Making extensions..."
    make extensions &> /dev/null
    
    msg1 "GLEW: Building..."
    make &> /dev/null
    
    cd ..
}
