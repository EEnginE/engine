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

#include "iInit.hpp"
#include "uLog.hpp"

#include <X11/XKBlib.h>
#include <sys/time.h>

/*
 * From class iInit (e_init.h)
 */

/*!
 * \brief Add 2 timevals
 * \param[in]  a      timeval 1
 * \param[in]  b      timeval 2
 * \param[out] result the result
 * \returns Nothing
 */
static inline void addTimeval( timeval &a, timeval &b, timeval &result ) {
   result.tv_sec = a.tv_sec + b.tv_sec;
   result.tv_usec = a.tv_usec + b.tv_usec;
   while ( result.tv_usec >= 1000000 ) {
      result.tv_sec++;
      result.tv_usec -= 1000000;
   }
}

/*!
 * \brief Subtract 2 timevals
 * \param[in]  a      timeval 1
 * \param[in]  b      timeval 2
 * \param[out] result the result
 * \returns Nothing
 */
static inline void subTimeval( timeval &a, timeval &b, timeval &result ) {
   result.tv_sec = a.tv_sec - b.tv_sec;
   result.tv_usec = a.tv_usec - b.tv_usec;
   while ( result.tv_usec < 0 ) {
      result.tv_sec--;
      result.tv_usec += 1000000;
   }
}

namespace e_engine {

// namespace unix_x11 {


int iInit::eventLoop() {
   //! \todo Move this in namespace unix_x11
   vEventLoopHasFinished_B = false;
   XEvent lEvent_X11;
   unsigned int lKeyState_uI, lButtonState_uI;
   timeval tv_select, tv, periode;

   LOG.nameThread( L"EVENT" );

   iLOG( "Event loop started" );

   int x11_fd;
   fd_set in_fds;
   x11_fd = ConnectionNumber( getDisplay() );

   // Timeout
   periode.tv_sec = 0;
   periode.tv_usec = 250000;

   // Prepare timevals
   tv_select.tv_sec = periode.tv_sec;
   tv_select.tv_usec = periode.tv_usec;
   gettimeofday( &tv, nullptr );
   addTimeval( tv, periode, tv );

   // Fix autotype keyrelease
   int lAutoRepeatTempReturn_B;
   if ( !XkbSetDetectableAutoRepeat( getDisplay(), 1, &lAutoRepeatTempReturn_B ) ) {
      wLOG( "Failed to better handle autorepeat (when holdink key down)" );
   }

   while ( vMainLoopRunning_B ) {
      if ( vEventLoopPaused_B ) {
         std::unique_lock<std::mutex> lLock_BT( vEventLoopMutex_BT );
         vEventLoopISPaused_B = true;
         while ( vEventLoopPaused_B )
            vEventLoopWait_BT.wait( lLock_BT );
         vEventLoopISPaused_B = false;
      }

      FD_ZERO( &in_fds );
      FD_SET( x11_fd, &in_fds );

      // Wait for X Event
      if ( select( x11_fd + 1, &in_fds, nullptr, nullptr, &tv_select ) ) {
         // Event
         gettimeofday( &tv_select, nullptr );
         subTimeval( tv, tv_select, tv_select );
      } else {
         // Tiemout
         tv_select.tv_sec = periode.tv_sec;
         tv_select.tv_usec = periode.tv_usec;
         gettimeofday( &tv, nullptr );
         addTimeval( tv, periode, tv );
      }

      while ( XPending( getDisplay() ) > 0 && getHaveContext() ) {

         XNextEvent( getDisplay(), &lEvent_X11 );
         lKeyState_uI = E_PRESSED;
         lButtonState_uI = E_PRESSED;
         char lEvent_CSTR[6];
         snprintf( lEvent_CSTR, 5, "%04X", lEvent_X11.type );
         switch ( lEvent_X11.type ) {

            case ConfigureNotify:
               if ( lEvent_X11.xconfigure.width != (int)GlobConf.win.width ||
                    lEvent_X11.xconfigure.height != (int)GlobConf.win.height ||
                    lEvent_X11.xconfigure.x != GlobConf.win.posX ||
                    lEvent_X11.xconfigure.y != GlobConf.win.posY ) {

                  iEventInfo tempInfo( this );
                  tempInfo.type = E_EVENT_RESIZE;
                  tempInfo.eResize.width = GlobConf.win.width = lEvent_X11.xconfigure.width;
                  tempInfo.eResize.height = GlobConf.win.height = lEvent_X11.xconfigure.height;
                  tempInfo.eResize.posX = GlobConf.win.posX = lEvent_X11.xconfigure.x;
                  tempInfo.eResize.posY = GlobConf.win.posY = lEvent_X11.xconfigure.y;

                  vResize_SIG( tempInfo );
               }
               break;

            case KeyRelease:
               lKeyState_uI = E_RELEASED;
            case KeyPress: {
               iEventInfo tempInfo( this );
               tempInfo.type = E_EVENT_KEY;
               tempInfo.eKey.state = lKeyState_uI;
               tempInfo.eKey.key =
                     processX11KeyInput( lEvent_X11.xkey, lKeyState_uI, getDisplay() );
               vKey_SIG( tempInfo );
            } break;

            case ButtonRelease:
               lButtonState_uI = E_RELEASED;
            case ButtonPress: {
               iEventInfo tempInfo( this );
               tempInfo.type = E_EVENT_MOUSE;
               tempInfo.iMouse.state = lButtonState_uI;
               tempInfo.iMouse.posX = lEvent_X11.xbutton.x;
               tempInfo.iMouse.posY = lEvent_X11.xbutton.y;

               switch ( lEvent_X11.xbutton.button ) {
                  case Button1:
                     tempInfo.iMouse.button = E_MOUSE_LEFT;
                     break;
                  case Button2:
                     tempInfo.iMouse.button = E_MOUSE_MIDDLE;
                     break;
                  case Button3:
                     tempInfo.iMouse.button = E_MOUSE_RIGHT;
                     break;
                  case Button4:
                     tempInfo.iMouse.button = E_MOUSE_WHEEL_UP;
                     break;
                  case Button5:
                     tempInfo.iMouse.button = E_MOUSE_WHEEL_DOWN;
                     break;
                  case 6:
                     tempInfo.iMouse.button = E_MOUSE_1;
                     break;
                  case 7:
                     tempInfo.iMouse.button = E_MOUSE_2;
                     break;
                  case 8:
                     tempInfo.iMouse.button = E_MOUSE_3;
                     break;
                  case 9:
                     tempInfo.iMouse.button = E_MOUSE_4;
                     break;
                  case 10:
                     tempInfo.iMouse.button = E_MOUSE_5;
                     break;
                  default:
                     tempInfo.iMouse.button = E_MOUSE_UNKNOWN;
                     wLOG( "Unknown mouse button: ", lEvent_X11.xbutton.button );
               }

               vMouse_SIG( tempInfo );
            } break;


            case MotionNotify: {
               iEventInfo tempInfo( this );

               tempInfo.iMouse.button = E_MOUSE_MOVE;
               tempInfo.type = E_EVENT_MOUSE;
               GlobConf.win.mousePosX = tempInfo.iMouse.posX = lEvent_X11.xmotion.x;
               GlobConf.win.mousePosY = tempInfo.iMouse.posY = lEvent_X11.xmotion.y;
               tempInfo.iMouse.state = E_PRESSED;

               GlobConf.win.mouseIsInWindow = true;

               vMouse_SIG( tempInfo );
            } break;

            case EnterNotify: {
               iEventInfo tempInfo( this );

               tempInfo.iMouse.button = E_MOUSE_ENTER;
               tempInfo.type = E_EVENT_MOUSE;
               GlobConf.win.mousePosX = tempInfo.iMouse.posX = lEvent_X11.xmotion.x;
               GlobConf.win.mousePosY = tempInfo.iMouse.posY = lEvent_X11.xmotion.y;
               tempInfo.iMouse.state = E_PRESSED;

               GlobConf.win.mouseIsInWindow = true;

               vMouse_SIG( tempInfo );
            } break;

            case LeaveNotify: {
               iEventInfo tempInfo( this );

               tempInfo.iMouse.button = E_MOUSE_LEAVE;
               tempInfo.type = E_EVENT_MOUSE;
               GlobConf.win.mousePosX = tempInfo.iMouse.posX = lEvent_X11.xmotion.x;
               GlobConf.win.mousePosY = tempInfo.iMouse.posY = lEvent_X11.xmotion.y;
               tempInfo.iMouse.state = E_PRESSED;

               GlobConf.win.mouseIsInWindow = false;

               vMouse_SIG( tempInfo );
            } break;

            case FocusIn: {
               iEventInfo tempInfo( this );
               tempInfo.type = E_EVENT_FOCUS;
               GlobConf.win.windowHasFocus = tempInfo.eFocus.hasFocus = true;
               vFocus_SIG( tempInfo );
            } break;

            case FocusOut: {
               iEventInfo tempInfo( this );
               tempInfo.type = E_EVENT_FOCUS;
               GlobConf.win.windowHasFocus = tempInfo.eFocus.hasFocus = false;
               vFocus_SIG( tempInfo );
            } break;


            case ClientMessage:
               // Check if the User pressed the [x] button or ALT+F4 [etc.]
               if ( (Atom)lEvent_X11.xclient.data.l[0] == unix_x11::atom_wmDeleteWindow ) {
                  iLOG( "User pressed the close button" );
                  iEventInfo tempInfo( this );
                  tempInfo.type = E_EVENT_WINDOWCLOSE;
                  vWindowClose_SIG( tempInfo );
               }
               break;

            default:

               dLOG( "Found Unknown Event: 0x", lEvent_CSTR );

               break;
         }
      }
   }
   vEventLoopHasFinished_B = true;
   return 1;
}

//} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
