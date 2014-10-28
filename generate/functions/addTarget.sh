#!/bin/bash


addTarget() {
    local I T DEP CMAKE_REPLACE CMAKE_FILE
    for (( I = 0; I < ${#LIBS[@]}; ++I )); do
	T="${LIBS[$I]}"
	DEP="${LIBS_DEP[$I]}"
	CMAKE_FILE="$T/CMakeLists.txt"
	
	if [ -z "$CMAKE_REPLACE" ]; then
	    CMAKE_REPLACE="include( \${PROJECT_SOURCE_DIR}\/${T}\/$SOURCE_FILE )\nadd_subdirectory( $T )"
	else
	    CMAKE_REPLACE="${CMAKE_REPLACE}\n\ninclude( \${PROJECT_SOURCE_DIR}\/${T}\/$SOURCE_FILE )\nadd_subdirectory( $T )"
	fi
	
	echo "INFO: Adding Target $T [$DEP]"
	finSources $T $(pwd)/$T/$SOURCE_FILE $X11 $WAYLAND $MIR $WINDOWS ${T^^}

	echo "INFO:    -- Generating $CMAKE_FILE"
	cat > $CMAKE_FILE <<EOF

if( NOT DEFINED ENGINE_BUILD_SHARED )
  set( ENGINE_BUILD_SHARED 1 )          # DEFAULT: We create a shared library ( .so or a .dll )
endif( NOT DEFINED ENGINE_BUILD_SHARED )

set( TEMP_DEPS $DEP )
set( TMP_NAME \${PROJECT_NAME}_${T} )

foreach( III \${TEMP_DEPS} )
  set( LIBS_TO_LINK \${LIBS_TO_LINK} \${PROJECT_NAME}_\${III} )
endforeach( III \${ARGV} )

include( GenerateExportHeader )

if( ENGINE_BUILD_SHARED )
  add_library( \${TMP_NAME} SHARED \${\${CURRENT_BASENAME}_SRC} \${\${CURRENT_BASENAME}_INC} ) # Create a shared library
  set( _BUILD_TYPE "shared" )
else( ENGINE_BUILD_SHARED )
  add_library( \${TMP_NAME} STATIC \${\${CURRENT_BASENAME}_SRC} \${\${CURRENT_BASENAME}_INC} ) # Create a static library
  set( _BUILD_TYPE "static" )
endif( ENGINE_BUILD_SHARED )

GENERATE_EXPORT_HEADER( \${TMP_NAME}
           BASE_NAME \${TMP_NAME}
           EXPORT_MACRO_NAME ${T}_EXPORT
           EXPORT_FILE_NAME \${PROJECT_SOURCE_DIR}/${T}/\${TMP_NAME}_Export.hpp
           STATIC_DEFINE ${T}_BUILT_AS_STATIC)
	 
if(CMAKE_CXX_COMPILER_ID MATCHES MSVC) 
  set_target_properties(
    \${TMP_NAME}
    PROPERTIES
      VERSION       \${CM_VERSION_MAJOR}.\${CM_VERSION_MINOR}.\${CM_VERSION_SUBMINOR}
      SOVERSION     \${CM_VERSION_MAJOR} 
    LINK_FLAGS	  "/LIBPATH:\${Boost_LIBRARY_DIRS} /FORCE:MULTIPLE"
  )
else() 
# Set some variables so that Cmake can do some fancy stuff with versions and filenames
  set_target_properties(
    \${TMP_NAME}
      PROPERTIES
        VERSION       \${CM_VERSION_MAJOR}.\${CM_VERSION_MINOR}.\${CM_VERSION_SUBMINOR}
        SOVERSION     \${CM_VERSION_MAJOR}
  )
endif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)
 
# Windows also needs some linking...
if( WIN32 )
  target_link_libraries(\${TMP_NAME} \${ENGINE_LINK} \${LIBS_TO_LINK} )
endif( WIN32 )

install( FILES \${\${CURRENT_BASENAME}_INC} DESTINATION \${CMAKE_INSTALL_PREFIX}/include/engine)

install( 
  TARGETS  \${TMP_NAME}
  RUNTIME  DESTINATION \${CMAKE_INSTALL_PREFIX}/bin/
  LIBRARY  DESTINATION \${CMAKE_INSTALL_PREFIX}/lib/ 
  ARCHIVE  DESTINATION \${CMAKE_INSTALL_PREFIX}/lib/
)

if( "\${LIBS_TO_LINK}" STREQUAL "" )
  set( LIBS_TO_LINK "NOTHING" )
endif( "\${LIBS_TO_LINK}" STREQUAL "" )

message( STATUS "Added \${_BUILD_TYPE} Library \${TMP_NAME} (Depends on: \${LIBS_TO_LINK})" )
set( ENGINE_LIBS \${ENGINE_LIBS} \${TMP_NAME} PARENT_SCOPE )

EOF
    done

    echo "INFO: Generating CMakeLists.txt"
    cat $CMAKE_LISTS_IN | sed "s/SED_REPLACE_ME_WITH_SUBDIRS/$CMAKE_REPLACE/g" > $PWD/CMakeLists.txt
    
}



