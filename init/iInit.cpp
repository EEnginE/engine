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
#include "uEnum2Str.hpp"

#if D_LOG_VULKAN_INIT
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

#define GET_VERSION( v )                                                                           \
   VK_VERSION_MAJOR( v ), L'.', VK_VERSION_MINOR( v ), L'.', VK_VERSION_PATCH( v )

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

   vExtensionsToUse.emplace_back( VK_KHR_SURFACE_EXTENSION_NAME );
   vExtensionsToUse.emplace_back( E_VK_KHR_SYSTEM_SURVACE_EXTENSION_NAME );

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

std::vector<VkExtensionProperties> iInit::getExtProprs( std::string _layerName ) {
   std::vector<VkExtensionProperties> lPorps;
   uint32_t lExtCount;
   VkResult lResult;

   const char *lNamePtr = _layerName.empty() ? nullptr : _layerName.c_str();

   lResult = vkEnumerateInstanceExtensionProperties( lNamePtr, &lExtCount, nullptr );
   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumerateInstanceExtensionProperties' returned ", uEnum2Str::toStr( lResult ) );
      return lPorps;
   }

   if ( lExtCount == 0 ) {
      return lPorps;
   }

   lPorps.resize( lExtCount );
   lResult = vkEnumerateInstanceExtensionProperties( lNamePtr, &lExtCount, lPorps.data() );
   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumerateInstanceExtensionProperties' returned ", uEnum2Str::toStr( lResult ) );
      return lPorps;
   }

   return lPorps;
}

int iInit::loadExtensionList() {
   std::vector<VkExtensionProperties> lPorps;

   lPorps = getExtProprs( "" );

   for ( auto const &i : vLayersToUse ) {
      std::vector<VkExtensionProperties> lTemp = getExtProprs( i );
      lPorps.insert( lPorps.end(), lTemp.begin(), lTemp.end() );
   }

   dVkLOG( "Extensions found:" );

   for ( auto const &i : lPorps ) {
      bool lFound = false;

      for ( auto const &j : vExtensionList ) {
         if ( j == i.extensionName ) {
            lFound = true;
            break;
         }
      }

      if ( lFound )
         continue;

      dVkLOG( "  -- '", i.extensionName, "'  -  specVersion: ", i.specVersion );
      vExtensionList.emplace_back( i.extensionName );
   }

   return 0;
}

/*!
 * \brief Creates the vulkan instance
 *
 * \returns  0 -- Success
 * \returns  1 -- Missing extension(s)
 */
int iInit::initVulkan( std::vector<std::string> _layers ) {
   VkResult lResult;
   uint32_t lPorpCount;

   lResult = vkEnumerateInstanceLayerProperties( &lPorpCount, nullptr );
   if ( lResult ) {
      eLOG( "'vkEnumerateInstanceLayerProperties' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   vLayerProperties_vk.resize( lPorpCount );
   lResult = vkEnumerateInstanceLayerProperties( &lPorpCount, vLayerProperties_vk.data() );
   if ( lResult ) {
      eLOG( "'vkEnumerateInstanceLayerProperties' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   dVkLOG( "InstanceLayerProperties: ", lPorpCount );
   for ( auto const &i : vLayerProperties_vk ) {
      dVkLOG( "  -- ", i.layerName, " (", i.description, ")" );
   }

   iLOG( "Using ", _layers.size(), " Vulkan Layers:" );
   for ( auto const &i : _layers ) {
      bool lFound = false;
      for ( auto const &j : vLayerProperties_vk ) {
         if ( i == j.layerName ) {
            vLayersToUse.emplace_back( i );
            iLOG( "  -- Using Layer '", i, "'" );
            lFound = true;
            break;
         }
      }

      if ( !lFound )
         wLOG( "Vulkan Layer '", i, "' not found!" );
   }

   int lRet = loadExtensionList();
   if ( lRet != 0 )
      return lRet;

   const char **lExtensions = new const char *[vExtensionsToUse.size()];

   iLOG( "Using ", vExtensionsToUse.size(), " extension: " );
   for ( uint32_t i = 0; i < vExtensionsToUse.size(); i++ ) {
      if ( !isExtensionSupported( vExtensionsToUse[i] ) ) {
         eLOG( "Extension '", i, "' is not supported!" );
         delete[] lExtensions;
         return 1;
      }
      lExtensions[i] = vExtensionsToUse[i].c_str();
      iLOG( "  -- '", vExtensionsToUse[i], '\'' );
   }

   const char **lLayers = new const char *[vLayersToUse.size()];
   for ( uint32_t i = 0; i < vLayersToUse.size(); i++ ) {
      lLayers[i] = vLayersToUse[i].c_str();
   }

   VkInstanceCreateInfo lCreateInfo_vk;
   VkApplicationInfo lAppInfo_vk;

   lAppInfo_vk.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   lAppInfo_vk.pNext              = nullptr;
   lAppInfo_vk.pApplicationName   = GlobConf.config.appName.c_str();
   lAppInfo_vk.pEngineName        = "EEnginE";
   lAppInfo_vk.apiVersion         = VK_MAKE_VERSION( 1, 0, 4 );
   lAppInfo_vk.applicationVersion = 1; //!< \todo change this const
   lAppInfo_vk.engineVersion      = 1; //!< \todo change this const

   lCreateInfo_vk.sType                   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   lCreateInfo_vk.pNext                   = nullptr;
   lCreateInfo_vk.flags                   = 0;
   lCreateInfo_vk.pApplicationInfo        = &lAppInfo_vk;
   lCreateInfo_vk.enabledLayerCount       = vLayersToUse.size();
   lCreateInfo_vk.ppEnabledLayerNames     = lLayers;
   lCreateInfo_vk.enabledExtensionCount   = vExtensionsToUse.size();
   lCreateInfo_vk.ppEnabledExtensionNames = lExtensions;

   lResult = vkCreateInstance( &lCreateInfo_vk, nullptr, &vInstance_vk );

   delete[] lExtensions;
   delete[] lLayers;

   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkCreateInstance' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   return 0;
}

int iInit::loadDevices() {
   uint32_t lCount;
   VkResult lResult = vkEnumeratePhysicalDevices( vInstance_vk, &lCount, nullptr );

   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumeratePhysicalDevices' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   std::vector<VkPhysicalDevice> lTempDevs;
   vPhysicalDevices_vk.resize( lCount );
   lTempDevs.resize( lCount );

   lResult = vkEnumeratePhysicalDevices( vInstance_vk, &lCount, lTempDevs.data() );

   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumeratePhysicalDevices' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   for ( uint32_t i = 0; i < lTempDevs.size(); i++ ) {
      vPhysicalDevices_vk[i].device = lTempDevs[i];

      vkGetPhysicalDeviceProperties( lTempDevs[i], &vPhysicalDevices_vk[i].properties );
      vkGetPhysicalDeviceFeatures( lTempDevs[i], &vPhysicalDevices_vk[i].features );
      vkGetPhysicalDeviceMemoryProperties( lTempDevs[i], &vPhysicalDevices_vk[i].memoryProperties );

      auto &lQueueAlias = vPhysicalDevices_vk[i].queueFamilyProperties;

      vkGetPhysicalDeviceQueueFamilyProperties( lTempDevs[i], &lCount, nullptr );
      lQueueAlias.resize( lCount );
      vkGetPhysicalDeviceQueueFamilyProperties( lTempDevs[i], &lCount, lQueueAlias.data() );

#if D_LOG_VULKAN_INIT
      auto &props = vPhysicalDevices_vk[i].properties;

      dLOG( L"GPU ", i, ":" );
      dLOG( L"  -- Device Properties:" );
      dLOG( L"    - apiVersion    = ", GET_VERSION( props.apiVersion ) );
      dLOG( L"    - driverVersion = ", GET_VERSION( props.driverVersion ) );
      dLOG( L"    - vendorID      = ", props.vendorID );
      dLOG( L"    - deviceID      = ", props.deviceID );
      dLOG( L"    - deviceType    = ", uEnum2Str::toStr( props.deviceType ) );
      dLOG( L"    - deviceName    = ", props.deviceName );
      dLOG( L"  -- Queue Family Properties:" );

      for ( uint32_t i = 0; i < lQueueAlias.size(); i++ ) {
         dLOG( L"    -- Queue Family ", i );
         dLOG( L"      - queueFlags: (prefix VK_QUEUE_)" );
         dLOG( L"        - GRAPHICS_BIT:       ",
               lQueueAlias[i].queueFlags & VK_QUEUE_GRAPHICS_BIT ? true : false );
         dLOG( L"        - COMPUTE_BIT:        ",
               lQueueAlias[i].queueFlags & VK_QUEUE_COMPUTE_BIT ? true : false );
         dLOG( L"        - TRANSFER_BIT:       ",
               lQueueAlias[i].queueFlags & VK_QUEUE_TRANSFER_BIT ? true : false );
         dLOG( L"        - SPARSE_BINDING_BIT: ",
               lQueueAlias[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT ? true : false );
         dLOG( L"      - queueCount                  = ", lQueueAlias[i].queueCount );
         dLOG( L"      - timestampValidBits          = ", lQueueAlias[i].timestampValidBits );
         dLOG( L"      - minImageTransferGranularity = ",
               lQueueAlias[i].minImageTransferGranularity.width,
               L'x',
               lQueueAlias[i].minImageTransferGranularity.height,
               L"; depth: ",
               lQueueAlias[i].minImageTransferGranularity.depth );
      }
#endif
   }

   return 0;
}

iInit::PhysicalDevice_vk *iInit::chooseDevice() {
   if ( vPhysicalDevices_vk.empty() )
      return nullptr;

   PhysicalDevice_vk *current = nullptr;

   for ( auto &i : vPhysicalDevices_vk ) {
      unsigned int lNumQueues        = 0;
      unsigned int lCurrentNumQueues = 0;
      bool lSupportsGraphicsBit      = false;

      if ( current == nullptr ) {
         current = &i;
         continue;
      }

      if ( i.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU ) {
         if ( current->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU )
            continue;

         // Integrated GPU may be better than first device type
         if ( i.properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU )
            continue;
      }

      // calc num of queues
      for ( auto const &j : i.queueFamilyProperties ) {
         lNumQueues += j.queueCount;
         if ( j.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
            lSupportsGraphicsBit = true;
         }
      }

      for ( auto const &j : current->queueFamilyProperties ) {
         lCurrentNumQueues += j.queueCount;
      }

      if ( !lSupportsGraphicsBit )
         continue;


      if ( lCurrentNumQueues > lNumQueues )
         continue;

      //! \todo add more tests here

      // i is better than current
      current = &i;
   }

   return current;
}

/*!
 * \brief creates the dvice with all queues
 * \returns 0 -- success
 */
int iInit::createDevice() {
   auto *lPDevPTR = chooseDevice();

   if ( lPDevPTR == nullptr ) {
      return 1000;
   }

   std::vector<VkDeviceQueueCreateInfo> lQueueCreateInfo;
   std::vector<std::vector<float>> lQueuePriorities;

   for ( auto const &i : lPDevPTR->queueFamilyProperties ) {
      lQueuePriorities.emplace_back();
      lQueuePriorities.back().resize( i.queueCount );

      // Setting priorities (1.0, 0.5, 0.25, ...)
      for ( uint32_t j = 0; j < i.queueCount; j++ ) {
         lQueuePriorities.back()[j] = 1.0f / static_cast<float>( j + 1 );
      }

      lQueueCreateInfo.emplace_back();
      lQueueCreateInfo.back().sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
      lQueueCreateInfo.back().pNext = nullptr;
      lQueueCreateInfo.back().flags = 0;
      lQueueCreateInfo.back().queueFamilyIndex = lQueueCreateInfo.size() - 1;
      lQueueCreateInfo.back().queueCount = i.queueCount;
      lQueueCreateInfo.back().pQueuePriorities = lQueuePriorities.back().data();
   }

   VkDeviceCreateInfo lCreateInfo;
   lCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
   lCreateInfo.pNext                   = nullptr;
   lCreateInfo.flags                   = 0;
   lCreateInfo.queueCreateInfoCount    = lQueueCreateInfo.size();
   lCreateInfo.pQueueCreateInfos       = lQueueCreateInfo.data();
   lCreateInfo.enabledLayerCount       = 0;
   lCreateInfo.ppEnabledLayerNames     = nullptr;
   lCreateInfo.enabledExtensionCount   = 0;
   lCreateInfo.ppEnabledExtensionNames = nullptr;
   lCreateInfo.pEnabledFeatures        = &lPDevPTR->features;

   auto lResult = vkCreateDevice( lPDevPTR->device, &lCreateInfo, nullptr, &vDevice_vk.device );
   if ( lResult ) {
      eLOG( "'vkCreateDevice' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   vDevice_vk.physicalDevice = lPDevPTR;

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
 * \returns  2 -- Failed to load devices
 * \returns  3 -- Failed to create a vulkan device
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

   if ( initVulkan( _layers ) )
      return 1;

   if ( loadDevices() )
      return 2;

   if ( createDevice() )
      return 3;

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

   if ( vCreateWindowReturn_I != 0 ) {
      return vCreateWindowReturn_I;
   }

   vIsVulkanSetup_B = true;

   return 0;
}

void iInit::destroyVulkan() {
   if ( !vIsVulkanSetup_B )
      return;

   VkResult lResult;

   if ( vDevice_vk.device != NULL ) {
      lResult = vkDeviceWaitIdle( vDevice_vk.device );

      if ( lResult ) {
         wLOG( "'vkDeviceWaitIdle' returned ", uEnum2Str::toStr( lResult ) );
      }

      vkDestroyDevice( vDevice_vk.device, nullptr );
   }

   vIsVulkanSetup_B = false;
   vExtensionList.clear();
   vkDestroyInstance( vInstance_vk, nullptr );
}

int iInit::shutdown() {
   destroyVulkan();
   closeWindow();

   return LOG.stopLogLoop();
}


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
   }

   return 1;
}

void iInit::quitMainLoop() { vMainLoopRunning_B = false; }

/*!
 * \brief Quit the main loop and close the window
 */
void iInit::closeWindow() {
   if ( vIsVulkanSetup_B || !getIsWindowCreated() )
      return;

   if ( vMainLoopRunning_B ) {
      quitMainLoop();
   }

   if ( vEventLoop_BT.joinable() )
      vEventLoop_BT.join();

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
