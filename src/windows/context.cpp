/*!
 * \file context.hpp
 * \brief \b Classes: \a eContext
 *
 * This file contains the class \b eContext which creates
 * the window in Windows and the OpenGL context on it.
 *
 * \sa e_context.cpp e_eInit.cpp
 */

#include "context.hpp"
#include "log.hpp"
#include <windows.h>

namespace e_engine {


LRESULT CALLBACK eContext::initialWndProc ( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   if ( _uMsg == WM_NCCREATE ) {
      LPCREATESTRUCT lCreateStruct_win32 = reinterpret_cast<LPCREATESTRUCT> ( _lParam );
      void * lCreateParam_win32 = lCreateStruct_win32->lpCreateParams;
      eContext *this__ = reinterpret_cast<eContext *> ( lCreateParam_win32 );


      if ( this__->vHWND_win32 != 0 ) {
         // This function was already called -- this should never happen
         eLOG "Internal Error: eContext::initialWndProc was already called!!" END
         this__->destroyContext();
         this__->vWindowsCallbacksError_B = true;
      }

      this__->vHWND_win32 = _hwnd;
      SetWindowLongPtr ( _hwnd,
                         GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR> ( this__ ) );
      SetWindowLongPtr ( _hwnd,
                         GWLP_WNDPROC,
                         reinterpret_cast<LONG_PTR> ( &eContext::staticWndProc ) );
      return this__->actualWndProc ( _uMsg, _wParam, _lParam );
   }
   // if it isn't WM_NCCREATE, do something sensible and wait until
   //   WM_NCCREATE is sent
   return DefWindowProc ( _hwnd, _uMsg, _wParam, _lParam );
}

LRESULT CALLBACK eContext::staticWndProc ( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   LONG_PTR lUserData_win32 = GetWindowLongPtr ( _hwnd, GWLP_USERDATA );
   eContext * this__ = reinterpret_cast<eContext *> ( lUserData_win32 );

   if ( ! this__ || _hwnd != this__->vHWND_win32 ) {
      eLOG "Bad Windows callback error" END
      this__->destroyContext();
      this__->vWindowsCallbacksError_B = true;
   }

   return this__->actualWndProc ( _uMsg, _wParam, _lParam );
}

LRESULT CALLBACK eContext::actualWndProc ( UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   switch ( _uMsg ) {
      default:
         return DefWindowProc ( vHWND_win32, _uMsg, _wParam, _lParam );
   }
   return 0;
}


eContext::eContext() {
   vWindowsCallbacksError_B = false;
}




/*!
 * \brief Creates a Windows OpenGL context
 *
 * \returns 1  on success
 * \returns -1 when RegisterClass failed
 * \returns 5  if there was a windows callback error
 */
int eContext::createContext() {
   LPCSTR lClassName_win32 = "OGL_CLASS";
   
   DWORD  lWinStyle = WS_OVERLAPPEDWINDOW;
   DWORD  lExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
   
   vInstance_win32 = GetModuleHandle(NULL);

   vWindowClass_win32.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;  // we want a unique DC and redraw on window changes
   vWindowClass_win32.lpfnWndProc   = &eContext::initialWndProc;
   vWindowClass_win32.cbClsExtra    = 0; // We do not need this
   vWindowClass_win32.cbWndExtra    = sizeof ( eContext * );
   vWindowClass_win32.hInstance     = vInstance_win32;
   vWindowClass_win32.hIcon         = NULL;  // We dont have a special icon
   vWindowClass_win32.hCursor       = NULL;  // We dont have a special cursor
   vWindowClass_win32.hbrBackground = NULL;  // We dont need a backgrund
   vWindowClass_win32.lpszMenuName  = NULL;  // We dont want a menue
   vWindowClass_win32.lpszClassName = lClassName_win32;

   if ( !RegisterClass ( &vWindowClass_win32 ) )
      return -1;

   if ( vWindowsCallbacksError_B )
      return 5;
   
   
   vWindowRect_win32.left   = WinData.win.posX;
   vWindowRect_win32.right  = WinData.win.posX + WinData.win.width;
   vWindowRect_win32.top    = WinData.win.posY;
   vWindowRect_win32.bottom = WinData.win.posY + WinData.win.height;
   
   AdjustWindowRectEx( &vWindowRect_win32, lWinStyle, false, lExtStyle );
         
   vHWND_win32 = CreateWindowEx( 
      lExtStyle,                                          // Extended window style
      lClassName_win32,                                   // Window class name
      WinData.config.appName.c_str(),                     // Window Name
      lWinStyle,                                          // Window style
      WinData.win.posX,                                   // X
      WinData.win.posY,                                   // Y
      vWindowRect_win32.right  - vWindowRect_win32.left,  // Width
      vWindowRect_win32.bottom - vWindowRect_win32.top,   // Height
      NULL,                                               // No parent window
      NULL,                                               // No menue
      vInstance_win32,                                    // The hinstance
      NULL                                                // We dont want spacial window creation
   );
   
   
   vHDC_win32 = GetDC( vHWND_win32 ); // Get the device context
   SetPixelFormat( vHDC_win32, 1 , &vPixelFormat_PFD ); // Set a dummy Pixel format
   vOpenGLContext_WGL = wglCreateContext( vHDC_win32 ); // Create a simple OGL Context

   return 1;
}

void eContext::destroyContext() {

}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
