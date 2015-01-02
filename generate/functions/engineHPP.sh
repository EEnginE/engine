#!/bin/bash

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

engineHPP() {
    local I i

    cat <<EOF
/*!
 * \file engine.hpp
 * \brief Includes all headder files from E Engine
 * \warning This is an automatically generated file
 */


#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "defines.hpp"

// For all display servers:

EOF

    for I in "${ALL_ALL_HPP[@]}"; do
	if [ "$(basename $I)" == "engine.hpp" ]; then
            continue
        fi
        echo "#include \"$(basename $I)\""
    done

    local DS_AND_OS=()
    local TEMP
    for i in "${OS[@]}"; do
	eval "TEMP=( \"\${DS_${i}[@]}\" )"
	for I in "${TEMP[@]}"; do
	    DS_AND_OS+=( "${i^^}_${I^^}" )
	done
    done


    local TEMP_SIZE TEMP
    for i in "${DS_AND_OS[@]}"; do
	eval "TEMP_SIZE=\${#ALL_$(echo $i | sed 's/^[a-zA-Z0-9_]*_//g' )_HPP[@]}"
	if (( TEMP_SIZE == 0 )); then
	    continue
	fi

	eval "TEMP=( \"\${ALL_$(echo $i | sed 's/^[a-zA-Z0-9_]*_//g' )_HPP[@]}\" )"

	echo ""
	echo "#if $i"
	echo ""

	for I in "${TEMP[@]}"; do
	    if [ "$(basename $I)" == "engine.hpp" ]; then
		continue
            fi
	    echo "#include \"$(basename $I)\""
	done

	echo ""
	echo "#endif // $i"
	echo ""
    done

    echo "#endif // ENGINE_HPP"
    echo ""

}
