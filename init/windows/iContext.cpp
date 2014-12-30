/*!
 * \file windows/iContext.cpp
 * \brief \b Classes: \a iContext
 *
 * This file contains the class \b iContext which creates
 * the window in Windows and the OpenGL context on it.
 *
 * Please note that the actualWndProc, the method for resolving
 * the window events is located within event.cpp
 *
 * \sa e_context.cpp e_iInit.cpp
 */

#include <windows.h>
#include "iContext.hpp"
#include "uLog.hpp"

namespace {

template <class T>
inline std::string numToSizeStringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = std::to_string( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}
}

namespace e_engine {

namespace windows_win32 {

namespace internal {
eWindowClassRegister CLASS_REGISTER;
}



iContext::iContext() {
   vWindowsCallbacksError_B = false;
   vHasContext_B = false;
   vHasGLEW_B = false;

   vIsCursorHidden_B = false;
   vIsMouseGrabbed_B = false;


   vWindowRecreate_B = false;
   vAThreadOwnsTheOpenGLContext_B = false;
}


/*!
 * \brief Enables VSync
 * \returns 0 No Window / OpenGL context
 * \returns 1 \c SUCCESS
 * \returns 2 Extention not supported
 * \returns 3 wglewIsSupported (main VSync function) returned ERROR_INVALID_DATA
 * \returns 4 wglewIsSupported (main VSync function) returned ERROR_DC_NOT_FOUND
 * \returns 5 wglewIsSupported (main VSync function) returned something unknown (!= 0)
 */
int iContext::enableVSync() {
   if ( !vHasContext_B )
      return 0;

   if ( wglewIsSupported( "WGL_EXT_swap_control" ) ) {
      if ( wglSwapIntervalEXT( 1 ) != TRUE ) {
         switch ( GetLastError() ) {
            case ERROR_INVALID_DATA:
               wLOG( "VSync Error [WGL] ERROR_INVALID_DATA; 1 seams to be not a good value on this "
                     "System\n==> VSync NOT enabled" );
               return 3;
            case ERROR_DC_NOT_FOUND:
               wLOG( "VSync Error [WGL] ERROR_DC_NOT_FOUND; There is no *current* OpenGL context "
                     "in this thread. Use makeContextCurrent() to fix this\n==> VSync NOT "
                     "enabled" );
               return 4;
            default:
               wLOG( "VSync Error [WGL] <UNKNOWN>; Unknown return value of glXSwapIntervalSGI\n==> "
                     "VSync NOT enabled" );
               return 5;
         }
      }

      iLOG( "VSync [WGL] enabled" );
      return 1;

   } else {
      wLOG( "VSync Error [WGL]; Extention WGL_EXT_swap_control not supported\n==> VSync NOT "
            "enabled" );
      return 2;
   }
}

/*!
 * \brief Disables VSync
 * \returns 0 No Window / OpenGL context
 * \returns 1 \c SUCCESS
 * \returns 2 Extention not supported
 * \returns 3 wglewIsSupported (main VSync function) returned ERROR_INVALID_DATA
 * \returns 4 wglewIsSupported (main VSync function) returned ERROR_DC_NOT_FOUND
 * \returns 5 wglewIsSupported (main VSync function) returned something unknown (!= 0)
 */
int iContext::disableVSync() {
   if ( !vHasContext_B )
      return 0;

   if ( wglewIsSupported( "WGL_EXT_swap_control" ) ) {
      if ( wglSwapIntervalEXT( 0 ) != TRUE ) {
         switch ( GetLastError() ) {
            case ERROR_INVALID_DATA:
               wLOG( "VSync Error [WGL] ERROR_INVALID_DATA; 0 seams to be not a good value on this "
                     "System\n==> VSync NOT disabled" );
               return 3;
            case ERROR_DC_NOT_FOUND:
               wLOG( "VSync Error [WGL] ERROR_DC_NOT_FOUND; There is no *current* OpenGL context "
                     "in this thread. Use makeContextCurrent() to fix this\n==> VSync NOT "
                     "disabled" );
               return 4;
            default:
               wLOG( "VSync Error [WGL] <UNKNOWN>; Unknown return value of glXSwapIntervalSGI\n==> "
                     "VSync NOT disabled" );
               return 5;
         }
      }

      iLOG( "VSync [WGL] disabled" );
      return 1;

   } else {
      wLOG( "VSync Error [WGL]; Extention WGL_EXT_swap_control not supported\n==> VSync NOT "
            "disabled" );
      return 2;
   }
}


void iContext::destroyContext() {
   if ( !vHasContext_B )
      return;

   iLOG( "Destroying everything" );

   glDeleteVertexArrays( 1, &vVertexArray_OGL );

   wglDeleteContext( vOpenGLContext_WGL );
   ReleaseDC( vHWND_Window_win32, vHDC_win32 );
   /*
    * DestroyWindow( vHWND_Window_win32 );
    *
    * Won't work here because:
    *
    *  1. Windows
    *  2. Must be called in the thread were the window was created
    *
    * Everything else is done in the event loop.
    */

   vHasContext_B = false;
}

/*!
 * \brief Make this context current
 * \returns true on success
 * \returns false when there was an error
 */
bool iContext::makeContextCurrent() {
   if ( !vHasContext_B ) {
      eLOG( "OpenGL context Error [WGL]; We do not have any context. Please create it with "
            "iInit::init() before you run this!" );
      return false;
   }
   if ( vAThreadOwnsTheOpenGLContext_B ) {
      eLOG( "The OpenGL Context is already in use in another or this thread! Cannot make it "
            "current now!" );
      return false;
   }
   bool lReturnVal_B = wglMakeCurrent( vHDC_win32, vOpenGLContext_WGL ) == TRUE ? true : false;
   if ( lReturnVal_B )
      vAThreadOwnsTheOpenGLContext_B = true;

   return lReturnVal_B;
}

/*!
 * \brief Make \b NO context current
 * \returns true on success
 * \returns false when there was an error
 */
bool iContext::makeNOContextCurrent() {
   if ( !vHasContext_B ) {
      eLOG( "OpenGL context Error [WGL]; We do not have any context. Please create it with "
            "iInit::init() before you run this!" );
      return false;
   }
   bool lReturnVal_B = wglMakeCurrent( NULL, NULL ) == TRUE ? true : false;
   if ( lReturnVal_B )
      vAThreadOwnsTheOpenGLContext_B = false;

   return lReturnVal_B;
}

/*!
 * \brief Returns if a OpenGL context is current for this thread
 * \returns true if a OpenGL context is current for this thread
 */
bool iContext::isAContextCurrentForThisThread() {
   return wglGetCurrentContext() == NULL ? false : true;
}


/*!
 * \brief Changes the window config
 * \param _width  The new width
 * \param _height The new height
 * \param _posX   The new X coordinate
 * \param _posY   The new Y coordinate
 * \returns The return value of \c SetWindowPos
 */
int
iContext::changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY ) {
   GlobConf.win.width = _width;
   GlobConf.win.height = _height;
   GlobConf.win.posX = _posX;
   GlobConf.win.posY = _posY;

   return SetWindowPos(
         vHWND_Window_win32, HWND_TOP, _posX, _posY, _width, _height, SWP_SHOWWINDOW );
}


/*!
 * \brief Sets the window state
 *
 * Uses SetWindowPos to set some states
 *
 * http://msdn.microsoft.com/en-us/library/windows/desktop/ms633545%28v=vs.85%29.aspx
 *
 * \param[in] _flags the uFlags
 * \param[in] _pos   the hWndInsertAfter option (default: keep)
 *
 * \returns the return falue of SetWindowPos
 */
bool iContext::setWindowState( UINT _flags, HWND _pos ) {
   if ( _pos == (HWND)1000 )
      _flags |= SWP_NOZORDER;

   return SetWindowPos( vHWND_Window_win32, _pos, 0, 0, 10, 10, _flags | SWP_NOSIZE | SWP_NOMOVE );
}


/*!
 * \brief Changes the state of the window
 *
 * \param[in] _action What to do
 * \param[in] _type1  The first thing to change
 * \param[in] _type2  The second thing to change (Default: NONE)
 *
 * \returns \c Success: \a true -- <c>Failed / not supported:</c> \a false
 *
 * \warning Only C_ADD supported; C_REMOVE and C_TOGGLE are treated as C_ADD
 *
 * \sa e_engine::ACTION, e_engine::WINDOW_ATTRIBUTE
 */
bool iContext::setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 ) {
   if ( !vHasGLEW_B )
      return false;

   if ( _type1 == _type2 ) {
      eLOG( "Changing the same attribute at the same time makes completely no sense. ==> Do "
            "nothing" );
      return false;
   }

   std::string lMode_STR;
   std::string lState1_str = "NOTHING", lState2_str = "NOTHING";
   bool lState1Supported_B = false, lState2Supported_B = false;

   switch ( _type1 ) {
      case MODAL:
         lState1_str = "MODAL";
         break;
      case STICKY:
         lState1_str = "STICKY";
         break;
      case SHADED:
         lState1_str = "SHADED";
         break;
      case SKIP_TASKBAR:
         lState1_str = "SKIP_TASKBAR";
         break;
      case SKIP_PAGER:
         lState1_str = "SKIP_PAGER";
         break;
      default:
         lState1Supported_B = true;
         break;
   }

   switch ( _type2 ) {
      case MODAL:
         lState2_str = "MODAL";
         break;
      case STICKY:
         lState2_str = "STICKY";
         break;
      case SHADED:
         lState2_str = "SHADED";
         break;
      case SKIP_TASKBAR:
         lState2_str = "SKIP_TASKBAR";
         break;
      case SKIP_PAGER:
         lState2_str = "SKIP_PAGER";
         break;
      default:
         lState2Supported_B = true;
         break;
   }

   if ( !lState1Supported_B ) {
      wLOG( "Window attribute ", lState1_str, " not suppored on Windows ==> change it to NONE, do "
                                              "(if possible) type2, and return false" );
      _type1 = NONE;
      lState1_str = "NOT_SUPPORTED";
   }

   if ( !lState2Supported_B ) {
      wLOG( "Window attribute ", lState1_str, " not suppored on Windows ==> change it to NONE, do "
                                              "(if possible) type1, and return false" );
      _type2 = NONE;
      lState2_str = "NOT_SUPPORTED";
   }

   switch ( _action ) {
      case C_REMOVE:
         lMode_STR = "Removed";
         break;
      case C_ADD:
         lMode_STR = "Enabled";
         break;
      case C_TOGGLE:
         lMode_STR = "Toggled";
         break;
      default:
         return -1;
   }

   HWND lDesktopHWND_win32 = GetDesktopWindow();
   RECT lDesktopRect_win32;

   if ( _type1 != NONE ) {
      switch ( _type1 ) {
         case HIDDEN:
            setWindowState( SWP_HIDEWINDOW );
            lState1_str = "HIDDEN";
            break;
         case FULLSCREEN:
            fullScreen( _action );
            lState1_str = "FULLSCREEN";
            break;
         case ABOVE:
            setWindowState( 0, HWND_TOP );
            lState1_str = "ABOVE";
            break;
         case BELOW:
            setWindowState( 0, HWND_BOTTOM );
            lState1_str = "BELOW";
            break;
         case DEMANDS_ATTENTION:
            lState1_str = "DEMANDS_ATTENTION";
         case FOCUSED:
            ShowWindow( vHWND_Window_win32, SW_SHOW );
            SetForegroundWindow( vHWND_Window_win32 );
            SetFocus( vHWND_Window_win32 );
            if ( lState1_str != "DEMANDS_ATTENTION" )
               lState1_str = "FOCUSED";
            break;
         case MAXIMIZED_VERT:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( GlobConf.win.width,
                                lDesktopRect_win32.bottom - lDesktopRect_win32.top,
                                GlobConf.win.posX,
                                lDesktopRect_win32.top );
            lState1_str = "MAXIMIZED_VERT";
            break;
         case MAXIMIZED_HORZ:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( lDesktopRect_win32.right - lDesktopRect_win32.left,
                                GlobConf.win.height,
                                lDesktopRect_win32.left,
                                GlobConf.win.posY );
            lState1_str = "MAXIMIZED_HORZ";
            break;
         default:
            return false;
      }
   }




   if ( _type2 != NONE ) {
      switch ( _type2 ) {
         case HIDDEN:
            setWindowState( SWP_HIDEWINDOW );
            lState2_str = "HIDDEN";
            break;
         case FULLSCREEN:
            fullScreen( _action );
            lState2_str = "FULLSCREEN";
            break;
         case ABOVE:
            setWindowState( 0, HWND_TOP );
            lState2_str = "ABOVE";
            break;
         case BELOW:
            setWindowState( 0, HWND_BOTTOM );
            lState2_str = "BELOW";
            break;
         case DEMANDS_ATTENTION:
            lState2_str = "DEMANDS_ATTENTION";
         case FOCUSED:
            ShowWindow( vHWND_Window_win32, SW_SHOW );
            SetForegroundWindow( vHWND_Window_win32 );
            SetFocus( vHWND_Window_win32 );
            if ( lState2_str != "DEMANDS_ATTENTION" )
               lState2_str = "FOCUSED";
            break;
         case MAXIMIZED_VERT:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( GlobConf.win.width,
                                lDesktopRect_win32.bottom - lDesktopRect_win32.top,
                                GlobConf.win.posX,
                                lDesktopRect_win32.top );
            lState2_str = "MAXIMIZED_VERT";
            break;
         case MAXIMIZED_HORZ:
            GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );
            changeWindowConfig( lDesktopRect_win32.right - lDesktopRect_win32.left,
                                GlobConf.win.height,
                                lDesktopRect_win32.left,
                                GlobConf.win.posY );
            lState2_str = "MAXIMIZED_HORZ";
            break;
         default:
            return false;
      }
   }

   iLOG( lMode_STR, " window attribute ", lState1_str, " and ", lState2_str );

   return lState1Supported_B && lState2Supported_B;
}


bool iContext::setDecoration( ACTION _action ) {
   bool lGlobConfOld_B = GlobConf.win.windowDecoration;

   switch ( _action ) {
      case C_ADD:
         GlobConf.win.windowDecoration = true;
         break;
      case C_REMOVE:
         GlobConf.win.windowDecoration = false;
         break;

      case C_TOGGLE:
         GlobConf.win.windowDecoration = !GlobConf.win.windowDecoration;
         break;

      default:
         eLOG( "This message is theoretically totally impossible! [bool iContext::setDecoration( "
               "ACTION _action )]" );
         return false;
   }

   if ( lGlobConfOld_B != GlobConf.win.windowDecoration ) {
      iLOG( "Window decoration ( ",
            GlobConf.win.windowDecoration ? "enabled" : "disabled",
            " ) needs window restart!" );
      vWindowRecreate_B = true;
   }

   return true;
}

int iContext::fullScreen( ACTION _action, bool _allMonitors ) {
   bool lGlobConfOld_B = GlobConf.win.fullscreen;

   switch ( _action ) {
      case C_ADD:
         GlobConf.win.fullscreen = true;
         break;
      case C_REMOVE:
         GlobConf.win.fullscreen = false;
         break;

      case C_TOGGLE:
         GlobConf.win.fullscreen = !GlobConf.win.fullscreen;
         break;

      default:
         eLOG( "This message is theoretically totally impossible! [bool iContext::setDecoration( "
               "ACTION _action )]" );
         return false;
   }

   if ( lGlobConfOld_B != GlobConf.win.fullscreen ) {
      iLOG( "Fullscreen ( ",
            GlobConf.win.fullscreen ? "enabled" : "disabled",
            " ) needs window restart!" );
      vWindowRecreate_B = true;
   }

   return 1;
}



/*!
 * \brief Grabs the mouse pointer (and the keyboard)
 *
 * \note You can only grab the mouse if it is not already grabbed by this window
 *
 * \returns true if successful and false if not
 */
bool iContext::grabMouse() {
   if ( vIsMouseGrabbed_B ) {
      wLOG( "Mouse is already grabbed" );
      return false;
   }

   RECT bounds; // The Rectangle that the Cursor will be forced to be in

   // GetWindowRect(vHWND_Window_win32, &bounds);

   // The following allows for more customization
   bounds.left = GlobConf.win.posX;
   bounds.top = GlobConf.win.posY;
   bounds.right = GlobConf.win.posX + GlobConf.win.width;
   bounds.bottom = GlobConf.win.posY + GlobConf.win.height;

   if ( ClipCursor( &bounds ) == 0 ) {
      wLOG( "Error while grabbing mouse: ", (uint64_t)GetLastError() );
      return false;
   }
   vIsMouseGrabbed_B = true;
   iLOG( "Mouse grabbed" );
   return true;
}

bool iContext::freeMouse() {
   if ( !vIsMouseGrabbed_B ) {
      wLOG( "Mouse is not grabbed" );
      return false;
   }
   if ( ClipCursor( NULL ) == 0 ) { // Reset the bounds
      wLOG( "Error while freeing mouse: ", (uint64_t)GetLastError() );
      return false;
   }
   vIsMouseGrabbed_B = false;
   iLOG( "Mouse ungrabbed" );
   return true;
}


/*!
 * \brief Get if the mouse is grabbed
 * \returns if the mouse is grabbed
 */
bool iContext::getIsMouseGrabbed() const { return vIsMouseGrabbed_B; }


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

   int result = SetCursorPos( GlobConf.win.posX + _posX, GlobConf.win.posY + _posY );

   if ( result == 0 ) {
      wLOG( "Error while setting mouse position: ", (uint64_t)GetLastError() );
      return false;
   }

   return false;
}


/*!
 * \brief Hides the cursor
 * \returns true if successful and false if not
 */
bool iContext::hideMouseCursor() {
   if ( vIsCursorHidden_B ) {
      wLOG( "Cursor is already hidden" );
      return false;
   }

   int showValue = ShowCursor( false );
   while ( showValue > -1 ) {
      showValue = ShowCursor( false );
   }

   vIsCursorHidden_B = true;
   iLOG( "Cursor hidden" );
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

   int showValue = ShowCursor( true );
   while ( showValue < 0 ) {
      showValue = ShowCursor( true );
   }

   vIsCursorHidden_B = false;
   iLOG( "Cursor visible" );
   return true;
}

/*!
 * \brief Get if the cursor is hidden
 * \returns true if the cursor is hidden
 */
bool iContext::getIsCursorHidden() const { return vIsCursorHidden_B; }




} // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
