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

function( generate_debug_defines )
   message( STATUS "Debug defines (Enable all: ${DEBUG_LOG_ALL} (change with -DDEBUG_LOG_ALL / single vars with -DLOG_...))" )

   foreach( I IN LISTS ALL_UNASIGNED_CPP ALL_UNASIGNED_HPP )
      file( STRINGS "${I}" TMP REGEX "^ *#if +D_LOG_[A-Z]+ *" )
      set( DEFINES_RAW ${DEFINES_RAW} ${TMP} )
   endforeach( I IN LISTS ALL_UNASIGNED_CPP ALL_UNASIGNED_HPP )

   foreach( I IN LISTS DEFINES_RAW )
      set( FOUND OFF )
      string( REGEX REPLACE "^ *#if +" "" TMP "${I}"   )
      string( REGEX REPLACE " +$"      "" TMP "${TMP}" )

      foreach( J IN LISTS DEFINES )
         if( ${TMP} STREQUAL ${J} )
            set( FOUND ON )
            break()
         endif( ${TMP} STREQUAL ${J} )
      endforeach( J IN LISTS DEFINES )

      if( FOUND )
         continue()
      endif( FOUND )

      list( APPEND DEFINES ${TMP} )
      string( REGEX REPLACE "^D_" "" TMP2 "${TMP}" )

      if( NOT DEFINED ${TMP2} )
         if( DEBUG_LOG_ALL )
            set( CM_${TMP} 1 )
         else( DEBUG_LOG_ALL )
            set( CM_${TMP} 0 )
         endif( DEBUG_LOG_ALL )
      else( NOT DEFINED ${TMP2} )
         set( CM_${TMP} ${${TMP2}} )
      endif( NOT DEFINED ${TMP2} )

      if( NOT ${CM_${TMP}} EQUAL 0 )
         set( STATUS_STR " [enabled]" )
      else( NOT ${CM_${TMP}} EQUAL 0 )
         set( STATUS_STR "[disabled]" )
      endif( NOT ${CM_${TMP}} EQUAL 0 )

      message( STATUS "  - Found ${STATUS_STR} ${TMP}" )

      set( CM_DEFINES_DEBUG_DEFINES "${CM_DEFINES_DEBUG_DEFINES}#define ${TMP} ${CM_${TMP}}\n" )

   endforeach( I IN LISTS DEFINES_RAW )

   set( CM_DEFINES_DEBUG_DEFINES "${CM_DEFINES_DEBUG_DEFINES}" PARENT_SCOPE )

   message( "" ) # Newline
endfunction( generate_debug_defines )
