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
#include "vkuSwapChain.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"

#if D_LOG_VULKAN_UTILS
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

using namespace e_engine;

vkuSwapChain::~vkuSwapChain() { destroy(); }

/*!
 * \brief Creates a new swapchain for a device and surface
 * \param _device  The device to use (the last device will be used if nullptr)
 * \param _surface The surface to use (the last surface will be used if VK_NULL_HANDLE)
 * \note If a swapchain is alreaddy created, then the old swapchain will be destroyed
 */
vkuSwapChain::LockAndResult vkuSwapChain::init(vkuDevicePTR _device, VkSurfaceKHR _surface) {
  std::unique_lock<std::mutex> lLock(vSwapChainCreateMutex);

  if (_device)
    vDevice = _device;

  if (_surface)
    vSurface = _surface;

  if (!vDevice || !*vDevice) {
    eLOG(L"Invalid device. Can not create swapchain");
    return {std::move(lLock), VK_ERROR_INITIALIZATION_FAILED};
  }

  if (vSurface == VK_NULL_HANDLE) {
    eLOG(L"Invalid vulkan surface. Can not create swapchain");
    return {std::move(lLock), VK_ERROR_INITIALIZATION_FAILED};
  }

  auto lSInfo = vDevice->getSurfaceInfo(vSurface);

  VkPresentModeKHR lModelToUse = VK_PRESENT_MODE_MAX_ENUM_KHR;
  vSwapchainFormat.format      = VK_FORMAT_UNDEFINED;

  dVkLOG("Recreating Swapchain:");
  dVkLOG("  -- Surface formats:");
  for (auto const &i : lSInfo.formats) {
    if (vSwapchainFormat.format == VK_FORMAT_UNDEFINED)
      vSwapchainFormat = i;

    if (i.format == cfg.preferedSurfaceFormat) {
      vSwapchainFormat = i;
    }

    dVkLOG("    - Format: ", uEnum2Str::toStr(i.format), "; colorSpace: ", uEnum2Str::toStr(i.colorSpace));
  }

  dVkLOG("  -- Present models:");
  for (auto const &i : lSInfo.presentModels) {
    dVkLOG("    -- model: ", uEnum2Str::toStr(i));

    if (lModelToUse == VK_PRESENT_MODE_MAX_ENUM_KHR)
      lModelToUse = i;

    if (cfg.preferMailBoxPresetMode && i == VK_PRESENT_MODE_MAILBOX_KHR)
      lModelToUse = i;

    if (cfg.prefereNonTearingPresentMode) {
      if (i == VK_PRESENT_MODE_MAILBOX_KHR || i == VK_PRESENT_MODE_FIFO_KHR || i == VK_PRESENT_MODE_FIFO_RELAXED_KHR)
        if (lModelToUse != VK_PRESENT_MODE_MAILBOX_KHR)
          lModelToUse = i;
    } else {
      if (i == VK_PRESENT_MODE_IMMEDIATE_KHR)
        lModelToUse = i;
    }
  }

  uint32_t lNumImages = lSInfo.surfaceInfo.minImageCount + 1;
  if (lNumImages > lSInfo.surfaceInfo.maxImageCount && lSInfo.surfaceInfo.maxImageCount != 0) // max == 0: unlimited
    lNumImages = lSInfo.surfaceInfo.maxImageCount;

  VkExtent2D lExtentToUse = lSInfo.surfaceInfo.currentExtent;

  if (lExtentToUse.width < lSInfo.surfaceInfo.minImageExtent.width ||
      lExtentToUse.height < lSInfo.surfaceInfo.minImageExtent.height)
    lExtentToUse = lSInfo.surfaceInfo.minImageExtent;

  if (lExtentToUse.width > lSInfo.surfaceInfo.maxImageExtent.width ||
      lExtentToUse.height > lSInfo.surfaceInfo.maxImageExtent.height)
    lExtentToUse = lSInfo.surfaceInfo.maxImageExtent;

  auto lUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
  if ((lSInfo.surfaceInfo.supportedUsageFlags & static_cast<VkImageUsageFlags>(lUsageFlags)) == 0) {
    lUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  }

  if ((lSInfo.surfaceInfo.supportedUsageFlags & static_cast<VkImageUsageFlags>(lUsageFlags)) == 0) {
    eLOG("Surface does not support requierd image usage flags");
    return {std::move(lLock), VK_ERROR_INITIALIZATION_FAILED};
  }


  VkSwapchainKHR lOldSwapchain = vSwapChain;

  VkSwapchainCreateInfoKHR lCreateInfo;
  lCreateInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  lCreateInfo.pNext                 = nullptr;
  lCreateInfo.flags                 = 0;
  lCreateInfo.surface               = vSurface;
  lCreateInfo.minImageCount         = lNumImages;
  lCreateInfo.imageFormat           = vSwapchainFormat.format;
  lCreateInfo.imageColorSpace       = vSwapchainFormat.colorSpace;
  lCreateInfo.imageExtent           = lExtentToUse;
  lCreateInfo.imageArrayLayers      = 1; //!< \todo stereo rendering
  lCreateInfo.imageUsage            = static_cast<VkImageUsageFlags>(lUsageFlags);
  lCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
  lCreateInfo.queueFamilyIndexCount = 0;
  lCreateInfo.pQueueFamilyIndices   = nullptr;
  lCreateInfo.preTransform          = lSInfo.surfaceInfo.currentTransform;
  lCreateInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  lCreateInfo.presentMode           = lModelToUse;
  lCreateInfo.clipped               = VK_TRUE;
  lCreateInfo.oldSwapchain          = lOldSwapchain;

  VkImageMemoryBarrier lBarrier;
  lBarrier.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  lBarrier.pNext               = nullptr;
  lBarrier.srcAccessMask       = 0;
  lBarrier.dstAccessMask       = VK_ACCESS_MEMORY_READ_BIT;
  lBarrier.oldLayout           = VK_IMAGE_LAYOUT_UNDEFINED;
  lBarrier.newLayout           = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  lBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  lBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  lBarrier.image               = VK_NULL_HANDLE;
  lBarrier.subresourceRange    = cfg.subResRange;

// clang-format off
#if D_LOG_VULKAN_UTILS
  auto lSupUsageFlags = lSInfo.surfaceInfo.supportedUsageFlags;
  auto lSupCompAlpha  = lSInfo.surfaceInfo.supportedCompositeAlpha;
  auto lSupTransforms = lSInfo.surfaceInfo.supportedTransforms;
  auto lMinExtent     = lSInfo.surfaceInfo.minImageExtent;
  auto lMaxExtent     = lSInfo.surfaceInfo.maxImageExtent;
  auto lCurrExtent    = lSInfo.surfaceInfo.currentExtent;

  dLOG( "  -- images Info:");
  dLOG( "    -- minImageCount:  ", lSInfo.surfaceInfo.minImageCount );
  dLOG( "    -- maxImageCount:  ", lSInfo.surfaceInfo.maxImageCount );
  dLOG( "    -- currentExtent:  ", lCurrExtent.width, "x", lCurrExtent.height );
  dLOG( "    -- minImageExtent: ", lMinExtent.width,  "x", lMinExtent.height  );
  dLOG( "    -- maxImageExtent: ", lMaxExtent.width,  "x", lMaxExtent.height  );
  dLOG( "  -- Supported usage flags: (prefix VK_IMAGE_USAGE_)" );
  dLOG( "    -- TRANSFER_SRC_BIT:                 ", ( lSupUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT )                           != 0 );
  dLOG( "    -- TRANSFER_DST_BIT:                 ", ( lSupUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT )                           != 0 );
  dLOG( "    -- USAGE_SAMPLED_BIT:                ", ( lSupUsageFlags & VK_IMAGE_USAGE_SAMPLED_BIT )                                != 0 );
  dLOG( "    -- STORAGE_BIT:                      ", ( lSupUsageFlags & VK_IMAGE_USAGE_STORAGE_BIT )                                != 0 );
  dLOG( "    -- ATTACHMENT_BIT:                   ", ( lSupUsageFlags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT )                       != 0 );
  dLOG( "    -- DEPTH_STENCIL_ATTACHMENT_BIT:     ", ( lSupUsageFlags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT )               != 0 );
  dLOG( "    -- TRANSIENT_ATTACHMENT_BIT:         ", ( lSupUsageFlags & VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT )                   != 0 );
  dLOG( "    -- INPUT_ATTACHMENT_BIT:             ", ( lSupUsageFlags & VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT )                       != 0 );
  dLOG( "  -- Supported composite alpha flags: (prefix VK_COMPOSITE_ALPHA_)" );
  dLOG( "    -- OPAQUE_BIT_KHR:                   ", ( lSupCompAlpha  & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR )                         != 0 );
  dLOG( "    -- PRE_MULTIPLIED_BIT_KHR:           ", ( lSupCompAlpha  & VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR )                 != 0 );
  dLOG( "    -- POST_MULTIPLIED_BIT_KHR:          ", ( lSupCompAlpha  & VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR )                != 0 );
  dLOG( "    -- INHERIT_BIT_KHR:                  ", ( lSupCompAlpha  & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR )                        != 0 );
  dLOG( "  -- Supported transform flags: (prefix VK_SURFACE_TRANSFORM_)" );
  dLOG( "    -- IDENTITY_BIT:                     ", ( lSupTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR )                     != 0 );
  dLOG( "    -- ROTATE_90_BIT:                    ", ( lSupTransforms & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR )                    != 0 );
  dLOG( "    -- ROTATE_180_BIT:                   ", ( lSupTransforms & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR )                   != 0 );
  dLOG( "    -- ROTATE_270_BIT:                   ", ( lSupTransforms & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR )                   != 0 );
  dLOG( "    -- HORIZONTAL_MIRROR_BIT:            ", ( lSupTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR )            != 0 );
  dLOG( "    -- HORIZONTAL_MIRROR_ROTATE_90_BIT:  ", ( lSupTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR )  != 0 );
  dLOG( "    -- HORIZONTAL_MIRROR_ROTATE_180_BIT: ", ( lSupTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR ) != 0 );
  dLOG( "    -- HORIZONTAL_MIRROR_ROTATE_270_BIT: ", ( lSupTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR ) != 0 );
  dLOG( "    -- INHERIT_BIT:                      ", ( lSupTransforms & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR )                      != 0 );

  dLOG("  -- Creating swapchain with:");
  dLOG("    - minImageCount:   ", lCreateInfo.minImageCount);
  dLOG("    - imageFormat:     ", uEnum2Str::toStr(lCreateInfo.imageFormat));
  dLOG("    - imageColorSpace: ", uEnum2Str::toStr(lCreateInfo.imageColorSpace));
  dLOG("    - preTransform:    ", uEnum2Str::toStr(lCreateInfo.preTransform));
  dLOG("    - presentMode:     ", uEnum2Str::toStr(lCreateInfo.presentMode));
  dLOG("    - imageUsage:      ", uEnum2Str::VkImageUsageFlagBits_toStr(lCreateInfo.imageUsage));
  dLOG("    - imageExtent:     ", lCreateInfo.imageExtent.width, "x", lCreateInfo.imageExtent.height);
#endif
  // clang-format on

  VkResult lRes = vkCreateSwapchainKHR(**vDevice, &lCreateInfo, nullptr, &vSwapChain);
  if (lRes) {
    eLOG("'vkCreateSwapchainKHR' returned ", uEnum2Str::toStr(lRes));
    return {std::move(lLock), lRes};
  }

  // Destroying old swapchain
  if (lOldSwapchain != VK_NULL_HANDLE) {
    dVkLOG(L"  -- Destroying old swapchain");
    dVkLOG(L"    - Destroying image views");
    for (auto i : vSwapchainViews)
      vkDestroyImageView(**vDevice, i, nullptr);

    vSwapchainViews.clear();
    vSwapchainImages.clear();

    dVkLOG(L"    - Destroying old swapchain");
    vkDestroySwapchainKHR(**vDevice, lOldSwapchain, nullptr);
  }

  // Aquire new images
  uint32_t lNum;
  lRes = vkGetSwapchainImagesKHR(**vDevice, vSwapChain, &lNum, nullptr);
  if (lRes) {
    eLOG("'vkGetSwapchainImagesKHR' returned ", uEnum2Str::toStr(lRes));
    return {std::move(lLock), lRes};
  }

  vSwapchainImages.resize(lNum);

  lRes = vkGetSwapchainImagesKHR(**vDevice, vSwapChain, &lNum, vSwapchainImages.data());
  if (lRes) {
    eLOG("'vkGetSwapchainImagesKHR' returned ", uEnum2Str::toStr(lRes));
    return {std::move(lLock), lRes};
  }

  dVkLOG("  -- Final number of swapchain images: ", lNum);

  std::vector<VkImageMemoryBarrier> lBarriers;
  for (auto &i : vSwapchainImages) {
    vSwapchainViews.emplace_back();
    lBarriers.push_back(lBarrier);

    VkImageViewCreateInfo lInfo;
    lInfo.sType      = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    lInfo.pNext      = nullptr;
    lInfo.flags      = 0;
    lInfo.image      = i;
    lInfo.viewType   = VK_IMAGE_VIEW_TYPE_2D;
    lInfo.format     = vSwapchainFormat.format;
    lInfo.components = {
        VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A,
    };
    lInfo.subresourceRange = cfg.subResRange;

    lBarriers.back().image = i;

    lRes = vkCreateImageView(**vDevice, &lInfo, nullptr, &vSwapchainViews.back());
    if (lRes) {
      eLOG("'vkCreateImageView' returned ", uEnum2Str::toStr(lRes));
      return {std::move(lLock), lRes};
    }
  }

  // We are techincally done now, but we should chanche the layout of the images

  uint32_t         lQueueFamily;
  VkQueue          lQueue = vDevice->getQueue(VK_QUEUE_TRANSFER_BIT, 0.0, &lQueueFamily);
  vkuCommandBuffer lBuf   = vkuCommandPoolManager::getBuffer(**vDevice, lQueueFamily);
  vkuFence_t       lFence(**vDevice);

  if (!lBuf) {
    eLOG(L"Failed to create command buffer for image layout transition");
    return {std::move(lLock), VK_SUBOPTIMAL_KHR};
  }

  if (lBuf.begin(VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT) != VK_SUCCESS) {
    eLOG(L"Failed to begin command buffer for image layout transition");
    return {std::move(lLock), VK_SUBOPTIMAL_KHR};
  }

  vkCmdPipelineBarrier(*lBuf,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                       0,
                       0,
                       nullptr,
                       0,
                       nullptr,
                       static_cast<uint32_t>(lBarriers.size()),
                       lBarriers.data());

  lBuf.end();

  VkSubmitInfo lInfo;
  lInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  lInfo.pNext                = nullptr;
  lInfo.waitSemaphoreCount   = 0;
  lInfo.pWaitSemaphores      = nullptr;
  lInfo.pWaitDstStageMask    = nullptr;
  lInfo.commandBufferCount   = 1;
  lInfo.pCommandBuffers      = &lBuf.get();
  lInfo.signalSemaphoreCount = 0;
  lInfo.pSignalSemaphores    = nullptr;

  {
    std::lock_guard<std::mutex> lGuard3(vDevice->getQueueMutex(lQueue));
    vkQueueSubmit(lQueue, 1, &lInfo, lFence[0]);
  }

  lRes = lFence();
  if (lRes) {
    eLOG("'vkWaitForFences' returned ", uEnum2Str::toStr(lRes));
    return {std::move(lLock), VK_SUBOPTIMAL_KHR};
  }

  return {std::move(lLock), 0};
}

void vkuSwapChain::destroy() {
  std::lock_guard<std::mutex> lLock(vSwapChainCreateMutex);

  if (!isCreated())
    return;

  dVkLOG(L"Destroying swapchain");
  dVkLOG(L"  -- Destroying image views");
  for (auto i : vSwapchainViews)
    vkDestroyImageView(**vDevice, i, nullptr);

  vSwapchainViews.clear();
  vSwapchainImages.clear();

  dVkLOG(L"  -- Destroying swapchain");
  vkDestroySwapchainKHR(**vDevice, vSwapChain, nullptr);

  vSwapChain = VK_NULL_HANDLE;
  vDevice    = nullptr;
}

vkuSwapChain::LockAndResult vkuSwapChain::acquireNextImage(VkSemaphore _semaphore, VkFence _fence, uint64_t _timeout) {
  std::unique_lock<std::mutex> lLock(vSwapChainCreateMutex);
  uint32_t                     lNextImg;

  auto lRes = vkAcquireNextImageKHR(**vDevice, vSwapChain, _timeout, _semaphore, _fence, &lNextImg);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to acquire next swap chain image:\nvkAcquireNextImageKHR returned: ", uEnum2Str::toStr(lRes));
    return {std::move(lLock), lRes};
  }

  return LockAndResult(std::move(lLock), lNextImg);
}



std::vector<vkuSwapChain::SwapChainImg> vkuSwapChain::getImages() const noexcept {
  std::vector<SwapChainImg> lTemp;
  lTemp.resize(vSwapchainViews.size());

  for (size_t i = 0; i < vSwapchainViews.size(); i++)
    lTemp[i]    = {vSwapchainImages[i], vSwapchainViews[i]};

  return lTemp;
}
