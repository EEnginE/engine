function( newLibrary )
  if( ${ARGC} LESS 1 )
    message( FATAL_ERROR "newLibrary needs at least one argument!" )
  endif( ${ARGC} LESS 1 )

  if( NOT EXISTS ${PROJECT_SOURCE_DIR}/${ARGV0}.cmake )
    message( FATAL_ERROR "Can not find ${PROJECT_SOURCE_DIR}/${ARGV0}.cmake! Please run ${PROJECT_SOURCE_DIR}/generate.sh!" )
  endif( NOT EXISTS ${PROJECT_SOURCE_DIR}/${ARGV0}.cmake )

  if( NOT DEFINED ENGINE_BUILD_SHARED )
    set( ENGINE_BUILD_SHARED 1 )          # DEFAULT: We create a shared library ( .so or a .dll )
  endif( NOT DEFINED ENGINE_BUILD_SHARED )

  foreach( III ${ARGV} )
    if( NOT "${III}" STREQUAL "${ARGV0}" )
      set( LIBS_TO_LINK ${LIBS_TO_LINK} ${PROJECT_NAME}_${III} )
    endif( NOT "${III}" STREQUAL "${ARGV0}" )
  endforeach( III ${ARGV} )

  include( ${PROJECT_SOURCE_DIR}/${ARGV0}.cmake )
  include (GenerateExportHeader)

  if( ENGINE_BUILD_SHARED )
    add_library( ${PROJECT_NAME}_${ARGV0} SHARED ${${CURRENT_BASENAME}_SRC} ${${CURRENT_BASENAME}_INC} ) # Create a shared library
	set( _BUILD_TYPE "shared" )
  else( ENGINE_BUILD_SHARED )
    add_library( ${PROJECT_NAME}_${ARGV0} STATIC ${${CURRENT_BASENAME}_SRC} ${${CURRENT_BASENAME}_INC} ) # Create a static library
    set( _BUILD_TYPE "static" )
  endif( ENGINE_BUILD_SHARED )

  GENERATE_EXPORT_HEADER( ${PROJECT_NAME}_${ARGV0}
             BASE_NAME ${PROJECT_NAME}_${ARGV0}
             EXPORT_MACRO_NAME ${ARGV0}_EXPORT
             EXPORT_FILE_NAME ${PROJECT_SOURCE_DIR}/${ARGV0}/${PROJECT_NAME}_${ARGV0}_Export.hpp
             STATIC_DEFINE ${ARGV0}_BUILT_AS_STATIC)
			 
  if(CMAKE_CXX_COMPILER_ID MATCHES MSVC) 
  set_target_properties(
   ${PROJECT_NAME}_${ARGV0}
   PROPERTIES
    VERSION       ${CM_VERSION_MAJOR}.${CM_VERSION_MINOR}.${CM_VERSION_SUBMINOR}
    SOVERSION     ${CM_VERSION_MAJOR} 
   LINK_FLAGS	  "/LIBPATH:${Boost_LIBRARY_DIRS} /FORCE:MULTIPLE"
  )
  else() 
  # Set some variables so that Cmake can do some fancy stuff with versions and filenames
  set_target_properties(
   ${PROJECT_NAME}_${ARGV0}
   PROPERTIES
    VERSION       ${CM_VERSION_MAJOR}.${CM_VERSION_MINOR}.${CM_VERSION_SUBMINOR}
    SOVERSION     ${CM_VERSION_MAJOR}
  )
  endif(CMAKE_CXX_COMPILER_ID MATCHES MSVC)

  
  # Windows also needs some linking...
  if( WIN32 )
    target_link_libraries(${PROJECT_NAME}_${ARGV0} ${ENGINE_LINK} ${LIBS_TO_LINK} )
  endif( WIN32 )

  install( FILES ${${CURRENT_BASENAME}_INC} DESTINATION ${CMAKE_INSTALL_PREFIX}/include/engine)

  install( 
   TARGETS  ${PROJECT_NAME}_${ARGV0}
   RUNTIME  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
   LIBRARY  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ 
   ARCHIVE  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
  )

  if( "${LIBS_TO_LINK}" STREQUAL "" )
    set( LIBS_TO_LINK "NOTHING" )
  endif( "${LIBS_TO_LINK}" STREQUAL "" )

  message( STATUS "Added ${_BUILD_TYPE} Library ${PROJECT_NAME}_${ARGV0} (Depends on: ${LIBS_TO_LINK})" )
  set( ENGINE_LIBS      ${ENGINE_LIBS}     ${PROJECT_NAME}_${ARGV0}  PARENT_SCOPE )
  set( ENGINE_LIBS_STR "${ENGINE_LIBS_STR} ${PROJECT_NAME}_${ARGV0}" PARENT_SCOPE )

endfunction( newLibrary )