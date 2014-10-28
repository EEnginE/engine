#!/bin/bash

REGEX_CPP_1=".*/[a-zA-Z0-9_\-]+\.cpp"
REGEX_HPP_1=".*/[a-zA-Z0-9_\-]+\.hpp"

REGEX_CPP_2=".*/[a-zA-Z0-9]+[a-zA-Z0-9_\-]*\.cpp"
REGEX_HPP_2=".*/[a-zA-Z0-9]+[a-zA-Z0-9_\-]*\.hpp"

dirs_func() {
    local ARGC=$#
    if (( ARGC == 0 )); then
        return
    fi
    
    local A B

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

finSources() {
    local ARGC=$#
    
    local X11_CPP     X11_HPP
    local WAYLAND_CPP WAYLAND_HPP
    local MIR_CPP     MIR_HPP
    local WINDOWS_CPP WINDOWS_HPP
    local ALL_CPP     ALL_HPP
    
    local TEMP TEMP_CPP TEMP_HPP SERACH_IN I
   
    if (( ARGC != 7 )); then
        echo "ERROR: findSources needs 7 arguments!"
    fi
    
    local DIR_TO_GO=$1
    local FILENAME=$2
    
    echo "INFO:    -- Generating source file $FILENAME for target $DIR_TO_GO..."
        
    if [ ! -d $DIR_TO_GO ]; then
        echo "ERROR: $DIR_TO_GO does not exist!"
        return
    fi
   
    local X11_DIRECTORY=${DIR_TO_GO}/$3
    local WAYLAND_DIRECTORY=${DIR_TO_GO}/$4
    local MIR_DIRECTORY=${DIR_TO_GO}/$5
    local WINDOWS_DIRECTORY=${DIR_TO_GO}/$6
    
    local BASENAME_VARS=$7
    
    local X11_DIRS="$X11_DIRECTORY"
    local WAYLAND_DIRS="$WAYLAND_DIRECTORY"
    local MIR_DIRS="$MIR_DIRECTORY"
    local WINDOWS_DIRS="$WINDOWS_DIRECTORY"
    local ALL_DIRS=""

########################################################################################
############################################################################################################################
#####                      ##########################################################################################################
###  Find every single file  ###########################################################################################################
#####                      ##########################################################################################################
############################################################################################################################
########################################################################################

    echo "INFO:      -- Searching everything..."

    TEMP=""
    if (( $( ls -l $DIR_TO_GO | grep ^d | wc -l ) )); then
        TEMP=$(ls -d $DIR_TO_GO/*/)
    fi
    
    for B in $TEMP; do
        SERACH_IN="$SERACH_IN $B"
    done

    SERACH_IN="$DIR_TO_GO/ $SERACH_IN"
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
    echo "INFO:      -- Assigning files to OS..."

    for I in $TEMP_CPP; do
        ## Test X11 ##
        echo $I | grep $X11_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            X11_CPP="$X11_CPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
        
        ## Test Wayland ##
        echo $I | grep $WAYLAND_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            WAYLAND_CPP="$WAYLAND_CPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
        
        ## Test Mir ##
        echo $I | grep $MIR_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            MIR_CPP="$MIR_CPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
    
        ## Test Windows ##
        echo $I | grep $WINDOWS_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            WINDOWS_CPP="$WINDOWS_CPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
    
        ALL_CPP="$ALL_CPP \${$VAR_FOR_CMAKE_ROOT}/$I"
    done



    #########
    ## HPP ##
    #########

    for I in $TEMP_HPP; do
        ## Test X11 ##
        echo $I | grep $X11_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            X11_HPP="$X11_HPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
        
        ## Test Wayland ##
        echo $I | grep $WAYLAND_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            WAYLAND_HPP="$WAYLAND_HPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
        
        ## Test Mir ##
        echo $I | grep $MIR_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            MIR_HPP="$MIR_HPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
    
        ## Test Windows ##
        echo $I | grep $WINDOWS_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            WINDOWS_HPP="$WINDOWS_HPP \${$VAR_FOR_CMAKE_ROOT}/$I"
            continue
        fi
    
        ALL_HPP="$ALL_HPP \${$VAR_FOR_CMAKE_ROOT}/$I"
    done


########################################################################################
############################################################################################################################
#####                          ######################################################################################################
###  Sort and remove Duplicates  #######################################################################################################
#####                          ######################################################################################################
############################################################################################################################
########################################################################################


    ALL_CPP=$(             echo "$ALL_CPP"                              | sed "s/ /\n/g" | sort | uniq )
    ALL_HPP=$(             echo "$ALL_HPP"                              | sed "s/ /\n/g" | sort | uniq )
    ALL_ALL_HPP=$(         echo "$ALL_ALL_HPP $ALL_HPP"                 | sed "s/ /\n/g" | sort | uniq )

    X11_CPP=$(             echo "$X11_CPP"                              | sed "s/ /\n/g" | sort | uniq )
    X11_HPP=$(             echo "$X11_HPP"                              | sed "s/ /\n/g" | sort | uniq )
    ALL_X11_HPP=$(         echo "$ALL_X11_HPP $X11_HPP"                 | sed "s/ /\n/g" | sort | uniq )
    
    WAYLAND_CPP=$(         echo "$WAYLAND_CPP"                          | sed "s/ /\n/g" | sort | uniq )
    WAYLAND_HPP=$(         echo "$WAYLAND_HPP"                          | sed "s/ /\n/g" | sort | uniq )
    ALL_WAYLAND_HPP_HPP=$( echo "$ALL_WAYLAND_HPP_HPP $WAYLAND_HPP_HPP" | sed "s/ /\n/g" | sort | uniq )
    
    MIR_CPP=$(             echo "$MIR_CPP"                              | sed "s/ /\n/g" | sort | uniq )
    MIR_HPP=$(             echo "$MIR_HPP"                              | sed "s/ /\n/g" | sort | uniq )
    ALL_MIR_HPP=$(         echo "$ALL_MIR_HPP $MIR_HPP"                 | sed "s/ /\n/g" | sort | uniq )

    WINDOWS_CPP=$(         echo "$WINDOWS_CPP"                          | sed "s/ /\n/g" | sort | uniq )
    WINDOWS_HPP=$(         echo "$WINDOWS_HPP"                          | sed "s/ /\n/g" | sort | uniq )
    ALL_WINDOWS_HPP=$(     echo "$ALL_WINDOWS_HPP $WINDOWS_HPP"         | sed "s/ /\n/g" | sort | uniq )



########################################################################################
############################################################################################################################
#####                               #################################################################################################
###  Assign Directories to Platforms  ##################################################################################################
#####                               #################################################################################################
############################################################################################################################
########################################################################################

    echo "INFO:      -- Assigning directories to OS..."

    for I in $SERACH_IN; do
        ## Test X11 ##
        echo $I | grep $X11_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            X11_DIRS="${X11_DIRS} $I"
            continue
        fi
        
        ## Test Wayland ##
        echo $I | grep $WAYLAND_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            WAYLAND_DIRS="${WAYLAND_DIRS} $I"
            continue
        fi
        
        ## Test Mir ##
        echo $I | grep $MIR_DIRECTORY &> /dev/null
        if [ $? -eq 0 ]; then
            MIR_DIRS="${MIR_DIRS} $I"
            continue
        fi
    
        ## Test Windows ##
        echo $I | grep $WINDOWS_DIRECTORY &> /dev/null
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
    
    WAYLAND_DIRS="$WAYLAND_DIRS " # Add a ' '
    WAYLAND_DIRS=$( echo "$WAYLAND_DIRS" | sed "s/\/ / /g" )
    WAYLAND_DIRS=$( echo "$WAYLAND_DIRS" | sed "s/ /\n/g" )
    WAYLAND_DIRS=$( echo "$WAYLAND_DIRS" | sort | uniq)
    
    MIR_DIRS="$MIR_DIRS " # Add a ' '
    MIR_DIRS=$( echo "$MIR_DIRS" | sed "s/\/ / /g" )
    MIR_DIRS=$( echo "$MIR_DIRS" | sed "s/ /\n/g" )
    MIR_DIRS=$( echo "$MIR_DIRS" | sort | uniq)

    WINDOWS_DIRS="$WINDOWS_DIRS " # Add a ' '
    WINDOWS_DIRS=$( echo "$WINDOWS_DIRS" | sed "s/\/ / /g" )
    WINDOWS_DIRS=$( echo "$WINDOWS_DIRS" | sed "s/ /\n/g" )
    WINDOWS_DIRS=$( echo "$WINDOWS_DIRS" | sort | uniq)

    ALL_DIRS="$ALL_DIRS " # Add a ' '
    ALL_DIRS=$( echo "$ALL_DIRS" | sed "s/\/ / /g" )
    ALL_DIRS=$( echo "$ALL_DIRS" | sed "s/ /\n/g" )
    ALL_DIRS=$( echo "$ALL_DIRS" | sort | uniq )



# = Print =====

    echo "INFO:      -- Printing $FILENAME..."

cat > $FILENAME << EOF
# $FILENAME
#
# Automatically generated by '$0'. Please do not edit
# this file manually.
# 

# X11 files
if( DISPLAY_SERVER MATCHES UNIX_X11 )

set(${BASENAME_VARS}_SOURCES_PLATFORM $X11_CPP
)

set(${BASENAME_VARS}_HEADER_PLATFORM $X11_HPP
)

include_directories(
$X11_DIRS
)

endif( DISPLAY_SERVER MATCHES UNIX_X11 )


# Wayland files
if( DISPLAY_SERVER MATCHES UNIX_WAYLAND )

set(${BASENAME_VARS}_SOURCES_PLATFORM $WAYLAND_CPP
)

set(${BASENAME_VARS}_HEADER_PLATFORM $WAYLAND_HPP
)

include_directories(
$WAYLAND_DIRS
)

endif( DISPLAY_SERVER MATCHES UNIX_WAYLAND )


# Mir files
if( DISPLAY_SERVER MATCHES UNIX_MIR )

set(${BASENAME_VARS}_SOURCES_PLATFORM $MIR_CPP
)

set(${BASENAME_VARS}_HEADER_PLATFORM $MIR_HPP
)

include_directories(
$MIR_DIRS
)

endif( DISPLAY_SERVER MATCHES UNIX_MIR )


# Windows files
if( DISPLAY_SERVER MATCHES WINDOWS )

set(${BASENAME_VARS}_SOURCES_PLATFORM $WINDOWS_CPP
)

set(${BASENAME_VARS}_HEADER_PLATFORM $WINDOWS_HPP
)

include_directories( 
$WINDOWS_DIRS
)

endif( DISPLAY_SERVER MATCHES WINDOWS )

set(${BASENAME_VARS}_SRC
\${${BASENAME_VARS}_SOURCES_PLATFORM}
$ALL_CPP
)

set(${BASENAME_VARS}_INC
\${${BASENAME_VARS}_HEADER_PLATFORM}
$ALL_HPP
)

include_directories(
$ALL_DIRS
)


# Needed for CMake macro
set( CURRENT_BASENAME ${BASENAME_VARS} )

EOF

}
