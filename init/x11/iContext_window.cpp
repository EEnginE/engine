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

#include <vulkan/vulkan.h>
#include <vulkan/vk_icd.h>

#include "iContext.hpp"
#include <X11/Xatom.h>
#include "uLog.hpp"
#include <regex>
#include "eCMDColor.hpp"

/* Don't need this for icon anymore
#define cimg_vDisplay_X11 0
#define cimg_use_png
#include <CImg.h>
*/

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

namespace e_engine {

namespace unix_x11 {

int iContext::initVulkan() {
   VkResult lResult;
   uint32_t lPorpCount;

   lResult = vkEnumerateInstanceLayerProperties( &lPorpCount, NULL );

   iLOG( "lResult:    ", lResult );
   iLOG( "lPorpCount: ", lPorpCount );

   return 0;
}

#if 0

Atom atom_wmDeleteWindow;

// Open a vDisplay_X11
// #################################################################################################
int iContext::createDisplay() {
   // Needed for XSetWMProperties
   vSizeHints_X11 = XAllocSizeHints();
   vWmHints_X11   = XAllocWMHints();
   vDisplay_X11 = XOpenDisplay( nullptr );
   if ( vDisplay_X11 == nullptr ) {
      eLOG( "Can not connect to the X-Server. Abort. (return -1)" );
      return -1;
   }
   vDisplayCreated_B = true;

   vX11VersionMajor_I = ProtocolVersion( vDisplay_X11 );
   vX11VersionMinor_I = ProtocolRevision( vDisplay_X11 );
   vScreen_X11        = DefaultScreen( vDisplay_X11 );

   return 1;
}

// Look for matching fbconfigs
// #################################################################################################
int iContext::createFrameBuffer() {
   //! \todo implement stub
   return 1;
}

// Create the X-Window
// ########################################################################################################
// ###
int iContext::createWindow() {
   vWindowAttributes_X11.event_mask = vEventMask_lI;

   vWindowAttributes_X11.border_pixel      = 0;
   vWindowAttributes_X11.bit_gravity       = NorthWestGravity;
   vWindowAttributes_X11.background_pixmap = None;
   vWindowAttributes_X11.colormap = vColorMap_X11 =
         XCreateColormap( vDisplay_X11, vRootWindow_X11, vVisualInfo_X11->visual, AllocNone );
   vWindowMask_X11 = CWBitGravity | CWEventMask | CWColormap | CWBorderPixel | CWBackPixmap;

   vColorMapCreated_B = true;

   vWindow_X11 = XCreateWindow( vDisplay_X11,    // Display
                                vRootWindow_X11, // Root window
                                GlobConf.win.posX,
                                GlobConf.win.posY, // X, Y
                                GlobConf.win.width,
                                GlobConf.win.height,                    // Width, Height
                                0,                                      // Border width
                                vVisualInfo_X11->depth,                 // Depth
                                InputOutput,                            // Type of the window
                                vVisualInfo_X11->visual,                // Visual of the window
                                static_cast<unsigned>( vWindowMask_X11 ), // Window mask
                                &vWindowAttributes_X11 ); // Window attributes structure
   if ( vWindow_X11 == 0 ) {
      eLOG( "Failed to create a Window. Abort. (return -4)" );
      return -4;
   }

   // Set the window type
   // clang-format off
   std::string lWindowType_str;
   switch ( GlobConf.win.winType ) {
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
      case NORMAL:        lWindowType_str = "_NET_WM_WINDOW_TYPE_NORMAL";        break;
   }
   // clang-format on

   // The Atoms needed
   Atom lWindowType_atom      = XInternAtom( vDisplay_X11, "_NET_WM_WINDOW_TYPE", True );
   Atom lWhichWindowType_atom = XInternAtom( vDisplay_X11, lWindowType_str.c_str(), True );

   if ( !lWindowType_atom )
      wLOG( "Failed to create X11 Atom _NET_WM_WINDOW_TYPE" );

   if ( !lWhichWindowType_atom )
      wLOG( "Failed to create X11 Atom ", lWindowType_str );


   if ( lWindowType_atom && lWhichWindowType_atom ) {
      std::regex lTypeRegex_EX( "^_[A-Z_]+_" );
      const char *lReplace_C = "";

      // Apply window type
      if ( !XChangeProperty( vDisplay_X11,
                             vWindow_X11,
                             lWindowType_atom,
                             XA_ATOM,
                             32,
                             PropModeReplace,
                             reinterpret_cast<unsigned char *>( &lWhichWindowType_atom ),
                             1 ) ) {
         wLOG( "Failed to set the window type to ",
               std::regex_replace( lWindowType_str, lTypeRegex_EX, lReplace_C ),
               " ( XChangeProperty( ..., _NET_WM_WINDOW_TYPE, ",
               lWindowType_str,
               ", ...);" );
      } else {
         iLOG( "Successfully set the Window type to ",
               std::regex_replace( lWindowType_str, lTypeRegex_EX, lReplace_C ) );
      }
   }

   char *lWinNameTemp_CSTR = const_cast<char *>( GlobConf.win.windowName.c_str() );
   char *lIcoNameTemp_CSTR = const_cast<char *>( GlobConf.win.iconName.c_str() );
   XStringListToTextProperty( &lWinNameTemp_CSTR, 1, &vWindowNameTP_X11 );
   XStringListToTextProperty( &lIcoNameTemp_CSTR, 1, &vWindowIconTP_X11 );

   vSizeHints_X11->flags      = PPosition | PSize | PMinSize; // | IconPixmapHint | IconMaskHint;
   vSizeHints_X11->min_width  = static_cast<int>( GlobConf.win.minWidth );
   vSizeHints_X11->min_height = static_cast<int>( GlobConf.win.minHeight );

   vWmHints_X11                = XAllocWMHints();
   vWmHints_X11->flags         = StateHint | InputHint;
   vWmHints_X11->initial_state = NormalState;
   vWmHints_X11->input         = True;

   // createIconPixmap();

   XSetWMName( vDisplay_X11, vWindow_X11, &vWindowNameTP_X11 );
   XSetWMIconName( vDisplay_X11, vWindow_X11, &vWindowIconTP_X11 );
   XSetWMHints( vDisplay_X11, vWindow_X11, vWmHints_X11 );
   XSetNormalHints( vDisplay_X11, vWindow_X11, vSizeHints_X11 );

   // When user presses the [x] Button the window doesnt close
   atom_wmDeleteWindow = XInternAtom( vDisplay_X11, "WM_DELETE_WINDOW", True );

   if ( atom_wmDeleteWindow )
      XSetWMProtocols( vDisplay_X11, vWindow_X11, &atom_wmDeleteWindow, 1 );
   else
      wLOG( "Failed to create X11 Atom WM_DELETE_WINDOW" );

   XMapWindow( vDisplay_X11, vWindow_X11 );


   vWindowCreated_B = true;

   XFree( vWmHints_X11 );
   XFree( vVisualInfo_X11 ); // Not needed anymore

   for ( ;; ) { // Wait until window is mapped
      XEvent e;
      XNextEvent( vDisplay_X11, &e );
      if ( e.type == MapNotify )
         break;
   }

   XFlush( vDisplay_X11 );

   return 1;
}

#endif

} // unix_x11

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
