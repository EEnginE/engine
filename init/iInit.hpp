/*!
 * \file iInit.hpp
 * \brief \b Classes: \a iInit
 * \sa e_iInit.cpp e_event.cpp
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


#ifndef E_INIT_HPP
#define E_INIT_HPP

#include "defines.hpp"

#include <vulkan/vulkan.h>

#if UNIX_X11
#include "x11/iWindow.hpp"
#include "iMouse.hpp"
#include "iInitSignals.hpp"

#elif WINDOWS
#include "windows/iContext.hpp"

#else
#error "PLATFORM not supported"
#endif

#include <thread>
#include <condition_variable>


namespace e_engine {

typedef void ( *RENDER_FUNC )( iEventInfo info );


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
class INIT_API iInit : public unix_x11::iWindow, public iInitSignals, public iMouse {
#elif WINDOWS
class INIT_API iInit : public windows_win32::iContext {
#else
#error "PLATFORM not supported"
#endif
 public:
   SLOT vGrabControl_SLOT; //!< Slot for grab control \sa iInit::s_advancedGrabControl

 private:
   std::vector<std::string> vExtensionList;

   VkInstance vInstance_vk;

   bool vMainLoopRunning_B; //!< Should the main loop be running?

   bool vEventLoopHasFinished_B; //!< Has the event loop finished?

   std::thread vEventLoop_BT; //!< The thread for the event loop

   std::mutex vEventLoopMutex_BT;

   std::condition_variable vEventLoopWait_BT;

   bool vEventLoopPaused_B; //!< SHOULD the event loop be paused?
   bool vEventLoopISPaused_B;

   bool vWasMouseGrabbed_B;

   int vCreateWindowReturn_I;

   bool vAreRenderLoopSignalsConnected_B;
   bool vIsVulkanSetup_B;

#if WINDOWS
   std::mutex vCreateWindowMutex_BT;
   std::condition_variable vCreateWindowCondition_BT;

   std::mutex vStartEventMutex_BT;
   std::condition_variable vStartEventCondition_BT;

   std::mutex vStopEventLoopMutex;
   std::condition_variable vStopEventLoopCondition;

   bool vContinueWithEventLoop_B;
#endif

   int loadExtensionList();
   int initVulkan();

   void destroyVulkan();

   // Thread Functions --------------------------------------------------------- ###
   int eventLoop();         //!< The event loop function ( In PLATFORM/e_event.cpp )
   void quitMainLoopCall(); //!< The actual function to quit the main loop

   // Signal handling ---------------------------------------------------------- ###
   static void handleSignal( int _signal ); //!< The signal handle function
   void _setThisForHandluSignal();          //!< Unfortunately you cannot pass this with signal()

   // Standart callbacks NEW --------------------------------------------------- ###

   void s_advancedGrabControl( iEventInfo const &_info );

   virtual void makeEInitEventBasicAbstract() {}

 public:
   iInit();
   virtual ~iInit();

   int init();
   int shutdown();
   int startMainLoop( bool _wait = true );

   void quitMainLoop();

   bool enableDefaultGrabControl();
   bool disableDefaultGrabControl();

   void closeWindow();

   bool isExtensionSupported( std::string _extension );

   static std::string vkResultToString( VkResult _res );
};

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
   bool is_set;

 public:
   __iInit_Pointer() {
      pointer = 0;
      is_set  = false;
   }
   ~__iInit_Pointer() { pointer = 0; }
   bool set( iInit *_THIS ) {
      if ( is_set == true ) {
         return false;
      }
      pointer = _THIS;
      is_set  = true;
      return true;
   }
   iInit *get() { return pointer; }
};
extern __iInit_Pointer __iInit_Pointer_OBJ;
}
}

#endif // E_INIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
