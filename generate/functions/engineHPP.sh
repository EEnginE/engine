#!/bin/bash

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

#define _USE_MATH_DEFINES

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

