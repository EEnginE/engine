#!/bin/env bash

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
