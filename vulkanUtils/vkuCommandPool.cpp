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

#include "vkuCommandPool.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"

#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

using namespace e_engine;

vkuCommandPool::vkuCommandPool(VkDevice _device, VkCommandPoolCreateFlags _flags, uint32_t _queueFamilyIndex) {
  init(_device, _flags, _queueFamilyIndex);
}

vkuCommandPool::~vkuCommandPool() { destroy(); }

/*!
 * \brief Initializes the command pool
 * \param _device           The Vulkan device to use
 * \param _flags            The create flags
 * \param _queueFamilyIndex The queue family to use
 */
VkResult vkuCommandPool::init(VkDevice _device, VkCommandPoolCreateFlags _flags, uint32_t _queueFamilyIndex) {
  vDevice = _device;
  if (vDevice == VK_NULL_HANDLE) {
    eLOG(L"Invalid parameter: _device is VK_NULL_HANDLE");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkCommandPoolCreateInfo lCreateInfo;
  lCreateInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  lCreateInfo.pNext            = nullptr;
  lCreateInfo.flags            = _flags;
  lCreateInfo.queueFamilyIndex = _queueFamilyIndex;

  VkResult lRes = vkCreateCommandPool(vDevice, &lCreateInfo, nullptr, &vCmdPool);
  if (lRes != VK_SUCCESS) {
    eLOG("'vkCreateCommandPool' returned ", uEnum2Str::toStr(lRes));
    eLOG("Failed to create cmd pool for thread ", std::this_thread::get_id(), " and queue family ", _queueFamilyIndex);
    vCmdPool = VK_NULL_HANDLE;
    vDevice  = VK_NULL_HANDLE;
  } else {
    dVkLOG("Created command pool for thread ", std::this_thread::get_id(), " and queue family ", _queueFamilyIndex);
  }

  return lRes;
}

/*!
 * \brief Destroyes the command pool
 * \warning All command buffers (vkuCommandBuffer) must be destoyed before this function is called
 * \note Command pools should only be destroyed before its vulkan device is destroyed.
 */
void vkuCommandPool::destroy() {
  if (vCmdPool) {
    vkDestroyCommandPool(vDevice, vCmdPool, nullptr);
  }
  vCmdPool = VK_NULL_HANDLE;
  vDevice  = VK_NULL_HANDLE;
}

vkuCommandBuffer vkuCommandPool::getBuffer(VkCommandBufferLevel _level) {
  if (*this) {
    return vkuCommandBuffer(this, _level);
  } else {
    return vkuCommandBuffer();
  }
}
