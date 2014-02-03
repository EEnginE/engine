/// \file init.cpp
/// \brief \b Classes: \a eInit
/// \sa e_event.cpp e_init.hpp
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


#include <csignal>

#include "init.hpp"
#include "system.hpp"
#include "log.hpp"

namespace e_engine {
namespace e_engine_internal {
__eInit_Pointer __eInit_Pointer_OBJ;
}

void eInit::_setThisForHandleSignal() {
   if ( ! e_engine_internal::__eInit_Pointer_OBJ.set( this ) ) {
      eLOG "There can only be ONE eInit Class" END
      throw std::string( "There can only be ONE eInit Class" );
   }
}

eInit::eInit() {
   vWindowClose_SLOT.setFunc( &eInit::s_standardWindowClose, this );
   vResize_SLOT.setFunc( &eInit::s_standardResize, this );
   vKey_SLOT.setFunc( &eInit::s_standardKey, this );
   vMouse_SLOT.setFunc( &eInit::s_standardMouse, this );
   
   addWindowCloseSlot( &vWindowClose_SLOT );
   addResizeSlot( &vResize_SLOT );
   addKeySlot( &vKey_SLOT );
   addMouseSlot( &vMouse_SLOT );

   vMainLoopRunning_B    = false;
   vBoolCloseWindow_B    = false;

   fRender               = standardRender;

   vEventLoopHasFinished_B  = true;
   vRenderLoopHasFinished_B = true;

   _setThisForHandleSignal();
}

/*!
 * \brief Creates the window and the OpenGL context
 *
 * Creates a \c X11 connection first, then looks for then
 * best FB config, then creates the window and at last
 * it creates the \c OpenGL context and inits \c GLEW
 *
 * \par Linux
 *
 * More information eContext
 *
 * \par
 * \returns  1 -- All good
 * \returns -1 -- Unable to connect to the X-Server
 * \returns -2 -- Need a newer GLX version
 * \returns -3 -- Unable to find any matching fbConfig
 * \returns -4 -- Failed to create a X11 Window
 * \returns  3 -- Failed to create a context
 * \returns  4 -- Feiled to init GLEW
 */
int eInit::init() {

   signal( SIGINT, handleSignal );
   signal( SIGTERM, handleSignal );

   if ( WinData.log.logFILE.logFileName.empty() ) {
      WinData.log.logFILE.logFileName =  SYSTEM.getLogFilePath();
      WinData.log.logFILE.logFileName += "/Log";
   }

   if ( WinData.log.logDefaultInit )
      LOG.devInit();

   LOG.startLogLoop();

   int ret = createContext();
   if ( ret != 1 ) {return ret;}

   standardRender( this ); // Fill the Window with black

   if ( WinData.win.fullscreen == true ) {
      if ( fullScreen( C_ADD ) == 1 ) {
         WinData.win.fullscreen = true;
      }
   }

   if ( WinData.win.VSync == true ) {
      enableVSync();
   }

   return 1;
}

int eInit::shutdown() {
   return LOG.stopLogLoop();
}


void eInit::handleSignal( int _signal ) {
   eInit *_THIS = e_engine_internal::__eInit_Pointer_OBJ.get();;

   if ( _signal == SIGINT ) {
      if ( WinData.handleSIGINT == true ) {
         wLOG "Received SIGINT (Crt-C) => Closing Window and exit(5);" END
         _THIS->closeWindow( true );
         _THIS->destroyContext();
         _THIS->shutdown();
         exit( 5 );
      }
      wLOG "Received SIGINT (Crt-C) => " ADD 'B', 'Y', "DO NOTHING" END
      return;
   }
   if ( _signal == SIGTERM ) {
      if ( WinData.handleSIGTERM == true ) {
         wLOG "Received SIGTERM => Closing Window and exit(6);" END
         _THIS->closeWindow( true );
         _THIS->destroyContext();
         _THIS->shutdown();
         exit( 6 );
      }
      wLOG "Received SIGTERM => Closing Window and exit(6);" ADD 'B', 'Y', "DO NOTHING" END
      return;
   }
}


/*!
 * \brief Starts the main loop can wait until if finished
 * \returns \c SUCCESS: \a 1 -- \C FAIL: \a 0
 */
int eInit::startMainLoop( bool _wait ) {
   if ( ! getHaveContext() ) {return 0;}
   vMainLoopRunning_B = true;
   makeNOContexCurrent();

   vEventLoop_BT  = boost::thread( &eInit::eventLoop, this );
   vRenderLoop_BT = boost::thread( &eInit::renderLoop, this );

   if ( _wait ) {
      vEventLoop_BT.join();
      vRenderLoop_BT.join();
   }

   makeContextCurrent();   // Only ONE thread can have a context

   if ( vBoolCloseWindow_B ) {destroyContext();} // eInit::closeWindow() called?

   return 1;
}

GLvoid eInit::quitMainLoop() {
   vQuitMainLoop_BT = boost::thread( &eInit::quitMainLoopCall, this );
}



int eInit::quitMainLoopCall( ) {
   vMainLoopRunning_B = false;

   if ( ! vEventLoopHasFinished_B )
      vEventLoop_BT.try_join_for( boost::chrono::milliseconds( WinData.timeoutForMainLoopThread_mSec ) );

   if ( ! vEventLoopHasFinished_B ) {
      vEventLoop_BT.interrupt();
      wLOG "Event Loop Timeout reached   -->  kill the thread" END
      vEventLoopHasFinished_B = true;
   }
   iLOG "Event loop finished" END


   if ( ! vRenderLoopHasFinished_B )
      vRenderLoop_BT.try_join_for( boost::chrono::milliseconds( WinData.timeoutForMainLoopThread_mSec ) );

   if ( ! vRenderLoopHasFinished_B ) {
      vRenderLoop_BT.interrupt();
      wLOG "Render Loop Timeout reached  -->  kill the thread" END
      vRenderLoopHasFinished_B = true;
   }

   iLOG "Render loop finished" END

   return 1;
}

int eInit::renderLoop( ) {
   iLOG "Render loop started" END
   vRenderLoopHasFinished_B = false;
   makeContextCurrent();  // Only ONE thread can have a context

   while ( vMainLoopRunning_B ) {
      fRender( this );
   }

   makeNOContexCurrent();
   vRenderLoopHasFinished_B = true;
   return 0;
}

int eInit::closeWindow( bool _waitUntilClosed ) {
   if ( ! getHaveContext() ) {return 0;}
   if ( vMainLoopRunning_B ) {
      vBoolCloseWindow_B = true;
      quitMainLoop();
      if ( _waitUntilClosed ) {vQuitMainLoop_BT.join();}
      return 1;
   }
   destroyContext();
   return 1;
}


void eInit::s_standardWindowClose( eWinInfo _info )  {
   wLOG "Standart WindowClose slot! Closing the window!" END
   _info.eInitPointer->closeWindow();
}



}






// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
