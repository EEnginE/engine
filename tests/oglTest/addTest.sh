#!/usr/bin/env bash

_ARGC_=$#

FILE_HPP=""
FILE_CPP=""

TEST_NAME=""
DESC=""

printHelp() {
    echo -e "  -- USAGE: <test-name> '<description>'\n\n"
    exit 0
}


genHPP() {
    echo "  -- Generating $FILE_HPP"
cat << EOF > $FILE_HPP
/*!
 * \file $(basename $FILE_HPP)
 * \brief \b Classes: \a $TEST_NAME
 *
 * Class for testing:
 * $DESC
 *
 */

#ifndef $(I=$(basename $FILE_HPP); echo ${I^^} | sed 's/\./_/g')
#define $(I=$(basename $FILE_HPP); echo ${I^^} | sed 's/\./_/g')

#include <engine.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace e_engine;

class $TEST_NAME {
   private:
      // Private stuff goes here

   public:
      ${TEST_NAME}() {}

      const static string desc;

      void runTest( uJSON_data &_data, string _dataRoot );
};

#endif // $(I=$(basename $FILE_HPP); echo ${I^^} | sed 's/\./_/g')

// kate: indent-mode cstyle; indent-width 3; replace-tabs on;

EOF
}

genCPP() {
    echo "  -- Generating $FILE_CPP"
cat << EOF > $FILE_CPP
/*!
 * \file $(basename $FILE_CPP)
 * \brief \b Classes: \a $TEST_NAME
 *
 * Class for testing:
 * $DESC
 *
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "$(basename $FILE_HPP)"

const string ${TEST_NAME}::desc = "$DESC";

void ${TEST_NAME}::runTest( uJSON_data &_data, string _dataRoot ) {
   /*
    * Add your code here
    *
    * use '_data( ... )' to save your results
    *
    */
}

/*
 * Begin recommended Bindings
 *
 * Syntax: '//#!BIND DATA(' <location in json file> , G_<TYPE>( <GlobConf value>, <default> );
 */
//#!BIND DATA( "test", "bind", "stuff", G_STR( GlobConf.dummy, "default" ) );

// kate: indent-mode cstyle; indent-width 3; replace-tabs on;

EOF
}


cat << EOF


        OpenGL tests adder
        ==================


EOF


if (( _ARGC_ != 2 )); then
    printHelp
fi


START_DIR=$(pwd)
cd $(dirname $0) # Make sure we are in the oglTest dir


TEST_NAME=$(echo $1 | sed 's/ /_/g')
DESC="$2"

FILE_HPP="$(pwd)/tests/${TEST_NAME}.hpp"
FILE_CPP="$(pwd)/tests/${TEST_NAME}.cpp"

FILES_EXISTS=0


cat << EOF
  -- Test Name:   $TEST_NAME
  -- Description: $DESC
  -- HPP File:    $FILE_HPP
  -- CPP File:    $FILE_CPP
EOF



if [ -e $FILE_HPP -a -f $FILE_HPP ]; then
    echo -en "  -- $FILE_HPP already exists! Overwrite [y/N]: "
    read OVERWRITE
    if [[ $OVERWRITE == "y" || $OVERWRITE == "Y" ]]; then
        genHPP
    fi
else
    genHPP
fi


if [ -e $FILE_CPP -a -f $FILE_CPP ]; then
    echo -en "  -- $FILE_CPP already exists! Overwrite [y/N]: "
    read OVERWRITE
    if [[ $OVERWRITE == "y" || $OVERWRITE == "Y" ]]; then
        genCPP
    fi
else
    genCPP
fi


cat << EOF
  -- DONE


EOF


cd $START_DIR

# kate: indent-mode shell; indent-width 3; replace-tabs on;
