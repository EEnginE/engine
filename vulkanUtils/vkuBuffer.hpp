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
#include "vkuDevice.hpp"
#include <vulkan.h>

namespace e_engine {

/*!
 * \brief Abstration class for a vulkan buffer
 *
 * Allows host access to device memory (currently write only).
 */
class vkuBuffer {
 public:
  struct Config {
    VkBufferUsageFlags    usage                       = 0;
    VkSharingMode         sharingMode                 = VK_SHARING_MODE_EXCLUSIVE;
    VkMemoryPropertyFlags memoryFlags                 = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
    std::vector<uint32_t> queueFamilyIndices          = {};
    bool                  deleteStagingBufferAfterUse = false;
  };

  class MemoryAccess final {
   private:
    void *         vPtr  = nullptr;
    VkDeviceSize   vSize = 0;
    vkuDevicePTR   vDevice;
    VkDeviceMemory vMem = VK_NULL_HANDLE;

   public:
    MemoryAccess(vkuDevicePTR _dev, VkDeviceMemory _mem, VkDeviceSize _size);
    ~MemoryAccess();

    MemoryAccess(MemoryAccess const &) = delete;
    MemoryAccess &operator=(const MemoryAccess &) = delete;

    MemoryAccess(MemoryAccess &&);
    MemoryAccess &operator=(MemoryAccess &&);

    inline VkDeviceSize size() const noexcept { return vSize; }
    inline void *       get() noexcept { return vPtr; }
    inline void *       operator*() noexcept { return vPtr; }
    inline bool         operator!() const noexcept { return vMem == VK_NULL_HANDLE; }
    inline explicit     operator bool() const noexcept { return vMem != VK_NULL_HANDLE; }
  };

 private:
  vkuDevicePTR   vDevice;
  VkBuffer       vMainBuffer         = VK_NULL_HANDLE;
  VkBuffer       vStagingBuffer      = VK_NULL_HANDLE;
  VkDeviceMemory vMainMemory         = VK_NULL_HANDLE;
  VkDeviceMemory vStagingMemory      = VK_NULL_HANDLE;
  VkDeviceSize   vSize               = 0;
  uint32_t       vMainMemoryIndex    = UINT32_MAX;
  uint32_t       vStagingMemoryIndex = UINT32_MAX;

  Config cfg;

 public:
  vkuBuffer() = delete;
  vkuBuffer(vkuDevicePTR _device);
  ~vkuBuffer();

  vkuBuffer(vkuBuffer const &) = delete;
  vkuBuffer &operator=(const vkuBuffer &) = delete;

  vkuBuffer(vkuBuffer &&);
  vkuBuffer &operator=(vkuBuffer &&);

  VkResult init(VkDeviceSize _size);
  void     destroy();

  MemoryAccess getBufferAccess();
  inline bool  requiresInternalCopying() const noexcept { return vMainMemoryIndex != vStagingMemoryIndex; }

  void     cmdSync(vkuCommandBuffer &_buff);
  VkResult sync();

  void destroyStagingBufferMemory();

  inline VkDeviceSize size() const noexcept { return vSize; }
  inline VkBuffer     get() const noexcept { return vMainBuffer; }
  inline Config       getConfig() const noexcept { return cfg; }
  inline Config *     getConfigPTR() noexcept { return &cfg; }
  inline bool         isCreated() const noexcept { return vMainBuffer != VK_NULL_HANDLE; }

  inline VkBuffer operator*() const noexcept { return vMainBuffer; }
  inline Config * operator->() noexcept { return &cfg; } //! \brief Allow config access via buffer->cfgField = 1;
  inline bool     operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};

} // namespace e_engine
