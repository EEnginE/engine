#!/usr/bin/env bash

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

addTarget() {
    local I T DEP CMAKE_FILE

    msg1 "Looking for libs..."

    for (( I = 0; I < ${#LIBS[@]}; ++I )); do
        T="${LIBS[$I]}"
        DEP="${LIBS_DEP[$I]}"
        CMAKE_FILE="$T/${CMAKE_LISTS_NAME}"

        found "lib \x1b[33m$T \x1b[35m[\x1b[36m$DEP\x1b[35m]"

        cat > "$CMAKE_FILE" <<EOF
# Automatically generated file; DO NOT EDIT

project( ${T,,} )
include( GenerateExportHeader )
set( ${T^^}_EXPORT_FILE \${CMAKE_CURRENT_SOURCE_DIR}/${T,,}${EXPORT_FILE_EXT} )
set( CMAKE_POSITION_INDEPENDENT_CODE ON )

EOF

        finSources "$T" "${T^^}" 1>> "$CMAKE_FILE"
        local TMP_NAME="${PROJECT_NAME}_${T}"

        local LINK_LIBS_SHARED="" LINK_LIBS_STATIC="" LINK_LIBS_NAME=""
        for i in $DEP; do
            LINK_LIBS_SHARED="$LINK_LIBS_SHARED ${PROJECT_NAME}_${i}_shared"
            LINK_LIBS_STATIC="$LINK_LIBS_STATIC ${PROJECT_NAME}_${i}_static"
            LINK_LIBS_NAME="${LINK_LIBS_NAME} ${i}"
        done
        [ -z "$LINK_LIBS_NAME" ] && LINK_LIBS_NAME=" -"

        cat >> "$CMAKE_FILE" <<EOF

set( ${T^^}_INC \${${T^^}_INC} \${${T^^}_EXPORT_FILE} )

add_library( ${TMP_NAME}_obj    OBJECT \${${T^^}_SRC} \${${T^^}_INC} )
add_library( ${TMP_NAME}_shared SHARED \$<TARGET_OBJECTS:${TMP_NAME}_obj> )
add_library( ${TMP_NAME}_static STATIC \$<TARGET_OBJECTS:${TMP_NAME}_obj> )

target_link_libraries( ${TMP_NAME}_shared \${ENGINE_LINK}${LINK_LIBS_SHARED} )
target_link_libraries( ${TMP_NAME}_static \${ENGINE_LINK}${LINK_LIBS_STATIC} )

generate_export_header(
  ${TMP_NAME}_shared
  BASE_NAME            ${T^^}
  EXPORT_MACRO_NAME    ${T^^}_API
  NO_EXPORT_MACRO_NAME ${T^^}_PRIVATE
  STATIC_DEFINE        ${TMP_NAME^^}_STATIC_DEFINE
  EXPORT_FILE_NAME     \${${T^^}_EXPORT_FILE}
)

set_target_properties(
  ${TMP_NAME}_shared ${TMP_NAME}_static
  PROPERTIES
    VERSION     \${CM_VERSION_MAJOR}.\${CM_VERSION_MINOR}.\${CM_VERSION_SUBMINOR}
    SOVERSION   \${CM_VERSION_MAJOR}
  PUBLIC_HEADER "\${${T^^}_INC}"
  OUTPUT_NAME   ${TMP_NAME}
)

install(
  TARGETS       ${TMP_NAME}_shared ${TMP_NAME}_static
  RUNTIME       DESTINATION bin
  LIBRARY       DESTINATION lib
  ARCHIVE       DESTINATION lib
  PUBLIC_HEADER DESTINATION include/engine
)

message( STATUS "Added Library ${TMP_NAME}\t (Depends on:${LINK_LIBS_NAME})" )

EOF
    done

}

# kate: indent-mode shell; indent-width 4; replace-tabs on; line-numbers on;
