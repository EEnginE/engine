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

function( run_git )
   set( CM_VERSION_GIT "GIT_FAILED" )

   # Set the version of this project
   set( CM_VERSION_MAJOR    0 )
   set( CM_VERSION_MINOR    0 )
   set( CM_VERSION_SUBMINOR 0 )
   set( CM_TAG_DIFF         0 )

   execute_process(
      COMMAND git --version
      RESULT_VARIABLE GIT_FOUND
      OUTPUT_VARIABLE DEV_NULL
   )

   if( ${GIT_FOUND} MATCHES 0 )
      execute_process(
         COMMAND git rev-parse HEAD
         OUTPUT_VARIABLE CM_VERSION_GIT
         WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
         RESULT_VARIABLE GIT_RESULT
      )

      # Remove newlines added by Git
      string( REGEX REPLACE "\n" "" CM_VERSION_GIT ${CM_VERSION_GIT} )

      execute_process(
         COMMAND git describe --abbrev=0 --tags
         OUTPUT_VARIABLE GIT_TAG_1
         WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
      )

      execute_process(
         COMMAND git describe --tags
         OUTPUT_VARIABLE GIT_TAG_2
         WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
         RESULT_VARIABLE GIT_HAS_TAGS
      )

      if( ${GIT_HAS_TAGS} EQUAL 0 )
         string( REGEX REPLACE "\n" "" GIT_TAG_1 "${GIT_TAG_1}" )
         string( REGEX REPLACE "\n" "" GIT_TAG_2 "${GIT_TAG_2}" )

         string( REGEX REPLACE "^v([0-9]+)[\\.0-9]+$" "\\1" CM_VERSION_MAJOR "${GIT_TAG_1}" )
         string( REGEX REPLACE "^v[\\.0-9]+([0-9]+)[\\.0-9]+$" "\\1" CM_VERSION_MINOR "${GIT_TAG_1}" )
         string( REGEX REPLACE "^v[\\.0-9]+([0-9]+)$" "\\1" CM_VERSION_SUBMINOR "${GIT_TAG_1}" )

         if( "${GIT_TAG_1}" STREQUAL "${GIT_TAG_2}" )
            set( CM_TAG_DIFF 0 )
            message( STATUS "${PROJECT_NAME} version: ${GIT_TAG_1} [RELEASE] ${CM_VERSION_GIT}" )
         else()
            string( REGEX REPLACE "^v[\\.0-9]+\\-([0-9]*)\\-[a-z0-9]*$" "\\1" CM_TAG_DIFF "${GIT_TAG_2}" )
            message( STATUS "${PROJECT_NAME} version: ${GIT_TAG_1} +${CM_TAG_DIFF} ${CM_VERSION_GIT}" )
         endif( "${GIT_TAG_1}" STREQUAL "${GIT_TAG_2}" )
      else( ${GIT_HAS_TAGS} EQUAL 0 )
         message( STATUS "Can not access tags ==> version is v0.0.1 +1" )
         set( CM_VERSION_MAJOR    0 )
         set( CM_VERSION_MINOR    0 )
         set( CM_VERSION_SUBMINOR 1 )
         set( CM_TAG_DIFF         1 )
      endif( ${GIT_HAS_TAGS} EQUAL 0 )

   endif( ${GIT_FOUND} MATCHES 0 )


   # Only set CMAKE_BUILD_TYPE when not defined
   if( NOT CMAKE_BUILD_TYPE )
      set( CMAKE_BUILD_TYPE DEBUG )

      # Is this tagged ==> Release?
      if( "${CM_TAG_DIFF}" STREQUAL "0" )
         set( CMAKE_BUILD_TYPE RELEASE )
      endif( "${CM_TAG_DIFF}" STREQUAL "0" )
   endif( NOT CMAKE_BUILD_TYPE )

   message( STATUS "Build type: ${CMAKE_BUILD_TYPE}\n" )

   set( DEBUG_LOGGING 0 )

   if( ENGINE_VERBOSE OR "${CMAKE_BUILD_TYPE}" MATCHES "DEBUG" )
      set( DEBUG_LOGGING 1 )
   endif( ENGINE_VERBOSE OR "${CMAKE_BUILD_TYPE}" MATCHES "DEBUG" )

   set( CMAKE_BUILD_TYPE    ${CMAKE_BUILD_TYPE}    PARENT_SCOPE )
   set( DEBUG_LOGGING       ${DEBUG_LOGGING}       PARENT_SCOPE )
   set( CM_VERSION_MAJOR    ${CM_VERSION_MAJOR}    PARENT_SCOPE )
   set( CM_VERSION_MINOR    ${CM_VERSION_MINOR}    PARENT_SCOPE )
   set( CM_VERSION_SUBMINOR ${CM_VERSION_SUBMINOR} PARENT_SCOPE )
   set( CM_TAG_DIFF         ${CM_TAG_DIFF}         PARENT_SCOPE )
   set( CM_VERSION_GIT      ${CM_VERSION_GIT}      PARENT_SCOPE )
endfunction( run_git )
