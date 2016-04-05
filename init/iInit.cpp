/*!
 * \file iInit.cpp
 * \brief \b Classes: \a iInit
 * \sa e_event.cpp e_iInit.hpp
 */
/*
 * Copyright (C) 2016 EEnginE project
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
#include "uEnum2Str.hpp"

#if D_LOG_VULKAN_INIT
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

#if D_LOG_VK_DEBUG_REPORT_INFO
#define CONTINUE_DB_REPORT_INFO
#else
#define CONTINUE_DB_REPORT_INFO return;
#endif

namespace e_engine {
namespace internal {
__iInit_Pointer __iInit_Pointer_OBJ;
}


PFN_vkCreateDebugReportCallbackEXT f_vkCreateDebugReportCallbackEXT   = nullptr;
PFN_vkDestroyDebugReportCallbackEXT f_vkDestroyDebugReportCallbackEXT = nullptr;

void iInit::_setThisForHandluSignal() {
   if ( !internal::__iInit_Pointer_OBJ.set( this ) ) {
      eLOG( "There can only be ONE iInit Class" );
      throw std::string( "There can only be ONE iInit Class" );
   }
}

iInit::iInit() : vGrabControl_SLOT( &iInit::s_advancedGrabControl, this ) {
   vExtensionsToUse.emplace_back( VK_EXT_DEBUG_REPORT_EXTENSION_NAME );
   vExtensionsToUse.emplace_back( VK_KHR_SURFACE_EXTENSION_NAME );
   vExtensionsToUse.emplace_back( E_VK_KHR_SYSTEM_SURVACE_EXTENSION_NAME );

   vDeviceExtensionsToUse.emplace_back( VK_KHR_SWAPCHAIN_EXTENSION_NAME );

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
      if ( !vWindow.grabMouse() ) {
         // Cannot grab again when X11 has not handled some events

         for ( unsigned short int i = 0; i < 25; ++i ) {
            iLOG( "Try Grab ", i + 1, " of 25" );
            if ( vWindow.grabMouse() )
               break; // Grab success
            B_SLEEP( milliseconds, 100 );
         }
      }
      return;
   }
   if ( ( _info.type == E_EVENT_FOCUS ) && !_info.eFocus.hasFocus && vWindow.getIsMouseGrabbed() ) {
      // Focus lost
      vWasMouseGrabbed_B = true;
      vWindow.freeMouse();
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

void iInit::vulkanDebugHandler( VkDebugReportFlagsEXT _flags,
                                VkDebugReportObjectTypeEXT,
                                uint64_t,
                                size_t,
                                int32_t _msgCode,
                                std::string _layerPrefix,
                                std::string _msg ) {
   std::string lTempStr;
   char lLogStr;

   if ( _flags & VK_DEBUG_REPORT_DEBUG_BIT_EXT ) {
      lLogStr = 'D';
      lTempStr += "VK_DEBUG:  ";
      CONTINUE_DB_REPORT_INFO
   }

   if ( _flags & VK_DEBUG_REPORT_INFORMATION_BIT_EXT ) {
      lLogStr = 'I';
      lTempStr += "VK_INFO:   ";
      CONTINUE_DB_REPORT_INFO
   }

   if ( _flags & VK_DEBUG_REPORT_WARNING_BIT_EXT ) {
      lLogStr = 'W';
      lTempStr += "VK_WARN:   ";
   }

   if ( _flags & VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT ) {
      lLogStr = 'W';
      lTempStr += "VK_P_WARN: ";
   }

   if ( _flags & VK_DEBUG_REPORT_ERROR_BIT_EXT ) {
      lLogStr = 'E';
      lTempStr += "VK_ERROR:  ";
   }

   LOG( lLogStr,
        false,
        W_FILE,
        __LINE__,
        W_FUNC,
        std::this_thread::get_id(),
        "(",
        _layerPrefix,
        ") ",
        _msgCode,
        ":",
        _msg );
}

VkBool32 vulkanDebugCallback( VkDebugReportFlagsEXT _flags,
                              VkDebugReportObjectTypeEXT _objType,
                              uint64_t _obj,
                              size_t _location,
                              int32_t _msgCode,
                              const char *_layerPrefix,
                              const char *_msg,
                              void *_usrData ) {
   if ( _usrData ) {
      reinterpret_cast<iInit *>( _usrData )
            ->vulkanDebugHandler( _flags, _objType, _obj, _location, _msgCode, _layerPrefix, _msg );
   }
   return VK_FALSE;
}


int iInit::initDebug() {
   f_vkCreateDebugReportCallbackEXT = reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
         vkGetInstanceProcAddr( vInstance_vk, "vkCreateDebugReportCallbackEXT" ) );

   f_vkDestroyDebugReportCallbackEXT = reinterpret_cast<PFN_vkDestroyDebugReportCallbackEXT>(
         vkGetInstanceProcAddr( vInstance_vk, "vkDestroyDebugReportCallbackEXT" ) );

   if ( !f_vkCreateDebugReportCallbackEXT )
      return 2;

   auto lRes = f_vkCreateDebugReportCallbackEXT(
         vInstance_vk, &vDebugCreateInfo_vk, nullptr, &vCallback );
   if ( lRes ) {
      eLOG( "'vkCreateDebugReportCallbackEXT' returned ", uEnum2Str::toStr( lRes ) );
      return 1;
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
 * \returns  1 -- Failed to init vulkan
 * \returns  2 -- Failed to create surface
 * \returns  3 -- Failed to load devices
 * \returns  4 -- Failed to create a vulkan device
 * \returns  5 -- Failed to create the swapchain
 */
int iInit::init( std::vector<std::string> _layers ) {
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

   vDebugCreateInfo_vk.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
   vDebugCreateInfo_vk.pNext = nullptr;
   vDebugCreateInfo_vk.flags = VK_DEBUG_REPORT_INFORMATION_BIT_EXT |
                               VK_DEBUG_REPORT_WARNING_BIT_EXT |
                               VK_DEBUG_REPORT_PERFORMANCE_WARNING_BIT_EXT |
                               VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_DEBUG_BIT_EXT;
   vDebugCreateInfo_vk.pfnCallback = &vulkanDebugCallback;
   vDebugCreateInfo_vk.pUserData   = reinterpret_cast<void *>( this );

   if ( initVulkan( _layers ) )
      return 1;

   if ( vEnableVulkanDebug )
      initDebug();

   vCreateWindowReturn_I = vWindow.createWindow();
   if ( vCreateWindowReturn_I != 0 ) {
      return vCreateWindowReturn_I;
   }

   vSurface_vk = vWindow.getVulkanSurface( vInstance_vk );
   if ( !vSurface_vk )
      return 2;

   if ( loadDevices() )
      return 3;

   vDevice_vk.pDevice = chooseDevice();

   if ( createDevice( _layers ) )
      return 4;

   if ( loadDeviceSurfaceInfo() )
      return 5;


#if WINDOWS
   // Windows needs the PeekMessage call in the same thread where the window is created
   std::unique_lock<std::mutex> lLock_BT( vCreateWindowMutex_BT );
   vEventLoop_BT = std::thread( &iInit::eventLoop, this );

   while ( vCreateWindowReturn_I == -1000 )
      vCreateWindowCondition_BT.wait( lLock_BT );

#endif

   vIsVulkanSetup_B = true;

   return 0;
}

int iInit::handleResize() {
   if ( !vIsVulkanSetup_B ) {
      eLOG( "Vulkan not setup yet" );
      return -1;
   }

   if ( !vSurface_vk )
      return 1;

   if ( loadDeviceSurfaceInfo() )
      return 2;

   return 0;
}

void iInit::destroyVulkan() {
   if ( !vIsVulkanSetup_B )
      return;

   dVkLOG( "Vulkan cleanup [iInit]:" );

   if ( vDevice_vk.device )
      vkDeviceWaitIdle( vDevice_vk.device );

   dVkLOG( "  -- destroying surface" );
   if ( vSurface_vk )
      vkDestroySurfaceKHR( vInstance_vk, vSurface_vk, nullptr );


   dVkLOG( "  -- destroying device" );
   if ( vDevice_vk.device )
      vkDestroyDevice( vDevice_vk.device, nullptr );

   dVkLOG( "  -- destroying debug callback" );
   if ( vCallback && f_vkDestroyDebugReportCallbackEXT )
      f_vkDestroyDebugReportCallbackEXT( vInstance_vk, vCallback, nullptr );

   dVkLOG( "  -- destroying vulkan instance" );
   if ( vInstance_vk )
      vkDestroyInstance( vInstance_vk, nullptr );

   vDevice_vk.device  = nullptr;
   vDevice_vk.pDevice = nullptr;
   vCallback          = nullptr;
   vInstance_vk       = nullptr;
   vSurface_vk        = nullptr;

   vIsVulkanSetup_B = false;
   vExtensionList.clear();
   vDeviceExtensionList.clear();
   vQueues_vk.clear();
   vLayerProperties_vk.clear();
   vDeviceLayerProperties_vk.clear();
   vPhysicalDevices_vk.clear();
   vSurfaceInfo_vk.formats.clear();
   vSurfaceInfo_vk.presentModels.clear();
   vQueueMutexMap.clear();

   dVkLOG( "  -- DONE" );
}

int iInit::shutdown() {
   destroyVulkan();
   closeWindow();

   return LOG.stopLogLoop();
}

/*!
 * \brief enables VSync
 * \note This will only take effect BEFORE creating the swapchain
 */
void iInit::enableVSync() { vEnableVSync = true; }

/*!
 * \brief disables VSync
 * \note This will only take effect BEFORE creating the swapchain
 */
void iInit::disableVSync() { vEnableVSync = false; }


void iInit::handleSignal( int _signal ) {
   iInit *_THIS = internal::__iInit_Pointer_OBJ.get();
   ;

   if ( _signal == SIGINT ) {
      if ( GlobConf.handleSIGINT == true ) {
         wLOG( "Received SIGINT (Crt-C) => Closing Window and exiting(5);" );
         _THIS->shutdown();
         exit( 5 );
      }
      wLOG( "Received SIGINT (Crt-C) => ", 'B', 'Y', "DO NOTHING" );
      return;
   }
   if ( _signal == SIGTERM ) {
      if ( GlobConf.handleSIGTERM == true ) {
         wLOG( "Received SIGTERM => Closing Window and exiting(6);" );
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

#if UNIX_X11 || UNIX_WAYLAND
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
   }

   return 1;
}

void iInit::quitMainLoop() { vMainLoopRunning_B = false; }

/*!
 * \brief Quit the main loop and close the window
 */
void iInit::closeWindow() {
   if ( vIsVulkanSetup_B || !vWindow.getIsWindowCreated() )
      return;

   if ( vMainLoopRunning_B ) {
      quitMainLoop();
   }

   if ( vEventLoop_BT.joinable() )
      vEventLoop_BT.join();

   vWindow.destroyWindow();

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
}

iWindowBasic *iInit::getWindow() { return static_cast<iWindowBasic *>( &vWindow ); }

bool iInit::isExtensionSupported( std::string _extension ) {
   for ( auto const &i : vExtensionList ) {
      if ( i == _extension ) {
         return true;
      }
   }
   return false;
}

bool iInit::isDeviceExtensionSupported( std::string _extension ) {
   for ( auto const &i : vDeviceExtensionList ) {
      if ( i == _extension ) {
         return true;
      }
   }
   return false;
}

/*!
 * \brief Returns the queue family index, that supports _flags
 * \param _flags Flags the Queue family MUST support
 * \returns the queue family index, that supports _flags (UINT32_MAX on not found / error)
 *
 * \vkIntern
 */
uint32_t iInit::getQueueFamily( VkQueueFlags _flags ) {
   if ( vDevice_vk.pDevice == nullptr )
      return UINT32_MAX;

   uint32_t lCounter = 0;
   for ( auto const &i : vDevice_vk.pDevice->queueFamilyProperties ) {
      if ( i.queueFlags & _flags )
         return lCounter;

      lCounter++;
   }
   return UINT32_MAX;
}

/*!
 * \brief Selects and returns a vulkan queue
 *
 * Selects and returns a vulkan queue. This function will not return the same
 * queue twice unless the queue is (manualy freed)
 *
 * \param _flags    Flags the Queue MUST support
 * \param _priority Queue target priority
 * \returns the best matching queue; nullptr if none found
 *
 * \vkIntern
 */
VkQueue iInit::getQueue( VkQueueFlags _flags, float _priority, uint32_t *_queueFamily ) {
   std::lock_guard<std::mutex> lGuard( vQueueAccessMutex );
   float lMinDiff = 100.0f;
   VkQueue lQueue = nullptr;

   for ( auto i : vQueues_vk ) {
      if ( !( i.flags & _flags ) )
         continue;

      auto lTemp = i.priority - _priority;
      lTemp = lTemp < 0 ? -lTemp : lTemp; // Make positive
      if ( lTemp < lMinDiff ) {
         lMinDiff = lTemp;
         lQueue = i.queue;
         if ( _queueFamily )
            *_queueFamily = i.familyIndex;
      }
   }

   return lQueue;
}


std::mutex &iInit::getQueueMutex( VkQueue _queue ) {
   std::lock_guard<std::mutex> lGuard( vQueueAccessMutex );
   return vQueueMutexMap[_queue];
}


/*!
 * \brief Checks whether a format is supported on the device
 * \vkIntern
 */
bool iInit::isFormatSupported( VkFormat _format ) {
   if ( vDevice_vk.pDevice == nullptr )
      return false;

   return vDevice_vk.pDevice->formats[_format].linearTilingFeatures != 0 ||
          vDevice_vk.pDevice->formats[_format].optimalTilingFeatures != 0 ||
          vDevice_vk.pDevice->formats[_format].bufferFeatures != 0;
}

/*!
 * \brief Checks whether a format supports a feature
 * \param _format The format to check
 * \param _flags  The flags the format must support
 * \param _type   The feature type
 *
 * Supported values for _type:
 *   - VK_IMAGE_TILING_LINEAR
 *   - VK_IMAGE_TILING_OPTIMAL
 *   - VK_IMAGE_TILING_MAX_ENUM == buffer
 *
 * \vkIntern
 */
bool iInit::formatSupportsFeature( VkFormat _format,
                                   VkFormatFeatureFlagBits _flags,
                                   VkImageTiling _type ) {
   if ( vDevice_vk.pDevice == nullptr )
      return false;

   switch ( _type ) {
      case VK_IMAGE_TILING_LINEAR:
         return ( vDevice_vk.pDevice->formats[_format].linearTilingFeatures & _flags ) != 0;
      case VK_IMAGE_TILING_OPTIMAL:
         return ( vDevice_vk.pDevice->formats[_format].optimalTilingFeatures & _flags ) != 0;
      case VK_IMAGE_TILING_MAX_ENUM:
         return ( vDevice_vk.pDevice->formats[_format].bufferFeatures & _flags ) != 0;
      default: return false;
   }
}

/*!
 * \brief returns the best supported memory index
 * \param _bits  The bitfield (see vkGetImageMemoryRequirements)
 * \param _flags Flags the memory type must support
 * \returns a memory index or UINT32_MAX on error
 * \vkIntern
 */
uint32_t iInit::getMemoryTypeIndexFromBitfield( uint32_t _bits, VkMemoryHeapFlags _flags ) {
   for ( uint16_t i = 0; i < vDevice_vk.pDevice->memoryProperties.memoryTypeCount; i++ ) {
      if ( ( _bits & 1 ) ) {
         // Finds best match because of memory type ordering
         if ( _flags ==
              ( vDevice_vk.pDevice->memoryProperties.memoryTypes[i].propertyFlags & _flags ) ) {
            return i;
         }
      }
      _bits >>= 1;
   }

   return UINT32_MAX;
}

/*!
 * \returns The vulkan device handle
 * \vkIntern
 */
VkDevice iInit::getDevice() { return vDevice_vk.device; }

/*!
 * \returns The vulkan surface
 * \vkIntern
 */
VkSurfaceKHR iInit::getVulkanSurface() { return vSurface_vk; }

/*!
 * \returns The vulkan surface info
 * \vkIntern
 */
iInit::SurfaceInfo_vk iInit::getSurfaceInfo() { return vSurfaceInfo_vk; }
}



// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
