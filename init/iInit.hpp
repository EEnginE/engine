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

#elif WINDOWS
#include "windows/iContext.hpp"
#endif

#include "uLog.hpp"
#include <boost/thread.hpp>



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
class iInit : public unix_x11::iContext, public iMouse {
#elif WINDOWS
class iInit : public windows_win32::iContext {
#endif
   private:
      RENDER_FUNC       fRender;

      bool              vMainLoopRunning_B;
      bool              vBoolCloseWindow_B;

      bool              vHaveGLEW_B;

      bool              vEventLoopHasFinished_B;
      bool              vRenderLoopHasFinished_B;

      boost::thread     vEventLoop_BT;
      boost::thread     vRenderLoop_BT;
      boost::thread     vQuitMainLoop_BT;
      
      boost::thread     vRestartThread_BT;
      boost::thread     vPauseThread_BT;
      
      boost::mutex      vMainLoopMutex_BT;
      boost::mutex      vEventLoopMutex_BT;
      
      boost::condition_variable vMainLoopWait_BT;
      boost::condition_variable vEventLoopWait_BT;
      
      bool              vMainLoopPaused_B;
      bool              vEventLoopPaused_B;
      
      bool              vMainLoopISPaused_B;
      bool              vEventLoopISPaused_B;
      
      bool              vLoopsPaused_B;
      
      bool              vWasMouseGrabbed_B;
      
      int               vCreateWindowReturn_I;

      
#if WINDOWS
      boost::mutex              vCreateWindowMutex_BT;
      boost::condition_variable vCreateWindowCondition_BT;
      
      boost::mutex              vStartEventMutex_BT;
      boost::condition_variable vStartEventCondition_BT;
      bool                      vContinueWithEventLoop_B;
#endif

      // Thread Functions --------------------------------------------------------- ###
      int  renderLoop();       //!< The render loop function
      int  eventLoop();        //!< The event loop function ( In PLATFORM/e_event.cpp )
      int  quitMainLoopCall(); //!< The actual function to quit the main loop

      // Signal handling ---------------------------------------------------------- ###
      static GLvoid handluSignal( int _signal ); //!< The signal handle function
      GLvoid _setThisForHandluSignal();          //!< Unfortunately you can not pass this with signal()

      // Standart callbacks NEW --------------------------------------------------- ###

      //! The standard render function
      static inline GLvoid standardRender( iEventInfo _info ) {
         glClearColor( 0, 0, 0, 1 );
         glClear( GL_COLOR_BUFFER_BIT );
         _info.iInitPointer->swapBuffers();
      }

      //! The standard Resize function
      GLvoid s_standardResize( iEventInfo _info ) {}

      //! The standard Window close function
      GLvoid s_standardWindowClose( iEventInfo _info );

      //! The standard Key function
      GLvoid s_standardKey( iEventInfo _info ) {}

      //! The standard Mouse function
      GLvoid s_standardMouse( iEventInfo _info ) {}
      
      //! The standard Mouse function
      GLvoid s_standardFocus( iEventInfo _info ) {
         iLOG "Focus " ADD _info.eFocus.hasFocus ? "got" : "lost" END
      }
      
      GLvoid s_advancedGrabControl( iEventInfo _info );
      
   public:
      iInit();
      ~iInit() {closeWindow();shutdown();}

      int    init();
      int    shutdown();
      int    startMainLoop( bool _wait = true );

      void   quitMainLoop();
      
      void   pauseMainLoop( bool _runInNewThread = false );
      void   continueMainLoop();
      
      void   restart( bool _runInNewThread = false );
      void   restartIfNeeded( bool _runInNewThread = false );

      /*!
       * \brief Quit the main loop and close the window
       * \param waitUntilClosed Wait until window is closed  \c DEFAULT: \b false
       * \returns \c SUCCESS: \a 1 -- \C FAIL: \a 0
       */
      int    closeWindow( bool _waitUntilClosed = false );
      

      GLvoid setRenderFunc( RENDER_FUNC _f )            {fRender = _f;}
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################


namespace e_engine_internal {

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
         if ( is_set == true ) {return false;}
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

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
