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

macro( find_helper_make_relative INPUT OUT )
   foreach( I IN LISTS ${INPUT} )
      file( TO_CMAKE_PATH "${I}" I )
      file( RELATIVE_PATH I "${ROOT_DIR}" "${I}" )
      set( ${OUT} ${${OUT}} "${I}" )
   endforeach( I IN LISTS ${INPUT} )
endmacro( find_helper_make_relative )

macro( find_helper_asign_and_export FILE_LIST PLT_LIST SUFFIX )
   foreach( I IN LISTS ${FILE_LIST} )
      set( MATCH_FOUND OFF )

      foreach( J IN LISTS ${PLT_LIST} )
         string( REGEX MATCH "^${${J}}/" REG_OUT "${I}" )

         if( NOT "${REG_OUT}" STREQUAL "" )
            set( SRC_${J}_${SUFFIX} "${SRC_${J}_${SUFFIX}}" "${I}" )
            set( MATCH_FOUND ON )
            break()
         endif( NOT "${REG_OUT}" STREQUAL "" )

      endforeach( J IN LISTS ${PLT_LIST} )

      if( MATCH_FOUND )
         continue()
      endif( MATCH_FOUND )

      set( SRC_ALL_${SUFFIX} "${SRC_ALL_${SUFFIX}}" "${I}" )

   endforeach( I IN LISTS ${FILE_LIST} )

   # Export variables
   foreach( J IN LISTS ${PLT_LIST} )
      set( SRC_${J}_${SUFFIX} ${SRC_${J}_${SUFFIX}} PARENT_SCOPE )
   endforeach( J IN LISTS ${PLT_LIST} )

   set( SRC_ALL_${SUFFIX} ${SRC_ALL_${SUFFIX}} PARENT_SCOPE )
endmacro( find_helper_asign_and_export )

function( find_source_files ROOT_DIR )
   file( GLOB_RECURSE CPP_LIST_RAW "${ROOT_DIR}/*.cpp" )
   file( GLOB_RECURSE HPP_LIST_RAW "${ROOT_DIR}/*.hpp" )

   find_helper_make_relative( CPP_LIST_RAW CPP_LIST )
   find_helper_make_relative( HPP_LIST_RAW HPP_LIST )

   find_helper_asign_and_export( CPP_LIST PLATFORM_LIST "CPP" )
   find_helper_asign_and_export( HPP_LIST PLATFORM_LIST "HPP" )

   set( UNASIGNED_CPP ${CPP_LIST} PARENT_SCOPE )
   set( UNASIGNED_HPP ${HPP_LIST} PARENT_SCOPE )
endfunction( find_source_files )


macro( export_helper_append_and_sort LIST_ALL TO_APPEND ROOT_DIR )
   if( DEFINED ${TO_APPEND} )
      list( SORT ${TO_APPEND} )
      foreach( I IN LISTS ${TO_APPEND} )
         set( ${LIST_ALL} ${${LIST_ALL}} "${ROOT_DIR}/${I}" )
      endforeach( I IN LISTS ${TO_APPEND} )
      list( SORT ${LIST_ALL} )
      set( ${LIST_ALL} ${${LIST_ALL}} PARENT_SCOPE )
   endif( DEFINED ${TO_APPEND} )
endmacro( export_helper_append_and_sort )

macro( export_found_files ROOT_DIR )
   foreach( I IN LISTS PLATFORM_LIST )
      export_helper_append_and_sort( ALL_SRC_${I}_CPP SRC_${I}_CPP ${ROOT_DIR} )
      export_helper_append_and_sort( ALL_SRC_${I}_HPP SRC_${I}_HPP ${ROOT_DIR} )
   endforeach( I IN LISTS PLATFORM_LIST )

   export_helper_append_and_sort( ALL_SRC_ALL_CPP SRC_ALL_CPP ${ROOT_DIR} )
   export_helper_append_and_sort( ALL_SRC_ALL_HPP SRC_ALL_HPP ${ROOT_DIR} )

   export_helper_append_and_sort( ALL_UNASIGNED_CPP UNASIGNED_CPP ${ROOT_DIR} )
   export_helper_append_and_sort( ALL_UNASIGNED_HPP UNASIGNED_HPP ${ROOT_DIR} )
endmacro( export_found_files )