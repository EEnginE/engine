/*!
 * \file x11/iContext.cpp
 * \brief \b Classes: \a iContext
 *
 * This file contains the definitions of the context
 * creation class iContext, whitch are all called \b after
 * \c GLEW is init. The other definitions are in file
 * e_context_window.cpp
 *
 * \sa e_context_window.cpp e_context.hpp e_iInit.hpp
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <regex>
#include "iContext.hpp"
#include "uLog.hpp"
#include "eCMDColor.hpp"

namespace e_engine {

namespace unix_x11 {

bool iContext::isExtensionSupported( const char *_extension ) {
   // DUMMY
   return false;
}

iContext::iContext() {
   vNumOfFBConfigs_I  = 0;
   vDisplay_X11       = nullptr;
   vWindow_X11        = 0;
   vWindowHasBorder_B = true;
   vHaveContext_B     = false;
   vHaveGLEW_B        = false;
   vDisplayCreated_B  = false;
   vWindowCreated_B   = false;
   vColorMapCreated_B = false;
   vWindowRecreate_B  = false;
   vIsMouseGrabbed_B  = false;
   vEventMask_lI      = KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask |
                   PointerMotionMask | ButtonMotionMask | ExposureMask | StructureNotifyMask |
                   SubstructureNotifyMask | VisibilityChangeMask | EnterWindowMask |
                   LeaveWindowMask | FocusChangeMask;

   vIsCursorHidden_B = false;
   vIsMouseGrabbed_B = false;

   vWindowRecreate_B = false;
}

iContext::~iContext() { destroyContext(); }


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
 * \returns  1 -- Versions are compatible
 * \returns -1 -- Unable to connect to the X-Server
 * \returns -2 -- Need a newer GLX version
 * \returns -3 -- Unable to find any matching fbConfig
 * \returns -4 -- Failed to create a X11 Window
 * \returns  3 -- Failed to create a context
 * \returns  4 -- Failed to init GLEW
 */
int iContext::createContext() {
   int lReturnValue_I;
   std::string lRandRVersionString_str;

   if ( XInitThreads() == 0 ) {
      wLOG( "Failed to call XInitThreads();" );
   }

   if ( ( lReturnValue_I = initVulkan() ) != 1 ) {
      return lReturnValue_I;
   }

   return 0;

   if ( initRandR( vDisplay_X11, vWindow_X11, vRootWindow_X11 ) ) {
      int lVRRmajor_I;
      int lVRRminor_I;
      getRandRVersion( lVRRmajor_I, lVRRminor_I );
      lRandRVersionString_str = std::to_string( lVRRmajor_I ) + '.' + std::to_string( lVRRminor_I );
   } else {
      lRandRVersionString_str = "!!! NOT SUPPORTED !!!";
   }

   std::wstring lC1_C = eCMDColor::color( 'B', 'C' );

   iLOG( "Versions:",
         "\n  - Engine: ",
         lC1_C,
         E_VERSION_MAJOR,
         ".",
         E_VERSION_MINOR,
         ".",
         E_VERSION_SUBMINOR,
         E_GIT_LAST_TAG_DIFF == 0 ? " [RELEASE] "
                                  : ( " +" + std::to_string( E_GIT_LAST_TAG_DIFF ) + " " ),
         E_VERSION_GIT,
         "\n  - X11:    ",
         lC1_C,
         vX11VersionMajor_I,
         ".",
         vX11VersionMinor_I,
         "\n  - RandR:  ",
         lC1_C,
         lRandRVersionString_str );

   if ( GlobConf.win.fullscreen == true ) {
      fullScreen( C_ADD );
   }

   if ( GlobConf.win.windowDecoration == true ) {
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
int iContext::changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY ) {
   if ( !vHaveGLEW_B )
      return 0;

   XWindowChanges lWindowChanges_X11;
   lWindowChanges_X11.width = static_cast<int>( GlobConf.win.width = _width );
   lWindowChanges_X11.height = static_cast<int>( GlobConf.win.height = _height );

   GlobConf.win.posX = lWindowChanges_X11.x = _posX;
   GlobConf.win.posY = lWindowChanges_X11.y = _posY;

   return XConfigureWindow(
         vDisplay_X11, vWindow_X11, CWX | CWY | CWWidth | CWHeight, &lWindowChanges_X11 );
}


/*!
 * \brief Destroy the window and the context
 */
void iContext::destroyContext() {
   endRandR();
   //! \todo Destroy / free vulkan
   if ( vWindowCreated_B == true ) {
      XDestroyWindow( vDisplay_X11, vWindow_X11 );
      vWindowCreated_B = false;
      vWindow_X11      = 0;
   }
   if ( vColorMapCreated_B == true ) {
      XFreeColormap( vDisplay_X11, vColorMap_X11 );
      vColorMapCreated_B = false;
   }
   if ( vDisplayCreated_B == true ) {
      XCloseDisplay( vDisplay_X11 );
      vDisplayCreated_B = false;
      vDisplay_X11 = nullptr;
      iLOG( "Everything destroyed" );
   }
   vHaveContext_B = false;
}


/*!
 * \brief Enables VSync
 * \returns 0 No Window / OpenGL context
 * \returns 1 \c SUCCESS
 */
int iContext::enableVSync() {
   //! \todo Replace stub
   return 1;
}

/*!
 * \brief Disables VSync
 * \returns 0 No Window / OpenGL context
 * \returns 1 \c SUCCESS
 */
int iContext::disableVSync() {
   //! \todo Replace stub
   return 1;
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
 * \param _action Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 * \returns \c Success: \a true -- \c Failed: \a false
 */
bool iContext::setDecoration( e_engine::ACTION _action ) {
   if ( !vHaveGLEW_B )
      return false;

   Atom lAtomMwmHints_X11 = XInternAtom( vDisplay_X11, "_MOTIF_WM_HINTS", True );
   if ( !lAtomMwmHints_X11 ) {
      wLOG( "Failed to create X11 Atom _MOTIF_WM_HINTS ==> Cannot set / remove window border" );
      return false;
   }

   if ( _action == C_TOGGLE ) {
      _action = ( vWindowHasBorder_B ) ? C_REMOVE : C_ADD;
   }

   vWindowHasBorder_B = ( _action == C_REMOVE ) ? false : true;

   struct MwmHints lHints_X11;

   lHints_X11.flags       = MWM_HINTS_DECORATIONS;
   lHints_X11.decorations = _action;

   if ( !XChangeProperty( vDisplay_X11,
                          vWindow_X11,
                          lAtomMwmHints_X11,
                          lAtomMwmHints_X11,
                          32,
                          PropModeReplace,
                          reinterpret_cast<unsigned char *>( &lHints_X11 ),
                          5 ) ) {
      wLOG( "Failed to set XChangeProperty( ..., _MOTIF_WM_HINTS, _MOTIF_WM_HINTS,...); ==> Can "
            "not set / remove window border " );
      return false;
   }

   switch ( _action ) {
      case C_ADD: GlobConf.win.windowDecoration    = true; break;
      case C_REMOVE: GlobConf.win.windowDecoration = false; break;
      case C_TOGGLE: GlobConf.win.windowDecoration = !GlobConf.win.windowDecoration; break;
   }

   iLOG( "Successfully ",
         ( _action == C_REMOVE ) ? "removed window decoration" : "added window decoration" );

   return true;
}


/*!
 * \brief Change the fullscreen mode
 * \param _action      Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 * \param _allMonitors \a true if the Fullscreen window should be mapped over all monitors
 * \returns \c Success: \a true -- \c Failed: \a false
 */
bool iContext::fullScreen( e_engine::ACTION _action, bool _allMonitors ) {
   bool ret1 = setAttribute( _action, FULLSCREEN );

   if ( !ret1 )
      return false;

   if ( _allMonitors && !( _action == C_REMOVE ) )
      return fullScreenMultiMonitor();

   return true;
}

/*!
 * \brief Change the maximize mode
 * \param _action Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 * \returns \c Success: \a true -- \c Failed: \a false
 */
bool iContext::maximize( e_engine::ACTION _action ) {
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
bool iContext::setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 ) {
   if ( !vHaveGLEW_B )
      return false;

   if ( _type1 == _type2 ) {
      eLOG( "Changing the same attribute at the same time makes completely no sense. ==> Do "
            "nothing" );
      return false;
   }

   Atom lAtomNetWmStateState1_X11 = 0;
   Atom lAtomNetWmStateState2_X11 = 0;
   std::regex lTypeRegex_EX( "^_NET_WM_STATE_" );
   const char *lReplace_C = "";

   std::string lMode_STR;
   std::string lState1_str = "NOTHING", lState2_str = "NOTHING";

   switch ( _action ) {
      case C_REMOVE: lMode_STR = "Removing"; break;
      case C_ADD: lMode_STR    = "Enabling"; break;
      case C_TOGGLE: lMode_STR = "Toggling"; break;
   }


   if ( _type1 != NONE ) {
      switch ( _type1 ) {
         // clang-format off
         case MODAL:             lState1_str = "_NET_WM_STATE_MODAL"; break;
         case STICKY:            lState1_str = "_NET_WM_STATE_STICKY"; break;
         case MAXIMIZED_VERT:    lState1_str = "_NET_WM_STATE_MAXIMIZED_VERT"; break;
         case MAXIMIZED_HORZ:    lState1_str = "_NET_WM_STATE_MAXIMIZED_HORZ"; break;
         case SHADED:            lState1_str = "_NET_WM_STATE_SHADED"; break;
         case SKIP_TASKBAR:      lState1_str = "_NET_WM_STATE_SKIP_TASKBAR"; break;
         case SKIP_PAGER:        lState1_str = "_NET_WM_STATE_SKIP_PAGER"; break;
         case HIDDEN:            lState1_str = "_NET_WM_STATE_HIDDEN"; break;
         case FULLSCREEN:        lState1_str = "_NET_WM_STATE_FULLSCREEN"; break;
         case ABOVE:             lState1_str = "_NET_WM_STATE_ABOVE"; break;
         case BELOW:             lState1_str = "_NET_WM_STATE_BELOW"; break;
         case DEMANDS_ATTENTION: lState1_str = "_NET_WM_STATE_DEMANDS_ATTENTION"; break;
         case FOCUSED:           lState1_str = "_NET_WM_STATE_FOCUSED"; break;
         case NONE: return false;
            // clang-format on
      }

      lAtomNetWmStateState1_X11 = XInternAtom( vDisplay_X11, lState1_str.c_str(), True );

      if ( !lAtomNetWmStateState1_X11 ) {
         wLOG( "Failed to create X11 Atom ", lState1_str );
         return false;
      }

      lState1_str = std::regex_replace( lState1_str, lTypeRegex_EX, lReplace_C );
   }

   if ( _type2 != NONE ) {
      switch ( _type2 ) {
         case MODAL: lState2_str             = "_NET_WM_STATE_MODAL"; break;
         case STICKY: lState2_str            = "_NET_WM_STATE_STICKY"; break;
         case MAXIMIZED_VERT: lState2_str    = "_NET_WM_STATE_MAXIMIZED_VERT"; break;
         case MAXIMIZED_HORZ: lState2_str    = "_NET_WM_STATE_MAXIMIZED_HORZ"; break;
         case SHADED: lState2_str            = "_NET_WM_STATE_SHADED"; break;
         case SKIP_TASKBAR: lState2_str      = "_NET_WM_STATE_SKIP_TASKBAR"; break;
         case SKIP_PAGER: lState2_str        = "_NET_WM_STATE_SKIP_PAGER"; break;
         case HIDDEN: lState2_str            = "_NET_WM_STATE_HIDDEN"; break;
         case FULLSCREEN: lState2_str        = "_NET_WM_STATE_FULLSCREEN"; break;
         case ABOVE: lState2_str             = "_NET_WM_STATE_ABOVE"; break;
         case BELOW: lState2_str             = "_NET_WM_STATE_BELOW"; break;
         case DEMANDS_ATTENTION: lState2_str = "_NET_WM_STATE_DEMANDS_ATTENTION"; break;
         case FOCUSED: lState2_str           = "_NET_WM_STATE_FOCUSED"; break;
         case NONE: return false;
      }

      lAtomNetWmStateState2_X11 = XInternAtom( vDisplay_X11, lState2_str.c_str(), True );

      if ( !lAtomNetWmStateState2_X11 ) {
         wLOG( "Failed to create X11 Atom ", lState2_str );
         return false;
      }

      lState2_str = std::regex_replace( lState2_str, lTypeRegex_EX, lReplace_C );
   }

   if ( !sendX11Event( "_NET_WM_STATE",
                       _action,
                       ( _type1 != NONE ) ? static_cast<long>( lAtomNetWmStateState1_X11 ) : 0,
                       ( _type2 != NONE ) ? static_cast<long>( lAtomNetWmStateState2_X11 ) : 0,
                       1 ) ) {
      wLOG( lMode_STR, ' ', lState1_str, " and ", lState2_str, " mode FAILED" );
      return false;
   }

   if ( _type1 == FULLSCREEN || _type2 == FULLSCREEN ) {
      switch ( _action ) {
         case C_ADD: GlobConf.win.fullscreen    = true; break;
         case C_REMOVE: GlobConf.win.fullscreen = false; break;
         case C_TOGGLE: GlobConf.win.fullscreen = !GlobConf.win.fullscreen; break;
      }
   }

   iLOG( lMode_STR, ' ', lState1_str, " and ", lState2_str, " mode SUCCEEDED" );

   return true;
}

/*!
 * \brief Try to map the fullscreen window to all monitors
 * \returns \a true when successful and \a false when not
 */
bool iContext::fullScreenMultiMonitor() {
   if ( !vHaveGLEW_B )
      return false;

   unsigned int lLeft_I;
   unsigned int lRight_I;
   unsigned int lTop_I;
   unsigned int lBot_I;

   getMostLeftRightTopBottomCRTC( lLeft_I, lRight_I, lTop_I, lBot_I );

   if ( !sendX11Event( "_NET_WM_FULLSCREEN_MONITORS", lTop_I, lBot_I, lLeft_I, lRight_I ) ) {
      wLOG( "Unable to map the fullscreen window to all monitors" );
      return false;
   }

   iLOG( "Successfully mapped the fullscreen window to all monitors" );

   return true;
}


/*!
 * \brief Try to map the fullscreen window to display _disp
 *
 * \param _disp The display where the fullscreen window should be
 *
 * \returns 1 when succeeded
 * \returns 2 when sending the X11 event failed
 * \returns the result of iRandR::getIndexOfDisplay() when there was a failure
 */
int iContext::setFullScreenMonitor( iDisplays &_disp ) {
   if ( !vHaveGLEW_B )
      return 0;

   int lDisp_I = getIndexOfDisplay( _disp );

   if ( lDisp_I < 0 ) {
      wLOG( "No valid iDisplays [ setFullScreenMonitor(...) ] ==> Return "
            "iRandR::getIndexOfDisplay( _disp ) = ",
            lDisp_I );
      return lDisp_I;
   }

   if ( !sendX11Event( "_NET_WM_FULLSCREEN_MONITORS", lDisp_I, lDisp_I, lDisp_I, lDisp_I ) ) {
      wLOG( "Unable to map the fullscreen window to monitor ", lDisp_I );
      return 2;
   }

   iLOG( "Successfully mapped the fullscreen window to monitor ", lDisp_I );

   return 1;
}


bool iContext::sendX11Event(
      std::string _atom, int64_t _l0, int64_t _l1, int64_t _l2, int64_t _l3, int64_t _l4 ) {
   Atom lAtom_X11 = XInternAtom( vDisplay_X11, _atom.c_str(), True );

   if ( !lAtom_X11 ) {
      wLOG( "Failed to create X11 Atom ", _atom );
      return false;
   }

   XEvent lEvent_X11;

   memset( &lEvent_X11, 0, sizeof( lEvent_X11 ) );
   lEvent_X11.type           = ClientMessage;
   lEvent_X11.xclient.window = vWindow_X11;

   lEvent_X11.xclient.message_type = lAtom_X11;
   lEvent_X11.xclient.format       = 32;
   lEvent_X11.xclient.data.l[0]    = _l0;
   lEvent_X11.xclient.data.l[1]    = _l1;
   lEvent_X11.xclient.data.l[2]    = _l2;
   lEvent_X11.xclient.data.l[3]    = _l3;
   lEvent_X11.xclient.data.l[4]    = _l4;

   return XSendEvent( vDisplay_X11,
                      DefaultRootWindow( vDisplay_X11 ),
                      False,
                      SubstructureRedirectMask | SubstructureNotifyMask,
                      &lEvent_X11 );
}





/*!
 * \brief Get the \c X11 version
 * \param[out] _major The major version number
 * \param[out] _minor The minor version number
 */
void iContext::getX11Version( int *_major, int *_minor ) {
   if ( !vDisplayCreated_B ) {
      *_major = -1;
      *_minor = -1;
      return;
   }
   *_major = vX11VersionMajor_I;
   *_minor = vX11VersionMinor_I;
}



/*!
 * \brief Grabs the mouse pointer (and the keyboard)
 *
 * \note You can only grab the mouse if the mouse is ungrabbed
 *
 * \returns true if successful and false if not
 */
bool iContext::grabMouse() {
   if ( vIsMouseGrabbed_B ) {
      wLOG( "Mouse is already grabbed" );
      return false;
   }

   int lReturn_I = XGrabPointer( vDisplay_X11, // Our connection to the X server
                                 vWindow_X11,  // The window owning the grab
                                 False,        // Send some additional events
                                 ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
                                       ButtonMotionMask, // We dont need a special event mask
                                 GrabModeAsync, // Mouse grabbing should be async (easier for us)
                                 GrabModeAsync, // Key grabbing should be async (easier for us)
                                 vWindow_X11,   // Lock the cursor in this window
                                 None,          // Use the default window cursor icon
                                 CurrentTime    // X11 needs a time
                                 );

   if ( lReturn_I != GrabSuccess ) {
      wLOG( "Failed to grab the mouse" );
      return false;
   }
   vIsMouseGrabbed_B = true;
   iLOG( "Mouse grabbed" );
   return true;
}


/*!
 * \brief Ungrabs the mouse pointer (and the keyboard)
 *
 * \note You can only ungrab the mouse if it is grabbed
 *
 * \returns true if successful and false if not
 */
bool iContext::freeMouse() {
   if ( !vIsMouseGrabbed_B ) {
      wLOG( "Mouse is not grabbed" );
      return false;
   }


   if ( XUngrabPointer( vDisplay_X11, CurrentTime ) == 0 ) {
      wLOG( "Failed to ungrab the mouse" );
      return false;
   }
   vIsMouseGrabbed_B = false;
   iLOG( "Mouse ungrabbed" );
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
bool iContext::moveMouse( unsigned int _posX, unsigned int _posY ) {
   if ( _posX > GlobConf.win.width || _posY > GlobConf.win.height ) {
      wLOG( "_posX and/or _posY outside the window" );
      return false;
   }

   XWarpPointer( vDisplay_X11,              // Our connection to the X server
                 None,                      // Move it from this window (unknown)...
                 vWindow_X11,               // ...to the window
                 0,                         // We dont...
                 0,                         // ...have any...
                 0,                         // ...information about...
                 0,                         // ...the source window!
                 static_cast<int>( _posX ), // Posx in the window
                 static_cast<int>( _posY )  // Posy in the window
                 );

   return false;
}

/*!
 * \brief Get if the mouse is grabbed
 * \returns if the mouse is grabbed
 */
bool iContext::getIsMouseGrabbed() const { return vIsMouseGrabbed_B; }



/*!
 * \brief Hides the cursor
 * \returns true if successful and false if not
 */
bool iContext::hideMouseCursor() {
   if ( vIsCursorHidden_B ) {
      wLOG( "Cursor is already hidden" );
      return false;
   }

   Pixmap lNoMouseCursorPixmap_X11;
   Colormap lColorMap_X11;
   Cursor lTransparrentCursor_X11;
   XColor lBlackColor_X11, lDummyColor_X11;
   char lNoRealData_C[] = {0, 0, 0, 0, 0, 0, 0, 0};

   lColorMap_X11 = DefaultColormap( vDisplay_X11, DefaultScreen( vDisplay_X11 ) );

   XAllocNamedColor( vDisplay_X11, lColorMap_X11, "black", &lBlackColor_X11, &lDummyColor_X11 );
   lNoMouseCursorPixmap_X11 =
         XCreateBitmapFromData( vDisplay_X11, vWindow_X11, lNoRealData_C, 8, 8 );

   lTransparrentCursor_X11 = XCreatePixmapCursor( vDisplay_X11,
                                                  lNoMouseCursorPixmap_X11,
                                                  lNoMouseCursorPixmap_X11,
                                                  &lBlackColor_X11,
                                                  &lBlackColor_X11,
                                                  0,
                                                  0 );

   XDefineCursor( vDisplay_X11, vWindow_X11, lTransparrentCursor_X11 );
   XFreeCursor( vDisplay_X11, lTransparrentCursor_X11 );
   if ( lNoMouseCursorPixmap_X11 != None )
      XFreePixmap( vDisplay_X11, lNoMouseCursorPixmap_X11 );
   XFreeColors( vDisplay_X11, lColorMap_X11, &lBlackColor_X11.pixel, 1, 0 );

   iLOG( "Cursor hidden" );

   vIsCursorHidden_B = true;
   return true;
}

/*!
 * \brief Shows the cursor
 * \returns true if successful and false if not
 */
bool iContext::showMouseCursor() {
   if ( !vIsCursorHidden_B ) {
      wLOG( "Cursor is already visible" );
      return false;
   }

   XUndefineCursor( vDisplay_X11, vWindow_X11 );
   vIsCursorHidden_B = false;
   iLOG( "Cursor visible" );
   return true;
}

/*!
 * \brief Get if the cursor is hidden
 * \returns true if the cursor is hidden
 */
bool iContext::getIsCursorHidden() const { return vIsCursorHidden_B; }





} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
