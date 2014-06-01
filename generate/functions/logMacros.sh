#!/bin/bash

########################################################################################
############################################################################################################################
#####          ######################################################################################################################
###  Log macros  #######################################################################################################################
#####          ######################################################################################################################
############################################################################################################################
########################################################################################

__macro_func() {
    ARGC=$#
    if (( ARGC != 5 )); then
        return
    fi
    
    if (( $5 != 0 )); then
        echo "#if defined $3"                                                               >> $1
        echo "#warning \"UNDEF $3 -- Please remove '#define $3' from your project files\""  >> $1
        echo "#undef $3"                                                                    >> $1
        echo "#endif // defined $3"                                                         >> $1
        echo ""                                                                             >> $1
    fi
    
    echo "#define $3 $4"                                                                    >> $2
}


generateLogMacros() {
    ARGC=$#
    
    if (( ARGC != 3 )); then
        echo "ERROR generateLogMacros needs 3 Arguments"
        return 4
    fi
    
    echo "INFO: Generating file $1..."
    
    local MACRO_PATH=$1
    local TYPES=$2
    local DO_UNDEF=$3

    MACRO_UNDEF_PATH="$(  dirname $MACRO_PATH )/macros_undef_temp"
    MACRO_DEFINE_PATH="$( dirname $MACRO_PATH )/macros_define_temp"

    for I in $MACRO_DEFINE_PATH $MACRO_UNDEF_PATH $MACRO_PATH; do
        if [ -e $I ]; then
            rm $I
        fi
    done
    unset I

    #### Headder ####

    IFNDEF_HEADDER="$( basename $MACRO_PATH )"
    IFNDEF_HEADDER="$( echo -n $IFNDEF_HEADDER | sed 's/\./_/g' )"
    IFNDEF_HEADDER="$( echo -n ${IFNDEF_HEADDER^^} )"

    echo "/*!"                                                                         >> $MACRO_PATH
    echo " * \\file $(basename $MACRO_PATH)"                                       >> $MACRO_PATH
    echo " * \\brief Here are some important macros for logging defined"               >> $MACRO_PATH
    echo " *"                                                                          >> $MACRO_PATH
    echo " * \\warning This is an automatically generated file from '$0'! DO NOT EDIT" >> $MACRO_PATH
    echo " */"                                                                         >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo "#ifndef $IFNDEF_HEADDER"                                                     >> $MACRO_PATH
    echo "#define $IFNDEF_HEADDER"                                                     >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH

    #### Body ####

    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "ADD"     ")->add("       $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "NEWLINE" ")->nl("        $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "POINT"   ")->point("     $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "S_COLOR" ")->setColor("  $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_END_"   ");"            $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "END"     ")->end();"     $DO_UNDEF

    echo "" >> $MACRO_DEFINE_PATH

    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_ADD"     "->add("       $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_NEWLINE" "->nl("        $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_POINT"   "->point("     $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_S_COLOR" "->setColor("  $DO_UNDEF
    __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_END"     "->end();"     $DO_UNDEF

    echo "" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',__FILE__,__LINE__,LOG_FUNCTION_NAME" $DO_UNDEF
    done
    unset I

    echo "" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "$( echo -n ${I,,} )LOG" "LOG('$( echo -n ${I^^} )',__FILE__,__LINE__,LOG_FUNCTION_NAME," $DO_UNDEF
    done
    unset I

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
    unset I

}
