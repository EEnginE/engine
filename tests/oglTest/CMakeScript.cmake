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


function( ogltest_gen_binding_class CM_CLASS_NAME DIR )
   set( CURRENT_ROOT "${ENGINE_TEST_ROOT}/oglTest" )
   string( TOUPPER "${CM_CLASS_NAME}" CM_CLASS_NAME_UC )

   configure_file( "${CURRENT_ROOT}/templates/bind.hpp.in" "${DIR}/${CM_CLASS_NAME}.hpp" )
   configure_file( "${CURRENT_ROOT}/templates/bind.cpp.in" "${DIR}/${CM_CLASS_NAME}.cpp" )
endfunction( ogltest_gen_binding_class )



macro( ogltest_check_bind_location FILE_PATH )
   file( STRINGS "${FILE_PATH}" RAW_STRINGS REGEX "^//#!BIND" )
   foreach( X IN LISTS RAW_STRINGS )
      string( REGEX REPLACE "^//#!BIND *" "" X "${X}" )
      string( REGEX REPLACE " +$"         "" X "${X}" )
      string( APPEND CM_OGLTEST_BINDINGS_LIST "      ${X},\n" )
   endforeach( X IN LISTS RAW_STRINGS )
endmacro( ogltest_check_bind_location )



function( ogltest_main )
   set( CM_CLASS_NAME    testStarter )
   set( CM_CLASS_NAME_UC TESTSTARTER )

   set( CURRENT_ROOT "${ENGINE_TEST_ROOT}/oglTest" )
   file( GLOB TESTS_RAW "${CURRENT_ROOT}/tests/*.cpp" )
   file( GLOB TESTS_HPP "${CURRENT_ROOT}/tests/*.hpp" )

   ################################
   # Parese the //#!BIND commands #
   ################################
   set( CM_OGLTEST_BINDINGS_LIST "" CACHE STRING "List of all ogl test bindings" FORCE )

   ogltest_check_bind_location( "${CURRENT_ROOT}/main.cpp" )

   foreach( I IN LISTS TESTS_RAW TESTS_HPP )
      ogltest_check_bind_location( "${I}" )
   endforeach( I IN LISTS TESTS_RAW TESTS_HPP )

   string( REGEX REPLACE ",\n$" "" CM_OGLTEST_BINDINGS_LIST "${CM_OGLTEST_BINDINGS_LIST}" )
   set( CM_OGLTEST_BINDINGS_LIST "${CM_OGLTEST_BINDINGS_LIST}" CACHE STRING "List of all ogl test bindings" FORCE )

   ##########################
   # Generate starter class #
   ##########################
   set( CM_COUNTER 0 )

   foreach( I IN LISTS TESTS_RAW )
      file( RELATIVE_PATH I "${CURRENT_ROOT}/tests" "${I}" )
      string( REGEX REPLACE "^test/" "" I "${I}" )
      string( REGEX REPLACE ".cpp$"  "" I "${I}" )
      string( LENGTH "${I}" STRING_LENGTH )

      list( APPEND TESTS "${I}" )
      string( APPEND CM_TESTS_STR      "\n * - ${I}" )
      string( APPEND CM_INCLUDE_TESTS  "#include \"tests/${I}.hpp\"\n" )
      string( APPEND CM_INIT_TESTS_STR "tests[${CM_COUNTER}] = \"${I}\";" )

      set( EQUAL_S "" )
      foreach( I RANGE 1 ${STRING_LENGTH} )
         string( APPEND EQUAL_S "=" )
      endforeach( I RANGE 1 ${STRING_LENGTH} )


      string( APPEND CM_START_TEST "
   // Begin Test ${I}
   if ( doThisTest[${CM_COUNTER}] ) {
      dLOG( \"\" );
      dLOG( \"      STARTING TEST '${I}'\" );
      dLOG( \"      ================${EQUAL_S}\" );
      dLOG( \"\" );
      dLOG( \"  -- \", ${I}::desc );
      dLOG( \"\" );

      ${I} l${I}_obj;
      l${I}_obj.runTest( _data, _dataRoot );

      dLOG( \"\" );
      dLOG( \"      END TEST '${I}'\" );
      dLOG( \"      ===========${EQUAL_S}\" );
      dLOG( \"\" );
   }\n\n" )

      set( SPACES "" )
      foreach( I RANGE ${STRING_LENGTH} 25 )
         string( APPEND SPACES " " )
      endforeach( I RANGE ${STRING_LENGTH} 25 )

      string( APPEND CM_LIST_TESTS "   dLOG( \" - ${I}${SPACES}\", ${I}::desc );\n" )

      math( EXPR CM_COUNTER "${CM_COUNTER} + 1" )
   endforeach( I IN LISTS TESTS_RAW )

   list( LENGTH TESTS CM_NUM_TESTS )

   configure_file( "${CURRENT_ROOT}/templates/testStarter.hpp.in" "${CURRENT_ROOT}/${CM_CLASS_NAME}.hpp" @ONLY )
   configure_file( "${CURRENT_ROOT}/templates/testStarter.cpp.in" "${CURRENT_ROOT}/${CM_CLASS_NAME}.cpp" @ONLY )
endfunction( ogltest_main )

ogltest_main()