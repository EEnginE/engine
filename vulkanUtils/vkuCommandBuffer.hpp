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
#include <vulkan.h>

namespace e_engine {

class vkuCommandPool;

/*!
 * \brief Wrapper for VkCommandBuffer
 * \note This object must be destroyed before the vkuCommandPool is destroyed
 * \warning There are no validity checks!
 */
class vkuCommandBuffer final {
 private:
  VkCommandBuffer vBuffer = VK_NULL_HANDLE;
  VkDevice        vDevice = VK_NULL_HANDLE;
  vkuCommandPool *vPool   = nullptr;

 public:
  vkuCommandBuffer() = default;
  vkuCommandBuffer(vkuCommandPool *_pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY);
  ~vkuCommandBuffer();

  vkuCommandBuffer(vkuCommandBuffer const &) = delete;
  vkuCommandBuffer(vkuCommandBuffer &&);

  vkuCommandBuffer &operator=(const vkuCommandBuffer &) = delete;

  vkuCommandBuffer &operator=(vkuCommandBuffer &&);

  VkResult init(vkuCommandPool *_pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY) noexcept;
  void destroy() noexcept;

  inline VkCommandBuffer &get() noexcept { return vBuffer; }
  inline VkCommandBuffer &operator*() noexcept { return vBuffer; }

  inline bool operator!() const noexcept { return !vPool || vBuffer == VK_NULL_HANDLE || vDevice == VK_NULL_HANDLE; }
  inline explicit operator bool() const noexcept { return vPool && vBuffer != VK_NULL_HANDLE && vDevice != VK_NULL_HANDLE; }

  // Vulkan bindings
  VkResult begin(VkCommandBufferUsageFlags _flags = 0, VkCommandBufferInheritanceInfo *_info = nullptr) noexcept;
  VkResult end() noexcept;
};
}
