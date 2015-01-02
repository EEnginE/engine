#!/bin/bash

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

BINDINGS_COUNTER=0
BINDINGS_FINAL=()

SCRIPT_FILE="oglTestBindings.sh"

genFile() {

cat > $SCRIPT_FILE << EOF
#!/bin/bash

# !WARNING! Automatically generated file. DO NOT EDIT

BINDS=(
EOF

for (( i = 0; i < BINDINGS_COUNTER; ++i )); do
    echo "'${BINDINGS_FINAL[$i]}'" >> $SCRIPT_FILE
done

cat >> $SCRIPT_FILE << _EOF_
)

VAR=""
IND=0

echoBinds() {
    IND_STR=""

    for (( i = 0; i < \$IND; ++i )); do
    IND_STR="\$IND_STR "
    done

    echo "\${IND_STR}\${VAR}("

    for (( i = 0; i < \${#BINDS[*]}; ++i )); do
    echo -n "\${IND_STR}   \${BINDS[\$i]}"
    if (( i != ( \${#BINDS[*]} - 1 ) )); then
        echo ","
    fi
    done

    echo -e "\n\${IND_STR});"
}

HPP_FILE=""
CPP_FILE=""
CLASS=""

genClass() {
cat << EOF > \$HPP_FILE
/*!
 * \file \$HPP_FILE
 * \brief \b Classes: \a \$CLASS
 *
 * \warning Automatically generated file DO NOT EDIT HERE
 */

#ifndef \$(echo \${HPP_FILE^^} | sed 's/\./_/g')
#define \$(echo \${HPP_FILE^^} | sed 's/\./_/g')

#include "engine.hpp"

class \$CLASS {
   public:
      static void process( e_engine::uJSON_data &_data );
      static bool process( std::string _file );
};

#endif // \$(echo \${HPP_FILE^^} | sed 's/\./_/g')

EOF

cat << EOF > \$CPP_FILE
/*!
 * \file \$CPP_FILE
 * \brief \b Classes: \a \$CLASS
 *
 * \warning Automatically generated file DO NOT EDIT HERE
 */

#include "\$HPP_FILE"

using namespace e_engine;

void \${CLASS}::process( e_engine::uJSON_data &_data ) {
EOF

VAR=_data
IND=3

echoBinds >> \$CPP_FILE

cat << EOF >> \$CPP_FILE
}

bool \${CLASS}::process( std::string _file ) {
   e_engine::uParserJSON parser( _file );
   if( parser.parse() == 1 ) {
      e_engine::uJSON_data data = parser.getData();
      \${CLASS}::process( data );
      return true;
   } else {
      return false;
   }
}

EOF

}

if (( \$# == 2 )); then
    VAR=\$1
    IND=\$2

    echoBinds

    exit 0
fi

if (( \$# == 3 )); then
    HPP_FILE=\$1
    CPP_FILE=\$2
    CLASS=\$3

    genClass

    exit 0
fi


echo "Usage: <data varname> <indent>"
echo "-- OR --"
echo "Usage: <file_hpp> <file_cpp> <class-name>"
exit 4

_EOF_

}

addBind() {
    BINDINGS_FINAL[$BINDINGS_COUNTER]=$1
    (( BINDINGS_COUNTER++ ))
}

# kate: indent-mode shell; indent-width 4; replace-tabs on;
