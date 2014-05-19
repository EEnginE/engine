#!/bin/bash

doGlew() {
    ARGC=$#
   
    if (( $ARGC != 1 )); then
        echo "ERROR: doGlew needs 1 argument"
        return
    fi
    
    cd GLEW
    
    if [ -d lib -a ! $1 -eq 1 ]; then
        echo "INFO: GLEW: Nothing to do here"
        cd $CD_TO_THIS_DIR
        return
    fi
    
    echo "INFO: GLEW: Making extensions..."
    make extensions
    
    echo "INFO: GLEW: building..."
    make
    
    cd $CD_TO_THIS_DIR
}
