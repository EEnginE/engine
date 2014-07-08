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
namespace e_engine_internal {
__iInit_Pointer __iInit_Pointer_OBJ;
}

void iInit::_setThisForHandluSignal() {
   if ( ! e_engine_internal::__iInit_Pointer_OBJ.set( this ) ) {
      eLOG "There can only be ONE iInit Class" END
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
   addMousuSlot( &vMouse_SLOT );
   addFocusSlot( &vFocus_SLOT );

   vMainLoopRunning_B       = false;
   vBoolCloseWindow_B       = false;

   vMainLoopPaused_B        = false;
   vEventLoopHasFinished_B  = false;
   vMainLoopISPaused_B      = false;
   vEventLoopISPaused_B     = false;
   vLoopsPaused_B           = false;

   fRender                  = standardRender;

   vEventLoopHasFinished_B  = true;
   vRenderLoopHasFinished_B = true;

   vWasMouseGrabbed_B       = false;

   vCreateWindowReturn_I    = -1000;

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
   if ( (_info.type == E_EVENT_FOCUS) && _info.eFocus.hasFocus && vWasMouseGrabbed_B ) {
      // Focus restored
      vWasMouseGrabbed_B = false;
      if ( ! grabMouse() ) {
         // Cannot grab again when X11 has not handled some events

         for ( unsigned short int i = 0; i < 25; ++i ) {
            iLOG "Try Grab " ADD i + 1 ADD " of 25" END
            if ( grabMouse() )
               break; // Grab success
            B_SLEEP( milliseconds, 100 );
         }

      }
      return;
   }
   if ( (_info.type == E_EVENT_FOCUS) && !_info.eFocus.hasFocus && getIsMouseGrabbed() ) {
      // Focus lost
      vWasMouseGrabbed_B = true;
      freiMouse();
      return;
   }
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
 * More information iContext
 *
 * \par
 * \returns  1 -- All good
 * \returns -1 -- Unable to connect to the X-Server
 * \returns -2 -- Need a newer GLX version
 * \returns -3 -- Unable to find any matching fbConfig
 * \returns -4 -- Failed to create a X11 Window
 * \returns  3 -- Failed to create a context
 * \returns  4 -- Failed to init GLEW
 */
int iInit::init() {

   signal( SIGINT, handluSignal );
   signal( SIGTERM, handluSignal );

   if ( GlobConf.log.logFILE.logFileName.empty() ) {
      GlobConf.log.logFILE.logFileName =  SYSTEM.getLogFilePath();
#if UNIX
      GlobConf.log.logFILE.logFileName += "/Log";
#elif WINDOWS
      GlobConf.log.logFILE.logFileName += "\\Log";
#endif
   }

   if ( GlobConf.log.logDefaultInit )
      LOG.devInit();

   LOG.startLogLoop();

#if WINDOWS
   // Windows needs the PeekMessage call in the same thread, where the window is created
   boost::unique_lock<boost::mutex> lLock_BT( vCreateWindowMutex_BT );
   vEventLoop_BT  = boost::thread( &iInit::eventLoop, this );

   while ( vCreateWindowReturn_I == -1000 ) vCreateWindowCondition_BT.wait( lLock_BT );

   makiContextCurrent();
#else
   vCreateWindowReturn_I = createContext();
#endif

   if ( vCreateWindowReturn_I != 1 ) { return vCreateWindowReturn_I; }

   standardRender( this ); // Fill the Window with black

   return 1;
}

int iInit::shutdown() {
   return LOG.stopLogLoop();
}


void iInit::handluSignal( int _signal ) {
   iInit *_THIS = e_engine_internal::__iInit_Pointer_OBJ.get();;

   if ( _signal == SIGINT ) {
      if ( GlobConf.handleSIGINT == true ) {
         wLOG "Received SIGINT (Crt-C) => Closing Window and exiting(5);" END
         _THIS->closeWindow( true );
         _THIS->destroyContext();
         _THIS->shutdown();
         exit( 5 );
      }
      wLOG "Received SIGINT (Crt-C) => " ADD 'B', 'Y', "DO NOTHING" END
      return;
   }
   if ( _signal == SIGTERM ) {
      if ( GlobConf.handleSIGTERM == true ) {
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
int iInit::startMainLoop( bool _wait ) {
   if ( ! getHaveContext() ) {
      wLOG "Can not start the main loop. There is no OpenGL context!" END
      return 0;
   }
   vMainLoopRunning_B = true;
   makeNOContextCurrent();

#if UNIX_X11
   vMainLoopRunning_B = true;

   vEventLoop_BT  = boost::thread( &iInit::eventLoop, this );
#elif WINDOWS
   {
      // Make sure lLockEvent_BT will be destroyed
      boost::lock_guard<boost::mutex> lLockEvent_BT( vStartEventMutex_BT );
      vContinueWithEventLoop_B = true;
      vStartEventCondition_BT.notify_one();
   }
#endif
   vRenderLoop_BT = boost::thread( &iInit::renderLoop, this );

   if ( _wait ) {
      vEventLoop_BT.join();
      vRenderLoop_BT.join();
   }

   if ( getHaveContext() ) makiContextCurrent();  // Only ONE thread can have a context

   if ( vBoolCloseWindow_B ) {destroyContext();} // iInit::closeWindow() called?

   return 1;
}

GLvoid iInit::quitMainLoop() {
   vQuitMainLoop_BT = boost::thread( &iInit::quitMainLoopCall, this );
}



int iInit::quitMainLoopCall( ) {
   vMainLoopRunning_B = false;

#if WINDOWS
   vContinueWithEventLoop_B = false;
#endif

#if ! WINDOWS
   if ( ! vEventLoopHasFinished_B )
#if BOOST_VERSION < 105000
   {
      boost::posix_time::time_duration duration = boost::posix_time::milliseconds( GlobConf.timeoutForMainLoopThread_mSec );
      vEventLoop_BT.timed_join( duration );
   }
#else
      vEventLoop_BT.try_join_for( boost::chrono::milliseconds( GlobConf.timeoutForMainLoopThread_mSec ) );
#endif

   if ( ! vEventLoopHasFinished_B ) {
      vEventLoop_BT.interrupt();
      wLOG "Event Loop Timeout reached   -->  kill the thread" END
      vEventLoopHasFinished_B = true;
   }
   iLOG "Event loop finished" END
#endif


   if ( ! vRenderLoopHasFinished_B )
#if BOOST_VERSION < 105000
   {
      boost::posix_time::time_duration duration = boost::posix_time::milliseconds( GlobConf.timeoutForMainLoopThread_mSec );
      vRenderLoop_BT.timed_join( duration );
   }
#else
      vRenderLoop_BT.try_join_for( boost::chrono::milliseconds( GlobConf.timeoutForMainLoopThread_mSec ) );
#endif

   if ( ! vRenderLoopHasFinished_B ) {
      vRenderLoop_BT.interrupt();
      wLOG "Render Loop Timeout reached  -->  kill the thread" END
      vRenderLoopHasFinished_B = true;
   }

   iLOG "Render loop finished" END

   return 1;
}

int iInit::renderLoop( ) {
   iLOG "Render loop started" END
   vRenderLoopHasFinished_B = false;
   makiContextCurrent();  // Only ONE thread can have a context

   if ( GlobConf.win.VSync == true )
      enableVSync();

   while ( vMainLoopRunning_B ) {
      if ( vLoopsPaused_B ) {
         boost::unique_lock<boost::mutex> lLock_BT( vMainLoopMutex_BT );
         makeNOContextCurrent();
         vMainLoopISPaused_B = true;
         while ( vMainLoopPaused_B ) vMainLoopWait_BT.wait( lLock_BT );
         while ( !getHaveContext() || vWindowRecreate_B ) B_SLEEP( milliseconds, 10 );
#if WINDOWS
         B_SLEEP( milliseconds, 100 ); //!< \todo Remove this workaround for Windows (problem with iContext::makiContextCurrent)
#endif
         vMainLoopISPaused_B = false;
         if ( ! makiContextCurrent() ) {
            eLOG "Failed to make context current ==> quit render loop" END
            return -1;
         }
      }

      fRender( this );
   }

   if ( getHaveContext() ) makeNOContextCurrent();
   vRenderLoopHasFinished_B = true;
   return 0;
}

int iInit::closeWindow( bool _waitUntilClosed ) {
   if ( ! getHaveContext() ) {return 0;}
   if ( vMainLoopRunning_B ) {
      vBoolCloseWindow_B = true;
      quitMainLoop();
      if ( _waitUntilClosed ) {vQuitMainLoop_BT.join();}
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
   wLOG "Standard WindowClose slot! Closing the window!" END
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
   if ( ! vMainLoopRunning_B )
      return;

   if ( _runInNewThread ) {
      vPauseThread_BT = boost::thread( &iInit::pauseMainLoop, this, false );
      return;
   }

   vMainLoopPaused_B  = true;
   vEventLoopPaused_B = true;

   vLoopsPaused_B = true;

   while ( !vMainLoopISPaused_B || !vEventLoopISPaused_B )
      B_SLEEP( milliseconds, 10 );

   iLOG "Loops paused" END
}

/*!
 * \brief Continues a paused main loop
 * \returns Nothing
 */
void iInit::continueMainLoop() {
   vLoopsPaused_B = false;

   boost::lock_guard<boost::mutex> lLockEvent_BT( vEventLoopMutex_BT );
   vEventLoopPaused_B = false;
   vEventLoopWait_BT.notify_one();

   boost::lock_guard<boost::mutex> lLockMain_BT( vMainLoopMutex_BT );
   vMainLoopPaused_B = false;
   vMainLoopWait_BT.notify_one();

   iLOG "Loops unpaused" END
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
   if ( !vMainLoopRunning_B )
      return;

   // Calling restart in the eventloop can cause some problems
   if ( _runInNewThread ) {
      vRestartThread_BT = boost::thread( &iInit::restart, this, false );
      return;
   }

   pauseMainLoop();
   vWindowRecreate_B = true;
   destroyContext();
#if UNIX
   createContext();
   standardRender( iEventInfo( this ) );
   makeNOContextCurrent();
#endif
   continueMainLoop();

#if WINDOWS
   vContinueWithEventLoop_B = false;
   if ( vEventLoop_BT.joinable() )
      vEventLoop_BT.join();
   else
      eLOG "Failed to join the event loop" END

      boost::unique_lock<boost::mutex> lLock_BT( vCreateWindowMutex_BT );
   vEventLoop_BT  = boost::thread( &iInit::eventLoop, this );

   while ( vCreateWindowReturn_I == -1000 ) vCreateWindowCondition_BT.wait( lLock_BT );

   {
      // Make sure lLockEvent_BT will be destroyed
      boost::lock_guard<boost::mutex> lLockEvent_BT( vStartEventMutex_BT );
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
   if ( vWindowRecreate_B )
      restart( _runInNewThread );
}




}






// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
