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

REGEX_CPP_1=".*/[a-zA-Z0-9_\-]+\.cpp"
REGEX_HPP_1=".*/[a-zA-Z0-9_\-]+\.hpp"

finSources() {
    local ARGC=$#

    local ALL_CPP=() ALL_HPP=()
    local i
    for i in "${DISPLAY_SERVER[@]}"; do
        eval "local ${i^^}_CPP=()"
        eval "local ${i^^}_HPP=()"
        eval "local ${i^^}_DIRECTORY=${1}/${i}"
    done


    local TEMP TEMP_CPP TEMP_HPP SERACH_IN I

    if (( ARGC != 2 )); then
        error "findSources needs 2 arguments!" >&2
        exit
    fi

    if [ ! -d "$1" ]; then
        error "$1 does not exist!" >&2
        exit
    fi

    local BASENAME_VARS=$2

#    ______ _           _         _ _    __ _ _
#    |  ___(_)         | |       | | |  / _(_) |
#    | |_   _ _ __   __| |   __ _| | | | |_ _| | ___  ___
#    |  _| | | '_ \ / _` |  / _` | | | |  _| | |/ _ \/ __|
#    | |   | | | | | (_| | | (_| | | | | | | | |  __/\__ \
#    \_|   |_|_| |_|\__,_|  \__,_|_|_| |_| |_|_|\___||___/
#
#

    TEMP=($(find "$1" -regex "$REGEX_HPP_1" -printf '%h\n' | sort -u | sed 's/^\.\///g'))

    cd "$1"

    TEMP_CPP=($(find . -regex "$REGEX_CPP_1" -type f -print | sort | sed 's/^\.\///g'))
    TEMP_HPP=($(find . -regex "$REGEX_HPP_1" -type f -print | sort | sed 's/^\.\///g'))

    cd - &> /dev/null

#      ___          _                                                            _
#     / _ \        (_)                                                          | |
#    / /_\ \___ ___ _  __ _ _ __    ___  ___  _   _ _ __ ___ ___    ___ ___   __| | ___
#    |  _  / __/ __| |/ _` | '_ \  / __|/ _ \| | | | '__/ __/ _ \  / __/ _ \ / _` |/ _ \
#    | | | \__ \__ \ | (_| | | | | \__ \ (_) | |_| | | | (_|  __/ | (_| (_) | (_| |  __/
#    \_| |_/___/___/_|\__, |_| |_| |___/\___/ \__,_|_|  \___\___|  \___\___/ \__,_|\___|
#                      __/ |
#                     |___/

    #########
    ## CPP ##
    #########

    for I in "${TEMP_CPP[@]}"; do
        # processBar $P_COUNTER $P_MAX $I >&2
        (( P_COUNTER++ ))

        ALL_SOURCE_FILES+=( $1/$I )
        for i in "${DISPLAY_SERVER[@]}"; do
            if [[ "$I" == *"${i}/"* ]]; then
                eval "${i^^}_CPP+=( '$I' )"
                i=0
                break
            fi
        done

        if [[ "$i" != "0" ]]; then
            ALL_CPP+=( "$I" )
        fi
    done



    #########
    ## HPP ##
    #########

    for I in "${TEMP_HPP[@]}"; do
        [[ "$I" == *"$EXPORT_FILE_EXT" ]] && continue 
        ALL_SOURCE_FILES+=( $1/$I )
        for i in "${DISPLAY_SERVER[@]}"; do
            if [[ "$I" == *"${i}/"* ]]; then
                eval "${i^^}_HPP+=( '$I' )"
                i=0
                break
            fi
        done

        if [[ "$i" != "0" ]]; then
            ALL_HPP+=( "$I" )
        fi
    done


#     _____                   _   _ ____________ _
#    /  ___|                 | | | || ___ \ ___ ( )
#    \ `--.  __ ___   _____  | |_| || |_/ / |_/ // ___
#     `--. \/ _` \ \ / / _ \ |  _  ||  __/|  __/  / __|
#    /\__/ / (_| |\ V /  __/ | | | || |   | |     \__ \
#    \____/ \__,_| \_/ \___| \_| |_/\_|   \_|     |___/
#
#


    ALL_ALL_HPP+=( "${ALL_HPP[@]}" )

    for i in "${DISPLAY_SERVER[@]}"; do
        eval "ALL_${i^^}_HPP+=( \"\${${i^^}_HPP[@]}\" )"
    done


#      ___          _              ______ _               _             _
#     / _ \        (_)             |  _  (_)             | |           (_)
#    / /_\ \___ ___ _  __ _ _ __   | | | |_ _ __ ___  ___| |_ ___  _ __ _  ___  ___
#    |  _  / __/ __| |/ _` | '_ \  | | | | | '__/ _ \/ __| __/ _ \| '__| |/ _ \/ __|
#    | | | \__ \__ \ | (_| | | | | | |/ /| | | |  __/ (__| || (_) | |  | |  __/\__ \
#    \_| |_/___/___/_|\__, |_| |_| |___/ |_|_|  \___|\___|\__\___/|_|  |_|\___||___/
#                      __/ |
#                     |___/


    for I in "${TEMP[@]}"; do
        for i in "${DISPLAY_SERVER[@]}"; do
            if [[ "$I" == *"${i}"* ]]; then
                eval "${i^^}_DIRS+=( '$I' )"
                i=0
                break
            fi
        done

        if [[ "$i" != "0" ]]; then
            ALL_DIRS+=( "$I" )
        fi
    done


    # = Print =====

    echo ""
    echo "###########################################"
    echo "# BEGIN AUTOMATICALLY GENERATED SOURCES ###"
    echo "###########################################"
    echo ""

    local TEMP_SIZE=()
    local TEMP_A=()

    for i in "${DISPLAY_SERVER[@]}"; do
        eval "TEMP_SIZE[0]=\${#${i^^}_CPP[@]}"
        eval "TEMP_SIZE[1]=\${#${i^^}_HPP[@]}"

        if (( "${TEMP_SIZE[0]}" != 0 || "${TEMP_SIZE[1]}" != 0 )); then
            echo "# ${i} files:"
            echo "if( DISPLAY_SERVER MATCHES ${i^^} )"
            echo ""
            echo "set( ${BASENAME_VARS}_CPP_P"
            eval "TEMP_A=( \${${i^^}_CPP[@]} )"
            for I in "${TEMP_A[@]}"; do
                echo "  $I"
            done
            echo ")"

            echo ""
            echo "set( ${BASENAME_VARS}_HPP_P"
            eval "TEMP_A=( \${${i^^}_HPP[@]} )"
            for I in "${TEMP_A[@]}"; do
                echo "  $I"
            done
            echo ")"
            echo ""
            echo "endif( DISPLAY_SERVER MATCHES ${i^^} )"
            echo ""
            echo ""
        fi

    done

    echo ""
    echo "set( ${BASENAME_VARS}_SRC"
    echo "  \${${BASENAME_VARS}_CPP_P}"
    echo ""
    for I in "${ALL_CPP[@]}"; do
        echo "  $I"
    done
    echo ")"

    echo ""
    echo "set( ${BASENAME_VARS}_INC"
    echo "  \${${BASENAME_VARS}_HPP_P}"
    echo ""
    for I in "${ALL_HPP[@]}"; do
        echo "  $I"
    done
    echo ")"
    echo ""
    echo ""
    echo "#########################################"
    echo "# END AUTOMATICALLY GENERATED SOURCES ###"
    echo "#########################################"
    echo ""
}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
