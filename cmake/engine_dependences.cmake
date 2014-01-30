
if( NOT DEFINED eCImgPrefix )
   set(eCImgPrefix /opt/projects/cimg)
endif(NOT DEFINED eCImgPrefix)


# Begin STATIC ###
if( NOT DEFINED SERCH_AS_STATIC_AS_POSSIBLE )
   set( SERCH_AS_STATIC_AS_POSSIBLE OFF )
endif()

if( NOT SERCH_AS_STATIC_AS_POSSIBLE )
   message( STATUS "Do not change CMAKE_FIND_LIBRARY_SUFFIXES ( ${CMAKE_FIND_LIBRARY_SUFFIXES} )" )
else()
   set( OLD_CMAKE_FIND_LIBRARY_SUFFIXES ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   if( WIN32 )
      set( CMAKE_FIND_LIBRARY_SUFFIXES .lib .dll ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   else()
      set( CMAKE_FIND_LIBRARY_SUFFIXES .a .so ${CMAKE_FIND_LIBRARY_SUFFIXES} )
   endif()
   message( STATUS "Changed CMAKE_FIND_LIBRARY_SUFFIXES for static linking ( ${CMAKE_FIND_LIBRARY_SUFFIXES} )" )
endif()
# End STATIC ###

#message( STATUS "BOOST_ROOT:       ${BOOST_ROOT}")


set(Boost_USE_MULTITHREADED ON)

if( NOT DEFINED BOOST_NO_STATIC )
   set(Boost_USE_STATIC_LIBS ON)
endif()

find_package(Boost   REQUIRED COMPONENTS system regex thread chrono filesystem date_time )
find_package(GLEW    REQUIRED)
#find_package(PNG     REQUIRED)
find_package(OpenGL  REQUIRED)
find_package(X11     REQUIRED)
find_package(Threads REQUIRED)
find_package(GMP     REQUIRED)

if( SERCH_AS_STATIC_AS_POSSIBLE )
   set( CMAKE_FIND_LIBRARY_SUFFIXES ${OLD_CMAKE_FIND_LIBRARY_SUFFIXES} )
endif()

set( ENGINE_DEBUG OFF )

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
 
set(ENGINE_INCL 
   ${Boost_INCLUDE_DIRS}
   ${GLEW_INCLUDE_DIRS}
   ${PNG_INCLUDE_DIRS}
   ${OPENGL_INCLUDE_DIR}
   ${PTHREAD_INCLUDE_DIRS}
   ${X11_INCLUDE_DIRS}
   ${X11_Xrandr_INCLUDE_PATH}
   ${ZLIB_INCLUDE_DIRS}
   ${GMP_INCLUDE_DIR}
   ${eCImgPrefix}
)









