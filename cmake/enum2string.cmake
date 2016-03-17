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

# Usage:
# enum2str_generate
#    PATH           <path to generate files in>
#    CLASS_NAME     <name of the class (file names will be PATH/CLASS_NAME.{hpp,cpp})>
#    FUNC_NAME      <the name of the function>
#    INCLUDES       <files to include (where the enums are)>
#    NAMESPACE      <namespace to use>
#    ENUMS          <list of enums to generate>
#    BLACKLIST      <blacklist for enum constants>
#    USE_CONSTEXPR  <whether to use constexpr or not (default: off)>
#    USE_C_STRINGS  <whether to use c strings instead of std::string or not (default: off)>
function( enum2str_generate )
   set( TARGET_LIST PATH CLASS_NAME FUNC_NAME INCLUDES ENUMS NAMESPACE BLACKLIST USE_CONSTEXPR USE_C_STRINGS )
   math( EXPR ARGC_M1 "${ARGC} - 1" )

   #########################################
   # Split ars into sections (TARGET_LIST) #
   #########################################

   foreach( I RANGE 0 ${ARGC_M1} )
      set( ARG ${ARGV${I}} )
      set( SKIP OFF )
      foreach( J IN LISTS TARGET_LIST )
         if( ARG STREQUAL J )
            set( CURRENT_TARGET ${ARG} )
            set( ${ARG} )
            set( SKIP ON )
            break()
         endif( ARG STREQUAL J )
      endforeach( J IN LISTS TARGET_LIST )

      if( SKIP )
         continue()
      endif( SKIP )

      if( CURRENT_TARGET STREQUAL "" )
         message( WARNING "enum2str_generate: no target set; skip ${ARG}" )
         continue()
      endif( CURRENT_TARGET STREQUAL "" )

      list( APPEND ${CURRENT_TARGET} ${ARG} )
   endforeach( I RANGE 1 ${ARGC_M1} )

   if( USE_C_STRINGS )
      set( STRING_TYPE "const char *" )
   else( USE_C_STRINGS )
      set( STRING_TYPE "std::string " )
   endif( USE_C_STRINGS )

   message( STATUS "Generating enum2str files" )

   __enum2str_checkSet( PATH )
   __enum2str_checkSet( CLASS_NAME )
   __enum2str_checkSet( NAMESPACE )
   __enum2str_checkSet( FUNC_NAME )

   set( HPP_FILE "${PATH}/${CLASS_NAME}.hpp" )
   set( CPP_FILE "${PATH}/${CLASS_NAME}.cpp" )

   enum2str_init()

   #########################
   # Loading include files #
   #########################

   get_property( INC_DIRS DIRECTORY ${CMAKE_HOME_DIRECTORY} PROPERTY INCLUDE_DIRECTORIES )
   message( STATUS "  - Resolving includes:" )

   foreach( I IN LISTS INCLUDES )
      set( FOUND 0 )
      set( TEMP )
      find_path( TEMP NAMES ${I} )
      list( APPEND INC_DIRS ${TEMP} )
      foreach( J IN LISTS INC_DIRS )
         if( EXISTS "${J}/${I}" )
            message( STATUS "    - ${I}: ${J}/${I}" )
            file( READ "${J}/${I}" TEMP )
            string( APPEND RAW_DATA "${TEMP}" )
            set( FOUND 1 )
            break()
         endif( EXISTS "${J}/${I}" )
      endforeach( J IN LISTS INC_DIRS )

      if( NOT "${FOUND}" STREQUAL "1" )
         message( FATAL_ERROR "Unable to find ${I}! (Try running include_directories(...))" )
      endif( NOT "${FOUND}" STREQUAL "1" )
   endforeach( I IN LISTS INCLUDES )

   #####################
   # Finding the enums #
   #####################

   set( CONSTANSTS 0 )

   foreach( I IN LISTS ENUMS )
      set( ENUM_NS "" )
      string( REGEX REPLACE ".*::" "" ENUM_NAME "${I}" )
      if( "${I}" MATCHES "(.*)::[^:]+" )
         string( REGEX REPLACE "(.*)::[^:]+" "\\1::" ENUM_NS "${I}" )
      endif( "${I}" MATCHES "(.*)::[^:]+" )

      string( REGEX MATCH "enum[ \t\n]+${ENUM_NAME}[ \t\n]+(:[^{]+)?{[^}]*}" P1 "${RAW_DATA}" )
      if( "${P1}" STREQUAL "" )
         string( REGEX MATCH "enum[ \t\n]+{[^}]*}[ \t\n]+${ENUM_NAME};" P1 "${RAW_DATA}" )

         if( "${P1}" STREQUAL "" )
            message( WARNING "enum '${I}' not found!" )
            continue()
         endif( "${P1}" STREQUAL "" )
      endif( "${P1}" STREQUAL "" )
      string( REGEX REPLACE "//[^\n]*" "" P1 "${P1}" )
      string( REGEX REPLACE "/\\*[^\\*]*\\*/" "" P1 "${P1}" )
      string( REGEX REPLACE "enum[ \t\n]+${ENUM_NAME}[ \t\n]+(:[^{]+)?" "" P1 "${P1}" )
      string( REGEX REPLACE "enum[ \t\n]{" "" P1 "${P1}" )
      string( REGEX REPLACE "}[ \t\n]*${ENUM_NAME}[ \t\n]*;" "" P1 "${P1}" )
      string( REGEX REPLACE "[ \t\n{};]" "" P1 "${P1}" )
      string( REGEX REPLACE ",$" "" P1 "${P1}" ) # Remove trailing ,
      string( REGEX REPLACE "," ";" L1 "${P1}" ) # Make a List

      set( ENUMS_TO_USE )
      set( RESULTS )

      # Checking enums
      foreach( J IN LISTS L1 )
         set( EQUALS "" )
         if( "${J}" MATCHES ".+=.+" )
            string( REGEX REPLACE ".+=(.+)" "\\1" EQUALS "${J}" )
         endif( "${J}" MATCHES ".+=.+" )
         string( REGEX REPLACE "=.*" "" J "${J}" )

         if( "${J}" IN_LIST BLACKLIST )
            continue()
         endif( "${J}" IN_LIST BLACKLIST )

         if( "${EQUALS}" STREQUAL "" )
            list( APPEND ENUMS_TO_USE "${J}" )
         else( "${EQUALS}" STREQUAL "" )
            # Avoid duplicates:
            if( "${J}" IN_LIST ENUMS_TO_USE )
               continue()
            endif( "${J}" IN_LIST ENUMS_TO_USE )
            if( "${EQUALS}" IN_LIST ENUMS_TO_USE )
               continue()
            endif( "${EQUALS}" IN_LIST ENUMS_TO_USE )
            if( "${EQUALS}" IN_LIST RESULTS )
               continue()
            endif( "${EQUALS}" IN_LIST RESULTS )

            list( APPEND RESULTS "${EQUALS}" )
            list( APPEND ENUMS_TO_USE "${J}" )
         endif( "${EQUALS}" STREQUAL "" )
      endforeach( J IN LISTS L1 )

      enum2str_add( "${I}" )
      list( LENGTH ENUMS_TO_USE NUM_ENUMS )
      math( EXPR CONSTANSTS "${CONSTANSTS} + ${NUM_ENUMS}" )
   endforeach( I IN LISTS ENUMS )

   list( LENGTH ENUMS NUM_ENUMS )
   message( STATUS "  - Generated ${NUM_ENUMS} enum2str functions" )
   message( STATUS "  - Found a total of ${CONSTANSTS} constants" )

   enum2str_end()
   message( "" )
endfunction( enum2str_generate )

macro( __enum2str_checkSet )
   if( NOT DEFINED ${ARGV0} )
      message( FATAL_ERROR "enum2str_generate: ${ARGV0} not set" )
   endif( NOT DEFINED ${ARGV0} )
endmacro( __enum2str_checkSet )

function( enum2str_add )
   if( USE_CONSTEXPR )
      file( APPEND "${HPP_FILE}" "   /*!\n    * \\brief Converts the enum ${ARGV0} to a c string\n" )
      file( APPEND "${HPP_FILE}" "    * \\param _var The enum value to convert\n" )
      file( APPEND "${HPP_FILE}" "    * \\returns _var converted to a c string\n    */\n" )
      file( APPEND "${HPP_FILE}" "   static constexpr const char *${FUNC_NAME}( ${ARGV0} _var ) noexcept {\n" )
      file( APPEND "${HPP_FILE}" "      switch ( _var ) {\n" )

      foreach( I IN LISTS ENUMS_TO_USE )
         file( APPEND "${HPP_FILE}" "         case ${ENUM_NS}${I}: return \"${I}\";\n" )
      endforeach( I IN LISTS ENUMS_TO_USE )

      file( APPEND "${HPP_FILE}" "         default: return \"<UNKNOWN>\";\n" )
      file( APPEND "${HPP_FILE}" "      }\n   }\n\n" )
   else( USE_CONSTEXPR )
      file( APPEND "${HPP_FILE}" "   static ${STRING_TYPE}${FUNC_NAME}( ${ARGV0} _var ) noexcept;\n" )

      file( APPEND "${CPP_FILE}" "/*!\n * \\brief Converts the enum ${ARGV0} to a ${STRING_TYPE}\n" )
      file( APPEND "${CPP_FILE}" " * \\param _var The enum value to convert\n" )
      file( APPEND "${CPP_FILE}" " * \\returns _var converted to a ${STRING_TYPE}\n */\n" )
      file( APPEND "${CPP_FILE}" "${STRING_TYPE}${CLASS_NAME}::${FUNC_NAME}( ${ARGV0} _var ) noexcept {\n" )
      file( APPEND "${CPP_FILE}" "   switch ( _var ) {\n" )

      foreach( I IN LISTS ENUMS_TO_USE )
         file( APPEND "${CPP_FILE}" "      case ${ENUM_NS}${I}: return \"${I}\";\n" )
      endforeach( I IN LISTS ENUMS_TO_USE )

      file( APPEND "${CPP_FILE}" "      default: return \"<UNKNOWN>\";\n" )
      file( APPEND "${CPP_FILE}" "   }\n}\n\n" )
   endif( USE_CONSTEXPR )
endfunction( enum2str_add )


function( enum2str_init )
   string( TOUPPER ${CLASS_NAME} CLASS_NAME_UPPERCASE )

   file( WRITE  "${HPP_FILE}" "/*!\n" )
   file( APPEND "${HPP_FILE}" "  * \\file ${CLASS_NAME}.hpp\n" )
   file( APPEND "${HPP_FILE}" "  * \\warning This is an automatically generated file!\n" )
   file( APPEND "${HPP_FILE}" "  */\n\n" )
   file( APPEND "${HPP_FILE}" "#ifndef ${CLASS_NAME_UPPERCASE}_HPP\n" )
   file( APPEND "${HPP_FILE}" "#define ${CLASS_NAME_UPPERCASE}_HPP\n\n" )
   file( APPEND "${HPP_FILE}" "#include <string>\n" )

   foreach( I IN LISTS INCLUDES )
      file( APPEND "${HPP_FILE}" "#include <${I}>\n" )
   endforeach( I IN LISTS INCLUDES )

   file( APPEND "${HPP_FILE}" "\nnamespace ${NAMESPACE} {\n\n" )
   file( APPEND "${HPP_FILE}" "class ${CLASS_NAME} {\n" )
   file( APPEND "${HPP_FILE}" " public:\n" )

   if( NOT USE_CONSTEXPR )
      file( WRITE  "${CPP_FILE}" "/*!\n" )
      file( APPEND "${CPP_FILE}" "  * \\file ${CLASS_NAME}.cpp\n" )
      file( APPEND "${CPP_FILE}" "  * \\warning This is an automatically generated file!\n" )
      file( APPEND "${CPP_FILE}" "  */\n\n" )
      file( APPEND "${CPP_FILE}" "#include \"${CLASS_NAME}.hpp\"\n\n" )
      file( APPEND "${CPP_FILE}" "namespace ${NAMESPACE} {\n\n" )
   endif( NOT USE_CONSTEXPR )
endfunction( enum2str_init )


function( enum2str_end )
   string( TOUPPER ${CLASS_NAME} CLASS_NAME_UPPERCASE )

   file( APPEND "${HPP_FILE}" "};\n\n}\n\n#endif // ${CLASS_NAME_UPPERCASE}_HPP\n\n" )
   if( NOT USE_CONSTEXPR )
      file( APPEND "${CPP_FILE}" "\n}\n" )
   endif( NOT USE_CONSTEXPR )

endfunction( enum2str_end )
