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

    msg1 "Generating file $1..."

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
    echo "#include \"defines.hpp\""                                                    >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo "#ifndef $IFNDEF_HEADDER"                                                     >> $MACRO_PATH
    echo "#define $IFNDEF_HEADDER"                                                     >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH
    echo ""                                                                            >> $MACRO_PATH

    #### Body ####

    echo "" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        #__macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH    "_$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',W_FILE,__LINE__,W_FUNC" $DO_UNDEF
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH " _$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',false,W_FILE,__LINE__,W_FUNC,std::this_thread::get_id()" $DO_UNDEF
    done

    echo "" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "_h$( echo -n ${I^^} )" "'$( echo -n ${I^^} )',true ,W_FILE,__LINE__,W_FUNC,std::this_thread::get_id()" $DO_UNDEF
    done

    echo -e "\n" >> $MACRO_DEFINE_PATH

    for I in $TYPES; do
        #__macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH  "$( echo -n ${I,,} )LOG" "LOG('$( echo -n ${I^^} )',W_FILE,__LINE__,W_FUNC," $DO_UNDEF
        __macro_func $MACRO_UNDEF_PATH $MACRO_DEFINE_PATH "$( echo -n ${I,,} )LOG(...)" "LOG.addLogEntry(_$( echo -n ${I^^} ),__VA_ARGS__)" $DO_UNDEF
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
