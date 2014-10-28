#!/bin/bash

########################################################################################
############################################################################################################################
#####          ######################################################################################################################
###  Log macros  #######################################################################################################################
#####          ######################################################################################################################
############################################################################################################################
########################################################################################

__macro_func() {
    local ARGC=$#
    if (( ARGC != 5 )); then
        return
    fi

    WITHOUT_BRS=$(echo $3 | sed 's/([a-zA-Z\.]*)$//g')

    if (( $5 != 0 )); then
        echo "#if defined $WITHOUT_BRS"                                                                         >> $1
        echo "#warning \"UNDEF $WITHOUT_BRS -- Please remove '#define $WITHOUT_BRS' from your project files\""  >> $1
        echo "#undef $WITHOUT_BRS"                                                                              >> $1
        echo "#endif // defined $WITHOUT_BRS"                                                                   >> $1
        echo ""                                                                                                 >> $1
    fi

    echo "#define $3 $4"                                                                                        >> $2
}


generateLogMacros() {
    local ARGC=$#
    local I
    
    if (( ARGC != 3 )); then
        echo "ERROR generateLogMacros needs 3 Arguments"
        return 4
    fi
    
    echo "INFO: Generating file $1..."
    
    local MACRO_PATH=$1
    local TYPES=$2
    local DO_UNDEF=$3

    local MACRO_UNDEF_PATH="$(  dirname $MACRO_PATH )/macros_undef_temp"
    local MACRO_DEFINE_PATH="$( dirname $MACRO_PATH )/macros_define_temp"

    for I in $MACRO_DEFINE_PATH $MACRO_UNDEF_PATH $MACRO_PATH; do
        if [ -e $I ]; then
            rm $I
        fi
    done

    #### Headder ####

    local IFNDEF_HEADDER="$( basename $MACRO_PATH )"
    local IFNDEF_HEADDER="$( echo -n $IFNDEF_HEADDER | sed 's/\./_/g' )"
    local IFNDEF_HEADDER="$( echo -n ${IFNDEF_HEADDER^^} )"

    echo "/*!"                                                                         >> $MACRO_PATH
    echo " * \\file $(basename $MACRO_PATH)"                                           >> $MACRO_PATH
    echo " * \\brief Some important macros for logging are defined here"               >> $MACRO_PATH
    echo " *"                                                                          >> $MACRO_PATH
    echo " * \\warning This is an automatically generated file of '$0'! DO NOT EDIT"   >> $MACRO_PATH
    echo " */"                                                                         >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo "#ifndef $IFNDEF_HEADDER"                                                     >> $MACRO_PATH
    echo "#define $IFNDEF_HEADDER"                                                     >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH

    #### Body ####

    echo "" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        #__macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH    "_$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',__FILE__,__LINE__,LOG_FUNCTION_NAME" $DO_UNDEF
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH " _$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',false,__FILE__,__LINE__,LOG_FUNCTION_NAME" $DO_UNDEF
    done

    echo "" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_h$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',true ,__FILE__,__LINE__,LOG_FUNCTION_NAME" $DO_UNDEF
    done

    echo -e "\n" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        #__macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH  "$( echo -n ${I,,} )LOG" "LOG('$( echo -n ${I^^} )',__FILE__,__LINE__,LOG_FUNCTION_NAME," $DO_UNDEF
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "$( echo -n ${I,,} )LOG(...)" "LOG.addLogEntry('$( echo -n ${I^^} )',false,__FILE__,__LINE__,LOG_FUNCTION_NAME,__VA_ARGS__)" $DO_UNDEF 
    done

    #### Footer ####

    if (( DO_UNDEF != 0 )); then
        cat  $MACRO_UNDEF_PATH  >> $MACRO_PATH
        echo ""                 >> $MACRO_PATH
        echo ""                 >> $MACRO_PATH
        echo ""                 >> $MACRO_PATH
    fi
    cat  $MACRO_DEFINE_PATH     >> $MACRO_PATH
    echo ""                     >> $MACRO_PATH
    echo ""                     >> $MACRO_PATH

    echo "#endif //$IFNDEF_HEADDER" >> $MACRO_PATH

    for I in $MACRO_DEFINE_PATH $MACRO_UNDEF_PATH; do
        if [ -e $I ]; then
            rm $I
        fi
    done

}
