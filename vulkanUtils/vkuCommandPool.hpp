/*
 * Copyright (C) 2017 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this File except in compliance with the License.
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
#include "vkuCommandBuffer.hpp"
#include <vulkan/vulkan.h>

namespace e_engine {

/*!
 * \brief Manages a command pool
 *
 * Command pools are managed by vkuCommandPoolManager
 *
 * \note This objects of this class should not be created / destroyed manually!
 */
class vkuCommandPool final {
 private:
  VkCommandPool vCmdPool = VK_NULL_HANDLE;
  VkDevice      vDevice  = VK_NULL_HANDLE;

 public:
  vkuCommandPool() = default;
  vkuCommandPool(VkDevice _device, VkCommandPoolCreateFlags _flags, uint32_t _queueFamilyIndex);
  ~vkuCommandPool();

  vkuCommandPool(vkuCommandPool const &) = delete;
  vkuCommandPool(vkuCommandPool &&)      = delete;

  vkuCommandPool &operator=(const vkuCommandPool &) = delete;
  vkuCommandPool &operator=(vkuCommandPool &&) = delete;

  VkResult init(VkDevice _device, VkCommandPoolCreateFlags _flags, uint32_t _queueFamilyIndex);
  void destroy();

  inline VkCommandPool get() const noexcept { return vCmdPool; }
  inline VkDevice      getDevice() const noexcept { return vDevice; }
  vkuCommandBuffer getBuffer(VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);

  inline bool operator!() const { return vCmdPool == VK_NULL_HANDLE || vDevice == VK_NULL_HANDLE; }
  inline explicit operator bool() const { return vCmdPool != VK_NULL_HANDLE && vDevice != VK_NULL_HANDLE; }
};
}
