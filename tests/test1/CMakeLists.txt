cmake_minimum_required(VERSION 2.8)

set( CSD ${PROJECT_SOURCE_DIR}/tests/test1 )

configure_file("${CSD}/config.in.h" "${CSD}/config.h")


# Optionen
add_definitions(-Wall -std=c++0x -O2)
add_executable( engine_test1 ${CSD}/main.cpp )

target_link_libraries(
 engine_test1

 ${PROJECT_NAME}
 ${ENGINE_LINK}
)

set_target_properties(
 engine_test1
 PROPERTIES
  INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib
  INSTALL_RPATH_USE_LINK_PATH TRUE
)


install( TARGETS engine_test1 RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin )
install( 
 FILES 
  ${CSD}/shaders/colors_p.frag 
  ${CSD}/shaders/colors_p.vert 
 DESTINATION ${CMAKE_INSTALL_PREFIX}/share/engineTests/test1
)