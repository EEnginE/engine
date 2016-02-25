/*!
 * \file iInit.cpp
 * \brief \b Classes: \a iInit
 * \sa e_event.cpp e_iInit.hpp
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

#include "defines.hpp"

#include <csignal>
#include <vulkan/vulkan.h>
#include <string.h>

#include "iInit.hpp"
#include "uSystem.hpp"
#include "uLog.hpp"

#if D_LOG_VULKAN_INIT
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

namespace e_engine {
namespace internal {
__iInit_Pointer __iInit_Pointer_OBJ;
}

void iInit::_setThisForHandluSignal() {
   if ( !internal::__iInit_Pointer_OBJ.set( this ) ) {
      eLOG( "There can only be ONE iInit Class" );
      throw std::string( "There can only be ONE iInit Class" );
   }
}

iInit::iInit() : vGrabControl_SLOT( &iInit::s_advancedGrabControl, this ) {

   vMainLoopRunning_B = false;

   vEventLoopHasFinished_B = false;
   vEventLoopISPaused_B    = false;
   vEventLoopPaused_B      = false;

   vEventLoopHasFinished_B = true;

   vWasMouseGrabbed_B = false;

   vCreateWindowReturn_I = -1000;

   vAreRenderLoopSignalsConnected_B = false;
   vIsVulkanSetup_B                 = false;

#if WINDOWS
   vContinueWithEventLoop_B = false;
#endif

   _setThisForHandluSignal();
}

iInit::~iInit() { shutdown(); }


/*!
 * \brief Handle focus change events when mouse is grabbed
 *
 * When the mouse is grabbed and focus is lost (Alt+tab), various
 * bad things can happen.
 *
 * This function makes sure that when focus was lost, the mouse will
 * be ungrabbed and when focus is restored that it will be locked again.
 */
void iInit::s_advancedGrabControl( iEventInfo const &_info ) {
   if ( ( _info.type == E_EVENT_FOCUS ) && _info.eFocus.hasFocus && vWasMouseGrabbed_B ) {
      // Focus restored
      vWasMouseGrabbed_B = false;
      if ( !grabMouse() ) {
         // Cannot grab again when X11 has not handled some events

         for ( unsigned short int i = 0; i < 25; ++i ) {
            iLOG( "Try Grab ", i + 1, " of 25" );
            if ( grabMouse() )
               break; // Grab success
            B_SLEEP( milliseconds, 100 );
         }
      }
      return;
   }
   if ( ( _info.type == E_EVENT_FOCUS ) && !_info.eFocus.hasFocus && getIsMouseGrabbed() ) {
      // Focus lost
      vWasMouseGrabbed_B = true;
      freeMouse();
      return;
   }
}

/*!
 * \brief Enables the default grab control
 * \returns true if grab control changed and false if not
 * \sa iInit::s_advancedGrabControl
 */
bool iInit::enableDefaultGrabControl() { return addFocusSlot( &vGrabControl_SLOT ); }

/*!
 * \brief Disables the default grab control
 * \returns true if grab control changed and false if not
 * \sa iInit::s_advancedGrabControl
 */
bool iInit::disableDefaultGrabControl() { return removeFocusSlot( &vGrabControl_SLOT ); }

std::string iInit::vkResultToString( VkResult _res ) {
   switch ( _res ) {
      case VK_SUCCESS: return "VK_SUCCESS";
      case VK_NOT_READY: return "VK_NOT_READY";
      case VK_TIMEOUT: return "VK_TIMEOUT";
      case VK_EVENT_SET: return "VK_EVENT_SET";
      case VK_EVENT_RESET: return "VK_EVENT_RESET";
      case VK_INCOMPLETE: return "VK_INCOMPLETE";
      case VK_ERROR_OUT_OF_HOST_MEMORY: return "VK_ERROR_OUT_OF_HOST_MEMORY";
      case VK_ERROR_OUT_OF_DEVICE_MEMORY: return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
      case VK_ERROR_INITIALIZATION_FAILED: return "VK_ERROR_INITIALIZATION_FAILED";
      case VK_ERROR_DEVICE_LOST: return "VK_ERROR_DEVICE_LOST";
      case VK_ERROR_MEMORY_MAP_FAILED: return "VK_ERROR_MEMORY_MAP_FAILED";
      case VK_ERROR_LAYER_NOT_PRESENT: return "VK_ERROR_LAYER_NOT_PRESENT";
      case VK_ERROR_EXTENSION_NOT_PRESENT: return "VK_ERROR_EXTENSION_NOT_PRESENT";
      case VK_ERROR_FEATURE_NOT_PRESENT: return "VK_ERROR_FEATURE_NOT_PRESENT";
      case VK_ERROR_INCOMPATIBLE_DRIVER: return "VK_ERROR_INCOMPATIBLE_DRIVER";
      case VK_ERROR_TOO_MANY_OBJECTS: return "VK_ERROR_TOO_MANY_OBJECTS";
      case VK_ERROR_FORMAT_NOT_SUPPORTED: return "VK_ERROR_FORMAT_NOT_SUPPORTED";
      case VK_ERROR_SURFACE_LOST_KHR: return "VK_ERROR_SURFACE_LOST_KHR";
      case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR: return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
      case VK_SUBOPTIMAL_KHR: return "VK_SUBOPTIMAL_KHR";
      case VK_ERROR_OUT_OF_DATE_KHR: return "VK_ERROR_OUT_OF_DATE_KHR";
      case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR: return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
      case VK_ERROR_VALIDATION_FAILED_EXT: return "VK_ERROR_VALIDATION_FAILED_EXT";
      case VK_RESULT_RANGE_SIZE: return "VK_RESULT_RANGE_SIZE";
      case VK_RESULT_MAX_ENUM: return "VK_RESULT_MAX_ENUM";
   }
}

int iInit::loadExtensionList() {
   VkResult lResult;
   uint32_t lExtCount;

   lResult = vkEnumerateInstanceExtensionProperties( NULL, &lExtCount, NULL );

   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumerateInstanceExtensionProperties' returned ", vkResultToString( lResult ) );
      return lResult;
   }

   dLOG( "Extensions: ", lExtCount );
   VkExtensionProperties *lPorps = new VkExtensionProperties[lExtCount];

   lResult = vkEnumerateInstanceExtensionProperties( NULL, &lExtCount, lPorps );

   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumerateInstanceExtensionProperties' returned ", vkResultToString( lResult ) );
      delete[] lPorps;
      return lResult;
   }

   dVkLOG( lExtCount, " Extensions found:" );

   for ( uint32_t i = 0; i < lExtCount; i++ ) {
      dVkLOG( "  -- '", lPorps[i].extensionName, "'  -  specVersion: ", lPorps[i].specVersion );
      vExtensionList.emplace_back( lPorps[i].extensionName );
   }

   delete[] lPorps;

   return 0;
}

/*!
 * \brief Creates the vulkan instance
 *
 * \returns  0 -- Success
 * \returns  1 -- Missing extension(s)
 */
int iInit::initVulkan() {
   VkResult lResult;
   uint32_t lPorpCount;

   lResult = vkEnumerateInstanceLayerProperties( &lPorpCount, NULL );
   dVkLOG( "InstanceLayerProperties: ", lPorpCount );
   //! \todo expand layers section


   int lRet = loadExtensionList();
   if ( lRet != 0 )
      return lRet;

   static const char *lExtensions[] = {VK_KHR_SURFACE_EXTENSION_NAME,
                                       E_VK_KHR_SYSTEM_SURVACE_EXTENSION_NAME};

   dVkLOG( "Using ", sizeof( lExtensions ) / sizeof( lExtensions[0] ), " extension: " );

   for ( auto const &i : lExtensions ) {
      if ( !isExtensionSupported( i ) ) {
         eLOG( "Extension '", i, "' is not supported!" );
         return 1;
      }
      dVkLOG( "  -- '", i, '\'' );
   }

   VkInstanceCreateInfo lCreateInfo_vk;
   VkApplicationInfo lAppInfo_vk;

   lAppInfo_vk.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   lAppInfo_vk.pNext              = NULL;
   lAppInfo_vk.pApplicationName   = GlobConf.config.appName.c_str();
   lAppInfo_vk.pEngineName        = "EEnginE";
   lAppInfo_vk.apiVersion         = VK_API_VERSION;
   lAppInfo_vk.applicationVersion = 1; //!< \todo change this const
   lAppInfo_vk.engineVersion      = 1; //!< \todo change this const

   lCreateInfo_vk.sType                   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   lCreateInfo_vk.pNext                   = NULL;
   lCreateInfo_vk.flags                   = 0;
   lCreateInfo_vk.pApplicationInfo        = &lAppInfo_vk;
   lCreateInfo_vk.enabledLayerCount       = 0;
   lCreateInfo_vk.ppEnabledLayerNames     = NULL;
   lCreateInfo_vk.enabledExtensionCount   = sizeof( lExtensions ) / sizeof( lExtensions[0] );
   lCreateInfo_vk.ppEnabledExtensionNames = lExtensions;

   lResult = vkCreateInstance( &lCreateInfo_vk, NULL, &vInstance_vk );

   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkCreateInstance' returned ", vkResultToString( lResult ) );
      return lResult;
   }

   return 0;
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
 * \returns  0 -- Success
 * \returns  1 -- Missing extension(s)
 */
int iInit::init() {

   signal( SIGINT, handleSignal );
   signal( SIGTERM, handleSignal );

   if ( GlobConf.log.logFILE.logFileName.empty() ) {
      GlobConf.log.logFILE.logFileName = SYSTEM.getLogFilePath();
#if UNIX
      GlobConf.log.logFILE.logFileName += "/Log";
#elif WINDOWS
      GlobConf.log.logFILE.logFileName += "\\Log";
#endif
   }

   if ( GlobConf.log.logDefaultInit )
      LOG.devInit();

   LOG.startLogLoop();

   auto lVkInitErr = initVulkan();

   if ( lVkInitErr ) {
      eLOG( "Failed to init Vulkan" );
      return lVkInitErr;
   }

#if WINDOWS
   // Windows needs the PeekMessage call in the same thread where the window is created
   std::unique_lock<std::mutex> lLock_BT( vCreateWindowMutex_BT );
   vEventLoop_BT = std::thread( &iInit::eventLoop, this );

   while ( vCreateWindowReturn_I == -1000 )
      vCreateWindowCondition_BT.wait( lLock_BT );

   makeContextCurrent();
#else
   vCreateWindowReturn_I = createWindow();
#endif

   if ( vCreateWindowReturn_I != 1 ) {
      return vCreateWindowReturn_I;
   }

   vIsVulkanSetup_B = true;

   return 0;
}

void iInit::destroyVulkan() {
   if ( !vIsVulkanSetup_B )
      return;

   vIsVulkanSetup_B = false;
   vExtensionList.clear();
   vkDestroyInstance( vInstance_vk, NULL );
}

int iInit::shutdown() {
   destroyVulkan();
   closeWindow();

   if ( vEventLoop_BT.joinable() )
      vEventLoop_BT.join();

   return LOG.stopLogLoop();
}


void iInit::handleSignal( int _signal ) {
   iInit *_THIS = internal::__iInit_Pointer_OBJ.get();
   ;

   if ( _signal == SIGINT ) {
      if ( GlobConf.handleSIGINT == true ) {
         wLOG( "Received SIGINT (Crt-C) => Closing Window and exiting(5);" );
         _THIS->closeWindow( true );
         _THIS->destroyWindow();
         _THIS->shutdown();
         exit( 5 );
      }
      wLOG( "Received SIGINT (Crt-C) => ", 'B', 'Y', "DO NOTHING" );
      return;
   }
   if ( _signal == SIGTERM ) {
      if ( GlobConf.handleSIGTERM == true ) {
         wLOG( "Received SIGTERM => Closing Window and exiting(6);" );
         _THIS->closeWindow( true );
         _THIS->destroyWindow();
         _THIS->shutdown();
         exit( 6 );
      }
      wLOG( "Received SIGTERM => Closing Window and exiting(6);", 'B', 'Y', "DO NOTHING" );
      return;
   }
}


/*!
 * \brief Starts the main loop
 * \returns \c SUCCESS: \a 1 -- \c FAIL: \a 0
 */
int iInit::startMainLoop( bool _wait ) {
   if ( !vIsVulkanSetup_B ) {
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

   vResize_SIG.send( _tempInfo );

   if ( !vAreRenderLoopSignalsConnected_B ) {
      eLOG( "iInit is not yet connected with a render system!" );
      return 0;
   }

#if UNIX_X11
   vEventLoop_BT = std::thread( &iInit::eventLoop, this );
#elif WINDOWS
   {
      // Make sure lLockEvent_BT will be destroyed
      std::lock_guard<std::mutex> lLockEvent_BT( vStartEventMutex_BT );
      vContinueWithEventLoop_B = true;
      vStartEventCondition_BT.notify_one();
   }
#endif

   if ( _wait ) {
#if WINDOWS
      {
         std::unique_lock<std::mutex> lLockEvent_BT( vStopEventLoopMutex );
         while ( !vEventLoopHasFinished_B )
            vStopEventLoopCondition.wait( lLockEvent_BT );
      }
#else
      if ( vEventLoop_BT.joinable() )
         vEventLoop_BT.join();
#endif

      // Wait for quit main loop to finish
      if ( vQuitMainLoop_BT.joinable() )
         vQuitMainLoop_BT.join();
   }

   return 1;
}

void iInit::quitMainLoop() { vQuitMainLoop_BT = std::thread( &iInit::quitMainLoopCall, this ); }



int iInit::quitMainLoopCall() {
   vMainLoopRunning_B = false;
   LOG.nameThread( L"kill" );

#if WINDOWS
   vContinueWithEventLoop_B = false;
#else
   if ( !vEventLoopHasFinished_B )
      vEventLoop_BT.join();

   if ( !vEventLoopHasFinished_B ) {
      wLOG( "Event Loop thread finished abnormaly" );
      vEventLoopHasFinished_B = true;
   }
   iLOG( "Event loop finished" );
#endif

   return 1;
}


int iInit::closeWindow( bool _waitUntilClosed ) {
   if ( vIsVulkanSetup_B ) {
      return 0;
   }
   if ( vMainLoopRunning_B ) {
      quitMainLoop();
      if ( _waitUntilClosed && vQuitMainLoop_BT.joinable() )
         vQuitMainLoop_BT.join();
   }
   destroyWindow();

#if WINDOWS
   // The event loop thread must do some stuff

   {
      std::lock_guard<std::mutex> lLockEvent_BT( vStartEventMutex_BT );
      vContinueWithEventLoop_B = true;
      vStartEventCondition_BT.notify_one();
   }

   if ( vEventLoop_BT.joinable() && _waitUntilClosed )
      vEventLoop_BT.join();

   iLOG( "Done close window" );

   vContinueWithEventLoop_B = false;
#endif

   vCreateWindowReturn_I = -1000;
   return 1;
}

bool iInit::isExtensionSupported( std::string _extension ) {
   for ( auto const &i : vExtensionList ) {
      if ( i == _extension ) {
         return true;
      }
   }
   return false;
}
}





// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
