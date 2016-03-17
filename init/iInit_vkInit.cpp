/*!
 * \file iInit_vkInit.cpp
 * \brief \b Classes: \a iInit
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

#include "iInit.hpp"
#include "uLog.hpp"
#include "uEnum2Str.hpp"

#include <vulkan/vulkan.h>
#include <string.h>

#if D_LOG_VULKAN_INIT
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

#define GET_VERSION( v )                                                                           \
   VK_VERSION_MAJOR( v ), L'.', VK_VERSION_MINOR( v ), L'.', VK_VERSION_PATCH( v )

namespace e_engine {

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

std::vector<VkExtensionProperties> iInit::getDeviceExtProprs( std::string _layerName,
                                                              VkPhysicalDevice _dev ) {
   std::vector<VkExtensionProperties> lPorps;
   uint32_t lExtCount;
   VkResult lResult;

   const char *lNamePtr = _layerName.empty() ? nullptr : _layerName.c_str();

   lResult = vkEnumerateDeviceExtensionProperties( _dev, lNamePtr, &lExtCount, nullptr );
   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumerateDeviceExtensionProperties' returned ", uEnum2Str::toStr( lResult ) );
      return lPorps;
   }

   if ( lExtCount == 0 ) {
      return lPorps;
   }

   lPorps.resize( lExtCount );
   lResult = vkEnumerateDeviceExtensionProperties( _dev, lNamePtr, &lExtCount, lPorps.data() );
   if ( lResult != VK_SUCCESS ) {
      eLOG( "'vkEnumerateDeviceExtensionProperties' returned ", uEnum2Str::toStr( lResult ) );
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

int iInit::loadDeviceExtensionList( VkPhysicalDevice _dev ) {
   std::vector<VkExtensionProperties> lPorps;

   lPorps = getDeviceExtProprs( "", _dev );

   for ( auto const &i : vLayersToUse ) {
      std::vector<VkExtensionProperties> lTemp = getDeviceExtProprs( i, _dev );
      lPorps.insert( lPorps.end(), lTemp.begin(), lTemp.end() );
   }

   dVkLOG( "Device Extensions found:" );

   for ( auto const &i : lPorps ) {
      bool lFound = false;

      for ( auto const &j : vDeviceExtensionList ) {
         if ( j == i.extensionName ) {
            lFound = true;
            break;
         }
      }

      if ( lFound )
         continue;

      dVkLOG( "  -- '", i.extensionName, "'  -  specVersion: ", i.specVersion );
      vDeviceExtensionList.emplace_back( i.extensionName );
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

#if D_LOG_VULKAN_INIT
   dVkLOG( "InstanceLayerProperties: ", lPorpCount );
   for ( auto const &i : vLayerProperties_vk ) {
      dVkLOG( "  -- ", i.layerName, " (", i.description, ")" );
   }
#endif

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

   void *lDebugPTR = nullptr;
   if ( vEnableVulkanDebug )
      lDebugPTR = static_cast<void *>( &vDebugCreateInfo_vk );

   VkInstanceCreateInfo lCreateInfo_vk;
   VkApplicationInfo lAppInfo_vk;

   lAppInfo_vk.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
   lAppInfo_vk.pNext              = nullptr;
   lAppInfo_vk.pApplicationName   = GlobConf.config.appName.c_str();
   lAppInfo_vk.pEngineName        = "EEnginE";
   lAppInfo_vk.apiVersion         = VK_MAKE_VERSION( 1, 0, 3 );
   lAppInfo_vk.applicationVersion = 1; //!< \todo change this const
   lAppInfo_vk.engineVersion      = 1; //!< \todo change this const

   lCreateInfo_vk.sType                   = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   lCreateInfo_vk.pNext                   = lDebugPTR;
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
int iInit::createDevice( std::vector<std::string> _layers ) {
   if ( vDevice_vk.pDevice == nullptr ) {
      return 1000;
   }

   uint32_t lPorpCount;
   auto lResult =
         vkEnumerateDeviceLayerProperties( vDevice_vk.pDevice->device, &lPorpCount, nullptr );
   if ( lResult ) {
      eLOG( "'vkEnumerateDeviceLayerProperties' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   vDeviceLayerProperties_vk.resize( lPorpCount );
   lResult = vkEnumerateDeviceLayerProperties(
         vDevice_vk.pDevice->device, &lPorpCount, vDeviceLayerProperties_vk.data() );
   if ( lResult ) {
      eLOG( "'vkEnumerateDeviceLayerProperties' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

#if D_LOG_VULKAN_INIT
   dVkLOG( "DeviceLayerProperties: ", lPorpCount );
   for ( auto const &i : vDeviceLayerProperties_vk ) {
      dVkLOG( "  -- ", i.layerName, " (", i.description, ")" );
   }
#endif

   iLOG( "Using ", _layers.size(), " Device Layers:" );
   for ( auto const &i : _layers ) {
      bool lFound = false;
      for ( auto const &j : vDeviceLayerProperties_vk ) {
         if ( i == j.layerName ) {
            vDeviceLayersToUse.emplace_back( i );
            iLOG( "  -- Using Layer '", i, "'" );
            lFound = true;
            break;
         }
      }

      if ( !lFound )
         wLOG( "Vulkan Layer '", i, "' not found!" );
   }


   loadDeviceExtensionList( vDevice_vk.pDevice->device );
   const char **lExtensions = new const char *[vDeviceExtensionsToUse.size()];

   iLOG( "Using ", vDeviceExtensionsToUse.size(), " device extension: " );
   for ( uint32_t i = 0; i < vDeviceExtensionsToUse.size(); i++ ) {
      if ( !isDeviceExtensionSupported( vDeviceExtensionsToUse[i] ) ) {
         eLOG( "Extension '", i, "' is not supported!" );
         delete[] lExtensions;
         return 1;
      }
      lExtensions[i] = vDeviceExtensionsToUse[i].c_str();
      iLOG( "  -- '", vDeviceExtensionsToUse[i], '\'' );
   }

   const char **lLayers = new const char *[vDeviceLayersToUse.size()];
   for ( uint32_t i = 0; i < vDeviceLayersToUse.size(); i++ ) {
      lLayers[i] = vDeviceLayersToUse[i].c_str();
   }

   std::vector<VkDeviceQueueCreateInfo> lQueueCreateInfo;
   std::vector<std::vector<float>> lQueuePriorities;
   float lPriority;
   uint32_t lFIndex;

   for ( auto const &i : vDevice_vk.pDevice->queueFamilyProperties ) {
      lQueuePriorities.emplace_back();
      lQueuePriorities.back().resize( i.queueCount );

      lFIndex = lQueueCreateInfo.size();

      VkBool32 lSupported;
      auto lRes = vkGetPhysicalDeviceSurfaceSupportKHR(
            vDevice_vk.pDevice->device, lFIndex, vSurface_vk, &lSupported );

      if ( lRes ) {
         eLOG( "'vkGetPhysicalDeviceSurfaceSupportKHR' returned ", uEnum2Str::toStr( lRes ) );
         lSupported = 0;
      }

      // Setting priorities (1.0, 0.5, 0.25, ...)
      for ( uint32_t j = 0; j < i.queueCount; j++ ) {
         lPriority = 1.0f / static_cast<float>( j + 1 );
         lQueuePriorities.back()[j] = lPriority;

         // Also create the queue object (only setting priority, and indexes)
         // The actual queue will be stored after creating the device (==> and the queues),
         // using this data.
         vQueues_vk.emplace_back( lPriority,
                                  vDevice_vk.pDevice->queueFamilyProperties[lFIndex].queueFlags,
                                  lFIndex,
                                  j,
                                  lSupported == VK_TRUE ? true : false );
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
   lCreateInfo.enabledLayerCount       = vDeviceLayersToUse.size();
   lCreateInfo.ppEnabledLayerNames     = lLayers;
   lCreateInfo.enabledExtensionCount   = vDeviceExtensionsToUse.size();
   lCreateInfo.ppEnabledExtensionNames = lExtensions;
   lCreateInfo.pEnabledFeatures        = &vDevice_vk.pDevice->features;

   iLOG( "Creating vulkan device on GPU '",
         vDevice_vk.pDevice->properties.deviceName,
         "'; id = ",
         vDevice_vk.pDevice->properties.deviceID );

   lResult =
         vkCreateDevice( vDevice_vk.pDevice->device, &lCreateInfo, nullptr, &vDevice_vk.device );
   if ( lResult ) {
      eLOG( "'vkCreateDevice' returned ", uEnum2Str::toStr( lResult ) );
      return lResult;
   }

   dVkLOG( "  -- Created Queues:" );
   for ( auto &i : vQueues_vk ) {
      vkGetDeviceQueue( vDevice_vk.device, i.familyIndex, i.index, &i.queue );
      dVkLOG( "    -- family: ", i.familyIndex, "; index: ", i.index, "; priority: ", i.priority );
   }

   return 0;
}


int iInit::createSwapchain() {
   VkSurfaceCapabilitiesKHR lSurfaceInfo;
   uint32_t lNum;
   auto lRes = vkGetPhysicalDeviceSurfaceFormatsKHR(
         vDevice_vk.pDevice->device, vSurface_vk, &lNum, nullptr );
   if ( lRes ) {
      eLOG( "'vkGetPhysicalDeviceSurfaceFormatsKHR' returned ", uEnum2Str::toStr( lRes ) );
      return 1;
   }

   std::vector<VkSurfaceFormatKHR> lFormats;
   lFormats.resize( lNum );
   lRes = vkGetPhysicalDeviceSurfaceFormatsKHR(
         vDevice_vk.pDevice->device, vSurface_vk, &lNum, lFormats.data() );
   if ( lRes ) {
      eLOG( "'vkGetPhysicalDeviceSurfaceFormatsKHR' returned ", uEnum2Str::toStr( lRes ) );
      return 1;
   }

#if D_LOG_VULKAN_INIT
   dVkLOG( "Surface formats:" );
   for ( auto const &i : lFormats ) {
      dVkLOG( "  -- Format: ",
              uEnum2Str::toStr( i.format ),
              "; colorSpace: ",
              uEnum2Str::toStr( i.colorSpace ) );
   }
#endif

   lRes = vkGetPhysicalDeviceSurfaceCapabilitiesKHR( vDevice_vk.pDevice->device, vSurface_vk, &lSurfaceInfo );

   eLOG( "AAA" );

   if ( lRes ) {
      eLOG( "'vkGetPhysicalDeviceSurfaceCapabilitiesKHR' returned ", uEnum2Str::toStr( lRes ) );
      return 1;
   }

#if 0
   VkSwapchainCreateInfoKHR lCreateInfo;
   lCreateInfo.sType         = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
   lCreateInfo.pNext         = nullptr;
   lCreateInfo.flags         = 0;
   lCreateInfo.surface       = vSurface_vk;
   lCreateInfo.minImageCount = lSurfaceInfo.maxImageCount;      //!< \todo
   lCreateInfo.imageFormat   = VK_FORMAT_A1R5G5B5_UNORM_PACK16; //!< \todo

   lRes = vkCreateSwapchainKHR( vDevice_vk.device, &lCreateInfo, nullptr, &vSwapchain_vk );
   if ( lRes ) {
      eLOG( "'vkCreateSwapchainKHR' returned ", uEnum2Str::toStr( lRes ) );
      return 1;
   }
#endif

   return 0;
}
}
