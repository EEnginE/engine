/*
 * Copyright (C) 2017 EEnginE project
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
#include "vkuCommandPool.hpp"
#include <mutex>
#include <plf_colony.h>
#include <thread>
#include <vulkan.h>

namespace e_engine {

class vkuCommandPoolManager {
 private:
  static vkuCommandPoolManager sManager;

  struct CmdPoolMetaData {
    vkuCommandPool           pool;
    uint32_t                 queueFamilyIndex;
    VkCommandPoolCreateFlags flags;
    VkDevice                 device;
    std::thread::id          threadID;

    CmdPoolMetaData(VkDevice _device, VkCommandPoolCreateFlags _flags, uint32_t _queueFamilyIndex);
  };

  plf::colony<CmdPoolMetaData> vPools;

  std::mutex vAccessMutex;

  vkuCommandPoolManager();

 public:
  static vkuCommandPoolManager &getManager();

  vkuCommandPool *getCmdPool(VkDevice                 _device,
                             uint32_t                 _queueFamilyIndex,
                             VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  vkuCommandBuffer getCommandBuffer(VkDevice                 _device,
                                    uint32_t                 _queueFamilyIndex,
                                    VkCommandBufferLevel     _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                    VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  static vkuCommandPool *get(VkDevice                 _device,
                             uint32_t                 _queueFamilyIndex,
                             VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  static vkuCommandBuffer getBuffer(VkDevice                 _device,
                                    uint32_t                 _queueFamilyIndex,
                                    VkCommandBufferLevel     _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                                    VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

  void cleanup(VkDevice _device);
};
}
