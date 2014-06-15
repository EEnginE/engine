# Try to find the Engine
# ENGINE_PREFIX       - The install location of the engine
# ENGINE_FOUND        - system has ENGINE lib
# ENGINE_INCLUDE_DIRS - the ENGINE include directory
# ENGINE_LIBRARIES    - Libraries needed to use ENGINE

if (ENGINE_INCLUDE_DIR AND ENGINE_LIBRARIES)
                # Already in cache, be silent
                set(ENGINE_FIND_QUIETLY TRUE)
endif (ENGINE_INCLUDE_DIR AND ENGINE_LIBRARIES)

find_package(Boost   REQUIRED COMPONENTS system regex thread chrono filesystem date_time )
find_package(GLEW    REQUIRED)
#find_package(PNG     REQUIRED)
find_package(OpenGL  REQUIRED)
find_package(Threads REQUIRED)
find_package(GMP     REQUIRED)

if( UNIX )
   find_package(X11  REQUIRED)
endif( UNIX )

find_path( 
 ENGINE_INCLUDE_DIR
 NAMES engine.hpp
 PATH_SUFFIXES engine 
 PATHS
  ${ENGINE_PREFIX}/include
  ${ENGINE_PREFIX}/include/engine
  ${CMAKE_INSTALL_PREFIX}/include
  ${CMAKE_INSTALL_PREFIX}/include/engine
  /usr/include
  /usr/include/engine
  /usr/local/include
  /usr/local/include/engine
)

find_library(
 ENGINE_LIBRARY
 NAMES engine utils
 PATHS 
  ${ENGINE_PREFIX}/lib
  ${CMAKE_INSTALL_PREFIX}/lib
  /usr/lib
  /usr/local/lib 
)


set( ENGINE_LIBRARIES
 ${ENGINE_LIBRARY}

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


set( ENGINE_INCLUDE_DIRS
 ${ENGINE_INCLUDE_DIR}

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


include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(Engine DEFAULT_MSG ENGINE_INCLUDE_DIR ENGINE_LIBRARY)


mark_as_advanced(ENGINE_INCLUDE_DIR ENGINE_LIBRARY)


