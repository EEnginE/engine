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

macro( gen_log_macro_helper_undef OUT_VAR DEF )
   string( APPEND ${OUT_VAR} "#if defined ${DEF}\n" )
   string( APPEND ${OUT_VAR} "#warning \"UNDEF ${DEF} -- Please remove '#define ${DEF}' from your project files\"\n" )
   string( APPEND ${OUT_VAR} "#undef ${DEF}\n" )
   string( APPEND ${OUT_VAR} "#endif // defined ${DEF}\n\n" )
endmacro( gen_log_macro_helper_undef )

function( gen_log_macros FILE_IN FILE_OUT )
   math( EXPR ARGC_M1 "${ARGC} - 1" )
   foreach( I RANGE 2 ${ARGC_M1} )
      string( TOUPPER "${ARGV${I}}" TYPE_UC )
      string( TOLOWER "${ARGV${I}}" TYPE_LC )

      gen_log_macro_helper_undef( CM_UNDEFS1 "_${TYPE_UC}" )
      gen_log_macro_helper_undef( CM_UNDEFS2 "_h${TYPE_UC}" )
      gen_log_macro_helper_undef( CM_UNDEFS3 "${TYPE_LC}LOG" )

      string( APPEND CM_DEFINES1 "#define _${TYPE_UC}  '${TYPE_UC}', false, W_FILE, __LINE__, W_FUNC, std::this_thread::get_id()\n" )
      string( APPEND CM_DEFINES2 "#define _h${TYPE_UC} '${TYPE_UC}', true,  W_FILE, __LINE__, W_FUNC, std::this_thread::get_id()\n" )
      string( APPEND CM_DEFINES3 "#define ${TYPE_LC}LOG( ... ) ::e_engine::LOG.addLogEntry( _${TYPE_UC}, __VA_ARGS__ )\n" )

   endforeach( I RANGE 1 ${ARGC_M2} )

   configure_file( "${FILE_IN}" "${FILE_OUT}" )
endfunction( gen_log_macros )
