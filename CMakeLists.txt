cmake_minimum_required(VERSION 2.8.7)

project(engine)


# START uninstall
configure_file("${PROJECT_SOURCE_DIR}/generate/cmake/cmake_uninstall.cmake.in"
               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
               IMMEDIATE @ONLY)

add_custom_target(uninstall COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
# STOP uninstall

set( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/generate/cmake )


# Set the output directory of the libraries and executables after the build process
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)

include( ${PROJECT_SOURCE_DIR}/generate/cmake/testSources.cmake )   # Automatically testing if we need to run generate.sh
testSources()

include( ${PROJECT_SOURCE_DIR}/generate/cmake/displayServer.cmake ) # Choose the low level graphic API
include( ${PROJECT_SOURCE_DIR}/generate/cmake/git.cmake )           # Get the project version
include( ${PROJECT_SOURCE_DIR}/generate/cmake/find.cmake )          # Find our dependences
include( ${PROJECT_SOURCE_DIR}/generate/cmake/compiler.cmake )      # Compiler Options
include( ${PROJECT_SOURCE_DIR}/generate/cmake/newLib.cmake )        # Get access to function "newLib"

include( ${PROJECT_SOURCE_DIR}/installTools.cmake )                  # Install all files in the 'tools' dir

# Only set CMAKE_BUILD_TYPE when not defined
if( NOT DEFINED CMAKE_BUILD_TYPE )
   set( CMAKE_BUILD_TYPE DEBUG )
   
   # Is this tagged ==> Release?
   if( "${CM_COMMIT_IS_TAGGED}" MATCHES "1" )
      set( CMAKE_BUILD_TYPE RELEASE )
   endif( "${CM_COMMIT_IS_TAGGED}" MATCHES "1" )
endif( NOT DEFINED CMAKE_BUILD_TYPE )

set( DEBUG_LOGGING 0 )

if( ENGINE_VERBOSE OR "${CMAKE_BUILD_TYPE}" MATCHES "DEBUG" )
   set( DEBUG_LOGGING 1 )
endif( ENGINE_VERBOSE OR "${CMAKE_BUILD_TYPE}" MATCHES "DEBUG" )


# Generate defines.hpp
configure_file(
 "${PROJECT_SOURCE_DIR}/defines.in.hpp"
 "${PROJECT_SOURCE_DIR}/defines.hpp"
)

# Generate Doxyfile
configure_file(
 "${PROJECT_SOURCE_DIR}/Doxyfile.in"
 "${PROJECT_SOURCE_DIR}/Doxyfile"
)


########################################################################################
############################################################################################################################
#####                     ###########################################################################################################
###  Now build and install  ############################################################################################################
#####                     ###########################################################################################################
############################################################################################################################
########################################################################################

# Include these directories by default (so you can use #include<lib.hpp> without path)
include_directories(
 ${Boost_INCLUDE_DIRS}
 ${GLEW_INCLUDE_DIRS}
 ${OPENGL_INCLUDE_DIR}
 ${X11_INCLUDE_DIRS}
 ${X11_Xrandr_INCLUDE_PATH}
 ${PROJECT_SOURCE_DIR}
)

set( ENGINE_LIBS ) # make ENGINE_LIBS global

newLibrary( utils )
newLibrary( init   utils )
newLibrary( render utils init )

# Generate FindEngine.cmake
configure_file(
 "${PROJECT_SOURCE_DIR}/generate/cmake/FindEngine.cmake.in"
 "${PROJECT_SOURCE_DIR}/generate/cmake/FindEngine.cmake"
 @ONLY
)


# Now build the tests
add_subdirectory(${PROJECT_SOURCE_DIR}/tests/)

# Also install some usefull cmakefiles
set(DEPENDENCES ${PROJECT_SOURCE_DIR}/generate/cmake/FindEngine.cmake
                ${PROJECT_SOURCE_DIR}/generate/cmake/FindGLEW2.cmake)
                
set(OTHER_INC ${PROJECT_SOURCE_DIR}/engine.hpp ${PROJECT_SOURCE_DIR}/defines.hpp )

# The install configuaration
install( FILES ${DEPENDENCES} DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/engine)
install( FILES ${OTHER_INC}   DESTINATION ${CMAKE_INSTALL_PREFIX}/include/engine)
install( 
 TARGETS  ${ENGINE_LIBS}
 RUNTIME  DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
 LIBRARY  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ 
 ARCHIVE  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
)




# Print some stuff
if( ENGINE_VERBOSE )
   message( STATUS "" )
   message( STATUS "" )
   message( STATUS "" )
   message( STATUS "    LIBRARIES" )
   message( STATUS "    =========" )
   message( STATUS "" )
   message( STATUS "GLEW libraries:     ${GLEW_LIBRARIES}" )
   message( STATUS "OpenGL libraries:   ${OPENGL_LIBRARIES}" )
   message( STATUS "X11 libraries:      ${X11_LIBRARIES}" )
   message( STATUS "RandR (X11):        ${X11_Xrandr_LIB}" )
   message( STATUS "Threads:            ${CMAKE_THREAD_LIBS_INIT}" )
   message( STATUS "Boost:              ${Boost_LIBRARIES}" )
   message( STATUS "Boost include:      ${Boost_INCLUDE_DIRS}" )
   message( STATUS "" )
   message( STATUS "" )
   message( STATUS "      ETC" )
   message( STATUS "      ===" )
   message( STATUS "" )
   message( STATUS "GIT commit:         ${CM_VERSION_GIT}" )
   message( STATUS "" )
   message( STATUS "Compiler (ID):      ${CMAKE_CXX_COMPILER_ID}" )
   message( STATUS "Compiler (Exec):    ${CMAKE_CXX_COMPILER}" )
   message( STATUS "Compiler (Version): ${ENGINE_COMPILER_VERSION}" )
   message( STATUS "Options:" )
   message( STATUS "  - DEBUG:          ${CMAKE_CXX_FLAGS_DEBUG}" )
   message( STATUS "  - RELEASE:        ${CMAKE_CXX_FLAGS_RELEASE}" )
   message( STATUS "" )
   message( STATUS "Building:           ${ENGINE_LIBS_STR}" )
   message( STATUS "" )
   message( STATUS "" )
endif( ENGINE_VERBOSE )
