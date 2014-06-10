/*!
 * \file x11/context.cpp
 * \brief \b Classes: \a eContext
 *
 * This file contains the definitions of the context
 * creation class eContext, whitch are all called \b after
 * \c GLEW is init. The other definitions are in file
 * e_context_window.cpp
 *
 * \sa e_context_window.cpp e_context.hpp e_eInit.hpp
 */
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <boost/regex.hpp>
#include <GL/glxew.h>   // Must be included BEFORE e_context.hpp!!! (GLEW)
#include "context.hpp"
#include "log.hpp"

namespace e_engine {

namespace unix_x11 {

bool eContext::isExtensionSupported( const char *_extension ) {
   const char *lExtList_CSTR = glXQueryExtensionsString( vDisplay_X11, vScreen_X11 );
   const char *lStart_CSTR;
   const char *lWhere_CSTR, *lTerminator_CSTR;

   // Extension names should not have spaces.
   lWhere_CSTR = strchr( _extension, ' ' );
   if ( lWhere_CSTR || *_extension == '\0' )
      return false;

   /* It takes a bit of care to be fool-proof about parsing the
      OpenGL extensions string. Don't be fooled by sub-strings,
      etc. */
   for ( lStart_CSTR = lExtList_CSTR; ; ) {
      lWhere_CSTR = strstr( lStart_CSTR, _extension );

      if ( !lWhere_CSTR )
         break;

      lTerminator_CSTR = lWhere_CSTR + strlen( _extension );

      if ( lWhere_CSTR == lStart_CSTR || *( lWhere_CSTR - 1 ) == ' ' )
         if ( *lTerminator_CSTR == ' ' || *lTerminator_CSTR == '\0' )
            return true;

      lStart_CSTR = lTerminator_CSTR;
   }
   return false;
}

eContext::eContext() {
   vNumOfFBConfigs_I      = 0;
   vDisplay_X11           = NULL;
   vWindow_X11            = 0;
   vWindowHasBorder_B     = true;
   vHaveContext_B         = false;
   vDisplayCreated_B      = false;
   vWindowCreated_B       = false;
   vColorMapCreated_B     = false;
   vWindowRecreate_B      = false;
   vIsMouseGrabbed_B      = false;
   vEventMask_lI          = KeyPressMask           | KeyReleaseMask       | ButtonPressMask     | ButtonReleaseMask   |
                            PointerMotionMask      | ButtonMotionMask     | ExposureMask        | StructureNotifyMask |
                            SubstructureNotifyMask | VisibilityChangeMask | EnterWindowMask     | LeaveWindowMask     |
                            FocusChangeMask;

   XInitThreads();
}



/*!
 * \brief Creates the window and the OpenGL context
 *
 * Creates a \c X11 connection first, then looks for the
 * best FB config, creates the window, aswell as
 * the \c OpenGL context and inits \c GLEW
 *
 * It uses the following private functions:
 * \code

createDisplay();
createFrameBuffer();
createWindow();
createContext();

vRandR_eRR.init( vDisplay_X11, vWindow_X11, vRootWindow_X11 )
 * \endcode
 *
 * Additionally it prints all versions with \b LOG
 *
 * \param argc argc from the main() fnction
 * \param argv argv from the main() fnction
 *
 * \returns  1 -- Versions are compatible
 * \returns -1 -- Unable to connect to the X-Server
 * \returns -2 -- Need a newer GLX version
 * \returns -3 -- Unable to find any matching fbConfig
 * \returns -4 -- Failed to create a X11 Window
 * \returns  3 -- Failed to create a context
 * \returns  4 -- Failed to init GLEW
 */
int eContext::createContext() {
   int lReturnValue_I;
   std::string lRandRVersionString_str;

   if ( XInitThreads() == 0 ) {
      wLOG "Failed to call XInitThreads();" END
   }

   if ( ( lReturnValue_I = createDisplay() )     != 1 ) {return lReturnValue_I;}
   if ( ( lReturnValue_I = createFrameBuffer() ) != 1 ) {return lReturnValue_I;}
   if ( ( lReturnValue_I = createWindow() )      != 1 ) {return lReturnValue_I;}
   if ( ( lReturnValue_I = createOGLContext() )  != 1 ) {return lReturnValue_I;}

   if ( initRandR( vDisplay_X11, vWindow_X11, vRootWindow_X11 ) ) {
      int lVRRmajor_I;
      int lVRRminor_I;
      getRandRVersion( lVRRmajor_I, lVRRminor_I );
      lRandRVersionString_str = boost::lexical_cast<std::string>( lVRRmajor_I ) + '.' + boost::lexical_cast<std::string>( lVRRminor_I );
   } else {
      lRandRVersionString_str = "!!! NOT SUPPORTED !!!";
   }


   if ( ! vHaveGLEW_B ) {
      // Init GLEW
      glewExperimental = GL_TRUE;
      vHaveGLEW_B = true;
      if ( GLEW_OK != glewInit() ) {
         eLOG "Failed to init GLEW. Aborting. (return 4)" END
         vHaveGLEW_B = false;
         return 4;
      }
   }

   iLOG "Versions:"
   POINT "Engine: "
   ADD 'B', 'C', E_VERSION_MAJOR    ADD 'B', 'C', "."
   ADD 'B', 'C', E_VERSION_MINOR    ADD 'B', 'C', "."
   ADD 'B', 'C', E_VERSION_SUBMINOR ADD( E_COMMIT_IS_TAGGED ? " [RELEASE] " : " +GIT " ) ADD E_VERSION_GIT
   POINT "OpenGL: " ADD 'B', 'C', glGetString( GL_VERSION )
   POINT "GLSL:   " ADD 'B', 'C', glGetString( GL_SHADING_LANGUAGE_VERSION )
   POINT "GLX:    " ADD 'B', 'C', vGLXVersionMajor_I ADD 'B', 'C' , "." ADD 'B', 'C' , vGLXVersionMinor_I
   POINT "X11:    " ADD 'B', 'C', vX11VersionMajor_I ADD 'B', 'C' , "." ADD 'B', 'C' , vX11VersionMinor_I
   POINT "GLEW:   " ADD 'B', 'C', glewGetString( GLEW_VERSION )
   POINT "RandR:  " ADD 'B', 'C', lRandRVersionString_str
   END

   if ( WinData.win.fullscreen == true ) {
      fullScreen( C_ADD );
   }

   if ( WinData.win.windowDecoration == true ) {
      setDecoration( C_ADD );
   } else {
      setDecoration( C_REMOVE );
   }

   return 1;
}


/*!
 * \brief Changes the window config
 * \param _width  The new width
 * \param _height The new height
 * \param _posX   The new X coordinate
 * \param _posY   The new Y coordinate
 * \returns The return value of \c XConfigureWindow
 */
int eContext::changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY ) {
   if ( ! vHaveGLEW_B )
      return 0;

   XWindowChanges lWindowChanges_X11;
   WinData.win.width  = lWindowChanges_X11.width  = ( int )_width;
   WinData.win.height = lWindowChanges_X11.height = ( int )_height;
   WinData.win.posX   = lWindowChanges_X11.x      = _posX;
   WinData.win.posY   = lWindowChanges_X11.y      = _posY;

   return XConfigureWindow( vDisplay_X11, vWindow_X11, CWX | CWY | CWWidth | CWHeight, &lWindowChanges_X11 );
}


/*!
 * \brief Destroy the window and the context
 * \returns Nothing
 */
void eContext::destroyContext() {
   endRandR();
   if ( vHaveContext_B == true ) {
      glXMakeCurrent( vDisplay_X11, 0, 0 );
      glXDestroyContext( vDisplay_X11, vOpenGLContext_GLX );
      vHaveContext_B = false;
   }
   if ( vWindowCreated_B == true ) {
      XDestroyWindow( vDisplay_X11, vWindow_X11 );
      vWindowCreated_B = false;
      vWindow_X11 = 0;
   }
   if ( vColorMapCreated_B == true ) {
      XFreeColormap( vDisplay_X11, vColorMap_X11 );
      vColorMapCreated_B = false;
   }
   if ( vDisplayCreated_B == true ) {
      XCloseDisplay( vDisplay_X11 );
      vDisplayCreated_B = false;
      vDisplay_X11 = NULL;
      iLOG "Everything destroyed" END
   }
   vHaveContext_B = false;
}


/*!
 * \brief Enables VSync
 * \returns 0 No Window / OpenGL context
 * \returns 1 \c SUCCESS
 * \returns 2 Extention not supported
 * \returns 3 glXSwapIntervalSGI (main VSync function) returned GLX_BAD_VALUE
 * \returns 4 glXSwapIntervalSGI (main VSync function) returned GLX_BAD_CONTEXT
 * \returns 5 glXSwapIntervalSGI (main VSync function) returned something unknown (!= 0)
 */
int eContext::enableVSync() {
   if ( ! vHaveGLEW_B )
      return 0;

   if ( glxewIsSupported( "GLX_SGI_swap_control" ) ) {
      switch ( glXSwapIntervalSGI( 1 ) ) {
         case 0: // Success
            iLOG "VSync [GLX] enabled" END
            return 1;
         case GLX_BAD_VALUE:
            wLOG    "VSync Error [GLX] GLX_BAD_VALUE; 1 seams to be not a good value on this System"
            NEWLINE "==> VSync NOT enabled" END
            return 3;
         case GLX_BAD_CONTEXT:
            wLOG    "VSync Error [GLX] GLX_BAD_CONTEXT; There is no *current* OpenGL context in this thread. Use makeContextCurrent() to fix this"
            NEWLINE "==> VSync NOT enabled" END
            return 4;
         default:
            wLOG    "VSync Error [GLX] <UNKNOWN>; Unknown return value of glXSwapIntervalSGI"
            NEWLINE "==> VSync NOT enabled" END
            return 5;
      }
   } else {
      wLOG    "VSync Error [GLX]; Extention GLX_SGI_swap_control not supported"
      NEWLINE "==> VSync NOT enabled" END
      return 2;
   }
}

/*!
 * \brief Disables VSync
 * \returns 0 No Window / OpenGL context
 * \returns 1 \c SUCCESS
 * \returns 2 Extention not supported
 * \returns 3 glXSwapIntervalSGI (main VSync function) returned GLX_BAD_VALUE
 * \returns 4 glXSwapIntervalSGI (main VSync function) returned GLX_BAD_CONTEXT
 * \returns 5 glXSwapIntervalSGI (main VSync function) returned something unknown (!= 0)
 */
int eContext::disableVSync() {
   if ( ! vHaveGLEW_B )
      return 0;

   if ( glxewIsSupported( "GLX_SGI_swap_control" ) ) {
      switch ( glXSwapIntervalSGI( 0 ) ) {
         case 0: // Success
            iLOG "VSync [GLX] disabled" END
            return 1;
         case GLX_BAD_VALUE:
            wLOG    "VSync Error [GLX] GLX_BAD_VALUE; 0 seams to be not a good value on this System"
            NEWLINE "==> VSync NOT disabled" END
            return 3;
         case GLX_BAD_CONTEXT:
            wLOG    "VSync Error [GLX] GLX_BAD_CONTEXT; There is no *current* OpenGL context in this thread. Use makeContextCurrent() to fix this"
            NEWLINE "==> VSync NOT disabled" END
            return 4;
         default:
            wLOG    "VSync Error [GLX] <UNKNOWN>; Unknown return value of glXSwapIntervalSGI"
            NEWLINE "==> VSync NOT disabled" END
            return 5;
      }
   } else {
      wLOG    "VSync Error [GLX]; Extention GLX_SGI_swap_control not supported"
      NEWLINE "==> VSync NOT disabled" END
      return 2;
   }
}


struct MwmHints {
   unsigned long flags;
   unsigned long functions;
   unsigned long decorations;
   long input_mode;
   unsigned long status;
};
const long unsigned int MWM_HINTS_DECORATIONS = ( 1L << 1 );


/*!
 * \brief Try to add or remove the window decoration
 * \warning A non X11 standard function is used! Functionality may be restricted
 * \param _mode Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 * \returns \c Success: \a true -- \c Failed: \a false
 */
bool eContext::setDecoration( e_engine::ACTION _action ) {
   if ( ! vHaveGLEW_B )
      return false;

   Atom lAtomMwmHints_X11 = XInternAtom( vDisplay_X11, "_MOTIF_WM_HINTS", True );
   if ( ! lAtomMwmHints_X11 ) {
      wLOG "Failed to create X11 Atom _MOTIF_WM_HINTS ==> Cannot set / remove window border" END
      return false;
   }

   if ( _action == C_TOGGLE ) {
      _action = ( vWindowHasBorder_B ) ? C_REMOVE : C_ADD;
   }

   vWindowHasBorder_B = ( _action == C_REMOVE ) ? false : true;

   struct MwmHints lHints_X11;

   lHints_X11.flags       = MWM_HINTS_DECORATIONS;
   lHints_X11.decorations = _action;

   if ( ! XChangeProperty( vDisplay_X11,
                           vWindow_X11,
                           lAtomMwmHints_X11,
                           lAtomMwmHints_X11,
                           32,
                           PropModeReplace,
                           ( unsigned char * )&lHints_X11,
                           5 )
      ) {
      wLOG "Failed to set XChangeProperty( ..., _MOTIF_WM_HINTS, _MOTIF_WM_HINTS,...); ==> Can not set / remove window border " END
      return false;
   }

   switch ( _action ) {
      case C_ADD:    WinData.win.windowDecoration = true;  break;
      case C_REMOVE: WinData.win.windowDecoration = false; break;
      case C_TOGGLE: WinData.win.windowDecoration = !WinData.win.windowDecoration; break;
   }

   iLOG "Successfully " ADD( _action == C_REMOVE ) ? "removed window decoration" : "added window decoration" END

   return true;
}


/*!
 * \brief Change the fullscreen mode
 * \param _mode        Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 * \param _allMonitors \a true if the Fullscreen window should be mapped over all monitors
 * \returns \c Success: \a true -- \c Failed: \a false
 */
bool eContext::fullScreen( e_engine::ACTION _action, bool _allMonitors ) {
   bool ret1 = setAttribute( _action, FULLSCREEN );

   if ( !ret1 )
      return false;

   if ( _allMonitors && !( _action == C_REMOVE ) )
      return fullScreenMultiMonitor();

   return true;
}

/*!
 * \brief Change the maximize mode
 * \param _mode Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 * \returns \c Success: \a true -- \c Failed: \a false
 */
bool eContext::maximize( e_engine::ACTION _action ) {
   return setAttribute( _action, MAXIMIZED_HORZ, MAXIMIZED_VERT );
}


/*!
 * \brief Sends an NET_WM_STATE event to the X-Server
 *
 * \param _action What to do
 * \param _type1  The first thing to change
 * \param _type2  The second thing to change (Default: NONE)
 *
 * \returns \c Success: \a true -- <c>Failed / not supported:</c> \a false
 *
 * \sa e_engine::ACTION, e_engine::WINDOW_ATTRIBUTE
 */
bool eContext::setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 ) {
   if ( ! vHaveGLEW_B )
      return false;

   if ( _type1 == _type2 ) {
      eLOG "Changing the same attribute at the same time makes completely no sense. ==> Do nothing" END
      return false;
   }

   Atom lAtomNetWmStateState1_X11;
   Atom lAtomNetWmStateState2_X11;
   boost::regex lTypeRegex_EX( "^_NET_WM_STATE_" );
   const char  *lReplace_C = "";

   std::string lMode_STR;
   std::string lState1_str = "NOTHING", lState2_str = "NOTHING";

   switch ( _action ) {
      case C_REMOVE:   lMode_STR = "Removing";  break;
      case C_ADD:      lMode_STR = "Enabling";  break;
      case C_TOGGLE:   lMode_STR = "Toggling";  break;
      default: return -1;
   }

   if ( _type1 != NONE ) {
      switch ( _type1 ) {
         case MODAL:                lState1_str = "_NET_WM_STATE_MODAL"             ; break;
         case STICKY:               lState1_str = "_NET_WM_STATE_STICKY"            ; break;
         case MAXIMIZED_VERT:       lState1_str = "_NET_WM_STATE_MAXIMIZED_VERT"    ; break;
         case MAXIMIZED_HORZ:       lState1_str = "_NET_WM_STATE_MAXIMIZED_HORZ"    ; break;
         case SHADED:               lState1_str = "_NET_WM_STATE_SHADED"            ; break;
         case SKIP_TASKBAR:         lState1_str = "_NET_WM_STATE_SKIP_TASKBAR"      ; break;
         case SKIP_PAGER:           lState1_str = "_NET_WM_STATE_SKIP_PAGER"        ; break;
         case HIDDEN:               lState1_str = "_NET_WM_STATE_HIDDEN"            ; break;
         case FULLSCREEN:           lState1_str = "_NET_WM_STATE_FULLSCREEN"        ; break;
         case ABOVE:                lState1_str = "_NET_WM_STATE_ABOVE"             ; break;
         case BELOW:                lState1_str = "_NET_WM_STATE_BELOW"             ; break;
         case DEMANDS_ATTENTION:    lState1_str = "_NET_WM_STATE_DEMANDS_ATTENTION" ; break;
         case FOCUSED:              lState1_str = "_NET_WM_STATE_FOCUSED"           ; break;
         default: return false;
      }

      lAtomNetWmStateState1_X11 = XInternAtom( vDisplay_X11, lState1_str.c_str(), True );

      if ( ! lAtomNetWmStateState1_X11 ) {
         wLOG "Failed to create X11 Atom " ADD lState1_str END
         return false;
      }

      lState1_str = boost::regex_replace( lState1_str, lTypeRegex_EX, lReplace_C );
   }

   if ( _type2 != NONE ) {
      switch ( _type2 ) {
         case MODAL:                lState2_str = "_NET_WM_STATE_MODAL"             ; break;
         case STICKY:               lState2_str = "_NET_WM_STATE_STICKY"            ; break;
         case MAXIMIZED_VERT:       lState2_str = "_NET_WM_STATE_MAXIMIZED_VERT"    ; break;
         case MAXIMIZED_HORZ:       lState2_str = "_NET_WM_STATE_MAXIMIZED_HORZ"    ; break;
         case SHADED:               lState2_str = "_NET_WM_STATE_SHADED"            ; break;
         case SKIP_TASKBAR:         lState2_str = "_NET_WM_STATE_SKIP_TASKBAR"      ; break;
         case SKIP_PAGER:           lState2_str = "_NET_WM_STATE_SKIP_PAGER"        ; break;
         case HIDDEN:               lState2_str = "_NET_WM_STATE_HIDDEN"            ; break;
         case FULLSCREEN:           lState2_str = "_NET_WM_STATE_FULLSCREEN"        ; break;
         case ABOVE:                lState2_str = "_NET_WM_STATE_ABOVE"             ; break;
         case BELOW:                lState2_str = "_NET_WM_STATE_BELOW"             ; break;
         case DEMANDS_ATTENTION:    lState2_str = "_NET_WM_STATE_DEMANDS_ATTENTION" ; break;
         case FOCUSED:              lState2_str = "_NET_WM_STATE_FOCUSED"           ; break;
         default: return false;
      }

      lAtomNetWmStateState2_X11 = XInternAtom( vDisplay_X11, lState2_str.c_str(), True );

      if ( ! lAtomNetWmStateState2_X11 ) {
         wLOG "Failed to create X11 Atom " ADD lState2_str END
         return false;
      }

      lState2_str               = boost::regex_replace( lState2_str, lTypeRegex_EX, lReplace_C );
   }

   if ( !
         sendX11Event(
            "_NET_WM_STATE",
            _action,
            ( _type1 != NONE ) ? lAtomNetWmStateState1_X11 : 0,
            ( _type2 != NONE ) ? lAtomNetWmStateState2_X11 : 0,
            1
         )
      ) {
      wLOG lMode_STR ADD ' ' ADD lState1_str ADD " and " ADD lState2_str ADD " mode FAILED" END
      return false;
   }

   if ( _type1 == FULLSCREEN || _type2 == FULLSCREEN ) {
      switch ( _action ) {
         case C_ADD:    WinData.win.fullscreen = true;  break;
         case C_REMOVE: WinData.win.fullscreen = false; break;
         case C_TOGGLE: WinData.win.fullscreen = !WinData.win.fullscreen; break;
      }
   }

   iLOG lMode_STR ADD ' ' ADD lState1_str ADD " and " ADD lState2_str ADD " mode SUCCEEDED" END

   return true;
}

/*!
 * \brief Try to map the fullscreen window to all monitors
 * \returns \a true when successful and \a false when not
 */
bool eContext::fullScreenMultiMonitor() {
   if ( ! vHaveGLEW_B )
      return false;

   unsigned int lLeft_I;
   unsigned int lRight_I;
   unsigned int lTop_I;
   unsigned int lBot_I;

   getMostLeftRightTopBottomCRTC( lLeft_I, lRight_I, lTop_I, lBot_I );

   if ( ! sendX11Event( "_NET_WM_FULLSCREEN_MONITORS", lTop_I, lBot_I, lLeft_I, lRight_I ) ) {
      wLOG "Unable to map the fullscreen window to all monitors" END
      return false;
   }

   iLOG "Successfully mapped the fullscreen window to all monitors" END

   return true;
}


/*!
 * \brief Try to map the fullscreen window to display _disp
 *
 * \param _disp The display where the fullscreen window should be
 *
 * \returns 1 when succeeded
 * \returns 2 when sending the X11 event failed
 * \returns the result of eRandR::getIndexOfDisplay() when there was a failure
 */
int eContext::setFullScreenMonitor( eDisplays _disp ) {
   if ( ! vHaveGLEW_B )
      return 0;

   int lDisp_I = getIndexOfDisplay( _disp );

   if ( lDisp_I < 0 ) {
      wLOG "No valid eDisplays [ setFullScreenMonitor(...) ] ==> Return eRandR::getIndexOfDisplay( _disp ) = " ADD lDisp_I END
      return lDisp_I;
   }

   if ( ! sendX11Event( "_NET_WM_FULLSCREEN_MONITORS", lDisp_I, lDisp_I, lDisp_I, lDisp_I ) ) {
      wLOG "Unable to map the fullscreen window to monitor " ADD lDisp_I END
      return 2;
   }

   iLOG "Successfully mapped the fullscreen window to monitor " ADD lDisp_I END

   return 1;
}


bool eContext::sendX11Event( std::string _atom, GLint64 _l0, GLint64 _l1, GLint64 _l2, GLint64 _l3, GLint64 _l4 ) {
   Atom   lAtom_X11   = XInternAtom( vDisplay_X11, _atom.c_str(), True );

   if ( ! lAtom_X11 ) {
      wLOG "Failed to create X11 Atom " ADD _atom END
      return false;
   }

   XEvent lEvent_X11;

   memset( &lEvent_X11, 0, sizeof( lEvent_X11 ) );
   lEvent_X11.type                 = ClientMessage;
   lEvent_X11.xclient.window       = vWindow_X11;

   lEvent_X11.xclient.message_type = lAtom_X11;
   lEvent_X11.xclient.format       = 32;
   lEvent_X11.xclient.data.l[0]    = _l0;
   lEvent_X11.xclient.data.l[1]    = _l1;
   lEvent_X11.xclient.data.l[2]    = _l2;
   lEvent_X11.xclient.data.l[3]    = _l3;
   lEvent_X11.xclient.data.l[4]    = _l4;

   return XSendEvent( vDisplay_X11, DefaultRootWindow( vDisplay_X11 ), False, SubstructureRedirectMask | SubstructureNotifyMask, &lEvent_X11 );
}





/*!
 * \brief Get the \c X11 version
 * \param[out] _major The major version number
 * \param[out] _minor The minor version number
 */
void eContext::getX11Version( int *_major, int *_minor )  {
   if ( !vDisplayCreated_B ) {
      *_major = -1;
      *_minor = -1;
      return;
   }
   *_major = vX11VersionMajor_I;
   *_minor = vX11VersionMinor_I;
}

/*!
 * \brief Get the \c GLX version
 * \param[out] _major The major version number
 * \param[out] _minor The minor version number
 */
void eContext::getGLXVersion( int *_major, int *_minor ) {
   if ( !vDisplayCreated_B ) {
      *_major = -1;
      *_minor = -1;
      return;
   }
   *_major = vGLXVersionMajor_I;
   *_minor = vGLXVersionMinor_I;
}


/*!
 * \brief Make this context current
 * \returns true on success
 * \returns false when there was an error
 */
bool eContext::makeContextCurrent() {
   if ( ! vHaveContext_B ) {
      eLOG "OpenGL context Error [GLX]; We do not have any context. Please create it with eInit::init() before you run this!" END
      return false;
   }
   return glXMakeCurrent( vDisplay_X11, vWindow_X11, vOpenGLContext_GLX ) == True ? true : false;
}

/*!
 * \brief Make \b NO context current
 * \returns true on success
 * \returns false when there was an error
 */
bool eContext::makeNOContextCurrent()  {
   if ( ! vHaveContext_B ) {
      eLOG "OpenGL context Error [GLX]; We do not have any context. Please create it with eInit::init() before you run this!" END
      return false;
   }
   return glXMakeCurrent( vDisplay_X11, 0, 0 ) == True ? true : false;
}

/*!
 * \brief Grabs the mouse pointer (and the keyboard)
 *
 * \note You can only grab the mouse if the mouse is ungrabbed
 *
 * \returns true if successful and false if not
 */
bool eContext::grabMouse() {
   if ( vIsMouseGrabbed_B ) {
      wLOG "Mouse is already grabbed" END
      return false;
   }

   int lReturn_I =
      XGrabPointer(
         vDisplay_X11,       // Our connection to the X server
         vWindow_X11,        // The window owning the grab
         False,              // Send some additional events
         ButtonPressMask   |
         ButtonReleaseMask |
         PointerMotionMask |
         ButtonMotionMask,   // We dont need a special event mask
         GrabModeAsync,      // Mouse grabbing should be async (easier for us)
         GrabModeAsync,      // Key grabbing should be async (easier for us)
         vWindow_X11,        // Lock the cursor in this window
         None,               // Use the default window cursor icon
         CurrentTime         // X11 needs a time
      );

   if ( lReturn_I != GrabSuccess ) {
      wLOG "Failed to grab the mouse" END
      return false;
   }
   vIsMouseGrabbed_B = true;
   return true;
}


/*!
 * \brief Ungrabs the mouse pointer (and the keyboard)
 *
 * \note You can only ungrab the mouse if it is grabbed
 *
 * \returns true if successful and false if not
 */
bool eContext::freeMouse() {
   if ( !vIsMouseGrabbed_B ) {
      wLOG "Mouse is not grabbed" END
      return false;
   }


   if ( XUngrabPointer( vDisplay_X11, CurrentTime ) == 0 ) {
      wLOG "Failed to ungrab the mouse" END
      return false;
   }
   vIsMouseGrabbed_B = false;
   return true;
}

/*!
 * \brief Sets the mouse position
 *
 * \param[in] _posX The x coordinate in our window
 * \param[in] _posY The y coordinate in our window
 *
 * \note _posX and _posY must be inside our window
 *
 * \returns true if successful and false if not
 */
bool eContext::moveMouse( unsigned int _posX, unsigned int _posY ) {
   if ( _posX > WinData.win.width || _posY > WinData.win.height ) {
      wLOG "_posX and/or _posY outside the window" END
      return false;
   }

   XWarpPointer(
      vDisplay_X11,  // Our connection to the X server
      None,          // Move it from this window (unknown)...
      vWindow_X11,   // ...to the window
      0,             // We dont...
      0,             // ...have any...
      0,             // ...information about...
      0,             // ...the source window!
      _posX,         // Posx in the window
      _posY          // Posy in the window
   );

   return false;
}

/*!
 * \brief Get if the mouse is grabbed
 * \returns if the mouse is grabbed
 */
bool eContext::getIsMouseGrabbed() const {
   return vIsMouseGrabbed_B;
}



/*!
 * \brief Hides the cursor
 * \returns true if successful and false if not
 */
bool eContext::hideMouseCursor() {
   if( vIsCursorHidden_B ) {
      wLOG "Cursor is already hidden" END
      return false;
   }
   
   Pixmap   lNoMouseCursorPixmap_X11;
   Colormap lColorMap_X11;
   Cursor   lTransparrentCursor_X11;
   XColor   lBlackColor_X11, lDummyColor_X11;
   char     lNoRealData_C[] = {0, 0, 0, 0, 0, 0, 0, 0};

   lColorMap_X11 = DefaultColormap( vDisplay_X11, DefaultScreen( vDisplay_X11 ) );

   XAllocNamedColor( vDisplay_X11, lColorMap_X11, "black", &lBlackColor_X11, &lDummyColor_X11 );
   lNoMouseCursorPixmap_X11 = XCreateBitmapFromData( vDisplay_X11, vWindow_X11, lNoRealData_C, 8, 8 );

   lTransparrentCursor_X11  = XCreatePixmapCursor(
                                 vDisplay_X11,
                                 lNoMouseCursorPixmap_X11,
                                 lNoMouseCursorPixmap_X11,
                                 &lBlackColor_X11,
                                 &lBlackColor_X11,
                                 0,
                                 0
                              );

   XDefineCursor( vDisplay_X11, vWindow_X11, lTransparrentCursor_X11 );
   XFreeCursor( vDisplay_X11, lTransparrentCursor_X11 );
   if ( lNoMouseCursorPixmap_X11 != None )
      XFreePixmap( vDisplay_X11, lNoMouseCursorPixmap_X11 );
   XFreeColors( vDisplay_X11, lColorMap_X11, &lBlackColor_X11.pixel, 1, 0 );
   
   vIsCursorHidden_B = true;
   return true;
}

/*!
 * \brief Shows the cursor
 * \returns true if successful and false if not
 */
bool eContext::showMouseCursor() {
   if ( !vIsCursorHidden_B ) {
      wLOG "Cursor is already visible" END
      return false;
   }
   
   XUndefineCursor( vDisplay_X11, vWindow_X11 );
   vIsCursorHidden_B = false;
   return true;
}

/*!
 * \brief Get if the cursor is hidden
 * \returns true if the cursor is hidden
 */
bool eContext::getIsCursorHidden() const {
   return vIsCursorHidden_B;
}





} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
