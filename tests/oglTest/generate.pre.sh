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

TESTS_DIR="tests"

CLASS_NAME="testStarter"
HPP_FILE="${CLASS_NAME}.hpp"
CPP_FILE="${CLASS_NAME}.cpp"

COUNTER=0
TESTS=( $( ls $TESTS_DIR | grep ".hpp" ) )

NUM_OF_TESTS=${#TESTS[*]}

source ./genBindings.sh

msg2 "Generating $HPP_FILE and $CPP_FILE"

cat > $HPP_FILE << EOF
/*!
 * \file $HPP_FILE
 * \brief \b Classes: \a $CLASS_NAME
 *
 * \warning Automatically generated file DO NOT EDIT HERE
 */

#ifndef $(echo ${HPP_FILE^^} | sed 's/\./_/g')
#define $(echo ${HPP_FILE^^} | sed 's/\./_/g')

#include <engine.hpp>
#include <vector>
#include <string>


/*!
 * \brief class for processing tests located in $TESTS_DIR
 *
 * Currently are $NUM_OF_TESTS tests available:
EOF

for (( i=0; i < NUM_OF_TESTS; ++i )); do
   TEST=$(echo ${TESTS[$i]} | sed 's/\.hpp//g')
   echo " * - $TEST" >> $HPP_FILE
done

cat >> $HPP_FILE << EOF
 *
 */
class $CLASS_NAME {
   private:
      bool        doThisTest[$NUM_OF_TESTS];
      std::string tests[$NUM_OF_TESTS];

   public:
      $CLASS_NAME();

      void allTestsOff();
      void allTestsOn();

      void enable( std::string _test );
      void disable( std::string _test );

      void run( e_engine::uJSON_data &_data, std::string _dataRoot );
      void list();
};

#endif // $(echo ${HPP_FILE^^} | sed 's/\./_/g')

// kate: indent-mode cstyle; indent-width 3; replace-tabs on;

EOF



cat > $CPP_FILE << EOF
/*!
 * \file $CPP_FILE
 * \brief \b Classes: \a $CLASS_NAME
 *
 * \warning Automatically generated file DO NOT EDIT HERE
 */

#include "$HPP_FILE"

using namespace std;
using namespace e_engine;

EOF

for (( i=0; i < NUM_OF_TESTS; ++i )); do
   echo "#include \"${TESTS_DIR}/${TESTS[$i]}\"" >> $CPP_FILE
done

cat >> $CPP_FILE << EOF

${CLASS_NAME}::${CLASS_NAME}() {
   allTestsOn();

EOF

for (( i=0; i < NUM_OF_TESTS; ++i )); do
   TEST=$(echo ${TESTS[$i]} | sed 's/\.hpp//g')
   echo "   tests[$i]=\"${TEST}\";" >> $CPP_FILE
done

cat >> $CPP_FILE << EOF
}

void ${CLASS_NAME}::allTestsOff() {
   iLOG( "Disabling all tests" );

   for( unsigned int i = 0; i < $NUM_OF_TESTS; ++i )
      doThisTest[i] = false;
}

void ${CLASS_NAME}::allTestsOn() {
   iLOG( "Enabling all tests" );

   for( unsigned int i = 0; i < $NUM_OF_TESTS; ++i )
      doThisTest[i] = true;
}

void ${CLASS_NAME}::enable( string _test ) {
   for( unsigned int i = 0; i < $NUM_OF_TESTS; ++i ) {
      if( tests[i] == _test ) {
         doThisTest[i] = true;
         iLOG( "Test '", tests[i], "' enabled" );
         return;
      }
   }
   wLOG( "Unable to find test '", _test, "' to enable it" );
}

void ${CLASS_NAME}::disable( string _test ) {
   for( unsigned int i = 0; i < $NUM_OF_TESTS; ++i ) {
      if( tests[i] == _test ) {
         doThisTest[i] = false;
         iLOG( "Test '", tests[i], "' disabled" );
         return;
      }
   }
   wLOG( "Unable to find test '", _test, "' to disable it" );
}


void ${CLASS_NAME}::run( uJSON_data &_data, string _dataRoot ) {
EOF




msg2 "Found $NUM_OF_TESTS tests"

for (( i=0; i < NUM_OF_TESTS; ++i )); do
   TEST=$(echo ${TESTS[$i]} | sed 's/\.hpp//g')
   processBar $i $NUM_OF_TESTS $TEST

   HPP="$(pwd)/${TESTS_DIR}/${TEST}.hpp"
   CPP="$(pwd)/${TESTS_DIR}/${TEST}.cpp"

   if [ ! -f $HPP -o ! -f $CPP ]; then
      continue
   fi

   OBJ="l${TEST}_obj"

   SPACE=""
   for (( j = ${#TEST}; j < 25; ++j )); do
      SPACE="$SPACE "
   done

   EQUAL_SIGNS=""
   for (( j = 0; j < ${#TEST}; ++j )); do
      EQUAL_SIGNS="${EQUAL_SIGNS}="
   done

cat >> $CPP_FILE << EOF

   // Begin Test $TEST
   if( doThisTest[$i] ) {
      dLOG( "" );
      dLOG( "      STARTING TEST '${TEST}'" );
      dLOG( "      ===============${EQUAL_SIGNS}=" );
      dLOG( "" );
      dLOG( "  -- ", ${TEST}::desc );
      dLOG( "" );

      $TEST ${OBJ};
      ${OBJ}.runTest( _data, _dataRoot );

      dLOG( "" );
      dLOG( "      END TEST '${TEST}'" );
      dLOG( "      ==========${EQUAL_SIGNS}=" );
      dLOG( "" );
   }

EOF

   IFS_backup=$IFS
   IFS=$'\n'
   for J in $(cat $CPP | grep '^//#!BIND' | sed 's/\/\/#!BIND[ ]*//g' ); do
      addBind $J
   done
   IFS=$IFS_backup
done

IFS_backup=$IFS
IFS=$'\n'
for J in $(cat main.cpp | grep '^//#!BIND' | sed 's/\/\/#!BIND[ ]*//g' ); do
   addBind $J
done
IFS=$IFS_backup

genFile # generates oglTestBindings.sh

chmod +x $SCRIPT_FILE

cat >> $CPP_FILE << EOF
}


void ${CLASS_NAME}::list() {
   iLOG( "Available tests: " );
EOF

for (( i=0; i < NUM_OF_TESTS; ++i )); do
   TEST=$(echo ${TESTS[$i]} | sed 's/\.hpp//g')

   SPACE=""
   for (( j = ${#TEST}; j < 25; ++j )); do
      SPACE="$SPACE "
   done

   echo "   dLOG( \" - ${TEST}${SPACE}-- \", ${TEST}::desc );" >> $CPP_FILE
done

cat >> $CPP_FILE << EOF
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on;

EOF
