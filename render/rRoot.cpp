/*!
 * \file rRoot.cpp
 * \brief \b Classes: \a rRoot
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
 *
 */

#include "rRoot.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"

#if D_LOG_VULKAN
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

namespace e_engine {

rRoot::rRoot( iInit *_init ) : vInitPtr( _init ) {}

rRoot::~rRoot() {
   dVkLOG( "Destroying command pools..." );

   std::lock_guard<std::mutex> lLock( vCommandPoolsMutex );
   vkDeviceWaitIdle( vInitPtr->getDevice() );
   for ( auto &i : vCmdPools_vk ) {
      dVkLOG( "  -- tID: ", i.first.tID, "; queue family: ", i.first.qfIndex );
      if ( i.second == nullptr ) {
         wLOG( "Command pool already destroyed" );
         continue;
      }

      vkDestroyCommandPool( vInitPtr->getDevice(), i.second, nullptr );
   }
}

/*!
 * \brief Get a command pool
 *
 * This function selects (or generates if needed) a command pool for the current thread (commad
 * pools are not thread safe ==> one command pool for every thread).
 *
 * \param _queueFamilyIndex The queue family index
 * \param __flags           command pool flags
 * \returns a command buffer (or nullptr)
 */
VkCommandPool rRoot::getCommandPool( uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flags ) {
   PoolInfo lTemp;
   lTemp.tID     = std::this_thread::get_id();
   lTemp.qfIndex = _queueFamilyIndex;
   lTemp.flags   = _flags;

   std::lock_guard<std::mutex> lLock( vCommandPoolsMutex );

   if ( vCmdPools_vk.count( lTemp ) > 0 )
      return vCmdPools_vk[lTemp];

   // Command pool does not exists
   dVkLOG( "Creating command pool for thread ", lTemp.tID, ", queue family ", lTemp.qfIndex );

   VkCommandPool lPool;
   VkCommandPoolCreateInfo lInfo;
   lInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
   lInfo.pNext            = nullptr;
   lInfo.flags            = _flags;
   lInfo.queueFamilyIndex = _queueFamilyIndex;

   auto lRes = vkCreateCommandPool( vInitPtr->getDevice(), &lInfo, nullptr, &lPool );
   if ( lRes ) {
      eLOG( "vkCreateCommandPool returned ", uEnum2Str::toStr( lRes ) );
   }

   vCmdPools_vk[lTemp] = lPool;

   return lPool;
}

/*!
 * \brief Get a command pool
 *
 * This function selects (or generates if needed) a command pool for the current thread (commad
 * pools are not thread safe ==> one command pool for every thread).
 *
 * \param _qFlags Flags the queue family MUST support
 * \param __flags command pool flags
 * \returns a command buffer (or nullptr)
 *
 * \note Wrapper for rRoot::getCommandPool
 */
VkCommandPool rRoot::getCommandPoolFlags( VkQueueFlags _qFlags, VkCommandPoolCreateFlags _flags ) {
   uint32_t lFamilyIndex = vInitPtr->getQueueFamily( _qFlags );
   if ( lFamilyIndex == UINT32_MAX ) {
      return nullptr;
   }

   return getCommandPool( lFamilyIndex, _flags );
}
}
