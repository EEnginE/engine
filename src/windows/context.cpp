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

namespace {
 
   template<class T>
   inline std::string numToSizeStringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = boost::lexical_cast<std::string> ( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

   
}

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
}

// Temp wndProc
LRESULT CALLBACK __WndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   switch ( _uMsg ) {
      default:
         break;
   }
   return DefWindowProc( _hwnd, _uMsg, _wParam, _lParam );
}

/*!
 * \brief Creates a Windows OpenGL context
 *
 * \returns -1 when RegisterClass failed
 * \returns 1  on success
 * \returns 2  if there is already a context
 * \returns 5  if there was a windows callback error
 * \returns 6  if there was an error while creating the temporary context
 * \returns 7  if there was an error while initiating GLEW
 * \returns 8  if there was no good pixel format descriptor
 */
int eContext::createContext() {
   if ( vHasContext_B )
      return 2;

   vClassName_win32             =  L"OGL_CLASS";
   LPCSTR lClassName_TEMP_win32 = "OGL_CLASS_TEMP";

   DWORD  lWinStyle = WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
   DWORD  lExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;


   HINSTANCE lInstance_TEMP_win32 = GetModuleHandle( NULL );
   WNDCLASS  lWindowClass_TEMP_win32;
   RECT      lWindowRect_TEMP_win32;
   HWND      lHWND_Window_TEMP_win32;


   if ( !e_engine_internal::CLASS_REGISTER.getC1() ) {
      lWindowClass_TEMP_win32.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;  // we want a unique DC and redraw on window changes
      lWindowClass_TEMP_win32.lpfnWndProc   = &__WndProc;
      lWindowClass_TEMP_win32.cbClsExtra    = 0; // We do not need this
      lWindowClass_TEMP_win32.cbWndExtra    = sizeof( eContext * );
      lWindowClass_TEMP_win32.hInstance     = lInstance_TEMP_win32;
      lWindowClass_TEMP_win32.hIcon         = NULL;  // We dont have a special icon
      lWindowClass_TEMP_win32.hCursor       = NULL;  // We dont have a special cursor
      lWindowClass_TEMP_win32.hbrBackground = NULL;  // We dont need a background
      lWindowClass_TEMP_win32.lpszMenuName  = NULL;  // We dont want a menu
      lWindowClass_TEMP_win32.lpszClassName = lClassName_TEMP_win32;

      if ( !RegisterClass( &lWindowClass_TEMP_win32 ) ) {
         eLOG "Failed to register the (temporary) new class" END
         return -1;
      }

      e_engine_internal::CLASS_REGISTER.setC1();
   }

   if ( vWindowsCallbacksError_B ) {
      eLOG "Problems with window callback" END
      return 5;
   }


   lWindowRect_TEMP_win32.left   = WinData.win.posX;
   lWindowRect_TEMP_win32.right  = WinData.win.posX + WinData.win.width;
   lWindowRect_TEMP_win32.top    = WinData.win.posY;
   lWindowRect_TEMP_win32.bottom = WinData.win.posY + WinData.win.height;

   AdjustWindowRectEx( &lWindowRect_TEMP_win32, lWinStyle, false, lExtStyle );

   lHWND_Window_TEMP_win32 = CreateWindowEx(
                                lExtStyle,                                          // Extended window style
                                lClassName_TEMP_win32,                              // Window class name
                                WinData.config.appName.c_str(),                     // Window Name
                                lWinStyle,                                          // Window style
                                WinData.win.posX,                                   // X
                                WinData.win.posY,                                   // Y
                                lWindowRect_TEMP_win32.right  - lWindowRect_TEMP_win32.left,  // Width
                                lWindowRect_TEMP_win32.bottom - lWindowRect_TEMP_win32.top,   // Height
                                NULL,                                               // No parent window
                                NULL,                                               // No menu
                                lInstance_TEMP_win32,                               // The instance
                                NULL                                                // We dont want special window creation
                             );


   vHDC_win32 = GetDC( lHWND_Window_TEMP_win32 );       // Get the device context
   SetPixelFormat( vHDC_win32, 1 , &vPixelFormat_PFD ); // Set a dummy Pixel format
   vOpenGLContext_WGL = wglCreateContext( vHDC_win32 ); // Create a simple OGL Context so that we can access windowsgl
   wglMakeCurrent( vHDC_win32, vOpenGLContext_WGL );    // Make the temporary context current

   if ( vHDC_win32 == 0 ) {
      eLOG "Failed to create a temporary OpenGL context! Unable to proceed!" END
      return 6;
   }

   if ( ! vHasGLEW_B ) {
      GLenum lGLEWReturn_ENUM = glewInit();
      if ( lGLEWReturn_ENUM != GLEW_OK ) {
         eLOG "Failed to init GLEW. Unable to proceed!" END
         return 7;
      }
      vHasGLEW_B = true;
   }




   // Now destroy the temporary stuff
   wglMakeCurrent( NULL, NULL ); // No context
   wglDeleteContext( vOpenGLContext_WGL );
   ReleaseDC( lHWND_Window_TEMP_win32, vHDC_win32 );
   DestroyWindow( lHWND_Window_TEMP_win32 );


   //
   // Create the actual context
   //


   vInstance_win32 = GetModuleHandle( NULL );

   if ( !e_engine_internal::CLASS_REGISTER.getC2() ) {
      vWindowClass_win32.style         = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;  // We want a unique DC and redraw on window changes
      vWindowClass_win32.lpfnWndProc   = &eContext::initialWndProc;
      vWindowClass_win32.cbClsExtra    = 0; // We do not need this
      vWindowClass_win32.cbWndExtra    = sizeof( eContext * );
      vWindowClass_win32.hInstance     = vInstance_win32;
      vWindowClass_win32.hIcon         = NULL;  // We dont have a special icon
      vWindowClass_win32.hCursor       = NULL;  // We dont have a special cursor
      vWindowClass_win32.hbrBackground = NULL;  // We dont need a background
      vWindowClass_win32.lpszMenuName  = NULL;  // We dont want a menu
      vWindowClass_win32.lpszClassName = vClassName_win32;


      if(RegisterClassW( &vWindowClass_win32 ) == 0) {
         eLOG "Failed to register the (final) window class " ADD GetLastError() END
         return -1;
      }
      

      e_engine_internal::CLASS_REGISTER.setC2();
   }

   if ( vWindowsCallbacksError_B ) {
      eLOG "Problems with window callback" END
      return 5;
   }

   vWindowRect_win32.left   = WinData.win.posX;
   vWindowRect_win32.right  = WinData.win.posX + WinData.win.width;
   vWindowRect_win32.top    = WinData.win.posY;
   vWindowRect_win32.bottom = WinData.win.posY + WinData.win.height;
   

   // Now do the same again, but this time create the actual window
   AdjustWindowRectEx( &vWindowRect_win32, lWinStyle, false, lExtStyle ); 
   vHWND_Window_win32 = CreateWindowExW( //The W  is required for it to be a Unicode window
                           lExtStyle,                                          // Extended window style
                           vClassName_win32,                                   // Window class name
                           (LPCWSTR) WinData.config.appName.c_str(),           // Window Name
                           lWinStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN ,     // Window style
                           WinData.win.posX,                                   // X
                           WinData.win.posY,                                   // Y
                           vWindowRect_win32.right  - vWindowRect_win32.left,  // Width
                           vWindowRect_win32.bottom - vWindowRect_win32.top,   // Height
                           NULL,                                               // No parent window
                           NULL,                                               // No menu
                           vInstance_win32,                                    // The instance
                           this                                                // We dont want spacial window creation
                        );

   /*!
    *\todo: Changed the vClassName_win32 and Windowname into a LPCWSTR, 
    * Changed the vWindowClass_win32 into a WNDCLASSW, 
    * used CreateWindowExW and RegisterClassW( &vWindowClass_win32 )
    * See http://technet.microsoft.com/en-ca/dd319108%28v=vs.90%29.aspx
    */
   
   
   ShowCursor( TRUE );

   vHDC_win32 = GetDC( vHWND_Window_win32 );            // Get the device context


   int lNumberOfPixelFormats_I = -10;
   
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

   int lAttributesCount[] = { WGL_NUMBER_PIXEL_FORMATS_ARB };
   int lAttributes[] = {
      // Must be true
      WGL_DRAW_TO_WINDOW_ARB,
      WGL_DOUBLE_BUFFER_ARB,
      WGL_SUPPORT_OPENGL_ARB,
      WGL_ACCELERATION_ARB,
      // Should be as big as possible
      WGL_DEPTH_BITS_ARB,
      WGL_STENCIL_BITS_ARB,
      WGL_RED_BITS_ARB,
      WGL_GREEN_BITS_ARB,
      WGL_BLUE_BITS_ARB,
      WGL_ALPHA_BITS_ARB,
      WGL_SAMPLES_ARB,
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

      int lPixelFormat[11];

   int lBestSamples_I = 0, lBestDepth = 0, lBestR_I = 0, lBestG_I = 0, lBestB_I = 0, lBestA_I = 0, lBestStencil_I = 0;
   int lBestFBConfig_I = -1;

   for ( int i = 1; i < lNumberOfPixelFormats_I; ++i ) {
      wglGetPixelFormatAttribivARB( vHDC_win32, i, 0, 11, lAttributes, lPixelFormat );
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

   if ( lBestFBConfig_I < 0 ) {
      eLOG "No suitable Pixel format descriptor found!" END
      return 8;
   }

   lEntry_LOG _ADD "   |========|=========|=======|=========|=======================|" NEWLINE NEWLINE END
   
   wglChoosePixelFormatARB( vHDC_win32, &lPixelAttributes[0], NULL, 1, &lBestFBConfig_I, ( UINT * )&lNumberOfPixelFormats_I );
   iLOG "Selected Pixel format descriptor: " ADD lBestFBConfig_I END

   SetPixelFormat( vHDC_win32, lBestFBConfig_I, &vPixelFormat_PFD );
   

   // Set new Error Handler
   GLushort version_list[][2] = {
      {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1},
      {3, 3}, {3, 2}, {3, 1}, {3, 0},
      {2, 1}, {2, 0},
      {1, 5}, {1, 4}, {1, 3}, {1, 2},
      {0, 0} // End marker
   };



   // Extension supported:
   GLint lAttributes_A_I[5];
   if (
      ( WinData.versions.glMinorVersion  < 0 || WinData.versions.glMajorVersion  < 0 ) &&
      ( WinData.versions.glMinorVersion != 0 && WinData.versions.glMajorVersion != 0 )
   ) {
      lAttributes_A_I[0] = 0;
      iLOG "No OpenGL Context options --> select the version automatically" END
   } else {
      iLOG "Trying to use OpenGL version " ADD WinData.versions.glMajorVersion ADD '.' ADD WinData.versions.glMinorVersion END
      lAttributes_A_I[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
      lAttributes_A_I[1] = WinData.versions.glMajorVersion;
      lAttributes_A_I[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
      lAttributes_A_I[3] = WinData.versions.glMinorVersion;
      lAttributes_A_I[4] = 0;
   }   

   for ( unsigned short int i = 0; version_list[i][0] != 0 || version_list[i][1] != 0; i++ ) {
      vOpenGLContext_WGL = wglCreateContextAttribsARB( vHDC_win32, 0, lAttributes_A_I );

      // Errors ?
      if ( !vOpenGLContext_WGL ) {
         // Select the next lower version
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


   wglMakeCurrent( vHDC_win32, vOpenGLContext_WGL );
   ShowWindow( vHWND_Window_win32, SW_SHOW );
   SetForegroundWindow( vHWND_Window_win32 );
   SetFocus( vHWND_Window_win32 );

   glClearColor( 0, 0, 0, 1 );
   glClear( GL_COLOR_BUFFER_BIT );
   swapBuffers();


   iLOG "Versions:"
   POINT "Engine: "
   ADD 'B', 'C', E_VERSION_MAJOR    ADD 'B', 'C', "."
   ADD 'B', 'C', E_VERSION_MINOR    ADD 'B', 'C', "."
   ADD 'B', 'C', E_VERSION_SUBMINOR ADD( E_COMMIT_IS_TAGGED ? " [RELEASE] " : " +GIT " ) ADD E_VERSION_GIT
   POINT "OpenGL: " ADD 'B', 'C', glGetString( GL_VERSION )
   POINT "GLSL:   " ADD 'B', 'C', glGetString( GL_SHADING_LANGUAGE_VERSION )
   POINT "GLEW:   " ADD 'B', 'C', glewGetString( GLEW_VERSION )
   END


   iLOG "OpenGL context created" END
   
   vHasContext_B = true;

   return 1;

//    vWindowsDestroy_B        = false;
//    vWindowsNCDestrox_B      = false;
// 
//    vHWND_Window_win32       = 0;
//    vHDC_win32               = 0;
//    vInstance_win32          = 0;
//    vOpenGLContext_WGL       = 0;
   //!\todo Merged
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

int eContext::fullScreen( ACTION _action, bool _allMonitors ) {
   bool lWinDataOld_B = WinData.win.fullscreen;

   switch ( _action ) {
      case C_ADD:
         WinData.win.fullscreen = true;
         break;
      case C_REMOVE:
         WinData.win.fullscreen = false;
         break;

      case C_TOGGLE:
         WinData.win.fullscreen = !WinData.win.fullscreen;
         break;

      default:
         eLOG "This message is theoretically totaly impossible! [bool eContext::setDecoration( ACTION _action )]" END
         return false;
   }

   if ( lWinDataOld_B != WinData.win.fullscreen )
      vWindowRecreate_B = true;

   return 1;
}






} // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
