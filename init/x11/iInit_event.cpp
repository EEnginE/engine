/*!
 * \file x11/iInit_event.cpp
 * \brief \b Classes: \a iInit
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

#include "uLog.hpp"
#include "iInit.hpp"

#include <sys/time.h>

namespace e_engine {

// namespace unix_x11 {

#define CAST_EVENT(type, source, dest) xcb_##type##_event_t *dest = reinterpret_cast<xcb_##type##_event_t *>(source);

int iInit::eventLoop() {
  //! \todo Move this in namespace unix_x11
  vEventLoopHasFinished_B = false;

  xcb_generic_event_t *lEvent_XCB;
  xcb_connection_t *   lConnection_XCB = vWindow.getXCBConnection();
  uint32_t             lAutoRepeatType = XCB_AUTO_REPEAT_MODE_ON;

  unsigned int lKeyState_uI, lButtonState_uI;

  LOG.nameThread(L"EVENT");

  iLOG("Event loop started");

  // Fix autotype keyrelease
  xcb_change_keyboard_control(lConnection_XCB, XCB_KB_AUTO_REPEAT_MODE, &lAutoRepeatType);

  while (vMainLoopRunning_B) {
    lEvent_XCB = xcb_wait_for_event(lConnection_XCB);

    if (lEvent_XCB == nullptr) {
      wLOG("XCB returned null event");
      continue;
    }

    lKeyState_uI    = E_PRESSED;
    lButtonState_uI = E_PRESSED;

    switch (lEvent_XCB->response_type & ~0x80) {
      case XCB_CONFIGURE_NOTIFY: {
        CAST_EVENT(configure_notify, lEvent_XCB, lEvent);
        if (lEvent->width != static_cast<int>(GlobConf.win.width) ||
            lEvent->height != static_cast<int>(GlobConf.win.height)) {

          iEventInfo tempInfo(this);
          tempInfo.type          = E_EVENT_RESIZE;
          tempInfo.eResize.width = GlobConf.win.width = lEvent->width;
          tempInfo.eResize.height = GlobConf.win.height = lEvent->height;
          tempInfo.eResize.posX                         = GlobConf.win.posX;
          tempInfo.eResize.posY                         = GlobConf.win.posY;

          vResize_SIG(tempInfo);
        }
      } break;

      case XCB_RESIZE_REQUEST:
      case XCB_EXPOSE:
      case XCB_VISIBILITY_NOTIFY:
      case XCB_PROPERTY_NOTIFY:
        // Ignore for now
        break;

      case XCB_KEY_RELEASE: lKeyState_uI = E_RELEASED; FALLTHROUGH
      case XCB_KEY_PRESS: {
        CAST_EVENT(key_press, lEvent_XCB, lEvent);
        iEventInfo tempInfo(this);
        tempInfo.type       = E_EVENT_KEY;
        tempInfo.eKey.state = lKeyState_uI;
        tempInfo.eKey.key   = vWindow.processX11KeyInput(
            lEvent->detail, static_cast<unsigned short>(lKeyState_uI), lEvent->state, lConnection_XCB);
        vKey_SIG(tempInfo);
      } break;

      case XCB_BUTTON_RELEASE: lButtonState_uI = E_RELEASED; FALLTHROUGH
      case XCB_BUTTON_PRESS: {
        CAST_EVENT(button_press, lEvent_XCB, lEvent);
        iEventInfo tempInfo(this);
        tempInfo.type         = E_EVENT_MOUSE;
        tempInfo.iMouse.state = static_cast<int>(lButtonState_uI);
        tempInfo.iMouse.posX  = static_cast<uint32_t>(lEvent->event_x);
        tempInfo.iMouse.posY  = static_cast<uint32_t>(lEvent->event_y);

        tempInfo.iMouse.button = E_MOUSE_LEFT;

        // clang-format off
            switch ( lEvent->detail ) {
               case 1:  tempInfo.iMouse.button = E_MOUSE_LEFT;       break;
               case 2:  tempInfo.iMouse.button = E_MOUSE_MIDDLE;     break;
               case 3:  tempInfo.iMouse.button = E_MOUSE_RIGHT;      break;
               case 4:  tempInfo.iMouse.button = E_MOUSE_WHEEL_UP;   break;
               case 5:  tempInfo.iMouse.button = E_MOUSE_WHEEL_DOWN; break;
               case 6:  tempInfo.iMouse.button = E_MOUSE_1;          break;
               case 7:  tempInfo.iMouse.button = E_MOUSE_2;          break;
               case 8:  tempInfo.iMouse.button = E_MOUSE_3;          break;
               case 9:  tempInfo.iMouse.button = E_MOUSE_4;          break;
               case 10: tempInfo.iMouse.button = E_MOUSE_5;          break;
               default:
                  tempInfo.iMouse.button       = E_MOUSE_UNKNOWN;
            }
        // clang-format on

        vMouse_SIG(tempInfo);
      } break;


      case XCB_MOTION_NOTIFY: {
        CAST_EVENT(motion_notify, lEvent_XCB, lEvent);
        iEventInfo tempInfo(this);

        tempInfo.iMouse.button = E_MOUSE_MOVE;
        tempInfo.type          = E_EVENT_MOUSE;
        tempInfo.iMouse.state  = E_PRESSED;
        tempInfo.iMouse.posX = GlobConf.win.mousePosX = static_cast<uint32_t>(lEvent->event_x);
        tempInfo.iMouse.posY = GlobConf.win.mousePosY = static_cast<uint32_t>(lEvent->event_y);

        GlobConf.win.mouseIsInWindow = true;
        vMouse_SIG(tempInfo);
      } break;

      case XCB_ENTER_NOTIFY: {
        CAST_EVENT(enter_notify, lEvent_XCB, lEvent);
        iEventInfo tempInfo(this);

        tempInfo.iMouse.button = E_MOUSE_ENTER;
        tempInfo.type          = E_EVENT_MOUSE;
        tempInfo.iMouse.state  = E_PRESSED;
        tempInfo.iMouse.posX = GlobConf.win.mousePosX = static_cast<uint32_t>(lEvent->event_x);
        tempInfo.iMouse.posY = GlobConf.win.mousePosY = static_cast<uint32_t>(lEvent->event_y);

        GlobConf.win.mouseIsInWindow = true;
        vMouse_SIG(tempInfo);
      } break;

      case XCB_LEAVE_NOTIFY: {
        CAST_EVENT(leave_notify, lEvent_XCB, lEvent);
        iEventInfo tempInfo(this);

        tempInfo.iMouse.button = E_MOUSE_LEAVE;
        tempInfo.type          = E_EVENT_MOUSE;
        tempInfo.iMouse.state  = E_PRESSED;
        tempInfo.iMouse.posX = GlobConf.win.mousePosX = static_cast<uint32_t>(lEvent->event_x);
        tempInfo.iMouse.posY = GlobConf.win.mousePosY = static_cast<uint32_t>(lEvent->event_y);

        GlobConf.win.mouseIsInWindow = false;
        vMouse_SIG(tempInfo);
      } break;

      case XCB_FOCUS_IN: {
        iEventInfo tempInfo(this);
        tempInfo.type               = E_EVENT_FOCUS;
        GlobConf.win.windowHasFocus = tempInfo.eFocus.hasFocus = true;
        vFocus_SIG(tempInfo);
      } break;

      case XCB_FOCUS_OUT: {
        iEventInfo tempInfo(this);
        tempInfo.type               = E_EVENT_FOCUS;
        GlobConf.win.windowHasFocus = tempInfo.eFocus.hasFocus = false;
        vFocus_SIG(tempInfo);
      } break;

      case XCB_CLIENT_MESSAGE: {
        CAST_EVENT(client_message, lEvent_XCB, lEvent);
        // Check if the User pressed the [x] button or ALT+F4 [etc.]
        if (lEvent->type == vWindow.getWmProtocolAtom()) {
          if (lEvent->data.data32[0] == vWindow.getWmDeleteWindowAtom()) {
            iLOG("User pressed the close button");
            iEventInfo tempInfo(this);
            tempInfo.type = E_EVENT_WINDOWCLOSE;
            vWindowClose_SIG(tempInfo);
          }
        }
        break;
      }

      case XCB_DESTROY_NOTIFY: {
        iLOG("User pressed the close button");
        iEventInfo tempInfo(this);
        tempInfo.type = E_EVENT_WINDOWCLOSE;
        vWindowClose_SIG(tempInfo);
        break;
      }

      default: dLOG("Found Unknown Event: ", lEvent_XCB->response_type); break;
    }
  }

  iLOG("Event Loop finished");
  vEventLoopHasFinished_B = true;
  return 1;
}

//} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
