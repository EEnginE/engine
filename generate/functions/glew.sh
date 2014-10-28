#!/bin/bash

doGlew() {
    local ARGC=$#
   
    if (( $ARGC != 1 )); then
        echo "ERROR: doGlew needs 1 argument"
        return
    fi
    
    cd GLEW
    
    git checkout master &> /dev/null
    git pull            &> /dev/null
    
    if [ -d lib -a ! $1 -eq 1 ]; then
        echo "INFO: GLEW: Nothing to do here"
        cd ..
        return
    fi
    
    echo "INFO: GLEW: Making extensions..."
    make extensions &> /dev/null
    
    echo "INFO: GLEW: Building..."
    make &> /dev/null
    
    cd ..
}
