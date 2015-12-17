cmake_minimum_required(VERSION 2.8.8)

set( CMAKE_CXX_FLAGS_DEBUG   "" )
set( CMAKE_CXX_FLAGS_RELEASE "" )
set( CMAKE_BUILD_TYPE RELEASE )


option( USE_SYSTEM_GLEW "Build dependency GLEW?" OFF )

if( NOT USE_SYSTEM_GLEW )
   set( BUILD_UTILS OFF CACHE BOOL "Disabling BUILD_UTILS" )
   include_directories( GLEW/build/cmake )
   add_subdirectory( GLEW/build/cmake )
   message( STATUS "Building dependency GLEW (use -DUSE_SYSTEM_GLEW=ON to disable)" )
else()
   message( STATUS "NOT building dependency GLEW (use -DUSE_SYSTEM_GLEW=OFF to enable)" )
endif( NOT USE_SYSTEM_GLEW )
