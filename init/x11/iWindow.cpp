/*!
 * \file x11/iWindow.cpp
 * \brief \b Classes: \a iWindow
 *
 * This file contains the definitions of the context
 * creation class iWindow, whitch are all called \b after
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

#include "defines.hpp"

#include <string.h> // memset
#include <xcb/xcb.h>
#include <xcb/xproto.h>

#include "iWindow.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "eCMDColor.hpp"

namespace e_engine {

namespace unix_x11 {

iWindow::iWindow() {
   vConnection_XCB    = nullptr;
   vSetup_XCB         = nullptr;
   vScreen_XCB        = nullptr;
   vWindowHasBorder_B = true;
   vWindowCreated_B   = false;
   vWindowRecreate_B  = false;
   vIsMouseGrabbed_B  = false;

   vIsCursorHidden_B = false;
   vIsMouseGrabbed_B = false;
}

iWindow::~iWindow() { destroyWindow(); }

internal::iXCBAtom::iXCBAtom( xcb_connection_t *_connection, std::string _name ) {
   genAtom( _connection, _name );
}

bool internal::iXCBAtom::genAtom( xcb_connection_t *_connection, std::string _name ) {
   if ( vAtomReply_XCB != nullptr )
      free( vAtomReply_XCB );

   xcb_intern_atom_cookie_t lTempCookie =
         xcb_intern_atom( _connection, 0, _name.size(), _name.c_str() );

   vAtomReply_XCB = xcb_intern_atom_reply( _connection, lTempCookie, nullptr );
   return true;
}

internal::iXCBAtom::~iXCBAtom() {
   if ( vAtomReply_XCB != nullptr ) {
      free( vAtomReply_XCB );
   }
}

/*!
 * \brief Creates the window and prints versions
 *
 * \returns  0 -- success
 * \returns  1 -- Unable to connect to the X-Server
 * \returns  2 -- Failed to init EWMH connection
 */
int iWindow::createWindow() {
   int         lScreenNum;
   std::string lRandRVersionString_str;

   vConnection_XCB = xcb_connect( NULL, &lScreenNum );
   if ( xcb_connection_has_error( vConnection_XCB ) != 0 ) {
      eLOG( "Failed to connect to the X-Server; Error code: ",
            xcb_connection_has_error( vConnection_XCB ) );
      return 1;
   }

   vSetup_XCB                      = xcb_get_setup( vConnection_XCB );
   xcb_screen_iterator_t lIter_XCB = xcb_setup_roots_iterator( vSetup_XCB );

   for ( int i = 0; i < lScreenNum; i++ )
      xcb_screen_next( &lIter_XCB );

   vScreen_XCB = lIter_XCB.data;

   uint32_t lValueMask_XCB = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
   uint32_t lValues[32];

   lValues[0] = vScreen_XCB->black_pixel;
   lValues[1] = XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_KEY_RELEASE |
                XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
                XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_1_MOTION |
                XCB_EVENT_MASK_BUTTON_2_MOTION | XCB_EVENT_MASK_BUTTON_3_MOTION |
                XCB_EVENT_MASK_BUTTON_4_MOTION | XCB_EVENT_MASK_BUTTON_5_MOTION |
                XCB_EVENT_MASK_BUTTON_MOTION | XCB_EVENT_MASK_EXPOSURE |
                XCB_EVENT_MASK_VISIBILITY_CHANGE | XCB_EVENT_MASK_STRUCTURE_NOTIFY |
                XCB_EVENT_MASK_FOCUS_CHANGE | XCB_EVENT_MASK_PROPERTY_CHANGE;

   /*
    * Removed Masks:
    * XCB_EVENT_MASK_POINTER_MOTION_HINT
    * XCB_EVENT_MASK_KEYMAP_STATE
    * XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY
    * XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT
    * XCB_EVENT_MASK_COLOR_MAP_CHANGE
    * XCB_EVENT_MASK_OWNER_GRAB_BUTTON
    * XCB_EVENT_MASK_RESIZE_REDIRECT
    */

   vWindow_XCB = xcb_generate_id( vConnection_XCB );
   xcb_create_window( vConnection_XCB,
                      XCB_COPY_FROM_PARENT,
                      vWindow_XCB,
                      vScreen_XCB->root,
                      GlobConf.win.posX,
                      GlobConf.win.posY,
                      GlobConf.win.width,
                      GlobConf.win.height,
                      0,
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,
                      vScreen_XCB->root_visual,
                      lValueMask_XCB,
                      lValues );

   vWindowCreated_B = true;

   // Set some properties
   vWmProtocol_ATOM.genAtom( vConnection_XCB, "WM_PROTOCOLS" );
   vWmDeleteWindow_ATOM.genAtom( vConnection_XCB, "WM_DELETE_WINDOW" );
   vNetWmState_ATOM.genAtom( vConnection_XCB, "_NET_WM_STATE" );
   vNetWmWindowType_ATOM.genAtom( vConnection_XCB, "_NET_WM_WINDOW_TYPE" );
   vNetWmName_ATOM.genAtom( vConnection_XCB, "_NET_WM_NAME" );
   vNetWmIconName_ATOM.genAtom( vConnection_XCB, "_NET_WM_ICON_NAME" );
   vWmName_ATOM.genAtom( vConnection_XCB, "WM_NAME" );
   vWmIconName_ATOM.genAtom( vConnection_XCB, "WM_ICON_NAME" );
   vMotifWmHints_ATOM.genAtom( vConnection_XCB, "_MOTIF_WM_HINTS" );

   iXCBAtom lNetWmBypass( vConnection_XCB, "_NET_WM_BYPASS_COMPOSITOR" );
   int      lBypassValue = 1;

   setWindowType( GlobConf.win.winType );
   setWmPropertyAtom( vWmProtocol_ATOM, vWmDeleteWindow_ATOM );
   setWmPropertyString( vNetWmName_ATOM, GlobConf.win.windowName );
   setWmPropertyString( vNetWmIconName_ATOM, GlobConf.win.iconName );
   setWmPropertyString( vWmName_ATOM, GlobConf.win.windowName );
   setWmPropertyString( vWmIconName_ATOM, GlobConf.win.iconName );
   setWmProperty( lNetWmBypass, XCB_ATOM_CARDINAL, 32, 1, &lBypassValue );

   // Draw the window
   xcb_map_window( vConnection_XCB, vWindow_XCB );
   xcb_flush( vConnection_XCB );

   if ( initRandR() ) {
      int lVRRmajor_I;
      int lVRRminor_I;
      getRandRVersion( lVRRmajor_I, lVRRminor_I );
      lRandRVersionString_str = std::to_string( lVRRmajor_I ) + '.' + std::to_string( lVRRminor_I );
   } else {
      lRandRVersionString_str = "!!! NOT SUPPORTED !!!";
   }

   std::wstring lC1_C          = eCMDColor::color( 'B', 'C' );
   std::string  lEngineGit_str = E_GIT_LAST_TAG_DIFF == 0
                                      ? " [RELEASE] "
                                      : ( " +" + std::to_string( E_GIT_LAST_TAG_DIFF ) + " " );

   // clang-format off
   iLOG( "Versions:",
         "\n  - Engine: ", lC1_C, E_VERSION_MAJOR, ".", E_VERSION_MINOR, ".", E_VERSION_SUBMINOR, lEngineGit_str, E_VERSION_GIT,
         "\n  - XCB:    ", lC1_C, vSetup_XCB->protocol_major_version, ".", vSetup_XCB->protocol_minor_version,
         "\n  - RandR:  ", lC1_C, lRandRVersionString_str );
   // clang-format on

   if ( GlobConf.win.fullscreen == true ) {
      fullScreen( C_ADD );
   }

   if ( GlobConf.win.windowDecoration == true ) {
      setDecoration( C_ADD );
   } else {
      setDecoration( C_REMOVE );
   }

   return 0;
}

void iWindow::setWmProperty( iXCBAtom &  _property,
                             xcb_atom_t  _type,
                             uint8_t     _format,
                             uint32_t    _length,
                             const void *_data ) {
   if ( !vWindowCreated_B )
      return;

   xcb_change_property( vConnection_XCB,
                        XCB_PROP_MODE_REPLACE,
                        vWindow_XCB,
                        _property.getAtom(),
                        _type,
                        _format,
                        _length,
                        _data );

   xcb_flush( vConnection_XCB );
}

void iWindow::setWmPropertyAtom( iXCBAtom &_property, iXCBAtom &_data ) {
   setWmProperty( _property, XCB_ATOM_ATOM, 32, 1, _data.getAtomRef() );
}
void iWindow::setWmPropertyString( iXCBAtom &_property, std::string _data ) {
   setWmProperty( _property, XCB_ATOM_STRING, 8, _data.length(), _data.c_str() );
}

void iWindow::setWindowNames( std::string _windowName, std::string _iconName ) {
   if ( !vWindowCreated_B )
      return;

   setWmPropertyString( vNetWmName_ATOM, _windowName );
   setWmPropertyString( vNetWmIconName_ATOM, _iconName );
   setWmPropertyString( vWmName_ATOM, _windowName );
   setWmPropertyString( vWmIconName_ATOM, _iconName );
}

void iWindow::setWindowType( WINDOW_TYPE _type ) {
   if ( !vWindowCreated_B )
      return;

   // clang-format off
   std::string lWindowType_str;
   switch ( _type ) {
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

   iXCBAtom lWinType( vConnection_XCB, lWindowType_str );
   setWmPropertyAtom( vNetWmWindowType_ATOM, lWinType );
}


/*!
 * \brief Changes the window config
 * \param _width  The new width
 * \param _height The new height
 * \param _posX   The new X coordinate
 * \param _posY   The new Y coordinate
 */
void iWindow::changeWindowConfig( unsigned int _width, unsigned int _height, int _posX, int _posY ) {
   if ( !vWindowCreated_B )
      return;

   uint16_t lMask = XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y | XCB_CONFIG_WINDOW_WIDTH |
                    XCB_CONFIG_WINDOW_HEIGHT;
   uint32_t lValues[] = {
         static_cast<uint32_t>( _posX ), static_cast<uint32_t>( _posY ), _width, _height};

   GlobConf.win.width  = _width;
   GlobConf.win.height = _height;
   GlobConf.win.posX   = _posX;
   GlobConf.win.posY   = _posY;

   xcb_configure_window( vConnection_XCB, vWindow_XCB, lMask, lValues );
   xcb_flush( vConnection_XCB );
}


/*!
 * \brief Destroy the window and the context
 */
void iWindow::destroyWindow() {
   endRandR();
   if ( vWindowCreated_B == true ) {
      xcb_disconnect( vConnection_XCB );
      vWindowCreated_B = false;
      vConnection_XCB  = nullptr;
      vSetup_XCB       = nullptr;
      vScreen_XCB      = nullptr;
   }

   vWindowCreated_B = false;
}

struct MwmHints {
   uint32_t flags;
   uint32_t functions;
   uint32_t decorations;
   int32_t  input_mode;
   uint32_t status;
};
const long unsigned int MWM_HINTS_DECORATIONS = ( 1L << 1 );


/*!
 * \brief Try to add or remove the window decoration
 * \warning A non X11 standard function is used! Functionality may be restricted
 * \param _action Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 */
void iWindow::setDecoration( e_engine::ACTION _action ) {
   if ( !vWindowCreated_B )
      return;

   if ( _action == C_TOGGLE ) {
      _action = ( vWindowHasBorder_B ) ? C_REMOVE : C_ADD;
   }

   struct MwmHints lHints_X11;

   memset( &lHints_X11, 0, sizeof( MwmHints ) );
   lHints_X11.flags       = MWM_HINTS_DECORATIONS;
   lHints_X11.decorations = _action;

   setWmProperty( vMotifWmHints_ATOM, vMotifWmHints_ATOM.getAtom(), 32, 5, &lHints_X11 );

   GlobConf.win.windowDecoration = vWindowHasBorder_B = ( _action == C_REMOVE ) ? false : true;
   iLOG( "Successfully ", ( _action == C_REMOVE ) ? "removed" : "added", " window decoration" );
}


/*!
 * \brief Change the fullscreen mode
 * \param _action      Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 * \param _allMonitors \a true if the Fullscreen window should be mapped over all monitors
 */
void iWindow::fullScreen( e_engine::ACTION _action, bool _allMonitors ) {
   setAttribute( _action, FULLSCREEN );

   if ( _allMonitors && !( _action == C_REMOVE ) )
      fullScreenMultiMonitor();
}

/*!
 * \brief Change the maximize mode
 * \param _action Can be ACTION::C_REMOVE, ACTION::C_ADD or ACTION::C_TOGGLE
 */
void iWindow::maximize( e_engine::ACTION _action ) {
   setAttribute( _action, MAXIMIZED_HORZ, MAXIMIZED_VERT );
}


/*!
 * \brief Sends an NET_WM_STATE event to the X-Server
 *
 * \param _action What to do
 * \param _type1  The first thing to change
 * \param _type2  The second thing to change (Default: NONE)
*
 * \sa e_engine::ACTION, e_engine::WINDOW_ATTRIBUTE
 */
void iWindow::setAttribute( ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 ) {
   if ( !vWindowCreated_B )
      return;

   if ( _type1 == _type2 ) {
      eLOG( "Changing the same attribute at the same time makes completely no sense. ==> Do "
            "nothing" );
      return;
   }

   iXCBAtom   lAtomHelper1;
   iXCBAtom   lAtomHelper2;
   xcb_atom_t lAtom1_XCB = 0;
   xcb_atom_t lAtom2_XCB = 0;

   std::string lMode_STR;
   std::string lState1_str = "_NET_WM_STATE_NOTHING", lState2_str = "_NET_WM_STATE_NOTHING";

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
         case NONE: return;
            // clang-format on
      }

      lAtomHelper1.genAtom( vConnection_XCB, lState1_str );
      lAtom1_XCB = lAtomHelper1.getAtom();
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
         case NONE: return;
      }

      lAtomHelper2.genAtom( vConnection_XCB, lState1_str );
      lAtom2_XCB = lAtomHelper2.getAtom();
   }

   sendX11Event( vNetWmState_ATOM,
                 _action,
                 static_cast<uint32_t>( lAtom1_XCB ),
                 static_cast<uint32_t>( lAtom2_XCB ),
                 1 );

   if ( _type1 == FULLSCREEN || _type2 == FULLSCREEN ) {
      switch ( _action ) {
         case C_ADD: GlobConf.win.fullscreen    = true; break;
         case C_REMOVE: GlobConf.win.fullscreen = false; break;
         case C_TOGGLE: GlobConf.win.fullscreen = !GlobConf.win.fullscreen; break;
      }
   }

   switch ( _action ) {
      case C_REMOVE: lMode_STR = "Removed"; break;
      case C_ADD: lMode_STR    = "Enabled"; break;
      case C_TOGGLE: lMode_STR = "Toggled"; break;
   }

   iLOG( lMode_STR, ' ', lState1_str.substr( 14 ), " and ", lState2_str.substr( 14 ), " mode." );
}

/*!
 * \brief Try to map the fullscreen window to all monitors
 */
void iWindow::fullScreenMultiMonitor() {
   if ( !vWindowCreated_B )
      return;

   unsigned int lLeft_I;
   unsigned int lRight_I;
   unsigned int lTop_I;
   unsigned int lBot_I;

   getMostLeftRightTopBottomCRTC( lLeft_I, lRight_I, lTop_I, lBot_I );

   iXCBAtom lAtom( vConnection_XCB, "_NET_WM_FULLSCREEN_MONITORS" );

   sendX11Event( lAtom, lTop_I, lBot_I, lLeft_I, lRight_I );
   iLOG( "Successfully mapped the fullscreen window to all monitors" );
}


/*!
 * \brief Try to map the fullscreen window to display _disp
 *
 * \param _disp The display where the fullscreen window should be
 */
void iWindow::setFullScreenMonitor( iDisplays &_disp ) {
   if ( !vWindowCreated_B )
      return;

   int lDisp_I = getIndexOfDisplay( _disp );

   if ( lDisp_I < 0 ) {
      wLOG( "No valid iDisplays [ setFullScreenMonitor(...) ] ==> Return "
            "iRandR::getIndexOfDisplay( _disp ) = ",
            lDisp_I );
      return;
   }

   iXCBAtom lAtom( vConnection_XCB, "_NET_WM_FULLSCREEN_MONITORS" );

   sendX11Event( lAtom, lDisp_I, lDisp_I, lDisp_I, lDisp_I );
   iLOG( "Successfully mapped the fullscreen window to monitor ", lDisp_I );
}


void iWindow::sendX11Event(
      iXCBAtom &_atom, uint32_t _l0, uint32_t _l1, uint32_t _l2, uint32_t _l3, uint32_t _l4 ) {
   if ( !vWindowCreated_B )
      return;

   xcb_client_message_event_t lEvent_XCB;

   memset( &lEvent_XCB, 0, sizeof( lEvent_XCB ) );

   lEvent_XCB.response_type  = XCB_CLIENT_MESSAGE;
   lEvent_XCB.window         = vWindow_XCB;
   lEvent_XCB.type           = _atom.getAtom();
   lEvent_XCB.format         = 32;
   lEvent_XCB.data.data32[0] = _l0;
   lEvent_XCB.data.data32[1] = _l1;
   lEvent_XCB.data.data32[2] = _l2;
   lEvent_XCB.data.data32[3] = _l3;
   lEvent_XCB.data.data32[4] = _l4;

   xcb_send_event( vConnection_XCB,
                   0,
                   vScreen_XCB->root,
                   XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
                   reinterpret_cast<const char *>( &lEvent_XCB ) );

   xcb_map_window( vConnection_XCB, vWindow_XCB );
   xcb_flush( vConnection_XCB );
}





/*!
 * \brief Get the \c X11 version
 * \param[out] _major The major version number
 * \param[out] _minor The minor version number
 */
void iWindow::getXCBVersion( int *_major, int *_minor ) {
   if ( !vWindowCreated_B ) {
      *_major = -1;
      *_minor = -1;
      return;
   }
   *_major = vSetup_XCB->protocol_major_version;
   *_minor = vSetup_XCB->protocol_minor_version;
}



/*!
 * \brief Grabs the mouse pointer (and the keyboard)
 *
 * \note You can only grab the mouse if the mouse is ungrabbed
 *
 * \returns true if successful and false if not
 */
bool iWindow::grabMouse() {
   if ( vIsMouseGrabbed_B ) {
      wLOG( "Mouse is already grabbed" );
      return false;
   }

   if ( !vWindowCreated_B )
      return false;

   uint16_t lEventMask = XCB_EVENT_MASK_BUTTON_PRESS | XCB_EVENT_MASK_BUTTON_RELEASE |
                         XCB_EVENT_MASK_ENTER_WINDOW | XCB_EVENT_MASK_LEAVE_WINDOW |
                         XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_BUTTON_1_MOTION |
                         XCB_EVENT_MASK_BUTTON_2_MOTION | XCB_EVENT_MASK_BUTTON_3_MOTION |
                         XCB_EVENT_MASK_BUTTON_4_MOTION | XCB_EVENT_MASK_BUTTON_5_MOTION |
                         XCB_EVENT_MASK_BUTTON_MOTION;

   auto lCookie =
         xcb_grab_pointer( vConnection_XCB,     // Our connection to the X server
                           1,                   // Still get events
                           vWindow_XCB,         // The window owning the grab
                           lEventMask,          // We dont need a special event mask
                           XCB_GRAB_MODE_ASYNC, // Mouse grabbing should be async (easier for us)
                           XCB_GRAB_MODE_ASYNC, // Key grabbing should be async (easier for us)
                           vWindow_XCB,         // Lock the cursor in this window
                           XCB_NONE,            // Use the default window cursor icon
                           XCB_CURRENT_TIME     // X11 needs a time
                           );

   xcb_grab_pointer_reply_t *lReply = xcb_grab_pointer_reply( vConnection_XCB, lCookie, nullptr );

   auto lStatus = lReply->status;
   free( lReply );

   if ( lStatus != XCB_GRAB_STATUS_SUCCESS && lStatus != XCB_GRAB_STATUS_ALREADY_GRABBED ) {
      wLOG( "Failed to grab the mouse; status: ", lStatus );
      return false;
   }

   xcb_flush( vConnection_XCB );

   vIsMouseGrabbed_B = true;
   iLOG( "Mouse grabbed; status: ", lStatus );

   return true;
}


/*!
 * \brief Ungrabs the mouse pointer (and the keyboard)
 *
 * \note You can only ungrab the mouse if it is grabbed
 */
void iWindow::freeMouse() {
   if ( !vIsMouseGrabbed_B ) {
      wLOG( "Mouse is not grabbed" );
      return;
   }

   if ( !vWindowCreated_B )
      return;


   xcb_ungrab_pointer( vConnection_XCB, XCB_CURRENT_TIME );
   xcb_flush( vConnection_XCB );
   vIsMouseGrabbed_B = false;
   iLOG( "Mouse ungrabbed" );
}

/*!
 * \brief Sets the mouse position
 *
 * \param[in] _posX The x coordinate in our window
 * \param[in] _posY The y coordinate in our window
 *
 * \note _posX and _posY must be inside our window
*/
void iWindow::moveMouse( unsigned int _posX, unsigned int _posY ) {
   if ( _posX > GlobConf.win.width || _posY > GlobConf.win.height ) {
      wLOG( "_posX and/or _posY outside the window" );
      return;
   }

   if ( !vWindowCreated_B )
      return;

   xcb_warp_pointer( vConnection_XCB,           // Our connection to the X server
                     XCB_NONE,                  // Move it from this window (unknown)...
                     vWindow_XCB,               // ...to the window
                     0,                         // We dont...
                     0,                         // ...have any...
                     0,                         // ...information about...
                     0,                         // ...the source window!
                     static_cast<int>( _posX ), // Posx in the window
                     static_cast<int>( _posY )  // Posy in the window
                     );

   xcb_flush( vConnection_XCB );
}

/*!
 * \brief Get if the mouse is grabbed
 * \returns if the mouse is grabbed
 */
bool iWindow::getIsMouseGrabbed() const { return vIsMouseGrabbed_B; }



/*!
 * \brief Hides the cursor
 */
void iWindow::hideMouseCursor() {
   if ( vIsCursorHidden_B ) {
      wLOG( "Cursor is already hidden" );
      return;
   }

   xcb_pixmap_t lPixmap = xcb_generate_id( vConnection_XCB );
   xcb_cursor_t lCursor = xcb_generate_id( vConnection_XCB );

   xcb_create_pixmap( vConnection_XCB, // Connection to the X-Server
                      1,               // The depth of the screen
                      lPixmap,         // The Pixmap ID
                      vWindow_XCB,     // The drawable
                      1,               // width
                      1                // height
                      );

   xcb_create_cursor( vConnection_XCB, lCursor, lPixmap, lPixmap, 0, 0, 0, 0, 0, 0, 0, 0 );
   xcb_change_window_attributes( vConnection_XCB, vWindow_XCB, XCB_CW_CURSOR, &lCursor );

   xcb_free_pixmap( vConnection_XCB, lPixmap );
   xcb_free_cursor( vConnection_XCB, lCursor );

   xcb_flush( vConnection_XCB );
   iLOG( "Cursor hidden" );
   vIsCursorHidden_B = true;
}

/*!
 * \brief Shows the cursor
 */
void iWindow::showMouseCursor() {
   if ( !vIsCursorHidden_B ) {
      wLOG( "Cursor is already visible" );
      return;
   }

   uint32_t lNewCursor = XCB_CURSOR_NONE;
   xcb_change_window_attributes( vConnection_XCB, vWindow_XCB, XCB_CW_CURSOR, &lNewCursor );
   xcb_flush( vConnection_XCB );
   vIsCursorHidden_B = false;
   iLOG( "Cursor visible" );
}

/*!
 * \brief Get if the cursor is hidden
 * \returns true if the cursor is hidden
 */
bool iWindow::getIsCursorHidden() const { return vIsCursorHidden_B; }

/*!
 * \brief Get if the window is created
 * \returns true if the window is created
 */
bool iWindow::getIsWindowCreated() const { return vWindowCreated_B; }

/*!
 * \brief Get the XCB connection to the X-Server
 * \returns the XCB connection to the X-Server
 */
xcb_connection_t *iWindow::getXCBConnection() { return vConnection_XCB; }

/*!
 * \brief Get the XCB WM_DELETE_WINDOW atom
 * \returns the XCB WM_DELETE_WINDOW atom
 */
xcb_atom_t iWindow::getWmDeleteWindowAtom() const { return vWmDeleteWindow_ATOM.getAtom(); }

/*!
 * \brief Get the XCB WM_PROTOCOLS atom
 * \returns the XCB WM_PROTOCOLS atom
 */
xcb_atom_t iWindow::getWmProtocolAtom() const { return vWmProtocol_ATOM.getAtom(); }

VkSurfaceKHR iWindow::getVulkanSurface( VkInstance _instance ) {
   VkXcbSurfaceCreateInfoKHR lInfo;
   lInfo.sType      = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
   lInfo.pNext      = nullptr;
   lInfo.flags      = 0;
   lInfo.connection = vConnection_XCB;
   lInfo.window     = vWindow_XCB;

   VkSurfaceKHR lSurface;
   auto         lRes = vkCreateXcbSurfaceKHR( _instance, &lInfo, nullptr, &lSurface );
   if ( lRes ) {
      eLOG( "'vkCreateXcbSurfaceKHR' returned ", uEnum2Str::toStr( lRes ) );
      return nullptr;
   }

   return lSurface;
}


} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
