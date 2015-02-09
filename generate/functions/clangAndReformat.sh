#!/usr/bin/env bash

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

reformatSource() {
    msg1 "Reformating source code..."

    CLANG_FORMAT=$(which clang-format 2>/dev/null)

    if [ -z "$CLANG_FORMAT" ]; then
        warning "clang-format not found: can not reformat the source code"
        warning "make sure that clang-format is in PATH"
        return
    fi

    if [ ! -f "$(pwd)/$CLANG_FORMAT_CONFIG" ]; then
        error "Can not find clang-format configuation file $CLANG_FORMAT_CONFIG"
        return
    fi

    cp -f "$(pwd)/$CLANG_FORMAT_CONFIG" .clang-format

    local i COUNTER NUM
    COUNTER=1
    NUM=${#ALL_SOURCE_FILES[@]}
    for i in "${ALL_SOURCE_FILES[@]}"; do
        processBar $COUNTER "$NUM" "$i"
        (( COUNTER++ ))

        if [ ! -f "$i" ]; then
            warning "File $i not found"
            continue
        fi

        "$CLANG_FORMAT" -i "$i"
    done
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
