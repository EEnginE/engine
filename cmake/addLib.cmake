
# Adds a engine library
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

# Variables:
#  ENGINE_LIB_ROOT:             root directory for the engine libs
#  ENGINE_INCLUDE_DIRECTORIES:  (output)
#  ENGINE_SUBDIRECTORIES:       (output) a list of all subdirectories
function( add_engine_lib LIB_NAME )
   if( ${ARGC} GREATER 1 )
      math( EXPR ARGC_M1 "${ARGC} - 1" )
      foreach( I RANGE 1 ${ARGC_M1} )
         set( CM_CURRENT_LIB_DEP_SHARED "${CM_CURRENT_LIB_DEP_SHARED} engine_${ARGV${I}}_shared" )
         set( CM_CURRENT_LIB_DEP_STATIC "${CM_CURRENT_LIB_DEP_STATIC} engine_${ARGV${I}}_static" )
         set( CM_CURRENT_LIB_DEP        "${CM_CURRENT_LIB_DEP} ${ARGV${I}}" )
      endforeach( I RANGE 1 ${ARGC_M2} )
   endif( ${ARGC} GREATER 1 )

   message( STATUS "Library ${LIB_NAME}: (depends on:${CM_CURRENT_LIB_DEP})" )

   find_source_files(  ${ENGINE_LIB_ROOT}/${LIB_NAME} )
   export_found_files( ${ENGINE_LIB_ROOT}/${LIB_NAME} )

   select_sources() # Sets CM_CURRENT_SRC_CPP and CM_CURRENT_SRC_HPP and CURRENT_INCLUDE_DIRS

   set( ENGINE_INCLUDE_DIRECTORIES ${ENGINE_INCLUDE_DIRECTORIES} "${ENGINE_LIB_ROOT}/${LIB_NAME}/" )
   foreach( I IN LISTS CURRENT_INCLUDE_DIRS )
      set( ENGINE_INCLUDE_DIRECTORIES ${ENGINE_INCLUDE_DIRECTORIES} "${ENGINE_LIB_ROOT}/${LIB_NAME}/${I}" )
   endforeach( I IN LISTS CURRENT_INCLUDE_DIRS )

   set( CM_CURRENT_LIB_LC  ${LIB_NAME} )
   string( TOUPPER ${LIB_NAME} CM_CURRENT_LIB_UC )
   set( CM_CURRENT_LIB_SRC "${CM_CURRENT_LIB_UC}_SRC" )
   set( CM_CURRENT_LIB_INC "${CM_CURRENT_LIB_UC}_INC" )

   configure_file( "${TEMPLATES_DIR}/CMakeLists.lib.in" "${ENGINE_LIB_ROOT}/${LIB_NAME}/CMakeLists.txt" @ONLY )
   set( ENGINE_INCLUDE_DIRECTORIES ${ENGINE_INCLUDE_DIRECTORIES}                           PARENT_SCOPE )
   set( ENGINE_SUBDIRECTORIES      ${ENGINE_SUBDIRECTORIES} ${ENGINE_LIB_ROOT}/${LIB_NAME} PARENT_SCOPE )
   set( ENGINE_LIBRARY_LIST        ${ENGINE_LIBRARY_LIST}   ${LIB_NAME}                    PARENT_SCOPE )
endfunction( add_engine_lib )
