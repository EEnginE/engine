
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


set(ENGINE_LINK 
   ${Boost_LIBRARIES}
   ${GLEW_LIBRARIES}
   ${OPENGL_LIBRARIES}
   ${X11_LIBRARIES}
   ${X11_Xrandr_LIB}
   ${CMAKE_THREAD_LIBS_INIT}
)
