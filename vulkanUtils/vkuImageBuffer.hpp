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
#include "vkuDevice.hpp"
#include <vulkan.h>

namespace e_engine {

/*!
 * \brief Container for a vulkan image
 *
 * Manages the image, image view and device memory of the image buffer
 *
 * \todo Support bigger device memory + offset (4th parameter in vkBindImageMemory)
 */
class vkuImageBuffer {
 public:
  struct Config {
    VkImageType             type             = VK_IMAGE_TYPE_2D;
    VkFormat                format           = VK_FORMAT_UNDEFINED;
    VkExtent3D              extent           = {0, 0, 0};
    uint32_t                mipLevels        = 1;
    uint32_t                arrayLayers      = 1;
    VkSampleCountFlagBits   samples          = VK_SAMPLE_COUNT_1_BIT;
    VkImageTiling           tiling           = VK_IMAGE_TILING_OPTIMAL;
    VkImageUsageFlags       usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    VkImageLayout           initialLayout    = VK_IMAGE_LAYOUT_UNDEFINED;
    VkSharingMode           sharingMode      = VK_SHARING_MODE_EXCLUSIVE;
    VkImageSubresourceRange subresourceRange = {0, 0, 1, 0, 1};
    VkComponentMapping      components       = {
        VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

    VkImageCreateFlags imageCreateFlags = 0;
  };

 private:
  VkImage        vImage     = VK_NULL_HANDLE;
  VkImageView    vImageView = VK_NULL_HANDLE;
  VkDeviceMemory vMemory    = VK_NULL_HANDLE;
  vkuDevicePTR   vDevice    = nullptr;

  Config cfg;

 public:
  vkuImageBuffer() : vkuImageBuffer(VK_NULL_HANDLE) {}
  vkuImageBuffer(vkuDevicePTR _device);
  ~vkuImageBuffer();

  vkuImageBuffer(vkuImageBuffer const &) = delete;
  vkuImageBuffer &operator=(const vkuImageBuffer &) = delete;

  vkuImageBuffer(vkuImageBuffer &&);
  vkuImageBuffer &operator=(vkuImageBuffer &&);

  VkResult init(vkuDevicePTR _device = nullptr);
  void destroy();

  inline VkImageView  get() const noexcept { return vImageView; }
  inline VkImage      getImage() const noexcept { return vImage; }
  inline vkuDevicePTR getDevice() const noexcept { return vDevice; }
  inline Config       getConfig() const noexcept { return cfg; }
  inline Config *     getConfigPTR() noexcept { return &cfg; }
  inline bool         isCreated() const noexcept { return vImage != VK_NULL_HANDLE; }

  inline VkImageView operator*() const noexcept { return vImageView; }
  inline Config *operator->() noexcept { return &cfg; } //! \brief Allow config access via buffer->cfgField = 1;

  inline bool operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};
}
