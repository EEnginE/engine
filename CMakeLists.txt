cmake_minimum_required(VERSION 2.8.7)

project(engine)

# Set here the Version of this project
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

# Set the output directory of the libraries and executables after the build process
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)


# Generate engine.hpp from the engine.in.hpp (which was generated from gen_sources.sh)
configure_file(
 "${PROJECT_SOURCE_DIR}/src/engine.in.hpp"
 "${PROJECT_SOURCE_DIR}/src/engine.hpp"
)


########################################################################################
############################################################################################################################
#####                                ################################################################################################
###  Choose the low level graphic API  #################################################################################################
#####                                ################################################################################################
############################################################################################################################
########################################################################################

if( NOT DEFINED DISPLAY_SERVER )
   if( UNIX )
      set( DISPLAY_SERVER UNIX_X11 ) # The default display server on UNIX/LINUX is X11
   elseif( WIN32 )
      set( DISPLAY_SERVER WINDOWS )  # The default display server on Windows is the Win API
   endif( UNIX )
endif( NOT DEFINED DISPLAY_SERVER )

# Only the displayervers X11, Wayland and Mir are currently supported in UNIX/LINUX
if( UNIX )
   if( NOT DISPLAY_SERVER MATCHES UNIX_X11 AND NOT DISPLAY_SERVER MATCHES UNIX_WAYLAND AND NOT DISPLAY_SERVER MATCHES UNIX_MIR )
       message( FATAL_ERROR "Display server '${DISPLAY_SERVER}' is not supported in UNIX (possible: UNIX_X11, UNIX_WAYLAND, UNIX_MIR)" )
   endif( NOT DISPLAY_SERVER MATCHES UNIX_X11 AND NOT DISPLAY_SERVER MATCHES UNIX_WAYLAND AND NOT DISPLAY_SERVER MATCHES UNIX_MIR )
endif( UNIX )

# Windows only supports Windows
if( WIN32 )
   if( NOT DISPLAY_SERVER MATCHES WINDOWS )
       message( FATAL_ERROR "Display server '${DISPLAY_SERVER}' is not supported in Windwes (possible: WINDOWS)" )
   endif( NOT DISPLAY_SERVER MATCHES WINDOWS )
endif( WIN32 )

# These vars will be parsed in the defines.in.hpp by cmake
set( CM_UNIX_X11     0 )
set( CM_UNIX_WAYLAND 0 )
set( CM_UNIX_MIR     0 )
set( CM_WINDOWS      0 )

# We currently support only Windows and Linux
if( DISPLAY_SERVER MATCHES UNIX_X11 )
   set( CM_UNIX_X11 1 )
elseif( DISPLAY_SERVER MATCHES WINDOWS )
   set( CM_WINDOWS  1 )
else( DISPLAY_SERVER MATCHES UNIX_X11 )
   message( FATAL_ERROR "${DISPLAY_SERVER} is currently not implemented. " )
endif( DISPLAY_SERVER MATCHES UNIX_X11 )

# Generate defines.hpp
configure_file(
 "${PROJECT_SOURCE_DIR}/src/defines.in.hpp"
 "${PROJECT_SOURCE_DIR}/src/defines.hpp"
)


########################################################################################
############################################################################################################################
#####                    ############################################################################################################
###  Find our dependences  #############################################################################################################
#####                    ############################################################################################################
############################################################################################################################
########################################################################################

if( NOT DEFINED ENGINE_LINK_SHARED )
   set( ENGINE_LINK_SHARED 1 )   # DEFAULT: link shared (.so / .dll)
endif( NOT DEFINED ENGINE_LINK_SHARED )

# Begin STATIC ###
if( ENGINE_LINK_SHARED )
   message( STATUS "Do not change CMAKE_FIND_LIBRARY_SUFFIXES ( ${CMAKE_FIND_LIBRARY_SUFFIXES} )" )
else( ENGINE_LINK_SHARED )
   set( OLD_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   if( WIN32 )
      set( CMAKE_FIND_LIBRARY_SUFFIXES .a .lib .dll ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   else( WIN32 )
      set( CMAKE_FIND_LIBRARY_SUFFIXES .a .so ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   endif( WIN32 )
   message( STATUS "Changed CMAKE_FIND_LIBRARY_SUFFIXES for static linking ( ${CMAKE_FIND_LIBRARY_SUFFIXES} )" )
endif( ENGINE_LINK_SHARED )
# End STATIC ###


set(Boost_USE_MULTITHREADED ON)
if( NOT ENGINE_LINK_SHARED )
   set(Boost_USE_STATIC_LIBS ON)
endif( NOT ENGINE_LINK_SHARED )

find_package(GLEW2   REQUIRED) # The orginal FindGLEW.cmake has no options to set a Root path
find_package(OpenGL  REQUIRED)
find_package(Threads REQUIRED)

if( UNIX )
   find_package(X11   REQUIRED)
   find_package(Boost 1.48.0 REQUIRED COMPONENTS system regex thread chrono filesystem )
elseif( WIN32 )
   find_package(Boost 1.48.0 REQUIRED COMPONENTS system regex thread_win32 chrono filesystem )
endif( UNIX )

if( ENGINE_DEBUG )
   message( STATUS "GLEW libraries:   ${GLEW_LIBRARIES}" )
   message( STATUS "OpenGL libraries: ${OPENGL_LIBRARIES}" )
   message( STATUS "X11 libraries:    ${X11_LIBRARIES}" )
   message( STATUS "Pthread:          ${PTHREAD_LIBRARIES}" )
   message( STATUS "Boost:            ${Boost_LIBRARIES}")
   message( STATUS "BOOST:            ${Boost_INCLUDE_DIRS}")
endif()

set(ENGINE_LINK 
   ${Boost_LIBRARIES}
   ${GLEW_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${X11_LIBRARIES}
   ${X11_Xrandr_LIB}
   ${CMAKE_THREAD_LIBS_INIT}
)

if( NOT ENGINE_LINK_SHARED )
   set( DEF_GLEW_STATIC_GNU "-DGLEW_STATIC" )
endif( NOT ENGINE_LINK_SHARED )

if( WIN32 )
   if( ENGINE_BUILD_SHARED )
      set( WIN_SHARED_OPTIONS -shared -Wl,--output-def,testdll.def,--out-implib,libtestdll.a )
   endif( ENGINE_BUILD_SHARED )
endif( WIN32 )


########################################################################################
############################################################################################################################
#####                ################################################################################################################
###  Compiler Options  #################################################################################################################
#####                ################################################################################################################
############################################################################################################################
########################################################################################

# Options GCC

# CMAKE_CXX_COMPILER_VERSION doesent work always defined, so this is needed
if( CMAKE_CXX_COMPILER_ID MATCHES GNU )
   if( NOT DEFINED CMAKE_CXX_COMPILER_VERSION )
      execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_COMPILER_VERSION)
   else( NOT DEFINED CMAKE_CXX_COMPILER_VERSION )
      set( GCC_COMPILER_VERSION ${CMAKE_CXX_COMPILER_VERSION} )
   endif( NOT DEFINED CMAKE_CXX_COMPILER_VERSION )
endif( CMAKE_CXX_COMPILER_ID MATCHES GNU )

set( GCC_ENABLE_CXX_11 "-std=c++11" )
if( CMAKE_CXX_COMPILER_ID MATCHES GNU AND GCC_COMPILER_VERSION LESS 4.7 )
   if( GCC_COMPILER_VERSION LESS 4.5 )
      message( FATAL_ERROR "Your GCC compiler version is to old! You need at least GCC v4.5" )
   endif( GCC_COMPILER_VERSION LESS 4.5 )
   set( GCC_ENABLE_CXX_11 "-std=c++0x" )
endif( CMAKE_CXX_COMPILER_ID MATCHES GNU AND GCC_COMPILER_VERSION LESS 4.7 )

set( GNU_COMPILER_OPTIONS_DEBUG   "-Wall ${GCC_ENABLE_CXX_11} ${DEF_GLEW_STATIC_GNU}" )
set( GNU_COMPILER_OPTIONS_RELEASE "-Wall ${GCC_ENABLE_CXX_11} ${DEF_GLEW_STATIC_GNU} -flto -O3" )

if( DEFINED EXTRA_WARNINGS )
   set( GNU_COMPILER_OPTIONS_DEBUG   "-Winline -Werror ${GNU_COMPILER_OPTIONS_DEBUG}" )
   set( GNU_COMPILER_OPTIONS_RELEASE "-Winline -Werror ${GNU_COMPILER_OPTIONS_RELEASE}" )
endif( DEFINED EXTRA_WARNINGS )


# Options LLVM
set( CLANG_ENABLE_CXX_11 "-std=c++11")

set( CLANG_COMPILER_OPTIONS_DEBUG   "-v ${CLANG_ENABLE_CXX_11} ${DEF_GLEW_STATIC_GNU}" )
set( CLANG_COMPILER_OPTIONS_RELEASE "${CLANG_ENABLE_CXX_11} ${DEF_GLEW_STATIC_GNU}" )

# TODO
#if( DEFINED EXTRA_WARNINGS )
#   set( CLANG_COMPILER_OPTIONS_DEBUG   "<insert here> ${CLANG_COMPILER_OPTIONS_DEBUG}" )
#   set( CLANG_COMPILER_OPTIONS_RELEASE "<insert here> ${CLANG_COMPILER_OPTIONS_RELEASE}" )
#endif( DEFINED EXTRA_WARNINGS )


# Options ICC  - TODO

set( INTEL_COMPILER_OPTIONS_DEBUG   "<insert here> ${DEF_GLEW_STATIC_GNU}" )
set( INTEL_COMPILER_OPTIONS_RELEASE "<insert here> ${DEF_GLEW_STATIC_GNU}" )

if( DEFINED EXTRA_WARNINGS )
   set( INTEL_COMPILER_OPTIONS_DEBUG   "<insert here> ${INTEL_COMPILER_OPTIONS_DEBUG}" )
   set( INTEL_COMPILER_OPTIONS_RELEASE "<insert here> ${INTEL_COMPILER_OPTIONS_RELEASE}" )
endif( DEFINED EXTRA_WARNINGS )


# Options MSVC - TODO

set( MSVC_COMPILER_OPTIONS_DEBUG   "<insert here> ${DEF_GLEW_STATIC_GNU}" )
set( MSVC_COMPILER_OPTIONS_RELEASE "<insert here> ${DEF_GLEW_STATIC_GNU}" )

if( DEFINED EXTRA_WARNINGS )
   set( MSVC_COMPILER_OPTIONS_DEBUG   "<insert here> ${MSVC_COMPILER_OPTIONS_DEBUG}" )
   set( MSVC_COMPILER_OPTIONS_RELEASE "<insert here> ${MSVC_COMPILER_OPTIONS_RELEASE}" )
endif( DEFINED EXTRA_WARNINGS )



# Detect Compiler

if( CMAKE_CXX_COMPILER_ID MATCHES GNU )
   set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   ${GNU_COMPILER_OPTIONS_DEBUG}" )
   set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${GNU_COMPILER_OPTIONS_RELEASE}" )

elseif( CMAKE_CXX_COMPILER_ID MATCHES Clang )
   set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   ${CLANG_COMPILER_OPTIONS_DEBUG}" )   # Uncomment this when done with LLVM/Clang
   set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${CLANG_COMPILER_OPTIONS_RELEASE}" ) # Uncomment this when done with LLVM/Clang
   message( WARNING "Extra debug warnings for ${CMAKE_CXX_COMPILER_ID} not (yet) supported" )    # Remove this when done with LLVM/Clang

elseif( CMAKE_CXX_COMPILER_ID MATCHES Intel )
   #set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   ${INTEL_COMPILER_OPTIONS_DEBUG}" )   # Uncomment this when done with Intel
   #set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${INTEL_COMPILER_OPTIONS_RELEASE}" ) # Uncomment this when done with Intel
   message( WARNING "Compiler flags for ${CMAKE_CXX_COMPILER_ID} not (yet) supported" )           # Remove this when done with Intel

elseif( CMAKE_CXX_COMPILER_ID MATCHES MSVC )
   #set( CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG}   ${MSVC_COMPILER_OPTIONS_DEBUG}" )    # Uncomment this when done with MSVC
   #set( CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${MSVC_COMPILER_OPTIONS_RELEASE}" )  # Uncomment this when done with MSVC
   message( WARNING "Compiler flags for ${CMAKE_CXX_COMPILER_ID} not (yet) supported" )           # Remove this when done with MSVC

else()
   message( WARNING "Unknown compiler ${CMAKE_CXX_COMPILER_ID}" )
endif()

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
)

# Get ENGINE_SRC and ENGINE_INC (automatically generated by gen_sources.sh)
include(sources.cmake)

if( NOT DEFINED ENGINE_BUILD_SHARED )
   set( ENGINE_BUILD_SHARED 1 )          # DEFAULT: We create a shared library ( .so or a .dll )
endif( NOT DEFINED ENGINE_BUILD_SHARED )

if( ENGINE_BUILD_SHARED )
   add_library( ${PROJECT_NAME} SHARED ${ENGINE_SRC} ) # Create the shared library
else( ENGINE_BUILD_SHARED )
   add_library( ${PROJECT_NAME} STATIC ${ENGINE_SRC} ) # Create the static library
endif( ENGINE_BUILD_SHARED )

# Windows also needs some linking...
if( WIN32 AND ENGINE_BUILD_SHARED )
   target_link_libraries( ${PROJECT_NAME} ${ENGINE_LINK} )
endif( WIN32 AND ENGINE_BUILD_SHARED )

# Set some variables so that Cmake can do some fancy stuff with versions and filenames
set_target_properties(
 ${PROJECT_NAME}
 PROPERTIES
  VERSION   ${CM_VERSION_MAJOR}.${CM_VERSION_MINOR}
  SOVERSION ${CM_VERSION_MAJOR}
)

# Now build the tests
add_subdirectory(${PROJECT_SOURCE_DIR}/tests/)

# Also install some usefull cmakefiles
set(DEPENDENCES ${PROJECT_SOURCE_DIR}/cmake/FindEngine.cmake
                ${PROJECT_SOURCE_DIR}/cmake/FindGLEW2.cmake)

# The install configuaration
install( FILES ${ENGINE_INC}      DESTINATION ${CMAKE_INSTALL_PREFIX}/include/engine)
install( FILES ${DEPENDENCES}     DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/engine)
install( 
 TARGETS ${PROJECT_NAME}
 RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX}/bin/
 LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/ 
 ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/
)

