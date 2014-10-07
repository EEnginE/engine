/*!
 * \file x11/iInit_event.cpp
 * \brief \b Classes: \a iInit
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

#include "iInit.hpp"
#include "uLog.hpp"

#include <X11/XKBlib.h>

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
   result.tv_sec  = a.tv_sec  + b.tv_sec;
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
   result.tv_sec  = a.tv_sec  - b.tv_sec;
   result.tv_usec = a.tv_usec - b.tv_usec;
   while ( result.tv_usec < 0 ) {
      result.tv_sec--;
      result.tv_usec += 1000000;
   }
}

namespace e_engine {

//namespace unix_x11 {


int iInit::eventLoop() {
   //! \todo Move this in namespace unix_x11
   vEventLoopHasFinished_B = false;
   XEvent lEvent_X11;
   unsigned int lKeyState_uI, lButtonState_uI;
   timeval tv_select, tv, periode;

   iLOG( "Event loop started" );

   int x11_fd;
   fd_set in_fds;
   x11_fd = ConnectionNumber( getDisplay() );

   // Timeout
   periode.tv_sec  = 0;
   periode.tv_usec = 250000;

   // Prepare timevals
   tv_select.tv_sec  = periode.tv_sec;
   tv_select.tv_usec = periode.tv_usec;
   gettimeofday( &tv, 0 );
   addTimeval( tv, periode, tv );

   // Fix autotype keyrelease
   int lAutoRepeatTempReturn_B;
   if ( ! XkbSetDetectableAutoRepeat( getDisplay(), 1, &lAutoRepeatTempReturn_B ) ) {
      wLOG( "Failed to better handle autorepeat (when holdink key down)" );
   }

   while ( vMainLoopRunning_B ) {
      if ( vEventLoopPaused_B ) {
         boost::unique_lock<boost::mutex> lLock_BT( vEventLoopMutex_BT );
         vEventLoopISPaused_B = true;
         while ( vEventLoopPaused_B ) vEventLoopWait_BT.wait( lLock_BT );
         vEventLoopISPaused_B = false;
      }

      FD_ZERO( &in_fds );
      FD_SET( x11_fd, &in_fds );

      // Wait for X Event
      if ( select( x11_fd + 1, &in_fds, NULL, NULL, &tv_select ) ) {
         // Event
         gettimeofday( &tv_select, NULL );
         subTimeval( tv, tv_select, tv_select );
      } else {
         // Tiemout
         tv_select.tv_sec  = periode.tv_sec;
         tv_select.tv_usec = periode.tv_usec;
         gettimeofday( &tv, 0 );
         addTimeval( tv, periode, tv );
      }

      while ( XPending( getDisplay() ) > 0 && getHaveContext() ) {

         XNextEvent( getDisplay(), &lEvent_X11 );
         lKeyState_uI    = E_PRESSED;
         lButtonState_uI = E_PRESSED;
         char lEvent_CSTR[6];
         snprintf( lEvent_CSTR, 5, "%04X", lEvent_X11.type );
         switch ( lEvent_X11.type ) {

            case ConfigureNotify:
               if ( lEvent_X11.xconfigure.width   != ( int ) GlobConf.win.width  || lEvent_X11.xconfigure.height != ( int ) GlobConf.win.height ||
                     lEvent_X11.xconfigure.x      != GlobConf.win.posX        || lEvent_X11.xconfigure.y      != GlobConf.win.posY ) {

                  iEventInfo tempInfo( this );
                  tempInfo.type           = E_EVENT_RESIZE;
                  tempInfo.eResize.width  = GlobConf.win.width  = lEvent_X11.xconfigure.width;
                  tempInfo.eResize.height = GlobConf.win.height = lEvent_X11.xconfigure.height;
                  tempInfo.eResize.posX   = GlobConf.win.posX   = lEvent_X11.xconfigure.x;
                  tempInfo.eResize.posY   = GlobConf.win.posY   = lEvent_X11.xconfigure.y;

                  vResize_SIG.sendSignal( tempInfo );
               }
               break;

            case KeyRelease: lKeyState_uI = E_RELEASED;
            case KeyPress: {
                  iEventInfo tempInfo( this );
                  tempInfo.type       = E_EVENT_KEY;
                  tempInfo.eKey.state = lKeyState_uI;
                  tempInfo.eKey.key   = processX11KeyInput( lEvent_X11.xkey, lKeyState_uI, getDisplay() );
                  vKey_SIG.sendSignal( tempInfo );
               }
               break;

            case ButtonRelease: lButtonState_uI = E_RELEASED;
            case ButtonPress: {
                  iEventInfo tempInfo( this );
                  tempInfo.type         = E_EVENT_MOUSE;
                  tempInfo.iMouse.state = lButtonState_uI;
                  tempInfo.iMouse.posX  = lEvent_X11.xbutton.x;
                  tempInfo.iMouse.posY  = lEvent_X11.xbutton.y;

                  switch ( lEvent_X11.xbutton.button ) {
                     case Button1: tempInfo.iMouse.button = E_MOUSE_LEFT;       break;
                     case Button2: tempInfo.iMouse.button = E_MOUSE_MIDDLE;     break;
                     case Button3: tempInfo.iMouse.button = E_MOUSE_RIGHT;      break;
                     case Button4: tempInfo.iMouse.button = E_MOUSE_WHEEL_UP;   break;
                     case Button5: tempInfo.iMouse.button = E_MOUSE_WHEEL_DOWN; break;
                     case 6:       tempInfo.iMouse.button = E_MOUSE_1;          break;
                     case 7:       tempInfo.iMouse.button = E_MOUSE_2;          break;
                     case 8:       tempInfo.iMouse.button = E_MOUSE_3;          break;
                     case 9:       tempInfo.iMouse.button = E_MOUSE_4;          break;
                     case 10:      tempInfo.iMouse.button = E_MOUSE_5;          break;
                     default:
                        tempInfo.iMouse.button = E_MOUSE_UNKNOWN;
                        wLOG( "Unknown mouse button: ", lEvent_X11.xbutton.button );
                  }

                  vMouse_SIG.sendSignal( tempInfo );
               }
               break;


            case MotionNotify: {
                  iEventInfo tempInfo( this );

                  tempInfo.iMouse.button = E_MOUSE_MOVE;
                  tempInfo.type          = E_EVENT_MOUSE;
                  GlobConf.win.mousePosX  = tempInfo.iMouse.posX   = lEvent_X11.xmotion.x;
                  GlobConf.win.mousePosY  = tempInfo.iMouse.posY   = lEvent_X11.xmotion.y;
                  tempInfo.iMouse.state  = E_PRESSED;

                  GlobConf.win.mouseIsInWindow = true;

                  vMouse_SIG.sendSignal( tempInfo );
               }
               break;

            case EnterNotify: {
                  iEventInfo tempInfo( this );

                  tempInfo.iMouse.button = E_MOUSE_ENTER;
                  tempInfo.type          = E_EVENT_MOUSE;
                  GlobConf.win.mousePosX  = tempInfo.iMouse.posX   = lEvent_X11.xmotion.x;
                  GlobConf.win.mousePosY  = tempInfo.iMouse.posY   = lEvent_X11.xmotion.y;
                  tempInfo.iMouse.state  = E_PRESSED;

                  GlobConf.win.mouseIsInWindow = true;

                  vMouse_SIG.sendSignal( tempInfo );
               }
               break;

            case LeaveNotify: {
                  iEventInfo tempInfo( this );

                  tempInfo.iMouse.button = E_MOUSE_LEAVE;
                  tempInfo.type          = E_EVENT_MOUSE;
                  GlobConf.win.mousePosX  = tempInfo.iMouse.posX   = lEvent_X11.xmotion.x;
                  GlobConf.win.mousePosY  = tempInfo.iMouse.posY   = lEvent_X11.xmotion.y;
                  tempInfo.iMouse.state  = E_PRESSED;

                  GlobConf.win.mouseIsInWindow = false;

                  vMouse_SIG.sendSignal( tempInfo );
               }
               break;

            case FocusIn: {
                  iEventInfo tempInfo( this );
                  tempInfo.type = E_EVENT_FOCUS;
                  GlobConf.win.windowHasFocus = tempInfo.eFocus.hasFocus = true;
                  vFocus_SIG.sendSignal( tempInfo );
               }
               break;

            case FocusOut: {
                  iEventInfo tempInfo( this );
                  tempInfo.type = E_EVENT_FOCUS;
                  GlobConf.win.windowHasFocus = tempInfo.eFocus.hasFocus = false;
                  vFocus_SIG.sendSignal( tempInfo );
               }
               break;


            case ClientMessage:
               // Check if the User pressed the [x] button or ALT+F4 [etc.]
               if ( ( Atom ) lEvent_X11.xclient.data.l[0] == unix_x11::atom_wmDeleteWindow ) {
                  iLOG( "User pressed the close button" );
                  iEventInfo tempInfo( this );
                  tempInfo.type = E_EVENT_WINDOWCLOSE;
                  vWindowClose_SIG.sendSignal( tempInfo );
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

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
