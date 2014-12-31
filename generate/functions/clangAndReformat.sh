#!/bin/bash

reformatSource() {
    msg1 "Reformating source code..."

    CLANG_FORMAT=$(which clang-format 2>/dev/null)

    if [ -z "$CLANG_FORMAT" ]; then
        warning "clang-format not found: can not reformat the source code"
        warning "make sure that clang-format is in PATH"
        return
    fi

    msg2 "Found clang-format: $CLANG_FORMAT"

    if [ ! -f "$(pwd)/$CLANG_FORMAT_CONFIG" ]; then
        error "Can not find clang-format configuation file $CLANG_FORMAT_CONFIG"
        return
    fi

    cp -f $(pwd)/$CLANG_FORMAT_CONFIG .clang-format

    local i COUNTER NUM
    COUNTER=1
    NUM=${#ALL_SOURCE_FILES[@]}
    for i in ${ALL_SOURCE_FILES[@]}; do
        processBar $COUNTER $NUM $i
        (( COUNTER++ ))

        if [ ! -f "$i" ]; then
            warning "File $i not found"
            continue
        fi

        $CLANG_FORMAT -i $i
    done
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
