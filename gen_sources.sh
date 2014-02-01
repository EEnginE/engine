#!/bin/bash

ARGC=$#
ARGV=$*

###########################
#### BEGIN Config Part ####
###########################

SOURCE_FILE="sources.cmake"
INCLUDE_FILE="src/engine.in.hpp"

## The platform dirs ##
X11_DIR="src/x11"
WINDOWS_DIR="src/windows"


## Log macro config ##
LOG_MACRO_PATH="src/log/macros.hpp"
LOG_TYPES="a b c d e f g h i j k l m n o p q r s t u v w x y z"
LOG_GEN_UNDEF=1

## RSA config ##
RSA_LOWER_LENGTH=256
RSA_UPPER_LENGTH=65536

###########################
####  END Config Part  ####
###########################

REGEX_CPP_1=".*/[a-zA-Z0-9_\-]+\.cpp"
REGEX_HPP_1=".*/[a-zA-Z0-9_\-]+\.hpp"

REGEX_CPP_2=".*/[a-zA-Z0-9]+[a-zA-Z0-9_\-]*\.cpp"
REGEX_HPP_2=".*/[a-zA-Z0-9]+[a-zA-Z0-9_\-]*\.hpp"

X11_CPP=""
X11_HPP=""

WINDOWS_CPP=""
WINDOWS_HPP=""

ALL_CPP=""
ALL_HPP=""

X11_DIRS="$X11_DIR"
WINDOWS_DIRS="$WINDOWS_DIR"
ALL_DIRS=""

TEMP=$(ls -d src/*/)
SERACH_IN=""


########################################################################################
############################################################################################################################
#####         #######################################################################################################################
###  Functions  ########################################################################################################################
#####         #######################################################################################################################
############################################################################################################################
########################################################################################

dirs_func() {
    ARGC=$#
    if (( ARGC == 0 )); then
        return
    fi
    
    local A

    for A in $*; do
        local DIRS="$(ls ${A})"
        for B in $DIRS; do
            local DIR=$(echo "${A}/${B}" | sed 's/\/\//\//g')
            if [ -d $DIR ]; then
                dirs_func "$DIR"
            fi
        done
        SERACH_IN="$SERACH_IN $A"
    done
}

macro_func() {
    ARGC=$#
    if (( ARGC != 4 )); then
        return
    fi
    
    if (( LOG_GEN_UNDEF != 0 )); then
        echo "#if defined $3"                                                               >> $1
        echo "#warning \"UNDEF $3 -- Please remove '#define $3' from your project files\""  >> $1
        echo "#undef $3"                                                                    >> $1
        echo "#endif // defined $3"                                                         >> $1
        echo ""                                                                             >> $1
    fi
    
    echo "#define $3 $4"                                                                    >> $2
}

########################################################################################
############################################################################################################################
#####                      ##########################################################################################################
###  Find every single file  ###########################################################################################################
#####                      ##########################################################################################################
############################################################################################################################
########################################################################################

for B in $TEMP; do
    SERACH_IN="$SERACH_IN $B"
done

SERACH_IN="src/ $SERACH_IN"
dirs_func $SERACH_IN

for C in $SERACH_IN; do
    TEMP_CPP="$TEMP_CPP $(find $C -maxdepth 1 -regex $REGEX_CPP_1 -print | sort)"
    TEMP_HPP="$TEMP_HPP $(find $C -maxdepth 1 -regex $REGEX_HPP_1 -print | sort)"
done


########################################################################################
############################################################################################################################
#####                                     ###########################################################################################
###  Assign to a platform or corssplatform  ############################################################################################
#####                                     ###########################################################################################
############################################################################################################################
########################################################################################

#########
## CPP ##
#########

for I in $TEMP_CPP; do
    ## Test X11 ##
    echo $I | grep $X11_DIR &> /dev/null
    if [ $? -eq 0 ]; then
        X11_CPP="$X11_CPP $I"
        continue
    fi
    
    ## Test Windows ##
    echo $I | grep $WINDOWS_DIR &> /dev/null
    if [ $? -eq 0 ]; then
        WINDOWS_CPP="$WINDOWS_CPP $I"
        continue
    fi
    
    ALL_CPP="$ALL_CPP $I"
done



#########
## HPP ##
#########

for I in $TEMP_HPP; do
    ## Test X11 ##
    echo $I | grep $X11_DIR &> /dev/null
    if [ $? -eq 0 ]; then
        X11_HPP="$X11_HPP $I"
        continue
    fi
    
    ## Test Windows ##
    echo $I | grep $WINDOWS_DIR &> /dev/null
    if [ $? -eq 0 ]; then
        WINDOWS_HPP="$WINDOWS_HPP $I"
        continue
    fi
    
    ALL_HPP="$ALL_HPP $I"
done


########################################################################################
############################################################################################################################
#####                          ######################################################################################################
###  Sort and remove Duplicates  #######################################################################################################
#####                          ######################################################################################################
############################################################################################################################
########################################################################################


ALL_CPP=$(     echo "$ALL_CPP"     | sed "s/ /\n/g" | sort | uniq )
ALL_HPP=$(     echo "$ALL_HPP"     | sed "s/ /\n/g" | sort | uniq )

X11_CPP=$(     echo "$X11_CPP"     | sed "s/ /\n/g" | sort | uniq )
X11_HPP=$(     echo "$X11_HPP"     | sed "s/ /\n/g" | sort | uniq )

WINDOWS_CPP=$( echo "$WINDOWS_CPP" | sed "s/ /\n/g" | sort | uniq )
WINDOWS_HPP=$( echo "$WINDOWS_HPP" | sed "s/ /\n/g" | sort | uniq )



########################################################################################
############################################################################################################################
#####          ######################################################################################################################
###  Log macros  #######################################################################################################################
#####          ######################################################################################################################
############################################################################################################################
########################################################################################

MACRO_UNDEF_PATH="$(  dirname $LOG_MACRO_PATH )/macros_undef_temp"
MACRO_DEFINE_PATH="$( dirname $LOG_MACRO_PATH )/macros_define_temp"

for I in $MACRO_DEFINE_PATH $MACRO_UNDEF_PATH $LOG_MACRO_PATH; do
    if [ -e $I ]; then
        rm $I
    fi
done
unset I

#### Headder ####

IFNDEF_HEADDER="$( basename $LOG_MACRO_PATH )"
IFNDEF_HEADDER="$( echo -n $IFNDEF_HEADDER | sed 's/\./_/g' )"
IFNDEF_HEADDER="$( echo -n ${IFNDEF_HEADDER^^} )"

echo "/*!"                                                                         >> $LOG_MACRO_PATH
echo " * \\file $(basename $LOG_MACRO_PATH)"                                       >> $LOG_MACRO_PATH
echo " * \\brief Here are some important macros for logging defined"               >> $LOG_MACRO_PATH
echo " *"                                                                          >> $LOG_MACRO_PATH
echo " * \\warning This is an automatically generated file from '$0'! DO NOT EDIT" >> $LOG_MACRO_PATH
echo " */"                                                                         >> $LOG_MACRO_PATH
echo ""                                                                            >> $LOG_MACRO_PATH
echo ""                                                                            >> $LOG_MACRO_PATH
echo "#ifndef $IFNDEF_HEADDER"                                                     >> $LOG_MACRO_PATH
echo "#define $IFNDEF_HEADDER"                                                     >> $LOG_MACRO_PATH
echo ""                                                                            >> $LOG_MACRO_PATH
echo ""                                                                            >> $LOG_MACRO_PATH

#### Body ####

macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "ADD"     ")->add("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "NEWLINE" ")->nl("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "POINT"   ")->point("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "S_COLOR" ")->setColor("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_END_"   ");"
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "END"     ")->end();"

echo "" >> $MACRO_DEFINE_PATH

macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_ADD"     "->add("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_NEWLINE" "->nl("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_POINT"   "->point("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_S_COLOR" "->setColor("
macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_END"     "->end();"

echo "" >> $MACRO_DEFINE_PATH

for I in $LOG_TYPES; do
    macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',__FILE__,__LINE__"
done
unset I

echo "" >> $MACRO_DEFINE_PATH

for I in $LOG_TYPES; do
    macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "$( echo -n ${I,,} )LOG" "LOG('$( echo -n ${I^^} )',__FILE__,__LINE__,"
done
unset I

#### Footer ####

if (( LOG_GEN_UNDEF != 0 )); then
    cat  $MACRO_UNDEF_PATH  >> $LOG_MACRO_PATH
    echo ""                 >> $LOG_MACRO_PATH
    echo ""                 >> $LOG_MACRO_PATH
    echo ""                 >> $LOG_MACRO_PATH
fi
cat  $MACRO_DEFINE_PATH     >> $LOG_MACRO_PATH
echo ""                     >> $LOG_MACRO_PATH
echo ""                     >> $LOG_MACRO_PATH

echo "#endif //$IFNDEF_HEADDER" >> $LOG_MACRO_PATH

for I in $MACRO_DEFINE_PATH $MACRO_UNDEF_PATH; do
    if [ -e $I ]; then
        rm $I
    fi
done
unset I


########################################################################################
############################################################################################################################
#####                               #################################################################################################
###  Assign Directories to Platforms  ##################################################################################################
#####                               #################################################################################################
############################################################################################################################
########################################################################################

for I in $SERACH_IN; do
    ## Test X11 ##
    echo $I | grep $X11_DIR &> /dev/null
    if [ $? -eq 0 ]; then
        X11_DIRS="${X11_DIRS} $I"
        continue
    fi
    
    ## Test Windows ##
    echo $I | grep $WINDOWS_DIR &> /dev/null
    if [ $? -eq 0 ]; then
        WINDOWS_DIRS="${WINDOWS_DIRS} $I"
        continue
    fi
    
    ALL_DIRS="${ALL_DIRS} $I"
done


X11_DIRS="$X11_DIRS " # Add a ' '
X11_DIRS=$( echo "$X11_DIRS" | sed "s/\/ / /g" )
X11_DIRS=$( echo "$X11_DIRS" | sed "s/ /\n/g" )
X11_DIRS=$( echo "$X11_DIRS" | sort | uniq)

WINDOWS_DIRS="$WINDOWS_DIRS " # Add a ' '
WINDOWS_DIRS=$( echo "$WINDOWS_DIRS" | sed "s/\/ / /g" )
WINDOWS_DIRS=$( echo "$WINDOWS_DIRS" | sed "s/ /\n/g" )
WINDOWS_DIRS=$( echo "$WINDOWS_DIRS" | sort | uniq)

ALL_DIRS="$ALL_DIRS " # Add a ' '
ALL_DIRS=$( echo "$ALL_DIRS" | sed "s/\/ / /g" )
ALL_DIRS=$( echo "$ALL_DIRS" | sed "s/ /\n/g" )
ALL_DIRS=$( echo "$ALL_DIRS" | sort | uniq )




for I in $ALL_HPP; do
    echo $I | grep -E $REGEX_HPP_2 &> /dev/null
    if [ $? -ne 0 ]; then
        continue
    fi
    
    if [ "$(basename $I)" == "engine.hpp" ]; then
        continue
    fi
    INC_ALL_HPP="${INC_ALL_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
done

for I in $X11_HPP; do
    echo $I | grep -E $REGEX_HPP_2 &> /dev/null
    if [ $? -ne 0 ]; then
        continue
    fi
    
    if [ "$(basename $I)" == "engine.hpp" ]; then
        continue
    fi
    INC_X11_HPP="${INC_X11_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
done

for I in $WINDOWS_HPP; do
    echo $I | grep -E $REGEX_HPP_2 &> /dev/null
    if [ $? -ne 0 ]; then
        continue
    fi
    
    if [ "$(basename $I)" == "engine.hpp" ]; then
        continue
    fi
    INC_WINDOWS_HPP="${INC_WINDOWS_HPP}$(echo -e "\n#include \"$(basename $I)\"")"
done




# = Print =====

cat > $SOURCE_FILE << EOF
# $SOURCE_FILE
#
# Automatically generated by '$0'. Please do not edit
# this file manually.
# 

if(UNIX)

set(SOURCES_PLATFORM
$X11_CPP
)

set(HEADER_PLATFORM
$X11_HPP
)

include_directories(
$X11_DIRS
)

endif(UNIX)

if(WIN32)

set(SOURCES_PLATFORM
$WINDOWS_CPP
)

set(HEADER_PLATFORM
$WINDOWS_HPP
)

include_directories(
$WINDOWS_DIRS
)

endif(WIN32)

set(ENGINE_SRC
\${SOURCES_PLATFORM}
$ALL_CPP
)

set(ENGINE_INC
\${HEADER_PLATFORM}
$ALL_HPP
)

include_directories(
$ALL_DIRS
)

EOF

cat > $INCLUDE_FILE << EOF
/// \file engine.hpp 
/// \brief Includes all headder files from E Engine
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

#if defined __linux__
$INC_X11_HPP

#endif // defined __linux__

#if defined WIN32
$INC_WINDOWS_HPP

#endif // defined WIN32

const unsigned short E_VERSION_MAJOR  =  @CM_VERSION_MAJOR@;
const unsigned short E_VERSION_MINOR  =  @CM_VERSION_MINOR@;
const char*          E_INSTALL_PREFIX = "@CM_INSTALL_PREFIX@";

#endif // ENGINE_HPP

EOF

CLOC_EXEC="$(which cloc)"

for I in $ARGV; do
    if [ "$I" = "rsa" ]; then
        ./etc/build.sh
        ./etc/rsa_max $RSA_LOWER_LENGTH $RSA_UPPER_LENGTH 255 src/crypto/rsa/rsa_save_bits.hpp e_engine unsigned_int getSaveBlockSize 3 15 7
    fi
done

if [ -n "$CLOC_EXEC" ]; then
    $CLOC_EXEC --not-match-d='[a-zA-Z_/]*\.[a-zA-Z\._]+' src include | tee temp_cloc.txt;
    CALC_ALL="$(cat temp_cloc.txt | grep SUM)"
    CALC_ALL="$(echo $CALC_ALL    | sed 's/SUM: [0-9]*//g')"
    CALC_ALL="$(echo $CALC_ALL    | sed 's/ /\+/g')"
    echo ""
    echo "TOTAL: $(echo $CALC_ALL | bc )"
    
    if [ -e temp_cloc.txt ]; then
        rm temp_cloc.txt
    fi
fi

