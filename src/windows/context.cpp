/*!
 * \file windows/context.cpp
 * \brief \b Classes: \a eContext
 *
 * This file contains the class \b eContext which creates
 * the window in Windows and the OpenGL context on it.
 *
 * Please note that the actualWndProc, the method for resolving
 * the window events is located within event.cpp
 *
 * \sa e_context.cpp e_eInit.cpp
 */

#include "context.hpp"
#include "log.hpp"
#include <windows.h>

namespace e_engine {

namespace windows_win32 {

namespace e_engine_internal {
eWindowClassRegister CLASS_REGISTER;
}



eContext::eContext() {
   vWindowsCallbacksError_B = false;
   vHasContext_B            = false;
   vHasGLEW_B               = false;

   vWindowRecreate_B        = false;
   
   vWindowsDestroy_B        = false;
   vWindowsNCDestrox_B      = false;
   
   vHWND_Window_win32       = 0;
   vHDC_win32               = 0;
   vInstance_win32          = 0;
   vOpenGLContext_WGL       = 0;
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
int eContext::enableVSync() {
   if ( ! vHasContext_B )
      return 0;

   if ( wglewIsSupported( "WGL_EXT_swap_control" ) ) {
      if ( wglSwapIntervalEXT( 1 ) != TRUE ) {
         switch ( GetLastError() ) {
            case ERROR_INVALID_DATA:
               wLOG    "VSync Error [WGL] ERROR_INVALID_DATA; 1 seams to be not a good value on this System"
               NEWLINE "==> VSync NOT enabled" END
               return 3;
            case ERROR_DC_NOT_FOUND:
               wLOG    "VSync Error [WGL] ERROR_DC_NOT_FOUND; There is no *current* OpenGL context in this thread. Use makeContextCurrent() to fix this"
               NEWLINE "==> VSync NOT enabled" END
               return 4;
            default:
               wLOG    "VSync Error [WGL] <UNKNOWN>; Unknown return value of glXSwapIntervalSGI"
               NEWLINE "==> VSync NOT enabled" END
               return 5;
         }
      }

      iLOG "VSync [WGL] enabled" END
      return 1;

   } else {
      wLOG    "VSync Error [WGL]; Extention WGL_EXT_swap_control not supported"
      NEWLINE "==> VSync NOT enabled" END
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
int eContext::disableVSync() {
   if ( ! vHasContext_B )
      return 0;

   if ( wglewIsSupported( "WGL_EXT_swap_control" ) ) {
      if ( wglSwapIntervalEXT( 0 ) != TRUE ) {
         switch ( GetLastError() ) {
            case ERROR_INVALID_DATA:
               wLOG    "VSync Error [WGL] ERROR_INVALID_DATA; 0 seams to be not a good value on this System"
               NEWLINE "==> VSync NOT disabled" END
               return 3;
            case ERROR_DC_NOT_FOUND:
               wLOG    "VSync Error [WGL] ERROR_DC_NOT_FOUND; There is no *current* OpenGL context in this thread. Use makeContextCurrent() to fix this"
               NEWLINE "==> VSync NOT disabled" END
               return 4;
            default:
               wLOG    "VSync Error [WGL] <UNKNOWN>; Unknown return value of glXSwapIntervalSGI"
               NEWLINE "==> VSync NOT disabled" END
               return 5;
         }
      }

      iLOG "VSync [WGL] disabled" END
      return 1;

   } else {
      wLOG    "VSync Error [WGL]; Extention WGL_EXT_swap_control not supported"
      NEWLINE "==> VSync NOT disabled" END
      return 2;
   }
}


void eContext::destroyContext() {
   if ( ! vHasContext_B )
      return;

   iLOG "Destroying everything" END
   
   wglDeleteContext( vOpenGLContext_WGL );
   ReleaseDC( vHWND_Window_win32, vHDC_win32 );
   /*
    * DestroyWindow( vHWND_Window_win32 );
    * 
    * Wont work herer because:
    * 
    *  1. Windows
    *  2. Must be called in the thread were the window was created
    * 
    */

   vHasContext_B = false;
}

/*!
 * \brief Make this context current
 * \returns true on success
 * \returns false when there was an error
 */
bool eContext::makeContextCurrent() {
   if ( ! vHasContext_B ) {
      eLOG "OpenGL context Error [WGL]; We do not have any context. Please create it with eInit::init() before you run this!" END
      return false;
   }
   return wglMakeCurrent( vHDC_win32, vOpenGLContext_WGL ) == TRUE ? true : false;
}

/*!
 * \brief Make \b NO context current
 * \returns true on success
 * \returns false when there was an error
 */
bool eContext::makeNOContextCurrent() {
   if ( ! vHasContext_B ) {
      eLOG "OpenGL context Error [WGL]; We do not have any context. Please create it with eInit::init() before you run this!" END
      return false;
   }
   return wglMakeCurrent( NULL, NULL ) == TRUE ? true : false;
}


bool eContext::setDecoration( ACTION _action ) {
   bool lWinDataOld_B = WinData.win.windowDecoration;

   switch ( _action ) {
      case C_ADD:
         WinData.win.windowDecoration = true;
         break;
      case C_REMOVE:
         WinData.win.windowDecoration = false;
         break;

      case C_TOGGLE:
         WinData.win.windowDecoration = !WinData.win.windowDecoration;
         break;

      default:
         eLOG "This message is theoretically totaly impossible! [bool eContext::setDecoration( ACTION _action )]" END
         return false;
   }

   if ( lWinDataOld_B != WinData.win.windowDecoration )
      vWindowRecreate_B = true;

   return true;
}





} // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
