cmake_minimum_required(VERSION 2.8.9)

project(engine)

# START uninstall
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
# STOP uninstall

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set( CMAKE_BUILD_TYPE DEBUG )

include( engine_dependences )

# Output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

set(CM_VERSION_MAJOR  0)
set(CM_VERSION_MINOR  1)
set(CM_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})

configure_file("${PROJECT_SOURCE_DIR}/src/engine.in.hpp"
               "${PROJECT_SOURCE_DIR}/src/engine.hpp")

# Optionen
add_definitions(-Wall -fPIC -std=c++11 -O0) # -Winline -std=c++0x -O3
include_directories( ${ENGINE_INCL} )
link_directories( ${PROJECT_BINARY_DIR}/lib )

# ENGINE_SRC and ENGINE_INC
include(sources.cmake)

#add_library(myengine STATIC ${ENGINE_SRC})
add_library(myengine SHARED ${ENGINE_SRC})

#target_link_libraries(myengine ${ENGINE_LINK} )

set(DEPENDENCES ${PROJECT_SOURCE_DIR}/cmake/engine_dependences.cmake
                ${PROJECT_SOURCE_DIR}/cmake/FindGMP.cmake)

install( FILES ${ENGINE_INC}      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/engine)
install( FILES ${DEPENDENCES}     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/engine)
#install( TARGETS myengine ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
install( TARGETS myengine LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/)
