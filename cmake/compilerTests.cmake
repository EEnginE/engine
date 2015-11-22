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

function( add_compiler_test TEST )
   string( TOUPPER "${TEST}" TEST_UC )
   try_compile(
      ${TEST_UC}_COMPILES
      "${PROJECT_BINARY_DIR}/bin/compilerTests"
      "${COMPILER_TESTS_DIR}/${TEST}.cpp"
      COMPILE_DEFINITIONS "${CMAKE_CXX_FLAGS_RELEASE}"
      OUTPUT_VARIABLE AAA )

   if( ${TEST_UC}_COMPILES )
      set( COMPILER_TEST_${TEST_UC}_PASSED 1 PARENT_SCOPE )
      message( "  - ${TEST} passed" )
   else( ${TEST_UC}_COMPILES )
      set( COMPILER_TEST_${TEST_UC}_PASSED 0 PARENT_SCOPE )
      message( "  - ${TEST} failed" )
   endif( ${TEST_UC}_COMPILES )
endfunction( add_compiler_test )