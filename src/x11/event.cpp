/*!
 * \file event.cpp
 * \brief \b Classes: \a eInit
 * \sa e_eInit.cpp e_eInit.hpp
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

#include "eInit.hpp"
#include "log.hpp"

/*
 * From class eInit (e_init.h)
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
   while( result.tv_usec >= 1000000 ) {
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
   while( result.tv_usec < 0 ) {
      result.tv_sec--;
      result.tv_usec += 1000000;
   }
}

namespace e_engine {

int eInit::eventLoop() {
   vEventLoopHasFinished_B = false;
   XEvent e;
   unsigned int key_state;
   timeval tv_select, tv, periode;

   iLOG "Event loop started" END

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


   while( vMainLoopRunning_B ) {
      FD_ZERO( &in_fds );
      FD_SET( x11_fd, &in_fds );

      // Wait for X Event
      if( select( x11_fd + 1, &in_fds, NULL, NULL, &tv_select ) ) {
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

      while( XPending( getDisplay() ) > 0 && getHaveContext() ) {
         XNextEvent( getDisplay(), &e );
         key_state = E_KEY_PRESSED;
         switch( e.type ) {

            case ConfigureNotify:
               if( e.xconfigure.width   != ( int ) WinData.win.width  || e.xconfigure.height != ( int ) WinData.win.height ||
                     e.xconfigure.x      != WinData.win.posX        || e.xconfigure.y      != WinData.win.posY ) {

                  eWinInfo tempInfo( this );
                  tempInfo.eResize.width  = WinData.win.width  = e.xconfigure.width;
                  tempInfo.eResize.height = WinData.win.height = e.xconfigure.height;
                  tempInfo.eResize.posX   = WinData.win.posX   = e.xconfigure.x;
                  tempInfo.eResize.posY   = WinData.win.posY   = e.xconfigure.y;

                  vResize_SIG.sendSignal( tempInfo );
               }
               break;

            case KeyRelease: key_state = E_KEY_RELEASED;
            case KeyPress: {
                  eWinInfo tempInfo( this );
                  tempInfo.eKey.state = key_state;
                  tempInfo.eKey.key = processX11KeyInput( e.xkey, key_state, getDisplay() );
                  vKey_SIG.sendSignal( tempInfo );
               }
               break;


            case ClientMessage:
               // Check if the User pressed the [x] button or ALT+F4 [etc.]
               if( ( Atom ) e.xclient.data.l[0] == atom_wmDeleteWindow ) {
                  iLOG "User pressed the close button" END
                  eWinInfo tempInfo( this );
                  tempInfo.type = 10;
                  vWindowClose_SIG.sendSignal( tempInfo );
               }
               break;

            default: break;
         }
      }
   }
   vEventLoopHasFinished_B = true;
   return 1;
}


}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
