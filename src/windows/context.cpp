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

namespace {

template<class T>
inline std::string StringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR =  _val;
   if ( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

template<class T>
inline std::string numToSizeStringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = boost::lexical_cast<std::string> ( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

}

LRESULT CALLBACK eContext::initialWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   iLOG "Initial WndProc called" END
   if ( _uMsg == WM_NCCREATE ) {
      iLOG "WM_NCCREATE" END
      LPCREATESTRUCT lCreateStruct_win32 = reinterpret_cast<LPCREATESTRUCT>( _lParam );
      void *lCreateParam_win32 = lCreateStruct_win32->lpCreateParams;
      eContext *this__ = reinterpret_cast<eContext *>( lCreateParam_win32 );


      if ( this__->vHWND_Window_win32 != 0 ) {
         // This function was already called -- this should never happen
         eLOG "Internal Error: eContext::initialWndProc was already called!!" END
         this__->destroyContext();
         this__->vWindowsCallbacksError_B = true;
      }

      this__->vHWND_Window_win32 = _hwnd;
      SetWindowLongPtr( _hwnd,
                        GWLP_USERDATA,
                        reinterpret_cast<LONG_PTR>( this__ ) );
      SetWindowLongPtr( _hwnd,
                        GWLP_WNDPROC,
                        reinterpret_cast<LONG_PTR>( &eContext::staticWndProc ) );
      return this__->actualWndProc( _uMsg, _wParam, _lParam );
   }
   // if it isn't WM_NCCREATE, do something sensible and wait until
   //   WM_NCCREATE is sent
   return DefWindowProc( _hwnd, _uMsg, _wParam, _lParam );
}

LRESULT CALLBACK eContext::staticWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   iLOG "static WndProc called" END
   LONG_PTR lUserData_win32 = GetWindowLongPtr( _hwnd, GWLP_USERDATA );
   eContext *this__ = reinterpret_cast<eContext *>( lUserData_win32 );

   if ( ! this__ || _hwnd != this__->vHWND_Window_win32 ) {
      eLOG "Bad Windows callback error" END
      this__->destroyContext();
      this__->vWindowsCallbacksError_B = true;
   }

   return this__->actualWndProc( _uMsg, _wParam, _lParam );
}

LRESULT CALLBACK eContext::actualWndProc( UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   iLOG "aergerg" END
   switch ( _uMsg ) {
      default:
         return DefWindowProc( vHWND_Window_win32, _uMsg, _wParam, _lParam );
   }
   return 0;
}


eContext::eContext() {
   vWindowsCallbacksError_B = false;
   vHasContext_B            = false;
   vHasGLEW_B               = false;
}

// Temp wndProc
LRESULT CALLBACK __WndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   iLOG "Temp WndProc called" END
   switch ( _uMsg ) {
      default:
         return DefWindowProc( _hwnd, _uMsg, _wParam, _lParam );
   }
   return 0;
}

/*!
 * \brief Creates a Windows OpenGL context
 *
 * \returns 1  on success
 * \returns 2  if there is already a context
 * \returns -1 when RegisterClass failed
 * \returns 5  if there was a windows callback error
 * \returns 6  if there was an error while creating the temporary context
 * \returns 7  if there was an error while initing GLEW
 * \returns 8  if there was no good pixelformatdescriptor
 */
int eContext::createContext() {
   if ( vHasContext_B )
      return 2;

   LPCSTR lClassName_win32 = "OGL_CLASS";

   DWORD  lWinStyle = WS_OVERLAPPEDWINDOW;
   DWORD  lExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;

   vInstance_win32 = GetModuleHandle( NULL );
   
   

   vWindowClass_win32.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;  // we want a unique DC and redraw on window changes
   vWindowClass_win32.lpfnWndProc   = &__WndProc;
   vWindowClass_win32.cbClsExtra    = 0; // We do not need this
   vWindowClass_win32.cbWndExtra    = sizeof( eContext * );
   vWindowClass_win32.hInstance     = vInstance_win32;
   vWindowClass_win32.hIcon         = NULL;  // We dont have a special icon
   vWindowClass_win32.hCursor       = NULL;  // We dont have a special cursor
   vWindowClass_win32.hbrBackground = NULL;  // We dont need a backgrund
   vWindowClass_win32.lpszMenuName  = NULL;  // We dont want a menue
   vWindowClass_win32.lpszClassName = lClassName_win32;

   if ( !RegisterClass( &vWindowClass_win32 ) ) {
      eLOG "Failed to register the (temporary) new class" END
      return -1;
   }

   if ( vWindowsCallbacksError_B ) {
      eLOG "Problems with window callback" END
      return 5;
   }


   vWindowRect_win32.left   = WinData.win.posX;
   vWindowRect_win32.right  = WinData.win.posX + WinData.win.width;
   vWindowRect_win32.top    = WinData.win.posY;
   vWindowRect_win32.bottom = WinData.win.posY + WinData.win.height;

   AdjustWindowRectEx( &vWindowRect_win32, lWinStyle, false, lExtStyle );

   vHWND_Window_win32 = CreateWindowEx(
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


   vHDC_win32 = GetDC( vHWND_Window_win32 );            // Get the device context
   SetPixelFormat( vHDC_win32, 1 , &vPixelFormat_PFD ); // Set a dummy Pixel format
   vOpenGLContext_WGL = wglCreateContext( vHDC_win32 ); // Create a simple OGL Context so that wa can access wgl
   wglMakeCurrent( vHDC_win32, vOpenGLContext_WGL );    // Make the temporary context current

   if ( vHDC_win32 == 0 ) {
      eLOG "Failed to create a temporery OpenGL context! Unable to proceed!" END
      return 6;
   }

   GLenum lGLEWReturn_ENUM = glewInit();

   if ( lGLEWReturn_ENUM != GLEW_OK ) {
      eLOG "Failed to init GLEW. Unable to proceed!" END
      return 7;
   }

   vHasGLEW_B = true;
   
   
   iLOG "Versions:"
   POINT "Engine: " 
      ADD 'B', 'C', E_VERSION_MAJOR    ADD 'B', 'C', "."
      ADD 'B', 'C', E_VERSION_MINOR    ADD 'B', 'C', "."
      ADD 'B', 'C', E_VERSION_SUBMINOR ADD ( E_COMMIT_IS_TAGGED ? " [RELEASE] " : " +GIT " ) ADD E_VERSION_GIT
   POINT "OpenGL: " ADD 'B', 'C', glGetString( GL_VERSION )
   POINT "GLSL:   " ADD 'B', 'C', glGetString( GL_SHADING_LANGUAGE_VERSION )
   POINT "GLEW:   " ADD 'B', 'C', glewGetString( GLEW_VERSION )
   END
   
   

   // Now destroy the temporary stuff
   wglMakeCurrent( NULL, NULL ); // No context
   wglDeleteContext( vOpenGLContext_WGL );
   ReleaseDC( vHWND_Window_win32, vHDC_win32 );
   DestroyWindow( vHWND_Window_win32 );
   
   
   //
   // Craeate the aktuall context
   //
   
   
   vWindowClass_win32.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;  // we want a unique DC and redraw on window changes
   vWindowClass_win32.lpfnWndProc   = &eContext::initialWndProc;
   vWindowClass_win32.cbClsExtra    = 0; // We do not need this
   vWindowClass_win32.cbWndExtra    = sizeof( eContext * );
   vWindowClass_win32.hInstance     = vInstance_win32;
   vWindowClass_win32.hIcon         = NULL;  // We dont have a special icon
   vWindowClass_win32.hCursor       = NULL;  // We dont have a special cursor
   vWindowClass_win32.hbrBackground = NULL;  // We dont need a backgrund
   vWindowClass_win32.lpszMenuName  = NULL;  // We dont want a menue
   vWindowClass_win32.lpszClassName = lClassName_win32;
   
   
   if ( !RegisterClass( &vWindowClass_win32 ) ) {
      eLOG "Failed to register the (final) window class" END
      return -1;
   }

   if ( vWindowsCallbacksError_B ) {
      eLOG "Problems with window callback" END
      return 5;
   }


   // Now do the same agin, but this time with real stuff
   AdjustWindowRectEx( &vWindowRect_win32, lWinStyle, false, lExtStyle );
   vHWND_Window_win32 = CreateWindowEx(
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

   vHDC_win32 = GetDC( vHWND_Window_win32 );            // Get the device context

   int lPixelAttributes[] = {
      WGL_DRAW_TO_WINDOW_ARB,     WinData.framebuffer.FBA_DRAW_TO_WINDOW,
      WGL_DEPTH_BITS_ARB,         WinData.framebuffer.FBA_DEPTH,
      WGL_STENCIL_BITS_ARB,       WinData.framebuffer.FBA_STENCIL,
      WGL_RED_BITS_ARB,           WinData.framebuffer.FBA_RED,
      WGL_GREEN_BITS_ARB,         WinData.framebuffer.FBA_GREEN,
      WGL_BLUE_BITS_ARB,          WinData.framebuffer.FBA_BLUE,
      WGL_ALPHA_BITS_ARB,         WinData.framebuffer.FBA_ALPHA,
      WGL_ACCELERATION_ARB,       WinData.framebuffer.FBA_ACCELERATION,
      WGL_SWAP_LAYER_BUFFERS_ARB, WinData.framebuffer.FBA_DOUBLEBUFFER,
      WGL_SUPPORT_OPENGL_ARB,     WinData.framebuffer.FBA_OGL_SUPPORTED,
      0
   };

   int lNumberOfPixelFormats_I = 10;

   int lAttributesCount[] = { WGL_NUMBER_PIXEL_FORMATS_ARB };
   int lAttributes[] = {
      // Must be true
      WGL_DRAW_TO_WINDOW_ARB,
      WGL_DOUBLE_BUFFER_ARB,
      WGL_SUPPORT_OPENGL_ARB,
      WGL_ACCELERATION_ARB,
      // Sould be as big as possible
      WGL_DEPTH_BITS_ARB,
      WGL_STENCIL_BITS_ARB,
      WGL_RED_BITS_ARB,
      WGL_GREEN_BITS_ARB,
      WGL_BLUE_BITS_ARB,
      WGL_ALPHA_BITS_ARB,
      WGL_SAMPLES_ARB,
      WGL_TRANSPARENT_RED_VALUE_ARB,
      WGL_TRANSPARENT_GREEN_VALUE_ARB,
      WGL_TRANSPARENT_BLUE_VALUE_ARB,
      WGL_TRANSPARENT_ALPHA_VALUE_ARB,
   };

   wglGetPixelFormatAttribivARB( vHDC_win32, 1, 0, 1, lAttributesCount, &lNumberOfPixelFormats_I );

   iLOG "Found " ADD lNumberOfPixelFormats_I ADD " pixel format descriptors" END

   LOG_ENTRY lEntry_LOG =
      iLOG "" S_COLOR 'O' , 'W'

      ADD     "   |========|=========|=======|=========|=======================|"
      NEWLINE "   |   " ADD 'B', 'W', "ID"
      ADD     "   | "  ADD 'B', 'W', "Samples" ADD " | "   ADD 'B', 'W', "Depth"
      ADD     " | "    ADD 'B', 'W', "Stencil"
      ADD     " |  "   ADD 'B', 'R', "R"
      ADD     "  -  "  ADD 'B', 'G', "G"
      ADD     "   - "  ADD 'B', 'B', "B"
      ADD     "  -  "  ADD 'B', 'C', "A"
      ADD     "  |"
      NEWLINE "   |--------|---------|-------|---------|-----------------------|" NEWLINE _END_

      int lPixelFormat[15];

   int lBestSamples_I = 0, lBestDepth = 0, lBestR_I = 0, lBestG_I = 0, lBestB_I = 0, lBestA_I = 0, lBestStencil_I = 0;
   int lBestFBConfig_I = -1;

   for ( int i = 0; i < lNumberOfPixelFormats_I; ++i ) {
      wglGetPixelFormatAttribivARB( vHDC_win32, i, 0, 15, lAttributes, lPixelFormat );
      if ( lPixelFormat[0] != 1 || lPixelFormat[1] != 1 || lPixelFormat[2] != 1 || lPixelFormat[3] != WinData.framebuffer.FBA_ACCELERATION )
         continue;

      int samples, depth, stencil, r, g, b, a;

      depth   = lPixelFormat[ 4 ];
      stencil = lPixelFormat[ 5 ];
      r       = lPixelFormat[ 6 ];
      g       = lPixelFormat[ 7 ];
      b       = lPixelFormat[ 8 ];
      a       = lPixelFormat[ 9 ];
      samples = lPixelFormat[ 10 ];

      lEntry_LOG _ADD "   |  "
      ADD numToSizeStringLeft( i, 6, ' ' )  ADD "|    "
      ADD numToSizeStringLeft( samples, 5, ' ' )     ADD "|   "  ADD numToSizeStringLeft( depth, 4, ' ' ) ADD "|    "
      ADD numToSizeStringLeft( stencil, 5, ' ' )     ADD "|  "
      ADD 'O', 'R', numToSizeStringLeft( r, 3, ' ' ) ADD "-  "
      ADD 'O', 'G', numToSizeStringLeft( g, 3, ' ' ) ADD "-  "
      ADD 'O', 'B', numToSizeStringLeft( b, 3, ' ' ) ADD "-  "
      ADD 'O', 'C', numToSizeStringLeft( a, 3, ' ' ) ADD "|" NEWLINE _END_

      if ( samples > lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I && b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestSamples_I = samples;
      }
      if ( samples >= lBestSamples_I && depth > lBestDepth && r >= lBestR_I && g >= lBestG_I && b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestDepth = depth;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r > lBestR_I && g >= lBestG_I && b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestR_I = r;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g > lBestG_I && b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestG_I = g;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I && b > lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestB_I = b;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I && b >= lBestB_I && a > lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestA_I = a;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I && b >= lBestB_I && a >= lBestA_I && stencil > lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestStencil_I = stencil;
      }
   }
   
   if( lBestFBConfig_I < 0 ) {
      eLOG "No suitable Pixelformatdescriptor found!" END
      return 8;
   }

   lEntry_LOG _ADD "   |========|=========|=======|=========|=======================|" NEWLINE NEWLINE END

   iLOG "Selected Pixel format descriptor: " ADD lBestFBConfig_I END

   // Set new Error Handler
   GLushort version_list[][2] = {
      {4, 6},
      {4, 5},
      {4, 4},
      {4, 3},
      {4, 2},
      {4, 1},
      {3, 3},
      {3, 2},
      {3, 1},
      {3, 0},
      {2, 1},
      {2, 0},
      {1, 5},
      {1, 4},
      {1, 3},
      {1, 2},
      {0, 0} // End marker
   };



   // Extension supported:
   GLint lAttributes_A_I[5];
   if (
      ( WinData.versions.glMinorVersion  < 0 || WinData.versions.glMajorVersion  < 0 ) &&
      ( WinData.versions.glMinorVersion != 0 && WinData.versions.glMajorVersion != 0 )
   ) {
      lAttributes_A_I[0] = 0;
      iLOG "No OpenGL Context options --> selct the version automatically" END
   } else {
      iLOG "Try to use OpenGL version " ADD WinData.versions.glMajorVersion ADD '.' ADD WinData.versions.glMinorVersion END
      lAttributes_A_I[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
      lAttributes_A_I[1] = WinData.versions.glMajorVersion;
      lAttributes_A_I[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
      lAttributes_A_I[3] = WinData.versions.glMinorVersion;
      lAttributes_A_I[4] = 0;
   }
   
   wglChoosePixelFormatARB(vHDC_win32, &lPixelAttributes[0], NULL, 1, &lBestFBConfig_I, (UINT*)&lNumberOfPixelFormats_I);
   
   SetPixelFormat( vHDC_win32, lBestFBConfig_I, &vPixelFormat_PFD );

   for ( unsigned short int i = 0; version_list[i][0] != 0 || version_list[i][1] != 0; i++ ) {
      vOpenGLContext_WGL = wglCreateContextAttribsARB( vHDC_win32, 0, lAttributes_A_I );

      // Errors ?
      if ( !vOpenGLContext_WGL ) {
         // Select the next lower vrsion
         while (
            ( version_list[i][0] >= lAttributes_A_I[1] && version_list[i][1] >= lAttributes_A_I[3] ) &&
            ( version_list[i][0] != 0 || version_list[i][1] != 0 )
         ) {i++;}

         wLOG "Failed to create an OpenGl version "   ADD lAttributes_A_I[1] ADD '.' ADD lAttributes_A_I[3]
         ADD  " context. Try to fall back to OpenGl " ADD version_list[i][0] ADD '.' ADD version_list[i][1] END


         lAttributes_A_I[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
         lAttributes_A_I[1] = version_list[i][0];
         lAttributes_A_I[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
         lAttributes_A_I[3] = version_list[i][1];
         lAttributes_A_I[4] = 0;

      } else {
         break;
      }
   }
   WinData.versions.glMajorVersion = lAttributes_A_I[1];
   WinData.versions.glMinorVersion = lAttributes_A_I[3];
   
   makeContextCurrent();
   
   ShowWindow( vHWND_Window_win32, SW_SHOW );
   SetForegroundWindow( vHWND_Window_win32 );
   SetFocus( vHWND_Window_win32 );
   
   iLOG "OpenGL context created" END


   vHasContext_B = true;

   return 1;
}


/*!
 * \brief Enables Vsync
 * \returns 0 (No Window)
 * \returns 1 ( \c SUCCESS )
 * \returns 2 ( Extention not supported )
 */
int eContext::enableVSync() {
   if( ! vHasGLEW_B )
      return 0;
   
   if ( wglewIsSupported( "WGL_EXT_swap_control" ) ) {
      wglSwapIntervalEXT( 1 );
      iLOG "VSync enabled" END
      return 1;
   } else {
      wLOG "Extention WGL_EXT_swap_control not supported --> no VSync" END
      return 2;
   }
}

/*!
 * \brief Disables Vsync
 * \returns 0 (No Window)
 * \returns 1 ( \c SUCCESS )
 * \returns 2 ( Extention not supported )
 */
int eContext::disableVSync() {
  if( ! vHasGLEW_B )
      return 0;
   
   if ( wglewIsSupported( "WGL_EXT_swap_control" ) ) {
      wglSwapIntervalEXT( 0 );
      iLOG "VSync disabled" END
      return 1;
   } else {
      wLOG "Extention WGL_EXT_swap_control not supported --> no VSync" END
      return 2;
   }
}


void eContext::destroyContext() {

}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
