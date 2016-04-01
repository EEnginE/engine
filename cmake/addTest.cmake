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

function( add_engine_test TEST_NAME )
   foreach( I IN LISTS ENGINE_LIBRARY_LIST )
      string( APPEND CM_ENGINE_LINK_LIST_SHARED "engine_${I}_shared " )
      string( APPEND CM_ENGINE_LINK_LIST_STATIC "engine_${I}_static " )
   endforeach( I IN LISTS ENGINE_LIBRARY_LIST )

   message( STATUS "Test ${TEST_NAME}:" )

   if( EXISTS ${ENGINE_TEST_ROOT}/${TEST_NAME}/CMakeScript.cmake )
      message( STATUS "  - Found CMakeScript.cmake" )
      include( ${ENGINE_TEST_ROOT}/${TEST_NAME}/CMakeScript.cmake )
   endif( EXISTS ${ENGINE_TEST_ROOT}/${TEST_NAME}/CMakeScript.cmake )

   find_source_files(  ${ENGINE_TEST_ROOT}/${TEST_NAME} )

   select_sources() # Sets CM_CURRENT_SRC_CPP and CM_CURRENT_SRC_HPP and CURRENT_INCLUDE_DIRS

   set( ENGINE_INCLUDE_DIRECTORIES ${ENGINE_INCLUDE_DIRECTORIES} "${ENGINE_TEST_ROOT}/${TEST_NAME}/" )
   foreach( I IN LISTS CURRENT_INCLUDE_DIRS )
      set( ENGINE_INCLUDE_DIRECTORIES ${ENGINE_INCLUDE_DIRECTORIES} "${ENGINE_TEST_ROOT}/${TEST_NAME}/${I}" )
   endforeach( I IN LISTS CURRENT_INCLUDE_DIRS )

   set( CM_CURRENT_LIB_LC  ${TEST_NAME} )
   string( TOUPPER ${TEST_NAME} CM_CURRENT_LIB_UC )
   set( CM_CURRENT_LIB_SRC "${CM_CURRENT_LIB_UC}_SRC" )
   set( CM_CURRENT_LIB_INC "${CM_CURRENT_LIB_UC}_INC" )

   configure_file( "${TEMPLATES_DIR}/CMakeLists.test.in" "${ENGINE_TEST_ROOT}/${TEST_NAME}/CMakeLists.txt" @ONLY )

   set( ENGINE_SUBDIRECTORIES      ${ENGINE_SUBDIRECTORIES} ${ENGINE_TEST_ROOT}/${TEST_NAME} PARENT_SCOPE )
   set( ENGINE_INCLUDE_DIRECTORIES ${ENGINE_INCLUDE_DIRECTORIES}                             PARENT_SCOPE )
endfunction( add_engine_test )

