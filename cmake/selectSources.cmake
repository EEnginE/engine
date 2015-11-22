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

macro( select_sources_helper_list_to_string IN_LIST OUT_STRING )
   list( SORT ${IN_LIST} )
   foreach( I IN LISTS ${IN_LIST} )
      set( ${OUT_STRING} "${${OUT_STRING}}\n   \${CMAKE_CURRENT_SOURCE_DIR}/${I}" )
   endforeach( I IN LISTS ${IN_LIST} )

   set( ${OUT_STRING} "${${OUT_STRING}}" PARENT_SCOPE )
endmacro( select_sources_helper_list_to_string )

function( select_sources_helper_add_dir FILE_NAME )
   string( REGEX REPLACE "[^/]+$" "" DIR_NAME "${FILE_NAME}" )

   if( NOT DEFINED CURRENT_INCLUDE_DIRS )
      set( CURRENT_INCLUDE_DIRS "${DIR_NAME}" PARENT_SCOPE )
      return()
   endif( NOT DEFINED CURRENT_INCLUDE_DIRS )

   foreach( X IN LISTS CURRENT_INCLUDE_DIRS )
      if( "${X}" STREQUAL "${DIR_NAME}" )
         return()
      endif( "${X}" STREQUAL "${DIR_NAME}" )
   endforeach()

   set( CURRENT_INCLUDE_DIRS ${CURRENT_INCLUDE_DIRS} "${DIR_NAME}" PARENT_SCOPE )
endfunction( select_sources_helper_add_dir )

# Sets CM_CURRENT_SRC_CPP and CM_CURRENT_SRC_HPP for the current platform
function( select_sources )
   foreach( I IN LISTS PLATFORM_LIST )
      set( BUILD_STR "" )
      if( DEFINED SRC_${I}_CPP )
         if( ${DISPLAY_SERVER} STREQUAL ${I} )
            set( BUILD_STR "[build]" )
            set( CURRENT_SRC_CPP ${CURRENT_SRC_CPP} ${SRC_${I}_CPP} )
            set( CURRENT_SRC_HPP ${CURRENT_SRC_HPP} ${SRC_${I}_HPP} )

            foreach( J IN LISTS SRC_${I}_HPP )
               select_sources_helper_add_dir( "${J}" )
            endforeach( J IN LISTS SRC_${I}_HPP )
         endif( ${DISPLAY_SERVER} STREQUAL ${I} )

         list( LENGTH SRC_${I}_CPP CPP_LENGTH )
         list( LENGTH SRC_${I}_HPP HPP_LENGTH )

         math( EXPR LENGTH_ALL "${CPP_LENGTH} + ${HPP_LENGTH}" )
         message( STATUS "  - Target ${${I}}: \t${LENGTH_ALL} files ${BUILD_STR}" )
      endif( DEFINED SRC_${I}_CPP )
   endforeach( I IN LISTS PLATFORM_LIST )

   list( LENGTH SRC_ALL_CPP CPP_LENGTH )
   list( LENGTH SRC_ALL_HPP HPP_LENGTH )

   math( EXPR LENGTH_ALL "${CPP_LENGTH} + ${HPP_LENGTH}" )
   message( STATUS "  - Target ALL: \t${LENGTH_ALL} files [build]\n" )

   set( CURRENT_SRC_CPP ${CURRENT_SRC_CPP} ${SRC_ALL_CPP} )
   set( CURRENT_SRC_HPP ${CURRENT_SRC_HPP} ${SRC_ALL_HPP} )

   select_sources_helper_list_to_string( CURRENT_SRC_CPP CM_CURRENT_SRC_CPP )
   select_sources_helper_list_to_string( CURRENT_SRC_HPP CM_CURRENT_SRC_HPP )

   foreach( I IN LISTS CURRENT_SRC_HPP )
      select_sources_helper_add_dir( "${I}" )
   endforeach( I IN LISTS CURRENT_SRC_HPP )

   set( CURRENT_INCLUDE_DIRS ${CURRENT_INCLUDE_DIRS} PARENT_SCOPE )
endfunction( select_sources )
