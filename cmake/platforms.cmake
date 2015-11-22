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

# Adds a operating system with (multiple) graphic API's
# Variables:
#  DISPLAY_SERVER  - the display server to use
#  PLATFORM_LIST   - list of all platforms added so far (output)
function( add_platform )
   if( ${ARGC} LESS 2 )
      message( FATAL_ERROR "function 'add_platform' needs at least 2 parameters" )
   endif( ${ARGC} LESS 2 )

   set( OS_VAR ${ARGV0} )
   if( ${OS_VAR} )
      set( CURRENT_OS_STRING " (current)" )
   endif( ${OS_VAR} )

   math( EXPR ARGC_M1 "${ARGC} - 1" )

   # Generate the platform list
   message( STATUS "Adding platform support for ${OS_VAR}${CURRENT_OS_STRING}. Supported API's are:" )
   foreach( I RANGE 1 ${ARGC_M1} )
      string( TOUPPER "${OS_VAR}_${ARGV${I}}" VAR_NAME )
      message( STATUS " - ${ARGV${I}}: \t ${VAR_NAME}" )
      set( PLATFORM_LIST ${PLATFORM_LIST} ${VAR_NAME} )
      set( ${VAR_NAME} "${ARGV${I}}" PARENT_SCOPE ) # store <api> in <OS>_<API> for the find sources script
   endforeach( I RANGE 1 ${ARGC_M1} )

   # Set default display server
   if( ${OS_VAR} )
      if( NOT DISPLAY_SERVER )
         string( TOUPPER "${OS_VAR}_${ARGV1}" VAR_NAME )
         set( DISPLAY_SERVER ${VAR_NAME} PARENT_SCOPE )
      endif( NOT DISPLAY_SERVER )
   endif( ${OS_VAR} )

   # Export to parent scope
   set( PLATFORM_LIST ${PLATFORM_LIST} PARENT_SCOPE )
endfunction( add_platform )


function( check_platform )
   set( FOUND_DISPLAY_SERVER OFF )

   foreach( I IN LISTS PLATFORM_LIST )
      set( CM_${I} 0 PARENT_SCOPE )

      if( ${DISPLAY_SERVER} STREQUAL ${I} )
         set( FOUND_DISPLAY_SERVER ON )
         set( CM_${I} 1 PARENT_SCOPE )
         message( STATUS "Using display server ${DISPLAY_SERVER} (change with -DDISPLAY_SERVER)\n" )
      endif( ${DISPLAY_SERVER} STREQUAL ${I} )

   endforeach( I IN LISTS PLATFORM_LIST )

   if( NOT FOUND_DISPLAY_SERVER )
      message( FATAL_ERROR "Unknown display server ${DISPLAY_SERVER}" )
   endif( NOT FOUND_DISPLAY_SERVER )

endfunction( check_platform )