/*!
 * \file windows/iContext_window.cpp
 * \brief \b Classes: \a iContext
 *
 * This file contains the class \b iContext which creates
 * the window in Windows and the OpenGL context on it.
 *
 * Please note that the actualWndProc, the method for resolving
 * the window events is located within event.cpp
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


#include "iContext.hpp"
#include "uLog.hpp"
#include "eCMDColor.hpp"

namespace e_engine {

namespace windows_win32 {

namespace {

template <class T>
inline std::string StringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = _val;
   if ( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

template <class T>
inline std::string numToSizeStringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = std::to_string( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}
}

// Temp wndProc
LRESULT CALLBACK __WndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   switch ( _uMsg ) {
      default: break;
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
int iContext::createContext() {
   if ( vHasContext_B )
      return 2;

   vHWND_Window_win32 = 0;
   vHDC_win32 = 0;
   vInstance_win32 = 0;
   vOpenGLContext_WGL = 0;

   vClassName_win32 = L"OGL_CLASS";
   LPCSTR lClassName_TEMP_win32 = "OGL_CLASS_TEMP";

   DWORD lWinStyle;
   DWORD lExtStyle;

   if ( GlobConf.win.windowDecoration && !GlobConf.win.fullscreen ) {
      lWinStyle = WS_OVERLAPPEDWINDOW | WS_MAXIMIZEBOX | WS_SIZEBOX | WS_CAPTION | WS_SYSMENU |
                  WS_MINIMIZEBOX;
      lExtStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
   } else {
      lWinStyle = WS_POPUP;
      lExtStyle = WS_EX_APPWINDOW;
   }


   HINSTANCE lInstance_TEMP_win32 = GetModuleHandle( NULL );
   WNDCLASS lWindowClass_TEMP_win32;
   RECT lWindowRect_TEMP_win32;
   HWND lHWND_Window_TEMP_win32;


   if ( !internal::CLASS_REGISTER.getC1() ) {
      lWindowClass_TEMP_win32.style =
            CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // we want a unique DC and redraw on window changes
      lWindowClass_TEMP_win32.lpfnWndProc = &__WndProc;
      lWindowClass_TEMP_win32.cbClsExtra = 0; // We do not need this
      lWindowClass_TEMP_win32.cbWndExtra = sizeof( iContext * );
      lWindowClass_TEMP_win32.hInstance = lInstance_TEMP_win32;
      lWindowClass_TEMP_win32.hIcon = NULL;         // We dont have a special icon
      lWindowClass_TEMP_win32.hCursor = NULL;       // We dont have a special cursor
      lWindowClass_TEMP_win32.hbrBackground = NULL; // We dont need a background
      lWindowClass_TEMP_win32.lpszMenuName = NULL;  // We dont want a menu
      lWindowClass_TEMP_win32.lpszClassName = lClassName_TEMP_win32;

      if ( !RegisterClass( &lWindowClass_TEMP_win32 ) ) {
         eLOG( "Failed to register the (temporary) new class" );
         return -1;
      }

      internal::CLASS_REGISTER.setC1();
   }

   if ( vWindowsCallbacksError_B ) {
      eLOG( "Problems with window callback" );
      return 5;
   }


   lWindowRect_TEMP_win32.left = 0;
   lWindowRect_TEMP_win32.right = 640;
   lWindowRect_TEMP_win32.top = 0;
   lWindowRect_TEMP_win32.bottom = 480;

   AdjustWindowRectEx( &lWindowRect_TEMP_win32, lWinStyle, false, lExtStyle );

   lHWND_Window_TEMP_win32 = CreateWindowEx( lExtStyle,             // Extended window style
                                             lClassName_TEMP_win32, // Window class name
                                             GlobConf.config.appName.c_str(), // Window Name
                                             lWinStyle,                       // Window style
                                             0,                               // X
                                             0,                               // Y
                                             640,                             // Width
                                             480,                             // Height
                                             NULL,                            // No parent window
                                             NULL,                            // No menu
                                             lInstance_TEMP_win32,            // The instance
                                             NULL // We dont want special window creation
                                             );


   vHDC_win32 = GetDC( lHWND_Window_TEMP_win32 );      // Get the device context
   SetPixelFormat( vHDC_win32, 1, &vPixelFormat_PFD ); // Set a dummy Pixel format
   vOpenGLContext_WGL = wglCreateContext(
         vHDC_win32 ); // Create a simple OGL Context so that we can access windowsgl
   wglMakeCurrent( vHDC_win32, vOpenGLContext_WGL ); // Make the temporary context current

   if ( vHDC_win32 == 0 ) {
      eLOG( "Failed to create a temporary OpenGL context! Unable to proceed!" );
      return 6;
   }

   if ( !vHasGLEW_B ) {
      GLenum lGLEWReturn_ENUM = glewInit();
      if ( lGLEWReturn_ENUM != GLEW_OK ) {
         eLOG( "Failed to init GLEW. Unable to proceed!" );
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

   if ( !internal::CLASS_REGISTER.getC2() ) {
      vWindowClass_win32.style =
            CS_OWNDC | CS_HREDRAW | CS_VREDRAW; // We want a unique DC and redraw on window changes
      vWindowClass_win32.lpfnWndProc = &iContext::initialWndProc;
      vWindowClass_win32.cbClsExtra = 0; // We do not need this
      vWindowClass_win32.cbWndExtra = sizeof( iContext * );
      vWindowClass_win32.hInstance = vInstance_win32;
      vWindowClass_win32.hIcon = NULL;                            // We dont have a special icon
      vWindowClass_win32.hCursor = LoadCursor( NULL, IDC_ARROW ); // Take the default mouse cursor
      vWindowClass_win32.hbrBackground = NULL;                    // We dont need a background
      vWindowClass_win32.lpszMenuName = NULL;                     // We dont want a menu
      vWindowClass_win32.lpszClassName = vClassName_win32;


      if ( RegisterClassW( &vWindowClass_win32 ) == 0 ) {
         eLOG( "Failed to register the (final) window class ", (uint64_t)GetLastError() );
         return -1;
      }


      internal::CLASS_REGISTER.setC2();
   }

   if ( vWindowsCallbacksError_B ) {
      eLOG( "Problems with window callback" );
      return 5;
   }

   if ( GlobConf.win.fullscreen ) {
      HWND lDesktopHWND_win32 = GetDesktopWindow();

      if ( GetWindowRect( lDesktopHWND_win32, &vWindowRect_win32 ) == 0 ) {
         vWindowRect_win32.left = GlobConf.win.posX;
         vWindowRect_win32.right = GlobConf.win.posX + GlobConf.win.width;
         vWindowRect_win32.top = GlobConf.win.posY;
         vWindowRect_win32.bottom = GlobConf.win.posY + GlobConf.win.height;
         wLOG( "Fullscreen failed" );
      }

      ChangeDisplaySettings( NULL, CDS_FULLSCREEN );
   } else {
      vWindowRect_win32.left = GlobConf.win.posX;
      vWindowRect_win32.right = GlobConf.win.posX + GlobConf.win.width;
      vWindowRect_win32.top = GlobConf.win.posY;
      vWindowRect_win32.bottom = GlobConf.win.posY + GlobConf.win.height;
   }

   GlobConf.win.posX = vWindowRect_win32.left;
   GlobConf.win.posY = vWindowRect_win32.top;
   GlobConf.win.width = vWindowRect_win32.right - vWindowRect_win32.left;
   GlobConf.win.height = vWindowRect_win32.bottom - vWindowRect_win32.top;

   // Now do the same again, but this time create the actual window
   AdjustWindowRectEx( &vWindowRect_win32, lWinStyle, false, lExtStyle );
   std::wstring lWindowName_wstr( GlobConf.config.appName.begin(), GlobConf.config.appName.end() );
   iLOG( "Window Name: ", lWindowName_wstr );
   vHWND_Window_win32 = CreateWindowExW( // The W  is required for it to be a Unicode window
         lExtStyle,                      // Extended window style
         vClassName_win32,               // Window class name
         lWindowName_wstr.c_str(),       // Window Name (converted to a wide string)
         lWinStyle | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, // Window style
         GlobConf.win.posX,                             // X
         GlobConf.win.posY,                             // Y
         GlobConf.win.width,                            // Width
         GlobConf.win.height,                           // Height
         NULL,                                          // No parent window
         NULL,                                          // No menu
         vInstance_win32,                               // The instance
         this                                           // We dont want spacial window creation
         );

   /*!
    *\todo: Changed the vClassName_win32 and Windowname into a LPCWSTR,
    * Changed the vWindowClass_win32 into a WNDCLASSW,
    * used CreateWindowExW and RegisterClassW( &vWindowClass_win32 )
    * See http://technet.microsoft.com/en-ca/dd319108%28v=vs.90%29.aspx
    */


   ShowCursor( TRUE );

   vHDC_win32 = GetDC( vHWND_Window_win32 ); // Get the device context


   int lNumberOfPixelFormats_I = -10;

   int lAttributesCount[] = {WGL_NUMBER_PIXEL_FORMATS_ARB};
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

   std::wstring lOFF_C = eCMDColor::color( 'O', 'W' );
   std::wstring lBW_C = eCMDColor::color( 'B', 'W' );
   std::wstring lBR_C = eCMDColor::color( 'B', 'R' );
   std::wstring lBG_C = eCMDColor::color( 'B', 'G' );
   std::wstring lBB_C = eCMDColor::color( 'B', 'B' );
   std::wstring lBC_C = eCMDColor::color( 'B', 'C' );

   std::wstring lR_C = eCMDColor::color( 'O', 'R' );
   std::wstring lG_C = eCMDColor::color( 'O', 'G' );
   std::wstring lB_C = eCMDColor::color( 'O', 'B' );
   std::wstring lC_C = eCMDColor::color( 'O', 'C' );

   // clang-format off
   iLOG( "Found ", lBG_C, lNumberOfPixelFormats_I, lG_C, " pixel format descriptors:\n\n", lOFF_C,
         "   |========|=========|=======|=========|=======================|\n", lOFF_C,
         "   |   ", lBW_C, "ID",      lOFF_C,
         "   | ",   lBW_C, "Samples", lOFF_C,
         " | ",     lBW_C, "Depth",   lOFF_C,
         " | ",     lBW_C, "Stencil", lOFF_C,
         " |  ",    lBR_C, "R",       lOFF_C,
         "  -  ",   lBG_C, "G",       lOFF_C,
         "  -  ",   lBB_C, "B",       lOFF_C,
         "  -  ",   lBC_C, "A",       lOFF_C,
         "  |\n",   lOFF_C,
         "   |--------|---------|-------|---------|-----------------------|" );
   // clang-format on

   int lPixelFormat[11];

   int lBestSamples_I = 0, lBestDepth = 0, lBestR_I = 0, lBestG_I = 0, lBestB_I = 0, lBestA_I = 0,
       lBestStencil_I = 0;
   int lBestFBConfig_I = -1;

   for ( int i = 1; i < lNumberOfPixelFormats_I; ++i ) {
      wglGetPixelFormatAttribivARB( vHDC_win32, i, 0, 11, lAttributes, lPixelFormat );
      if ( lPixelFormat[0] != 1 || lPixelFormat[1] != 1 || lPixelFormat[2] != 1 ||
           lPixelFormat[3] != GlobConf.framebuffer.FBA_ACCELERATION )
         continue;

      int samples, depth, stencil, r, g, b, a;

      depth = lPixelFormat[4];
      stencil = lPixelFormat[5];
      r = lPixelFormat[6];
      g = lPixelFormat[7];
      b = lPixelFormat[8];
      a = lPixelFormat[9];
      samples = lPixelFormat[10];

      // clang-format off
      LOG( _hD, "   |  ",
           numToSizeStringLeft( i, 6, ' ' ),       "|    ",
           numToSizeStringLeft( samples, 5, ' ' ), "|   ",
           numToSizeStringLeft( depth, 4, ' ' ),   "|    ",
           numToSizeStringLeft( stencil, 5, ' ' ), "|  ",
           lR_C, numToSizeStringLeft( r, 3, ' ' ), lOFF_C, "-  ",
           lG_C, numToSizeStringLeft( g, 3, ' ' ), lOFF_C, "-  ",
           lB_C, numToSizeStringLeft( b, 3, ' ' ), lOFF_C, "-  ",
           lC_C, numToSizeStringLeft( a, 3, ' ' ), lOFF_C, "|" );
      // clang-format on

      if ( samples > lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I &&
           b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestSamples_I = samples;
      }
      if ( samples >= lBestSamples_I && depth > lBestDepth && r >= lBestR_I && g >= lBestG_I &&
           b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestDepth = depth;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r > lBestR_I && g >= lBestG_I &&
           b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestR_I = r;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g > lBestG_I &&
           b >= lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestG_I = g;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I &&
           b > lBestB_I && a >= lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestB_I = b;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I &&
           b >= lBestB_I && a > lBestA_I && stencil >= lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestA_I = a;
      }
      if ( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I &&
           b >= lBestB_I && a >= lBestA_I && stencil > lBestStencil_I ) {
         lBestFBConfig_I = i;
         lBestStencil_I = stencil;
      }
   }

   if ( lBestFBConfig_I < 0 ) {
      eLOG( "No suitable Pixel format descriptor found!" );
      return 8;
   }

   LOG( _hD, "   |========|=========|=======|=========|=======================|\n\n" );

   iLOG( "Selected Pixel format descriptor: ", lBestFBConfig_I );

   SetPixelFormat( vHDC_win32, lBestFBConfig_I, &vPixelFormat_PFD );


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
   if ( ( GlobConf.versions.glMinorVersion < 0 || GlobConf.versions.glMajorVersion < 0 ) &&
        ( GlobConf.versions.glMinorVersion != 0 && GlobConf.versions.glMajorVersion != 0 ) ) {
      lAttributes_A_I[0] = 0;
      iLOG( "No OpenGL Context options --> select the version automatically" );
   } else {
      iLOG( "Trying to use OpenGL version ",
            GlobConf.versions.glMajorVersion,
            '.',
            GlobConf.versions.glMinorVersion );
      lAttributes_A_I[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
      lAttributes_A_I[1] = GlobConf.versions.glMajorVersion;
      lAttributes_A_I[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
      lAttributes_A_I[3] = GlobConf.versions.glMinorVersion;
      lAttributes_A_I[4] = 0;
   }

   for ( unsigned short int i = 0; version_list[i][0] != 0 || version_list[i][1] != 0; i++ ) {
      vOpenGLContext_WGL = wglCreateContextAttribsARB( vHDC_win32, 0, lAttributes_A_I );

      // Errors ?
      if ( !vOpenGLContext_WGL ) {
         // Select the next lower version
         while ( ( version_list[i][0] >= lAttributes_A_I[1] &&
                   version_list[i][1] >= lAttributes_A_I[3] ) &&
                 ( version_list[i][0] != 0 || version_list[i][1] != 0 ) ) {
            i++;
         }

         wLOG( "Failed to create an OpenGl version ",
               lAttributes_A_I[1],
               '.',
               lAttributes_A_I[3],
               " context. Try to fall back to OpenGl ",
               version_list[i][0],
               '.',
               version_list[i][1] );


         lAttributes_A_I[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
         lAttributes_A_I[1] = version_list[i][0];
         lAttributes_A_I[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
         lAttributes_A_I[3] = version_list[i][1];
         lAttributes_A_I[4] = 0;

         GlobConf.versions.glMajorVersion = version_list[i][0];
         GlobConf.versions.glMinorVersion = version_list[i][1];

      } else { break; }
   }


   wglMakeCurrent( vHDC_win32, vOpenGLContext_WGL );
   ShowWindow( vHWND_Window_win32, SW_SHOW );
   SetForegroundWindow( vHWND_Window_win32 );
   SetFocus( vHWND_Window_win32 );

   glClearColor( 0, 0, 0, 1 );
   glClear( GL_COLOR_BUFFER_BIT );
   swapBuffers();

   std::wstring lC1_C = eCMDColor::color( 'B', 'C' );

   // clang-format off
   iLOG( "Versions:",
         "\n  - Engine: ", lC1_C, E_VERSION_MAJOR, ".", E_VERSION_MINOR, ".", E_VERSION_SUBMINOR,
         E_GIT_LAST_TAG_DIFF == 0 ? " [RELEASE] "
                                  : ( " +" + std::to_string( E_GIT_LAST_TAG_DIFF ) + " " ), E_VERSION_GIT,
         "\n  - OpenGL: ", lC1_C, (char *)glGetString( GL_VERSION ),
         "\n  - GLSL:   ", lC1_C, (char *)glGetString( GL_SHADING_LANGUAGE_VERSION ),
         "\n  - GLEW:   ", lC1_C, (char *)glewGetString( GLEW_VERSION ) );
   // clang-format on


   iLOG( "OpenGL context created" );

   glGenVertexArrays( 1, &vVertexArray_OGL );
   glBindVertexArray( vVertexArray_OGL );

   vHasContext_B = true;

   vWindowsDestroy_B = false;
   vWindowsNCDestrox_B = false;

   return 1;
}



} // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
