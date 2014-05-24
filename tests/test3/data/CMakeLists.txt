# Automatically generated file; DO NOT EDIT

cmake_minimum_required(VERSION 2.8.7 )

set( CSD ${CMAKE_HOME_DIRECTORY}/tests/test1/data/ )

if( EXISTS ${CSD}/custom.cmake )
   include( ${CSD}/custom.cmake )
endif( EXISTS ${CSD}/custom.cmake )

if( EXISTS ${CSD}/config.in.hpp )
   configure_file("${CSD}/config.in.hpp" "${CSD}/config.hpp")
endif( EXISTS ${CSD}/config.in.hpp )

include( sources.cmake )

add_executable( test1 ${TEST1_SRC} )

target_link_libraries(
 test1

 ${PROJECT_NAME}
 ${ENGINE_LINK}
)

set_target_properties(
 test1
 PROPERTIES
  INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
  INSTALL_RPATH_USE_LINK_PATH TRUE
)


install( TARGETS engine_test1 RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )

if( EXISTS ${CSD}/data AND IS_DIRECTORY ${CSD}/data )
   install( DIRECTORY ${CSD}/data DESTINATION ${CMAKE_INSTALL_PREFIX}/share/engineTests/test1 )
endif( EXISTS ${CSD}/data AND IS_DIRECTORY ${CSD}/data )

