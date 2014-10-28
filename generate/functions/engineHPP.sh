#!/bin/bash

engineHPP() {
    local ARGC=$#
    local I
    if (( ARGC != 1 )); then
        echo "ERROR: engineHPP needs 1 argument"
        return
    fi
    
    echo "INFO: Generating file $1..."

    for I in $ALL_ALL_HPP; do
        echo $I | grep -E $REGEX_HPP_2 &> /dev/null
        if [ $? -ne 0 ]; then
            continue
        fi
    
        if [ "$(basename $I)" == "engine.hpp" ]; then
            continue
        fi
        INC_ALL_HPP="${INC_ALL_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
    done

    for I in $ALL_X11_HPP; do
        echo $I | grep -E $REGEX_HPP_2 &> /dev/null
        if [ $? -ne 0 ]; then
            continue
        fi
    
        if [ "$(basename $I)" == "engine.hpp" ]; then
            continue
        fi
        INC_X11_HPP="${INC_X11_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
    done

    for I in $ALL_WAYLAND_HPP; do
        echo $I | grep -E $REGEX_HPP_2 &> /dev/null
        if [ $? -ne 0 ]; then
        continue
        fi
    
        if [ "$(basename $I)" == "engine.hpp" ]; then
            continue
        fi
        INC_WAYLAND_HPP="${INC_WAYLAND_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
    done

    for I in $ALL_MIR_HPP; do
        echo $I | grep -E $REGEX_HPP_2 &> /dev/null
        if [ $? -ne 0 ]; then
            continue
        fi
    
        if [ "$(basename $I)" == "engine.hpp" ]; then
            continue
        fi
        INC_MIR_HPP="${INC_MIR_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
    done

    for I in $ALL_WINDOWS_HPP; do
        echo $I | grep -E $REGEX_HPP_2 &> /dev/null
        if [ $? -ne 0 ]; then
            continue
        fi
    
        if [ "$(basename $I)" == "engine.hpp" ]; then
            continue
        fi
        INC_WINDOWS_HPP="${INC_WINDOWS_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
    done



cat > $1 << EOF
/*!
 * \file engine.hpp
 * \brief Includes all headder files from E Engine
 */
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 * 
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 * 
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 * 
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ENGINE_HPP
#define ENGINE_HPP

$INC_ALL_HPP

#if UNIX_X11
$INC_X11_HPP

#endif // UNIX_X11

#if UNIX_WAYLAND
$INC_WAYLAND_HPP

#endif // UNIX_WAYLAND

#if UNIX_MIR
$INC_MIR_HPP

#endif // UNIX_MIR

#if WINDOWS
$INC_WINDOWS_HPP

#endif // WINDOWS

#endif // ENGINE_HPP

EOF

}

