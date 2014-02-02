cmake_minimum_required(VERSION 2.8.9)

project(engine)


set(CM_VERSION_MAJOR  0)
set(CM_VERSION_MINOR  1)


# START uninstall
configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
# STOP uninstall

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake)
set( CMAKE_BUILD_TYPE DEBUG )

# Output
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

configure_file(
 "${PROJECT_SOURCE_DIR}/src/engine.in.hpp"
 "${PROJECT_SOURCE_DIR}/src/engine.hpp"
)


set(CM_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})


set(Boost_USE_MULTITHREADED ON)
if( NOT DEFINED BOOST_NO_STATIC )
   set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost   REQUIRED COMPONENTS system regex thread chrono filesystem date_time )
find_package(GLEW2   REQUIRED) # The orginal FindGLEW.cmake has no options to set a Root path
#find_package(PNG     REQUIRED)
find_package(OpenGL  REQUIRED)
find_package(Threads REQUIRED)
find_package(GMP     REQUIRED)

if( UNIX )
   find_package(X11  REQUIRED)
endif( UNIX )

if( ENGINE_DEBUG )
   message( STATUS "GLEW libraries:   ${GLEW_LIBRARIES}" )
   message( STATUS "libpng libraries: ${PNG_LIBRARIES}" )
   message( STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}" )
   message( STATUS "X11 libraries:    ${X11_LIBRARIES}" )
   message( STATUS "ZLIB libraries:   ${ZLIB_LIBRARIES}" )
   message( STATUS "Pthread:          ${PTHREAD_LIBRARIES}" )
   message( STATUS "Boost:            ${Boost_LIBRARIES}")
   message( STATUS "BOOST:            ${Boost_INCLUDE_DIRS}")
endif()

set(ENGINE_LINK 
   ${Boost_LIBRARIES}
   ${GLEW_LIBRARIES}
   ${PNG_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${PTHREAD_LIBRARIES}
   ${X11_LIBRARIES}
   ${X11_Xrandr_LIB}
   ${ZLIB_LIBRARIES}
   ${GMP_LIBRARIES}
   ${CMAKE_THREAD_LIBS_INIT}
)


# Optionen
add_definitions(-Wall -fPIC -std=c++11 -O0) # -Winline -std=c++0x -O3
include_directories(
 ${Boost_INCLUDE_DIRS}
 ${GLEW_INCLUDE_DIRS}
 ${PNG_INCLUDE_DIRS}
 ${OPENGL_INCLUDE_DIR}
 ${PTHREAD_INCLUDE_DIRS}
 ${X11_INCLUDE_DIRS}
 ${X11_Xrandr_INCLUDE_PATH}
 ${ZLIB_INCLUDE_DIRS}
 ${GMP_INCLUDE_DIR}
)

# ENGINE_SRC and ENGINE_INC
include(sources.cmake)

#add_library( ${PROJECT_NAME} STATIC ${ENGINE_SRC} )
add_library( ${PROJECT_NAME} SHARED ${ENGINE_SRC} )

set_target_properties(
 ${PROJECT_NAME}
 PROPERTIES
  VERSION   ${CM_VERSION_MAJOR}.${CM_VERSION_MINOR}
  SOVERSION ${CM_VERSION_MAJOR}
)


add_subdirectory(${PROJECT_SOURCE_DIR}/tests/)


set(DEPENDENCES ${PROJECT_SOURCE_DIR}/cmake/FindEngine.cmake
                ${PROJECT_SOURCE_DIR}/cmake/FindGMP.cmake)

install( FILES ${ENGINE_INC}      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/engine)
install( FILES ${DEPENDENCES}     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/engine)
install( 
 TARGETS ${PROJECT_NAME} 
 LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ 
 ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
)
