/*!
 * \file iInit.hpp
 * \brief \b Classes: \a iInit
 * \sa e_iInit.cpp e_event.cpp
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


#ifndef E_INIT_HPP
#define E_INIT_HPP

#include "defines.hpp"

#if UNIX_X11
#include <GL/glxew.h>
#include "x11/iContext.hpp"
#include "iMouse.hpp"
#include "iInitSignals.hpp"

#elif WINDOWS
#include "windows/iContext.hpp"

#else
#error "PLATFORM not supported"
#endif

#include "uLog.hpp"
#include <thread>


namespace e_engine {

typedef GLvoid( *RENDER_FUNC )( iEventInfo info );


/*!
 * \class e_engine::iInit
 * \brief Init the <b>E Engine</b>
 *
 * This class handels the context with the iContext class. It
 * sets the Crt-C handle function and starts the main and eventLoop
 * loop. This should be after uConfig the 1st class you generate
 * and you will do all window and curser config with it.
 *
 * \note Until now there is no MS Windows or Mac support
 *
 * \note Cursor functions are not implemented yet
 *
 * \sa iContext uConfig e_iInit.cpp e_event.cpp
 */
#if UNIX_X11
class iInit : public unix_x11::iContext, public iInitSignals, public iMouse {
#elif WINDOWS
class iInit : public windows_win32::iContext {
#else
#error "PLATFORM not supported"
#endif
   public:
      SLOT   vGrabControl_SLOT;   //!< Slot for grab control \sa iInit::s_advancedGrabControl

   private:
      bool        vMainLoopRunning_B;   //!< Should the main loop be running?

      bool        vEventLoopHasFinished_B; //!< Has the event loop finished?

      std::thread vEventLoop_BT;           //!< The thread for the event loop
      std::thread vQuitMainLoop_BT;

      std::thread vRestartThread_BT;
      std::thread vPauseThread_BT;

      std::mutex  vEventLoopMutex_BT;

      std::condition_variable vEventLoopWait_BT;

      bool        vEventLoopPaused_B;    //!< SHOULD the event loop be paused?
      bool        vEventLoopISPaused_B;

      bool        vWasMouseGrabbed_B;

      int         vCreateWindowReturn_I;

      bool        vAreRenderLoopSignalsConnected_B;

      uSignal<void, bool> vStartRenderLoopSignal_SIG;
      uSignal<void>       vStopRenderLoopSignal_SIG;

      uSignal<void>       vPauseRenderLoop_SIG;
      uSignal<void>       vContinueRenderLoop_SIG;


#if WINDOWS
      std::mutex              vCreateWindowMutex_BT;
      std::condition_variable vCreateWindowCondition_BT;

      std::mutex              vStartEventMutex_BT;
      std::condition_variable vStartEventCondition_BT;

      std::mutex              vStopEventLoopMutex;
      std::condition_variable vStopEventLoopCondition;

      bool                    vContinueWithEventLoop_B;
#endif

      // Thread Functions --------------------------------------------------------- ###
      int  eventLoop();        //!< The event loop function ( In PLATFORM/e_event.cpp )
      int  quitMainLoopCall(); //!< The actual function to quit the main loop

      // Signal handling ---------------------------------------------------------- ###
      static GLvoid handleSignal( int _signal ); //!< The signal handle function
      GLvoid _setThisForHandluSignal();          //!< Unfortunately you cannot pass this with signal()

      // Standart callbacks NEW --------------------------------------------------- ###

      //! The standard render function
      static inline GLvoid standardRender( iEventInfo const &_info ) {
         glClearColor( 0, 0, 0, 1 );
         glClear( GL_COLOR_BUFFER_BIT );
         _info.iInitPointer->swapBuffers();
      }

      GLvoid s_advancedGrabControl( iEventInfo const &_info );

      virtual void makeEInitEventBasicAbstract() {}

   public:
      iInit();
      ~iInit() {closeWindow(); shutdown();}

      int    init();
      int    shutdown();
      int    startMainLoop( bool _wait = true );

      void   quitMainLoop();

      void   pauseMainLoop( bool _runInNewThread = false );
      void   continueMainLoop();

      void   restart( bool _runInNewThread = false );
      void   restartIfNeeded( bool _runInNewThread = false );

      bool   enableDefaultGrabControl();
      bool   disableDefaultGrabControl();

      /*!
       * \brief Quit the main loop and close the window
       * \param waitUntilClosed Wait until window is closed  \c DEFAULT: \b false
       * \returns \c SUCCESS: \a 1 -- \C FAIL: \a 0
       */
      int    closeWindow( bool _waitUntilClosed = false );

      template<class __C>
      void   addRenderSlots( uSlot<void, __C, bool> *_start, uSlot<void, __C> *_stop, uSlot<void, __C> *_pause, uSlot<void, __C> *_continue ) {
         vStartRenderLoopSignal_SIG.connect( _start );
         vStopRenderLoopSignal_SIG.connect( _stop );
         vPauseRenderLoop_SIG.connect( _pause );
         vContinueRenderLoop_SIG.connect( _continue );

         vAreRenderLoopSignalsConnected_B = true;
      }
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################


namespace internal {

/*!
 * \class __iInit_Pointer
 * \brief Need this for the static signal Callback
 *
 * \todo Remove this workaround / hack
 *
 */
class __iInit_Pointer {
   private:
      iInit *pointer;
      bool  is_set;
   public:
      __iInit_Pointer() {
         pointer = 0;
         is_set  = false;
      }
      ~__iInit_Pointer() {pointer = 0;}
      bool set( iInit *_THIS ) {
         if( is_set == true ) {return false;}
         pointer = _THIS;
         is_set = true;
         return true;
      }
      iInit *get() {return pointer;}
};
extern __iInit_Pointer __iInit_Pointer_OBJ;
}

}

#endif // E_INIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
