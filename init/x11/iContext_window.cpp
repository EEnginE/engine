/*!
 * \file x11/iContext_window.cpp
 * \brief \b Classes: \a iContext
 *
 * This file contains the definitions of the context creation
 * class iContext, which does \b Not need \c GLEW. The
 * includation of glxew.h would overwrite all functions from
 * glx.h but doesn't reimplement them until \c GLEW is initiated,
 * which is impossible until a valid OpenGL context is created
 *
 * \sa e_context.hpp e_context.cpp e_iInit.hpp
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

#include "iContext.hpp"
#include <X11/Xatom.h>
#include "uLog.hpp"
#include <boost/regex.hpp>
#include "eCMDColor.hpp"

/* Don't need this for icon anymore
#define cimg_vDisplay_X11 0
#define cimg_use_png
#include <CImg.h>
*/

namespace {

template<class T>
inline std::string StringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR =  _val;
   if( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

template<class T>
inline std::string numToSizeStringLeft( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = boost::lexical_cast<std::string> ( _val );
   if( _size > lResult_STR.size() )
      lResult_STR.append( ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

}

namespace e_engine {

namespace unix_x11 {

Atom atom_wmDeleteWindow;

// Open a vDisplay_X11 ############################################################################################################# ###
int iContext::createDisplay() {
   // Needed for XSetWMProperties
   vSizeHints_X11  = XAllocSizeHints();
   vWmHints_X11    = XAllocWMHints();
   vDisplay_X11    = XOpenDisplay( NULL );
   if( vDisplay_X11 == 0 ) {
      eLOG( "Can not connect to the X-Server. Abort. (return -1)" );
      return -1;
   }
   vDisplayCreated_B  = true;

   vX11VersionMajor_I = ProtocolVersion( vDisplay_X11 );
   vX11VersionMinor_I = ProtocolRevision( vDisplay_X11 );
   vScreen_X11        = DefaultScreen( vDisplay_X11 );
   // GLX version test
   if( !glXQueryVersion( vDisplay_X11, &vGLXVersionMajor_I, &vGLXVersionMinor_I ) || ( vGLXVersionMajor_I < GlobConf.versions.minGlxMajorVer )
         || ( vGLXVersionMajor_I == GlobConf.versions.minGlxMajorVer && vGLXVersionMinor_I < GlobConf.versions.minGlxMinorVer ) ) {


      eLOG(
            "This is GLX Version "  , vGLXVersionMajor_I             , '.', vGLXVersionMinor_I             ,
            " but min. GLX Version ", GlobConf.versions.minGlxMajorVer, '.', GlobConf.versions.minGlxMinorVer,
            " is required. Abort. (return -2)"
      );
      return -2;

   }
   return 1;
}

// Look for matching fbconfigs ################################################################################################# ###
int iContext::createFrameBuffer() {
   int fbAttributes[] = {
      GLX_RENDER_TYPE,      GlobConf.framebuffer.FBA_RENDER_TYPE,
      GLX_X_RENDERABLE,     GlobConf.framebuffer.FBA_RENDERABLE,
      GLX_DRAWABLE_TYPE,    GlobConf.framebuffer.FBA_DRAWABLE_TYPE,
      GLX_DOUBLEBUFFER,     GlobConf.framebuffer.FBA_DOUBLEBUFFER,
      GLX_RED_SIZE,         GlobConf.framebuffer.FBA_RED,
      GLX_GREEN_SIZE,       GlobConf.framebuffer.FBA_GREEN,
      GLX_BLUE_SIZE,        GlobConf.framebuffer.FBA_BLUE,
      GLX_ALPHA_SIZE,       GlobConf.framebuffer.FBA_ALPHA,
      GLX_DEPTH_SIZE,       GlobConf.framebuffer.FBA_DEPTH,
      GLX_STENCIL_SIZE,     GlobConf.framebuffer.FBA_STENCIL,
      GLX_X_VISUAL_TYPE,    GlobConf.framebuffer.FBA_VISUAL_TYPE,
      //GLX_STEREO,           FBA_STEREO,
      0
   };

   // Get the config
   vFBConfig_GLX = glXChooseFBConfig( vDisplay_X11, vScreen_X11, fbAttributes, &vNumOfFBConfigs_I );
   if( vNumOfFBConfigs_I == 0 ) {
      eLOG( "No matching framebufferconfig found. Abort. (return -3)" );
      return -3;
   }

   vBestFBConfig_I = -1;
   int lBestSamples_I = -1, lBestDepth = -1 , lBestR_I = -1, lBestG_I = -1, lBestB_I = -1, lBestA_I = -1, i;

   std::wstring lOFF_C = eCMDColor::color( 'O', 'W' );
   std::wstring lBW_C  = eCMDColor::color( 'B', 'W' );
   std::wstring lBR_C  = eCMDColor::color( 'B', 'R' );
   std::wstring lBG_C  = eCMDColor::color( 'B', 'G' );
   std::wstring lBB_C  = eCMDColor::color( 'B', 'B' );
   std::wstring lBC_C  = eCMDColor::color( 'B', 'C' );

   std::wstring lR_C   = eCMDColor::color( 'O', 'R' );
   std::wstring lG_C   = eCMDColor::color( 'O', 'G' );
   std::wstring lB_C   = eCMDColor::color( 'O', 'B' );
   std::wstring lC_C   = eCMDColor::color( 'O', 'C' );

   iLOG( "Found ", lBG_C, vNumOfFBConfigs_I, lG_C, " framebufferconfigs:\n\n",
         lOFF_C,
         "   |=========|========|=========|=======|=========|=======================|\n", lOFF_C,
         "   | ",  lBW_C, "NUMBER",  lOFF_C, "  |   ", lBW_C, "ID",    lOFF_C,
         "   | " , lBW_C, "Samples", lOFF_C, " | "  ,  lBW_C, "Depth", lOFF_C,
         " | "   , lBW_C, "Stencil", lOFF_C,
         " |  "  , lBR_C, "R", lOFF_C,
         "  -  " , lBG_C, "G", lOFF_C,
         "  -  " , lBB_C, "B", lOFF_C,
         "  -  " , lBC_C, "A", lOFF_C,
         "  |\n" , lOFF_C,
         "   |---------|--------|---------|-------|---------|-----------------------|"

       );
   for( i = 0; i < vNumOfFBConfigs_I; i++ ) {
      XVisualInfo *temp = glXGetVisualFromFBConfig( vDisplay_X11, vFBConfig_GLX[i] );
      if( temp ) {
         char lBuffer_C[5];
         std::snprintf( lBuffer_C, 4, "%X", ( GLuint ) temp->visualid );
         int samples, depth, stencil, r, g, b, a;

         glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[i], GLX_SAMPLES,      &samples );
         glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[i], GLX_STENCIL_SIZE, &stencil );
         glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[i], GLX_RED_SIZE,     &r );
         glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[i], GLX_GREEN_SIZE,   &g );
         glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[i], GLX_BLUE_SIZE,    &b );
         glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[i], GLX_ALPHA_SIZE,   &a );
         // glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[i], GLX_DEPTH_SIZE, &depth ); This returns the depth SIZE and NOT the depth
         depth = temp->depth;

         LOG(
               _hD, "   |    ",
               numToSizeStringLeft( i, 5, ' ' ),       "| "
               "0x0", StringLeft( lBuffer_C, 4, ' ' ), "|    ",
               numToSizeStringLeft( samples, 5, ' ' ), "|   ",
               numToSizeStringLeft( depth, 4, ' ' ), "|    ",
               numToSizeStringLeft( stencil, 5, ' ' ), "|  ",
               lR_C, numToSizeStringLeft( r, 3, ' ' ), lOFF_C, "-  ",
               lG_C, numToSizeStringLeft( g, 3, ' ' ), lOFF_C, "-  ",
               lB_C, numToSizeStringLeft( b, 3, ' ' ), lOFF_C, "-  ",
               lC_C, numToSizeStringLeft( a, 3, ' ' ), lOFF_C, "|"
         );
         if( samples > lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I && b >= lBestB_I && a >= lBestA_I ) {
            vBestFBConfig_I = i;
            lBestSamples_I = samples;
         }
         if( samples >= lBestSamples_I && depth > lBestDepth && r >= lBestR_I && g >= lBestG_I && b >= lBestB_I && a >= lBestA_I ) {
            vBestFBConfig_I = i;
            lBestDepth = depth;
         }
         if( samples >= lBestSamples_I && depth >= lBestDepth && r > lBestR_I && g >= lBestG_I && b >= lBestB_I && a >= lBestA_I ) {
            vBestFBConfig_I = i;
            lBestR_I = r;
         }
         if( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g > lBestG_I && b >= lBestB_I && a >= lBestA_I ) {
            vBestFBConfig_I = i;
            lBestG_I = g;
         }
         if( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I && b > lBestB_I && a >= lBestA_I ) {
            vBestFBConfig_I = i;
            lBestB_I = b;
         }
         if( samples >= lBestSamples_I && depth >= lBestDepth && r >= lBestR_I && g >= lBestG_I && b >= lBestB_I && a > lBestA_I ) {
            vBestFBConfig_I = i;
            lBestA_I = a;
         }
      }
      XFree( temp );
   }
   LOG( _hD, "   |=========|========|=========|=======|=========|=======================|\n\n" );
#if E_DEBUG_LOGGING

   int lFBAttribs_A_I[8];

   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_X_RENDERABLE,  &lFBAttribs_A_I[0] );
   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_DOUBLEBUFFER,  &lFBAttribs_A_I[1] );
   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_RENDER_TYPE,   &lFBAttribs_A_I[2] );
   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_X_VISUAL_TYPE, &lFBAttribs_A_I[3] );
   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_CONFIG_CAVEAT, &lFBAttribs_A_I[4] );
   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_DRAWABLE_TYPE, &lFBAttribs_A_I[5] );
   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_STEREO,        &lFBAttribs_A_I[6] );
   glXGetFBConfigAttrib( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_LEVEL,         &lFBAttribs_A_I[7] );

   dLOG(
         "Adiditional framebuffer info:  (should be) [would be REALY helpful to be] {MUST BE}",
         "\n  - GLX_X_RENDERABLE   {", GL_TRUE       , "}      - ", lFBAttribs_A_I[0],
         "\n  - GLX_DOUBLEBUFFER   {", GL_TRUE       , "}      - ", lFBAttribs_A_I[1],
         "\n  - GLX_RENDER_TYPE    [", GLX_RGBA_BIT  , "]      - ", lFBAttribs_A_I[2],
         "\n  - GLX_X_VISUAL_TYPE  [", GLX_TRUE_COLOR, "]  - ",     lFBAttribs_A_I[3],
         "\n  - GLX_CONFIG_CAVEAT  [", GLX_NONE      , "]  - ",     lFBAttribs_A_I[4], "  ( ", GLX_NON_CONFORMANT_CONFIG, " - GLX_NON_CONFORMANT_CONFIG is also OK )",
         "\n  - GLX_DRAWABLE_TYPE  (", GLX_WINDOW_BIT, ")      - ", lFBAttribs_A_I[5],
         "\n  - GLX_STEREO          ", " "           , "       - ", lFBAttribs_A_I[6],
         "\n  - GLX_LEVEL           ", " "           , "       - ", lFBAttribs_A_I[7]
   );

#endif // E_DEBUG_LOGGING

   vVisualInfo_X11 = glXGetVisualFromFBConfig( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I] );  // Choose best fbconfig

   char lBuffer_C[5];
   std::snprintf( lBuffer_C, 4, "%X", ( GLuint ) vVisualInfo_X11->visualid );

   iLOG( "Use framebufferconfig ", vBestFBConfig_I, " ( 0x0", lBuffer_C, " )" );

   vRootWindow_X11 = RootWindow( vDisplay_X11, vVisualInfo_X11->screen );

   return 1;
}

// Create the X-Window ######################################################################################################## ###
int iContext::createWindow() {
   vWindowAttributes_X11.event_mask        = vEventMask_lI;

   vWindowAttributes_X11.border_pixel      = 0;
   vWindowAttributes_X11.bit_gravity       = NorthWestGravity;
   vWindowAttributes_X11.background_pixmap = None ;
   vWindowAttributes_X11.colormap = vColorMap_X11   = XCreateColormap( vDisplay_X11, vRootWindow_X11, vVisualInfo_X11->visual, AllocNone );
   vWindowMask_X11 = CWBitGravity | CWEventMask | CWColormap | CWBorderPixel | CWBackPixmap;

   vColorMapCreated_B = true;

   vWindow_X11 = XCreateWindow( vDisplay_X11,                                     // Display
         vRootWindow_X11,                                  // Root window
         GlobConf.win.posX, GlobConf.win.posY,          // X, Y
         GlobConf.win.width, GlobConf.win.height,       // Width, Height
         0,                                           // Border width
         vVisualInfo_X11->depth,                           // Depth
         InputOutput,                                 // Type of the window
         vVisualInfo_X11->visual,                          // Visual of the window
         vWindowMask_X11,                                  // Window mask
         &vWindowAttributes_X11 );                         // Window attributes structure
   if( vWindow_X11 == 0 ) {
      eLOG( "Failed to create a Window. Abort. (return -4)" );
      return -4;
   }

   // Set the window type
   std::string lWindowType_str;
   switch( GlobConf.win.winType ) {
      case DESKTOP:       lWindowType_str = "_NET_WM_WINDOW_TYPE_DESKTOP";       break;
      case DOCK:          lWindowType_str = "_NET_WM_WINDOW_TYPE_DOCK";          break;
      case TOOLBAR:       lWindowType_str = "_NET_WM_WINDOW_TYPE_TOOLBAR";       break;
      case MENU:          lWindowType_str = "_NET_WM_WINDOW_TYPE_MENU";          break;
      case UTILITY:       lWindowType_str = "_NET_WM_WINDOW_TYPE_UTILITY";       break;
      case SPLASH:        lWindowType_str = "_NET_WM_WINDOW_TYPE_SPLASH";        break;
      case DIALOG:        lWindowType_str = "_NET_WM_WINDOW_TYPE_DIALOG";        break;
      case DROPDOWN_MENU: lWindowType_str = "_NET_WM_WINDOW_TYPE_DROPDOWN_MENU"; break;
      case POPUP_MENU:    lWindowType_str = "_NET_WM_WINDOW_TYPE_POPUP_MENU";    break;
      case TOOLTIP:       lWindowType_str = "_NET_WM_WINDOW_TYPE_TOOLTIP";       break;
      case NOTIFICATION:  lWindowType_str = "_NET_WM_WINDOW_TYPE_NOTIFICATION";  break;
      case COMBO:         lWindowType_str = "_NET_WM_WINDOW_TYPE_COMBO";         break;
      case DND:           lWindowType_str = "_NET_WM_WINDOW_TYPE_DND";           break;
      default:            lWindowType_str = "_NET_WM_WINDOW_TYPE_NORMAL";        break;
   }

   // The Atoms needed
   Atom lWindowType_atom      = XInternAtom( vDisplay_X11, "_NET_WM_WINDOW_TYPE", True );
   Atom lWhichWindowType_atom = XInternAtom( vDisplay_X11, lWindowType_str.c_str(), True );

   if( ! lWindowType_atom )
      wLOG( "Failed to create X11 Atom _NET_WM_WINDOW_TYPE" );

   if( ! lWhichWindowType_atom )
      wLOG( "Failed to create X11 Atom ", lWindowType_str );


   if( lWindowType_atom && lWhichWindowType_atom ) {
      boost::regex lTypeRegex_EX( "^_[A-Z_]+_" );
      const char  *lReplace_C = "";

      // Apply window type
      if( ! XChangeProperty( vDisplay_X11,
            vWindow_X11,
            lWindowType_atom,
            XA_ATOM,
            32,
            PropModeReplace,
            ( unsigned char * ) &lWhichWindowType_atom,
            1 )
        ) {
         wLOG( "Failed to set the window type to ", boost::regex_replace( lWindowType_str, lTypeRegex_EX, lReplace_C )
               ,  " ( XChangeProperty( ..., _NET_WM_WINDOW_TYPE, ", lWindowType_str, ", ...);" );
      } else {
         iLOG( "Successfully set the Window type to ", boost::regex_replace( lWindowType_str, lTypeRegex_EX, lReplace_C ) );
      }
   }

   char *lWinNameTemp_CSTR = ( char * ) GlobConf.win.windowName.c_str();
   char *lIcoNameTemp_CSTR = ( char * ) GlobConf.win.iconName.c_str();
   XStringListToTextProperty( &lWinNameTemp_CSTR, 1, &vWindowNameTP_X11 );
   XStringListToTextProperty( &lIcoNameTemp_CSTR, 1, &vWindowIconTP_X11 );

   vSizeHints_X11->flags       = PPosition | PSize | PMinSize;// | IconPixmapHint | IconMaskHint;
   vSizeHints_X11->min_width   = GlobConf.win.minWidth;
   vSizeHints_X11->min_height  = GlobConf.win.minHeight;

   vWmHints_X11 = XAllocWMHints();
   vWmHints_X11->flags         = StateHint | InputHint;
   vWmHints_X11->initial_state = NormalState;
   vWmHints_X11->input         = True;

   //createIconPixmap();

   XSetWMName( vDisplay_X11,      vWindow_X11, &vWindowNameTP_X11 );
   XSetWMIconName( vDisplay_X11,  vWindow_X11, &vWindowIconTP_X11 );
   XSetWMHints( vDisplay_X11,     vWindow_X11,  vWmHints_X11 );
   XSetNormalHints( vDisplay_X11, vWindow_X11,  vSizeHints_X11 );

   // When user presses the [x] Button the window doesnt close
   atom_wmDeleteWindow = XInternAtom( vDisplay_X11, "WM_DELETE_WINDOW", True );

   if( atom_wmDeleteWindow )
      XSetWMProtocols( vDisplay_X11, vWindow_X11, &atom_wmDeleteWindow, 1 );
   else
      wLOG( "Failed to create X11 Atom WM_DELETE_WINDOW" );

   XMapWindow( vDisplay_X11, vWindow_X11 );


   vWindowCreated_B = true;

   XFree( vWmHints_X11 );
   XFree( vVisualInfo_X11 );  // Not needed anymore

   for( ;; ) {          // Wait until window is mapped
      XEvent e;
      XNextEvent( vDisplay_X11, &e );
      if( e.type == MapNotify ) break;
   }

   XFlush( vDisplay_X11 );

   return 1;
}

// Create OpenGL Context ###################################################################################################### ###
// Error handler for the X-Server so it doesn't exit the program
static bool gContextErrorOccoured_B = false;
static int contextERROR_HANDLE( Display *dpy, XErrorEvent *event ) {
   gContextErrorOccoured_B = true;
   return 0;
}
int iContext::createOGLContext() {
// Set new Error Handler
   GLushort version_list[][2] = {
      {4, 6}, {4, 5}, {4, 4}, {4, 3}, {4, 2}, {4, 1},
      {3, 3}, {3, 2}, {3, 1}, {3, 0},
      {2, 1}, {2, 0},
      {1, 5}, {1, 4}, {1, 3}, {1, 2},
      {0, 0} // End marker
   };

   if( ! vHaveGLEW_B ) {
      // Make the function pointer
      glXCreateContextAttribsARB = 0;
      glXCreateContextAttribsARB = ( glXCreateContextAttribsARBProc ) glXGetProcAddressARB( ( GLubyte * ) "glXCreateContextAttribsARB" );
   }

   gContextErrorOccoured_B = false;
   int ( *oldHandler )( Display *, XErrorEvent * ) = XSetErrorHandler( &contextERROR_HANDLE );

   if( !isExtensionSupported( "GLX_ARB_create_context" ) || !glXCreateContextAttribsARB ) {
      // Extension not supported:
      wLOG( "glXCreateContextAttribsARB not found => Fall back to old-style context creation" );

      vOpenGLContext_GLX = glXCreateNewContext( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], GLX_RGBA_TYPE, 0, true );
   } else {

      // Extension supported:
      GLint lAttributes_A_I[5];
      if(
            ( GlobConf.versions.glMinorVersion  < 0 || GlobConf.versions.glMajorVersion  < 0 ) &&
            ( GlobConf.versions.glMinorVersion != 0 && GlobConf.versions.glMajorVersion != 0 )
      ) {
         lAttributes_A_I[0] = 0;
         iLOG( "No OpenGL Context options --> selct the version automatically" );
      } else {
         iLOG( "Try to use OpenGL version ", GlobConf.versions.glMajorVersion, '.', GlobConf.versions.glMinorVersion );
         lAttributes_A_I[0] = GLX_CONTEXT_MAJOR_VERSION_ARB;
         lAttributes_A_I[1] = GlobConf.versions.glMajorVersion;
         lAttributes_A_I[2] = GLX_CONTEXT_MINOR_VERSION_ARB;
         lAttributes_A_I[3] = GlobConf.versions.glMinorVersion;
         lAttributes_A_I[4] = 0;
      }

      for( unsigned short int i = 0; version_list[i][0] != 0 || version_list[i][1] != 0; i++ ) {
         vOpenGLContext_GLX = glXCreateContextAttribsARB( vDisplay_X11, vFBConfig_GLX[vBestFBConfig_I], 0, true, lAttributes_A_I );

         // Errors ?
         XSync( vDisplay_X11, false );
         if( gContextErrorOccoured_B == true || !vOpenGLContext_GLX ) {
            // Select the next lower version
            while(
                  ( version_list[i][0] >= lAttributes_A_I[1] && version_list[i][1] >= lAttributes_A_I[3] ) &&
                  ( version_list[i][0] != 0 || version_list[i][1] != 0 )
            ) {i++;}

            wLOG( "Failed to create an OpenGl version "  , lAttributes_A_I[1], '.', lAttributes_A_I[3]
                  ,  " context. Try to fall back to OpenGl ", version_list[i][0], '.', version_list[i][1] );

            gContextErrorOccoured_B = false;

            lAttributes_A_I[0] = GLX_CONTEXT_MAJOR_VERSION_ARB;
            lAttributes_A_I[1] = version_list[i][0];
            lAttributes_A_I[2] = GLX_CONTEXT_MINOR_VERSION_ARB;
            lAttributes_A_I[3] = version_list[i][1];
            lAttributes_A_I[4] = 0;

            GlobConf.versions.glMajorVersion = version_list[i][0];
            GlobConf.versions.glMinorVersion = version_list[i][1];

         } else {
            break;
         }
      }
   }

   XSync( vDisplay_X11, false );
   if( gContextErrorOccoured_B == true || !vOpenGLContext_GLX ) {
      eLOG( "Failed to create a context. Abrobt. (return 3)" );
      return 3;
   }
   XSetErrorHandler( oldHandler );
   glXMakeCurrent( vDisplay_X11, vWindow_X11, vOpenGLContext_GLX );
   XFlush( vDisplay_X11 );

   vHaveContext_B = true;

   XFree( vFBConfig_GLX );    // Framebufferconfig is not need anymore

   return 1;
}

} // unix_x11

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; ;


