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
 */

#include "vkuCommandPoolManager.hpp"

using namespace e_engine;

vkuCommandPoolManager::vkuCommandPoolManager() {}
vkuCommandPoolManager::CmdPoolMetaData::CmdPoolMetaData(VkDevice                 _device,
                                                        VkCommandPoolCreateFlags _flags,
                                                        uint32_t                 _queueFamilyIndex)
    : pool(_device, _flags, _queueFamilyIndex),
      queueFamilyIndex(_queueFamilyIndex),
      flags(_flags),
      device(_device),
      threadID(std::this_thread::get_id()) {}

/*!
 * \brief Returns the singleton
 */
vkuCommandPoolManager &vkuCommandPoolManager::getManager() { return sManager; }

/*!
 * \brief Retrurns an excisting command pool or creates one
 *
 * This function selects (or generates if needed) a command pool for the current thread (command
 * pools are not thread safe ==> one command pool for every thread).
 *
 * \param _device           The Device
 * \param _queueFamilyIndex The queue family index
 * \param _flags            Flags used to create the pool
 */
vkuCommandPool *vkuCommandPoolManager::getCmdPool(VkDevice                 _device,
                                                  uint32_t                 _queueFamilyIndex,
                                                  VkCommandPoolCreateFlags _flags) {
  std::lock_guard<std::mutex> lGuard(vAccessMutex);

  for (auto &i : vPools) {
    if (i.threadID == std::this_thread::get_id() && i.device == _device && i.queueFamilyIndex == _queueFamilyIndex &&
        i.flags == _flags) {
      return &i.pool;
    }
  }

  return &vPools.emplace(_device, _flags, _queueFamilyIndex)->pool;
}

/*!
 * \brief Wrapper for vkuCommandPool::getBuffer()
 */
vkuCommandBuffer vkuCommandPoolManager::getCommandBuffer(VkDevice                 _device,
                                                         uint32_t                 _queueFamilyIndex,
                                                         VkCommandBufferLevel     _level,
                                                         VkCommandPoolCreateFlags _flags) {
  return getCmdPool(_device, _queueFamilyIndex, _flags)->getBuffer(_level);
}


/*!
 * \brief (static) wrapper for vkuCommandPoolManager::getCmdPool()
 */
vkuCommandPool *vkuCommandPoolManager::get(VkDevice                 _device,
                                           uint32_t                 _queueFamilyIndex,
                                           VkCommandPoolCreateFlags _flags) {
  return getManager().getCmdPool(_device, _queueFamilyIndex, _flags);
}

/*!
 * \brief (static) wrapper for vkuCommandPool::getBuffer()
 */
vkuCommandBuffer vkuCommandPoolManager::getBuffer(VkDevice                 _device,
                                                  uint32_t                 _queueFamilyIndex,
                                                  VkCommandBufferLevel     _level,
                                                  VkCommandPoolCreateFlags _flags) {
  return getManager().getCmdPool(_device, _queueFamilyIndex, _flags)->getBuffer(_level);
}

/*!
 * \brief Removes all command polls created on a device
 * \param _device The device to clean
 *
 * This function should be called just before the vulkan device is destroyed and AFTER all
 * command buffers on the device are destroyed.
 *
 * \note This function is automatically called in iInit::destroyVulkan()
 * \warning Do not call this function manually, unless you know what you are doing!
 */
void vkuCommandPoolManager::cleanup(VkDevice _device) {
  std::lock_guard<std::mutex> lGuard(vAccessMutex);

  auto iter = vPools.begin();
  while (iter != vPools.end()) {
    if (iter->device == _device) {
      iter = vPools.erase(iter);
    } else {
      ++iter;
    }
  }
}

vkuCommandPoolManager vkuCommandPoolManager::sManager;
