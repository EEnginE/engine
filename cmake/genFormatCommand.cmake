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

function( generate_format_command CMD_NAME CM_CLANG_FORMAT_VER FILE_IN FILE_OUT )
   list( APPEND SOURCES_RAW ${ALL_UNASIGNED_HPP} )
   list( APPEND SOURCES_RAW ${ALL_UNASIGNED_CPP} )
   list( SORT SOURCES_RAW )

   foreach( I IN LISTS SOURCES_RAW )
      string( APPEND CM_ALL_SOURCE_FILES "   ${I}\n" )
   endforeach( I IN LISTS SOURCES_RAW )

   configure_file( "${FILE_IN}" "${FILE_OUT}" @ONLY )
   add_custom_target( ${CMD_NAME} COMMAND ${CMAKE_COMMAND} -P ${FILE_OUT} )
endfunction( generate_format_command )
