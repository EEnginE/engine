/*!
 * \file rRoot.hpp
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

#pragma once

#include "defines.hpp"
#include <unordered_map>
#include <mutex>
#include <vulkan/vulkan.h>
#include "rRoot_structs.hpp"

namespace e_engine {

class iInit;

/*!
 * \brief root render class
 *
 * This class is responsible for
 *   - command pool handling
 *   - presenting images (todo)
 *   - eventually other stuff
 *
 * \warning An object of this class must be destroyed BEFORE the vulkan context is destroyed (= the
 *          iInit object is destroyed)!!!
 */
class RENDER_API rRoot {
 public:
   typedef internal::CommandPoolInfo PoolInfo;

 private:
   iInit *vInitPtr;

   std::unordered_map<PoolInfo, VkCommandPool> vCmdPools_vk;

   std::mutex vCommandPoolsMutex;

 public:
   rRoot() = delete;
   rRoot( iInit *_init );
   virtual ~rRoot();

   VkCommandPool getCommandPool(
         uint32_t _queueFamilyIndex,
         VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

   VkCommandPool getCommandPoolFlags(
         VkQueueFlags _qFlags,
         VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );
};
}
