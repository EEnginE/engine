/*
 * Copyright (C) 2017 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this fFile except in compliance with the License.
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
#include <vulkan/vulkan.h>

namespace e_engine {

class vkuCommandPool final {
 private:
  VkCommandPool vCmdPool = VK_NULL_HANDLE;
  VkDevice      vDevice  = VK_NULL_HANDLE;

 public:
  vkuCommandPool() = delete;
  vkuCommandPool(VkDevice _device, VkCommandPoolCreateFlags _flags, uint32_t _queueFamilyIndex);
  ~vkuCommandPool();

  vkuCommandPool(vkuCommandPool const &) = delete;
  vkuCommandPool(vkuCommandPool &&)      = delete;

  vkuCommandPool &operator=(const vkuCommandPool &) = delete;
  vkuCommandPool &operator=(vkuCommandPool &&) = delete;

  inline VkCommandPool get() const noexcept { return vCmdPool; }
};
}
