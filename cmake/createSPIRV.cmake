# Copyright (C) 2016 EEnginE project
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

function( createSPIRV )
   if( NOT "${ARGC}" STREQUAL "3" )
      message( FATAL_ERROR "createSPIRV: USAGE: <shader name> <shader path> <class path>" )
   endif( NOT "${ARGC}" STREQUAL "3" )

   set( S_NAME "${ARGV0}" )
   set( CLASSNAME "SPIRV_${ARGV0}" )
   set( FILENAME_HPP "${CLASSNAME}.hpp" )
   set( FILENAME_CPP "${CLASSNAME}.cpp" )

   message( STATUS "     - Creating SPIR-V data class (${CLASSNAME}) for ${ARGV0}" )

   set( SHADER_TYPES vert tesc tese geom frag comp )
   foreach( I IN LISTS SHADER_TYPES )
      string( TOUPPER "${I}" I_UPPER )

      set( HAS_${I_UPPER}  "false" )
      set( FINAL_DATA_     ${I_UPPER} "" )
      string( RANDOM LENGTH 10 RAND )
      set( SOURCE_FILE     "${ARGV1}/${ARGV0}.${I}" )
      set( SPIRV_FILE      "${ARGV1}/${ARGV0}.${I}.${RAND}.spirv" )

      if( NOT EXISTS "${SOURCE_FILE}" )
         continue()
      endif( NOT EXISTS "${SOURCE_FILE}" )

      execute_process(
         COMMAND "${GLSL_TO_SPIRV_COMPILER}" -V "${SOURCE_FILE}" -o "${SPIRV_FILE}"
         RESULT_VARIABLE OUT_RES
         OUTPUT_VARIABLE CMD_OUTPUT
         ERROR_VARIABLE  CMD_OUTPUT)

      if( NOT "${OUT_RES}" STREQUAL "0" )
         message( SEND_ERROR "Failed to compile GLSL file ${SOURCE_FILE} (${OUT_RES})" )
         message( SEND_ERROR "${CMD_OUTPUT}" )
         continue()
      endif( NOT "${OUT_RES}" STREQUAL "0" )

      if( NOT EXISTS "${SPIRV_FILE}" )
         message( FATAL_ERROR "Compiled SPIR-V file does not exist" )
      endif( NOT EXISTS "${SPIRV_FILE}" )

      message( STATUS "       - Found shader type ${I}" )

      file( READ "${SPIRV_FILE}" DATA_RAW HEX )
      file( REMOVE "${SPIRV_FILE}" )
      string( LENGTH "${DATA_RAW}" FILE_SIZE )

      set( FINAL_DATA_${I_UPPER} "   " )
      set( J 0 )
      while( ${J} LESS ${FILE_SIZE} )
         string( SUBSTRING "${DATA_RAW}" ${J} 2 TEMP )

         string( APPEND FINAL_DATA_${I_UPPER} "0x${TEMP}," )

         math( EXPR J      "${J} + 2" )
         math( EXPR MOD_4  "${J} % 8" )
         math( EXPR MOD_16 "${J} % 32" )

         if( MOD_16 EQUAL 0 )
            string( APPEND FINAL_DATA_${I_UPPER} "\n   " )
         elseif( MOD_4 EQUAL 0 )
             string( APPEND FINAL_DATA_${I_UPPER} "    " )
         else( MOD_4 EQUAL 0 )
             string( APPEND FINAL_DATA_${I_UPPER} " " )
         endif( MOD_16 EQUAL 0 )
      endwhile( ${J} LESS ${FILE_SIZE} )

      string( REGEX REPLACE ",[ \n]*$" "" FINAL_DATA_${I_UPPER} "${FINAL_DATA_${I_UPPER}}" )
      set( HAS_${I_UPPER} "true" )

      set( LAYOUT_1 "layout[ \t\n]*\\([a-zA-Z_0-9,= \t\n]*location[ \t\n]*=[ \t\n]*[0-9]+[a-zA-Z_0-9,= \t\n]*\\)[ \t\n]*" )
      set( LAYOUT_2 "layout[ \t\n]*\\([a-zA-Z_0-9,= \t\n]*binding[ \t\n]*=[ \t\n]*[0-9]+[a-zA-Z_0-9,= \t\n]*\\)[ \t\n]*" )
      set( LAYOUT_3 "layout[ \t\n]*\\([ \t\n]*push_constant[ \t\n]*\\)[ \t\n]*" )
      set( BLOCK "{([ \t\n]*[a-zA-Z_0-9]+[ \t\n]+[][a-zA-Z_0-9]+[ \t\n]*`[ \t\n]*)*}" )

      # parsing GLSL code
      file( READ "${SOURCE_FILE}" RAW_DATA )
      string( REGEX REPLACE ";" "`" RAW_DATA "${RAW_DATA}" ) # ';' seperates CMake list elements, so we can't use it
      string( REGEX MATCHALL "${LAYOUT_1}in[ \t\n]+[a-zA-Z_0-9]+[ \t\n]+[][a-zA-Z_0-9]+[ \t\n]*`"  S_IN  "${RAW_DATA}" )
      string( REGEX MATCHALL "${LAYOUT_1}out[ \t\n]+[a-zA-Z_0-9]+[ \t\n]+[][a-zA-Z_0-9]+[ \t\n]*`" S_OUT "${RAW_DATA}" )
      string( REGEX MATCHALL "${LAYOUT_2}uniform[ \t\n]+[a-zA-Z_0-9]+[ \t\n]+[][a-zA-Z_0-9]+[ \t\n]*`" S_U1 "${RAW_DATA}" )
      string( REGEX MATCHALL "${LAYOUT_3}uniform[ \t\n]+[a-zA-Z_0-9]+[ \t\n]*${BLOCK}" S_PUSH    "${RAW_DATA}" )
      string( REGEX MATCHALL "${LAYOUT_2}uniform[ \t\n]+[a-zA-Z_0-9]+[ \t\n]*${BLOCK}" S_UNIFORM "${RAW_DATA}" )

      foreach( J IN LISTS S_IN )
         set( ARRAY 1 )
         string( REGEX REPLACE ".*\\)[ \t\n]*in[ \t\n]+([a-zA-Z_0-9]+).*"             "\\1" TP  "${J}" )
         string( REGEX REPLACE ".*in[ \t\n]+[a-zA-Z_0-9]+[ \t\n]+([][a-zA-Z_0-9]+).*" "\\1" NAM "${J}" )
         string( REGEX REPLACE ".*location[ \t\n]*=[ \t\n]*([0-9]+).*"                "\\1" LOC "${J}" )
         string( REGEX REPLACE "[a-zA-Z_0-9]+\\[?([0-9]*)\\]?" "\\1" TMP "${NAM}" )
         string( REGEX REPLACE "\\[[0-9]*\\]" "" NAM "${NAM}" )
         if( NOT "${TMP}" STREQUAL "" )
            set( ARRAY ${TMP} )
         endif( NOT "${TMP}" STREQUAL "" )
         string( APPEND INPUT_${I_UPPER} "            {\"${TP}\", \"${NAM}\", ${LOC}, ${ARRAY}},\n" )
      endforeach( J IN LISTS S_IN )


      foreach( J IN LISTS S_OUT )
         set( ARRAY 1 )
         string( REGEX REPLACE ".*\\)[ \t\n]*out[ \t\n]+([a-zA-Z_0-9]+).*"             "\\1" TP  "${J}" )
         string( REGEX REPLACE ".*out[ \t\n]+[a-zA-Z_0-9]+[ \t\n]+([][a-zA-Z_0-9]+).*" "\\1" NAM "${J}" )
         string( REGEX REPLACE ".*location[ \t\n]*=[ \t\n]*([0-9]+).*"                 "\\1" LOC "${J}" )
         string( REGEX REPLACE "[a-zA-Z_0-9]+\\[?([0-9]*)\\]?" "\\1" TMP "${NAM}" )
         string( REGEX REPLACE "\\[[0-9]*\\]" "" NAM "${NAM}" )
         if( NOT "${TMP}" STREQUAL "" )
            set( ARRAY ${TMP} )
         endif( NOT "${TMP}" STREQUAL "" )
         string( APPEND OUTPUT_${I_UPPER} "            {\"${TP}\", \"${NAM}\", ${LOC}, ${ARRAY}},\n" )
      endforeach( J IN LISTS S_OUT )


      foreach( J IN LISTS S_U1 )
         set( ARRAY 1 )
         string( REGEX REPLACE ".*\\)[ \t\n]*uniform[ \t\n]+([a-zA-Z_0-9]+).*"             "\\1" TP  "${J}" )
         string( REGEX REPLACE ".*uniform[ \t\n]+[a-zA-Z_0-9]+[ \t\n]+([][a-zA-Z_0-9]+).*" "\\1" NAM "${J}" )
         string( REGEX REPLACE ".*binding[ \t\n]*=[ \t\n]*([0-9]+).*"                      "\\1" LOC "${J}" )
         string( REGEX REPLACE "[a-zA-Z_0-9]+\\[?([0-9]*)\\]?" "\\1" TMP "${NAM}" )
         string( REGEX REPLACE "\\[[0-9]*\\]" "" NAM "${NAM}" )
         if( NOT "${TMP}" STREQUAL "" )
            set( ARRAY ${TMP} )
         endif( NOT "${TMP}" STREQUAL "" )
         string( APPEND UNIFORM_${I_UPPER} "            {\"${TP}\", \"${NAM}\", ${LOC}, ${ARRAY}},\n" )
      endforeach( J IN LISTS S_U1 )


      foreach( J IN LISTS S_PUSH )
         string( REGEX MATCH   "{.*}"           TEMP "${J}" )
         string( REGEX REPLACE "[{}]"       ""  TEMP "${TEMP}" )
         string( REGEX REPLACE "`[ \t\n]*$" ""  TEMP "${TEMP}" )
         string( REGEX REPLACE "`"          ";" TEMP "${TEMP}" )

         foreach( K IN LISTS TEMP )
            set( ARRAY 1 )
            string( REGEX REPLACE "[ \t\n]*([a-zA-Z_0-9]+)[ \t\n]+[a-zA-Z_0-9]+.*"   "\\1" TP  "${K}" )
            string( REGEX REPLACE "[ \t\n]*[a-zA-Z_0-9]+[ \t\n]+([][a-zA-Z_0-9]+).*" "\\1" NAM "${K}" )
            string( REGEX REPLACE "[a-zA-Z_0-9]+\\[?([0-9]*)\\]?" "\\1" TMP "${NAM}" )
            string( REGEX REPLACE "\\[[0-9]*\\]" "" NAM "${NAM}" )
            if( NOT "${TMP}" STREQUAL "" )
               set( ARRAY ${TMP} )
            endif( NOT "${TMP}" STREQUAL "" )
            string( APPEND PUSH_${I_UPPER} "            {\"${TP}\", \"${NAM}\", UINT32_MAX, ${ARRAY}},\n" )
         endforeach( K IN LISTS TEMP )

         string( REGEX REPLACE ",\n$" "" PUSH_${I_UPPER} "${PUSH_${I_UPPER}}" )
      endforeach( J IN LISTS S_PUSH )


      foreach( J IN LISTS S_UNIFORM )
         string( REGEX REPLACE ".*\\)[ \t\n]*uniform[ \t\n]+([a-zA-Z_0-9]+).*" "\\1" NAM  "${J}" )
         string( REGEX REPLACE ".*binding[ \t\n]*=[ \t\n]*([0-9]+).*"          "\\1" BIND "${J}" )

         string( APPEND UNIFORM_B_${I_UPPER} "            {\"${NAM}\", ${BIND}, {\n" )

         string( REGEX MATCH   "{.*}"           TEMP "${J}" )
         string( REGEX REPLACE "[{}]"       ""  TEMP "${TEMP}" )
         string( REGEX REPLACE "`[ \t\n]*$" ""  TEMP "${TEMP}" )
         string( REGEX REPLACE "`"          ";" TEMP "${TEMP}" )

         foreach( K IN LISTS TEMP )
            set( ARRAY 1 )
            string( REGEX REPLACE "[ \t\n]*([a-zA-Z_0-9]+)[ \t\n]+[a-zA-Z_0-9]+.*"   "\\1" TP  "${K}" )
            string( REGEX REPLACE "[ \t\n]*[a-zA-Z_0-9]+[ \t\n]+([][a-zA-Z_0-9]+).*" "\\1" NAM "${K}" )
            string( REGEX REPLACE "[a-zA-Z_0-9]+\\[?([0-9]*)\\]?" "\\1" TMP "${NAM}" )
            string( REGEX REPLACE "\\[[0-9]*\\]" "" NAM "${NAM}" )
            if( NOT "${TMP}" STREQUAL "" )
               set( ARRAY ${TMP} )
            endif( NOT "${TMP}" STREQUAL "" )
            string( APPEND UNIFORM_B_${I_UPPER} "               {\"${TP}\", \"${NAM}\", UINT32_MAX, ${ARRAY}},\n" )
         endforeach( K IN LISTS TEMP )

         string( REGEX REPLACE ",\n$" "" UNIFORM_B_${I_UPPER} "${UNIFORM_B_${I_UPPER}}" )
         string( APPEND UNIFORM_B_${I_UPPER} "\n            }},\n" )
      endforeach( J IN LISTS S_UNIFORM )

      string( REGEX REPLACE ",\n$" "" INPUT_${I_UPPER}     "${INPUT_${I_UPPER}}" )
      string( REGEX REPLACE ",\n$" "" OUTPUT_${I_UPPER}    "${OUTPUT_${I_UPPER}}" )
      string( REGEX REPLACE ",\n$" "" UNIFORM_B_${I_UPPER} "${UNIFORM_B_${I_UPPER}}" )
   endforeach( I IN LISTS SHADER_TYPES )

   configure_file( "${TEMPLATES_DIR}/spirv.in.hpp" "${ARGV2}/${FILENAME_HPP}" @ONLY )
   configure_file( "${TEMPLATES_DIR}/spirv.in.cpp" "${ARGV2}/${FILENAME_CPP}" @ONLY )

endfunction( createSPIRV )
