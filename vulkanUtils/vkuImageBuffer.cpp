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

#include "defines.hpp"
#include "vkuImageBuffer.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"

using namespace e_engine;

vkuImageBuffer::vkuImageBuffer(VkDevice _device) : vDevice(_device) {}
vkuImageBuffer::~vkuImageBuffer() { destroy(); }

VkResult vkuImageBuffer::init(VkDevice _device) {
  if (isCreated()) {
    wLOG(L"Image buffer already created");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (_device != VK_NULL_HANDLE)
    vDevice = _device;

  if (vDevice == VK_NULL_HANDLE) {
    eLOG(L"");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (cfg.format == VK_FORMAT_UNDEFINED) {
    eLOG(L"Image format must not be VK_FORMAT_UNDEFINED");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  VkImageCreateInfo     lImageCreate;
  VkMemoryAllocateInfo  lMemoryAlloc;
  VkMemoryRequirements  lRequirements;
  VkImageViewCreateInfo lImageViewCreate;

  lImageCreate.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  lImageCreate.pNext                 = nullptr;
  lImageCreate.flags                 = cfg.imageCreateFlags;
  lImageCreate.imageType             = cfg.type;
  lImageCreate.format                = cfg.format;
  lImageCreate.extent                = cfg.extent;
  lImageCreate.mipLevels             = cfg.mipLevels;
  lImageCreate.arrayLayers           = cfg.arrayLayers;
  lImageCreate.samples               = cfg.samples;
  lImageCreate.tiling                = cfg.tiling;
  lImageCreate.usage                 = cfg.usage;
  lImageCreate.sharingMode           = cfg.sharingMode;
  lImageCreate.queueFamilyIndexCount = 0;
  lImageCreate.pQueueFamilyIndices   = nullptr;
  lImageCreate.initialLayout         = cfg.initialLayout;

  lImageViewCreate.sType            = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  lImageViewCreate.pNext            = nullptr;
  lImageViewCreate.flags            = 0;
  lImageViewCreate.image            = vImage;
  lImageViewCreate.viewType         = VK_IMAGE_VIEW_TYPE_2D;
  lImageViewCreate.format           = cfg.format;
  lImageViewCreate.components       = cfg.components;
  lImageViewCreate.subresourceRange = cfg.subresourceRange;

  if (lImageCreate.arrayLayers <= 1) {
    switch (lImageCreate.imageType) {
      case VK_IMAGE_TYPE_1D: lImageViewCreate.viewType = VK_IMAGE_VIEW_TYPE_1D; break;
      case VK_IMAGE_TYPE_2D: lImageViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D; break;
      case VK_IMAGE_TYPE_3D: lImageViewCreate.viewType = VK_IMAGE_VIEW_TYPE_3D; break;
      default: break;
    }
  } else {
    switch (lImageCreate.imageType) {
      case VK_IMAGE_TYPE_1D: lImageViewCreate.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY; break;
      case VK_IMAGE_TYPE_2D: lImageViewCreate.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY; break;
      case VK_IMAGE_TYPE_3D: lImageViewCreate.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY; break;
      default: break;
    }
  }

#if D_LOG_VULKAN_UTILS
  dLOG(L"Creating image buffer:");
  dLOG(L"  -- Creating image:");
  dLOG(L"    - flags:         ", uEnum2Str::VkImageCreateFlagBits_toStr(lImageCreate.flags));
  dLOG(L"    - imageType:     ", uEnum2Str::toStr(lImageCreate.imageType));
  dLOG(L"    - format:        ", uEnum2Str::toStr(lImageCreate.format));
  dLOG(L"    - extent:        ", cfg.extent.width, L'x', cfg.extent.height, L'x', cfg.extent.depth);
  dLOG(L"    - mipLevels:     ", lImageCreate.mipLevels);
  dLOG(L"    - arrayLayers:   ", lImageCreate.arrayLayers);
  dLOG(L"    - samples:       ", uEnum2Str::toStr(lImageCreate.samples));
  dLOG(L"    - tiling:        ", uEnum2Str::toStr(lImageCreate.tiling));
  dLOG(L"    - usage:         ", uEnum2Str::VkImageUsageFlagBits_toStr(lImageCreate.mipLevels));
  dLOG(L"    - sharingMode:   ", uEnum2Str::toStr(lImageCreate.sharingMode));
  dLOG(L"    - initialLayout: ", uEnum2Str::toStr(lImageCreate.initialLayout));
#endif

  auto lRes = vkCreateImage(vDevice, &lImageCreate, nullptr, &vImage);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to create image: vkCreateImage returned ", uEnum2Str::toStr(lRes));
    vImage = VK_NULL_HANDLE;
    return lRes;
  }

  vkGetImageMemoryRequirements(vDevice, vImage, &lRequirements);
  lMemoryAlloc.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  lMemoryAlloc.pNext           = nullptr;
  lMemoryAlloc.allocationSize  = lRequirements.size;
  lMemoryAlloc.memoryTypeIndex = 0; //! \todo FIX THIS!!!

#if D_LOG_VULKAN_UTILS
  dLOG(L"  -- Allocating Memory:");
  dLOG(L"    - allocationSize:  ", lMemoryAlloc.allocationSize);
  dLOG(L"    - memoryTypeIndex: ", lMemoryAlloc.memoryTypeIndex);
#endif

  lRes = vkAllocateMemory(vDevice, &lMemoryAlloc, nullptr, &vMemory);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to create image: vkAllocateMemory returned ", uEnum2Str::toStr(lRes));
    vkDestroyImage(vDevice, vImage, nullptr);
    vImage  = VK_NULL_HANDLE;
    vMemory = VK_NULL_HANDLE;
    return lRes;
  }

  lRes = vkBindImageMemory(vDevice, vImage, vMemory, 0);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to create image: vkAllocateMemory returned ", uEnum2Str::toStr(lRes));
    vkFreeMemory(vDevice, vMemory, nullptr);
    vkDestroyImage(vDevice, vImage, nullptr);
    vImage  = VK_NULL_HANDLE;
    vMemory = VK_NULL_HANDLE;
    return lRes;
  }

#if D_LOG_VULKAN_UTILS
  dLOG(L"  -- Creating Image view:");
  dLOG(L"    - viewType:  ", uEnum2Str::toStr(lImageViewCreate.viewType));
  dLOG(L"    - components:");
  dLOG(L"      - r: ", uEnum2Str::toStr(lImageViewCreate.components.r));
  dLOG(L"      - g: ", uEnum2Str::toStr(lImageViewCreate.components.g));
  dLOG(L"      - b: ", uEnum2Str::toStr(lImageViewCreate.components.b));
  dLOG(L"      - a: ", uEnum2Str::toStr(lImageViewCreate.components.a));
  dLOG(L"    - subresourceRange:");
  dLOG(L"      - aspectMask:     ",
       uEnum2Str::VkImageAspectFlagBits_toStr(lImageViewCreate.subresourceRange.aspectMask));
  dLOG(L"      - baseMipLevel:   ", lImageViewCreate.subresourceRange.baseMipLevel);
  dLOG(L"      - levelCount:     ", lImageViewCreate.subresourceRange.levelCount);
  dLOG(L"      - baseArrayLayer: ", lImageViewCreate.subresourceRange.baseArrayLayer);
  dLOG(L"      - layerCount:     ", lImageViewCreate.subresourceRange.layerCount);
  dLOG(L"  -- Created resource:");
  dLOG(L"    - Image:         ", reinterpret_cast<uint64_t>(vImage));
  dLOG(L"    - Image View:    ", reinterpret_cast<uint64_t>(vImageView));
  dLOG(L"    - Device Memory: ", reinterpret_cast<uint64_t>(vMemory));
#endif

  lRes = vkCreateImageView(vDevice, &lImageViewCreate, nullptr, &vImageView);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to create image view: vkCreateImageView returned ", uEnum2Str::toStr(lRes));
    vkFreeMemory(vDevice, vMemory, nullptr);
    vkDestroyImage(vDevice, vImage, nullptr);
    vImage     = VK_NULL_HANDLE;
    vMemory    = VK_NULL_HANDLE;
    vImageView = VK_NULL_HANDLE;
    return lRes;
  }

  return VK_SUCCESS;
}

void vkuImageBuffer::destroy() {
  if (!isCreated()) {
    return;
  }

  vkDestroyImageView(vDevice, vImageView, nullptr);
  vkDestroyImage(vDevice, vImage, nullptr);
  vkFreeMemory(vDevice, vMemory, nullptr);

  vImageView = VK_NULL_HANDLE;
  vImage     = VK_NULL_HANDLE;
  vMemory    = VK_NULL_HANDLE;
}
