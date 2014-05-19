
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

