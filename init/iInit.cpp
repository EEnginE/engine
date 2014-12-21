/*!
 * \file iInit.cpp
 * \brief \b Classes: \a iInit
 * \sa e_event.cpp e_iInit.hpp
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


#include <csignal>

#include "iInit.hpp"
#include "uSystem.hpp"
#include "uLog.hpp"

namespace e_engine {
namespace internal {
__iInit_Pointer __iInit_Pointer_OBJ;
}

void iInit::_setThisForHandluSignal() {
   if( ! internal::__iInit_Pointer_OBJ.set( this ) ) {
      eLOG( "There can only be ONE iInit Class" );
      throw std::string( "There can only be ONE iInit Class" );
   }
}

iInit::iInit() {
   vWindowClose_SLOT.setFunc( &iInit::s_standardWindowClose, this );
   vResize_SLOT.setFunc( &iInit::s_standardResize, this );
   vKey_SLOT.setFunc( &iInit::s_standardKey, this );
   vMouse_SLOT.setFunc( &iInit::s_standardMouse, this );
   vFocus_SLOT.setFunc( &iInit::s_standardFocus, this );

   vGrabControl_SLOT.setFunc( &iInit::s_advancedGrabControl, this );

   addWindowCloseSlot( &vWindowClose_SLOT );
   addResizeSlot( &vResize_SLOT );
   addKeySlot( &vKey_SLOT );
   addMouseSlot( &vMouse_SLOT );
   addFocusSlot( &vFocus_SLOT );

   vMainLoopRunning_B       = false;

   vEventLoopHasFinished_B  = false;
   vEventLoopISPaused_B     = false;
   vEventLoopPaused_B       = false;

   vEventLoopHasFinished_B  = true;

   vWasMouseGrabbed_B       = false;

   vCreateWindowReturn_I    = -1000;

   vAreRenderLoopSignalsConnected_B = false;

#if WINDOWS
   vContinueWithEventLoop_B = false;
#endif

   _setThisForHandluSignal();
}


/*!
 * \brief Handle focus change events when mouse is grabbed
 *
 * When the mouse is grabbed and focus is lost (Alt+tab), various
 * bad things can happen.
 *
 * This function makes sure that when focus was lost, the mouse will
 * be ungrabbed and when focus is restored that it will be locked again.
 *
 * Use iInit::addFocusSlot( iInit::getAdvancedGrabControlSlot() );
 */
GLvoid iInit::s_advancedGrabControl( iEventInfo _info ) {
   if( ( _info.type == E_EVENT_FOCUS ) && _info.eFocus.hasFocus && vWasMouseGrabbed_B ) {
      // Focus restored
      vWasMouseGrabbed_B = false;
      if( ! grabMouse() ) {
         // Cannot grab again when X11 has not handled some events

         for( unsigned short int i = 0; i < 25; ++i ) {
            iLOG( "Try Grab ", i + 1, " of 25" );
            if( grabMouse() )
               break; // Grab success
            B_SLEEP( milliseconds, 100 );
         }

      }
      return;
   }
   if( ( _info.type == E_EVENT_FOCUS ) && !_info.eFocus.hasFocus && getIsMouseGrabbed() ) {
      // Focus lost
      vWasMouseGrabbed_B = true;
      freeMouse();
      return;
   }
}


/*!
 * \brief Creates the window and the OpenGL context
 *
 * Creates a \c X11 connection first, then looks for the
 * best FB config, then creates the window and at last
 * it creates the \c OpenGL context and inits \c GLEW
 *
 * \par Linux
 *
 * More information iContext
 *
 * \par
 * \returns  1 -- Success
 * \returns -1 -- Unable to connect to the X-Server
 * \returns -2 -- Need a newer GLX version
 * \returns -3 -- Unable to find any matching fbConfig
 * \returns -4 -- Failed to create a X11 Window
 * \returns  3 -- Failed to create a context
 * \returns  4 -- Failed to init GLEW
 * \returns  5 -- Bad OpenGL version (at least 3.3)
 */
int iInit::init() {

   signal( SIGINT, handleSignal );
   signal( SIGTERM, handleSignal );

   if( GlobConf.log.logFILE.logFileName.empty() ) {
      GlobConf.log.logFILE.logFileName =  SYSTEM.getLogFilePath();
#if UNIX
      GlobConf.log.logFILE.logFileName += "/Log";
#elif WINDOWS
      GlobConf.log.logFILE.logFileName += "\\Log";
#endif
   }

   if( GlobConf.log.logDefaultInit )
      LOG.devInit();

   LOG.startLogLoop();

#if WINDOWS
   // Windows needs the PeekMessage call in the same thread where the window is created
   std::unique_lock<std::mutex> lLock_BT( vCreateWindowMutex_BT );
   vEventLoop_BT  = std::thread( &iInit::eventLoop, this );

   while( vCreateWindowReturn_I == -1000 ) vCreateWindowCondition_BT.wait( lLock_BT );

   makeContextCurrent();
#else
   vCreateWindowReturn_I = createContext();
#endif

   if( vCreateWindowReturn_I != 1 ) { return vCreateWindowReturn_I; }

   if( GlobConf.extensions.queryAll() < OGL_VERSION_3_3 ) {
      eLOG( "Bad OpenGL version. At least version 3.3 is needed. Try updating your driver" );
      destroyContext();
      return 5;
   }
   standardRender( this ); // Fill the Window with black

   return 1;
}

int iInit::shutdown() {
   closeWindow();
   return LOG.stopLogLoop();
}


void iInit::handleSignal( int _signal ) {
   iInit *_THIS = internal::__iInit_Pointer_OBJ.get();;

   if( _signal == SIGINT ) {
      if( GlobConf.handleSIGINT == true ) {
         wLOG( "Received SIGINT (Crt-C) => Closing Window and exiting(5);" );
         _THIS->closeWindow( true );
         _THIS->destroyContext();
         _THIS->shutdown();
         exit( 5 );
      }
      wLOG( "Received SIGINT (Crt-C) => ", 'B', 'Y', "DO NOTHING" );
      return;
   }
   if( _signal == SIGTERM ) {
      if( GlobConf.handleSIGTERM == true ) {
         wLOG( "Received SIGTERM => Closing Window and exiting(6);" );
         _THIS->closeWindow( true );
         _THIS->destroyContext();
         _THIS->shutdown();
         exit( 6 );
      }
      wLOG( "Received SIGTERM => Closing Window and exiting(6);", 'B', 'Y', "DO NOTHING" );
      return;
   }
}


/*!
 * \brief Starts the main loop
 * \returns \c SUCCESS: \a 1 -- \C FAIL: \a 0
 */
int iInit::startMainLoop( bool _wait ) {
   if( ! getHaveContext() ) {
      wLOG( "Cannot start the main loop. There is no OpenGL context!" );
      return 0;
   }
   vMainLoopRunning_B = true;

   // Send a resize signal to ensure that the viewport is updated
   iEventInfo _tempInfo( this );
   _tempInfo.iInitPointer   = this;
   _tempInfo.type           = E_EVENT_RESIZE;
   _tempInfo.eResize.width  = GlobConf.win.width;
   _tempInfo.eResize.height = GlobConf.win.height;
   _tempInfo.eResize.posX   = GlobConf.win.posX;
   _tempInfo.eResize.posY   = GlobConf.win.posY;

   vResize_SIG.sendSignal( _tempInfo );

   if( !vAreRenderLoopSignalsConnected_B ) {
      eLOG( "iInit is not yet connected with a render system!" );
      return 0;
   }

#if UNIX_X11
   vEventLoop_BT  = std::thread( &iInit::eventLoop, this );
#elif WINDOWS
   {
      // Make sure lLockEvent_BT will be destroyed
      std::lock_guard<std::mutex> lLockEvent_BT( vStartEventMutex_BT );
      vContinueWithEventLoop_B = true;
      vStartEventCondition_BT.notify_one();
   }
#endif

   if( _wait ) {
      vStartRenderLoopSignal_SIG( true );
      if( vEventLoop_BT.joinable() )
         vEventLoop_BT.join();

      // Wait for quit main loop to finish
      if( vQuitMainLoop_BT.joinable() )
         vQuitMainLoop_BT.join();

   } else {
      vStartRenderLoopSignal_SIG( false );
   }

   return 1;
}

GLvoid iInit::quitMainLoop() {
   vQuitMainLoop_BT = std::thread( &iInit::quitMainLoopCall, this );
}



int iInit::quitMainLoopCall( ) {
   vMainLoopRunning_B = false;

#if WINDOWS
   vContinueWithEventLoop_B = false;
#endif

#if ! WINDOWS
   if( ! vEventLoopHasFinished_B )
      vEventLoop_BT.join();

   if( ! vEventLoopHasFinished_B ) {
      wLOG( "Event Loop thread finished abnormaly" );
      vEventLoopHasFinished_B = true;
   }
   iLOG( "Event loop finished" );
#endif

   vStopRenderLoopSignal_SIG();

   return 1;
}


int iInit::closeWindow( bool _waitUntilClosed ) {
   if( ! getHaveContext() ) {return 0;}
   if( vMainLoopRunning_B ) {
      quitMainLoop();
      if( _waitUntilClosed ) {vQuitMainLoop_BT.join();}
#if WINDOWS
      destroyContext();
#endif
      return 1;
   }
   destroyContext();
   vCreateWindowReturn_I = -1000;
   return 1;
}


void iInit::s_standardWindowClose( iEventInfo _info )  {
   wLOG( "Standard WindowClose slot! Closing the window!" );
   _info.iInitPointer->closeWindow();
}


/*!
 * \brief Paused the main loop
 *
 * \param[in] _runInNewThread set this to true if wish to run the pause in e new thread (default: false)
 *
 * \warning Set _runInNewThread to true if you are running this in the event loop (in a event slot) because
 *          there are some problems with Windows and the event loop.
 *
 * \returns Nothing
 */
void iInit::pauseMainLoop( bool _runInNewThread ) {
   if( ! vMainLoopRunning_B )
      return;

   if( _runInNewThread ) {
      vPauseThread_BT = std::thread( &iInit::pauseMainLoop, this, false );
      return;
   }

   vEventLoopPaused_B = true;
   vPauseRenderLoop_SIG();

   while( !vEventLoopISPaused_B )
      B_SLEEP( milliseconds, 10 );

   iLOG( "Loops paused" );
}

/*!
 * \brief Continues a paused main loop
 * \returns Nothing
 */
void iInit::continueMainLoop() {
   std::lock_guard<std::mutex> lLockEvent_BT( vEventLoopMutex_BT );
   vEventLoopPaused_B = false;
   vEventLoopWait_BT.notify_one();

   vContinueRenderLoop_SIG();

   iLOG( "Loops unpaused" );
}

/*!
 * \brief Recreates the Window / OpenGL context
 *
 * Does nothing when the main loop is not running
 *
 * \param[in] _runInNewThread set this to true if wish to run the restart in e new thread (default: false)
 *
 * \warning Set _runInNewThread to true if you are running this in the event loop (in a event slot) because
 *          there are some problems with Windows and the event loop.
 *
 * \returns Nothing
 */
void iInit::restart( bool _runInNewThread ) {
   if( !vMainLoopRunning_B )
      return;

   // Calling restart in the eventloop can cause some problems
   if( _runInNewThread ) {
      vRestartThread_BT = std::thread( &iInit::restart, this, false );
      return;
   }

   pauseMainLoop();
   vWindowRecreate_B = true;
   makeContextCurrent();
   destroyContext();
#if UNIX
   createContext();
   standardRender( iEventInfo( this ) );
   makeNOContextCurrent();
#endif
   continueMainLoop();

#if WINDOWS
   vContinueWithEventLoop_B = false;
   if( vEventLoop_BT.joinable() )
      vEventLoop_BT.join();
   else
      eLOG( "Failed to join the event loop" );

   std::unique_lock<std::mutex> lLock_BT( vCreateWindowMutex_BT );
   vEventLoop_BT  = std::thread( &iInit::eventLoop, this );

   while( vCreateWindowReturn_I == -1000 ) vCreateWindowCondition_BT.wait( lLock_BT );

   {
      // Make sure lLockEvent_BT will be destroyed
      std::lock_guard<std::mutex> lLockEvent_BT( vStartEventMutex_BT );
      vContinueWithEventLoop_B = true;
      vStartEventCondition_BT.notify_one();
   }
#endif

   vWindowRecreate_B = false;
}

/*!
 * \brief Recreates the window / OpenGL context if needed
 *
 * Does nothing when the main loop is not running
 * (runs iInit::restart)
 *
 * \param[in] _runInNewThread set this to true if wish to run the restart in e new thread (default: false)
 *
 * \warning Set _runInNewThread to true if you are running this in the event loop (in a event slot) because
 *          there are some problems with Windows and the event loop.
 *
 * \returns Nothing
 */
void iInit::restartIfNeeded( bool _runInNewThread ) {
   if( vWindowRecreate_B )
      restart( _runInNewThread );
}




}






// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
