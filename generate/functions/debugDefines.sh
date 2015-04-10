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

makeDebugDefines() {
   msg2 "Searching log macros in soure code..." >&2

   local i COUNTER NUM TEMP
   COUNTER=1
   NUM=${#ALL_SOURCE_FILES[@]}
   for i in "${ALL_SOURCE_FILES[@]}"; do
      processBar $COUNTER "$NUM" "$i" >&2
      (( COUNTER++ ))

      if [ ! -f "$i" ]; then
           warning "File $i not found"
           continue
      fi

      TEMP="$TEMP $(grep '^#if[ ]*D_LOG' "$i" | sed 's/[#a-zA-Z 0-9#]*\(D_LOG_[A-Z]*\) \?[#a-zA-Z 0-9\/]*/\1/g')"
   done

   DEBUG_DEFINES=( $(echo "$TEMP" | sed 's/ /\n/g' | sort | uniq ) )

   local IFNDEF_HEADDER="$( basename "$DEBUG_DEF_FILE" )"
   local IFNDEF_HEADDER="${IFNDEF_HEADDER//./_}"
   local IFNDEF_HEADDER="${IFNDEF_HEADDER^^}"

   cat <<EOF > "${DEBUG_DEF_FILE}.in.hpp"
/*!
 * \\file $(basename "$DEBUG_DEF_FILE").hpp
 * \\brief Some important macros for logging are defined here
 *
 * \\warning This is an automatically generated file of '$0'! DO NOT EDIT
 */

#ifndef ${IFNDEF_HEADDER}_HPP
#define ${IFNDEF_HEADDER}_HPP

EOF

   local temp
   local msg_t

   for i in "${DEBUG_DEFINES[@]}"; do
      temp="${i//D_/}"
      msg_t="$msg_t[${i//D_LOG_/}] "
      echo "#define $i  @${i}_CM@" >> "${DEBUG_DEF_FILE}.in.hpp"

      cat <<EOF

if( NOT DEFINED ${temp} )
   if( ENGINE_VERBOSE OR DEBUG_LOG_ALL )
      set( ${i}_CM 1 )
      message( STATUS "Enabled ${temp}. Use -D${temp}=0 or -DDEBUG_LOG_ALL=0 to disable" )
   else( ENGINE_VERBOSE OR DEBUG_LOG_ALL )
      set( ${i}_CM 0 )
      message( STATUS "Disabled ${temp}. Use -D${temp}=1 or -DDEBUG_LOG_ALL=1 to enable" )
   endif( ENGINE_VERBOSE OR DEBUG_LOG_ALL )
else( NOT DEFINED ${temp} )
   message( STATUS "User defined debug status ${temp}: \${${temp}} (change with -D${temp})" )
   set( ${i}_CM \${${temp}} )
endif( NOT DEFINED ${temp} )

EOF

   done

   msg_t="${msg_t//[/\\x1b[35m[\\x1b[36m}"
   msg_t="${msg_t//]/\\x1b[35m]}"
   msg2 "$msg_t" >&2


   cat <<EOF >> "${DEBUG_DEF_FILE}.in.hpp"

#endif // ${IFNDEF_HEADDER}_HPP

EOF

   cat <<EOF

configure_file(
 "\${PROJECT_SOURCE_DIR}/${DEBUG_DEF_FILE}.in.hpp"
 "\${PROJECT_SOURCE_DIR}/${DEBUG_DEF_FILE}.hpp"
)

EOF
}
