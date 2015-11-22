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

function( generate_engine_hpp FILE_IN FILE_OUT )
   foreach( I IN LISTS PLATFORM_LIST )
      if( DEFINED SRC_${I}_HPP )
         if( ${DISPLAY_SERVER} STREQUAL ${I} )
            list( APPEND INCLUDES_RAW ${ALL_SRC_${I}_HPP} )
         endif( ${DISPLAY_SERVER} STREQUAL ${I} )
      endif( DEFINED SRC_${I}_HPP )
   endforeach( I IN LISTS PLATFORM_LIST )

   list( APPEND INCLUDES_RAW ${ALL_SRC_ALL_HPP} )

   foreach( I IN LISTS INCLUDES_RAW )
      string( REGEX REPLACE "^[^/]*/" "" FILE_NAME "${I}" )
      list( APPEND INCLUDES "${FILE_NAME}" )
   endforeach( I IN LISTS INCLUDES_RAW )

   list( SORT INCLUDES )

   foreach( I IN LISTS INCLUDES )
      string( APPEND CM_ALL_ENGINE_INCLUDES "#include \"${I}\"\n" )
   endforeach( I IN LISTS INCLUDES )

   configure_file( "${FILE_IN}" "${FILE_OUT}" )

endfunction( generate_engine_hpp )
