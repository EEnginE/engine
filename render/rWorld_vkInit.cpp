/*!
 * \file rWorld.cpp
 * \brief \b Classes: \a rWorld
 */
/*
 * Copyright (C) 2016 EEnginE project
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

#include "defines.hpp"
#include "uConfig.hpp"

#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"
#include "rWorld.hpp"

#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

namespace e_engine {



/*!
 * \brief recreates the vulkan swapchain
 * \return 0 on success
 */
int rWorld::recreateSwapchain() {
  vSurface_vk = vInitPtr->getVulkanSurface();
  auto lSInfo = vInitPtr->getSurfaceInfo();

  VkPresentModeKHR lModelToUse = VK_PRESENT_MODE_MAX_ENUM_KHR;
  vSwapchainFormat.format      = VK_FORMAT_UNDEFINED;

  dVkLOG("Surface device info:");
  dVkLOG("  -- Surface formats:");
  for (auto const &i : lSInfo.formats) {
    if (vSwapchainFormat.format == VK_FORMAT_UNDEFINED)
      vSwapchainFormat = i;

    if (i.format == GlobConf.vk.preferedSurfaceFormat) {
      dVkLOG("    -- Format: ",
             uEnum2Str::toStr(i.format),
             "; colorSpace: ",
             uEnum2Str::toStr(i.colorSpace),
             " [PREFERED]");
      vSwapchainFormat = i;
    } else {
      dVkLOG("    -- Format: ", uEnum2Str::toStr(i.format), "; colorSpace: ", uEnum2Str::toStr(i.colorSpace));
    }
  }

  dVkLOG("  -- Present models:");
  for (auto const &i : lSInfo.presentModels) {
    if (lModelToUse == VK_PRESENT_MODE_MAX_ENUM_KHR)
      lModelToUse = i;

    if (GlobConf.vk.enableVSync) {
      if (i == VK_PRESENT_MODE_MAILBOX_KHR ||
          (i == VK_PRESENT_MODE_FIFO_KHR && lModelToUse != VK_PRESENT_MODE_MAILBOX_KHR))
        lModelToUse = i;
    } else {
      if (i == VK_PRESENT_MODE_IMMEDIATE_KHR)
        lModelToUse = i;
    }
    dVkLOG("    -- model: ", uEnum2Str::toStr(i));
  }

// clang-format off
#if D_LOG_VULKAN_INIT
   auto lSupUsageFlags = lSInfo.surfaceInfo.supportedUsageFlags;
   auto lSupCompAlpha  = lSInfo.surfaceInfo.supportedCompositeAlpha;
   auto lSupTransforms = lSInfo.surfaceInfo.supportedTransforms;

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
#endif
  // clang-format on

  uint32_t lNumImages = lSInfo.surfaceInfo.minImageCount + 1;
  if (lNumImages > lSInfo.surfaceInfo.maxImageCount)
    lNumImages = lSInfo.surfaceInfo.maxImageCount;

  VkExtent2D lExtentToUse = lSInfo.surfaceInfo.currentExtent;
  lExtentToUse.width      = GlobConf.win.width;
  lExtentToUse.height     = GlobConf.win.height;

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
    return 6;
  }

  VkSwapchainCreateInfoKHR lCreateInfo;
  lCreateInfo.sType                 = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  lCreateInfo.pNext                 = nullptr;
  lCreateInfo.flags                 = 0;
  lCreateInfo.surface               = vSurface_vk;
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
  lCreateInfo.oldSwapchain          = nullptr;

#if D_LOG_VULKAN
  dLOG("Creating swapchain with:");
  dLOG("  -- minImageCount:   ", lCreateInfo.minImageCount);
  dLOG("  -- imageFormat:     ", uEnum2Str::toStr(lCreateInfo.imageFormat));
  dLOG("  -- imageColorSpace: ", uEnum2Str::toStr(lCreateInfo.imageColorSpace));
  dLOG("  -- preTransform:    ", uEnum2Str::toStr(lCreateInfo.preTransform));
  dLOG("  -- presentMode:     ", uEnum2Str::toStr(lCreateInfo.presentMode));
  dLOG("  -- imageUsage:      ", uEnum2Str::toStr(static_cast<VkImageUsageFlagBits>(lCreateInfo.imageUsage)));
  dLOG("  -- imageExtent:     ", lCreateInfo.imageExtent.width, "x", lCreateInfo.imageExtent.height);
#endif

  vkDeviceWaitIdle(vDevice_vk);
  VkResult lRes = vkCreateSwapchainKHR(vDevice_vk, &lCreateInfo, nullptr, &vSwapchain_vk);
  if (lRes) {
    eLOG("'vkCreateSwapchainKHR' returned ", uEnum2Str::toStr(lRes));
    return 1;
  }

  vkDeviceWaitIdle(vDevice_vk);

  return 0;
}



/*!
 * \brief recreates swapchain image views
 * \returns 0 on success
 */
int rWorld::recreateSwapchainImages(VkCommandBuffer _buf) {
  if (!vDevice_vk)
    return 1;

  uint32_t lNum;
  auto     lRes = vkGetSwapchainImagesKHR(vDevice_vk, vSwapchain_vk, &lNum, nullptr);
  if (lRes) {
    eLOG("'vkGetSwapchainImagesKHR' returned ", uEnum2Str::toStr(lRes));
    return 1;
  }

  vSwapchainImages_vk.resize(lNum);

  lRes = vkGetSwapchainImagesKHR(vDevice_vk, vSwapchain_vk, &lNum, vSwapchainImages_vk.data());
  if (lRes) {
    eLOG("'vkGetSwapchainImagesKHR' returned ", uEnum2Str::toStr(lRes));
    return 2;
  }

  dVkLOG("Final number of swapchain images: ", lNum);

  for (auto &i : vSwapchainImages_vk) {
    vSwapchainViews_vk.emplace_back();

    VkImageViewCreateInfo lInfo;
    lInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    lInfo.pNext                           = nullptr;
    lInfo.flags                           = 0;
    lInfo.image                           = i;
    lInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    lInfo.format                          = vSwapchainFormat.format;
    lInfo.components.r                    = VK_COMPONENT_SWIZZLE_R;
    lInfo.components.g                    = VK_COMPONENT_SWIZZLE_G;
    lInfo.components.b                    = VK_COMPONENT_SWIZZLE_B;
    lInfo.components.a                    = VK_COMPONENT_SWIZZLE_A;
    lInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    lInfo.subresourceRange.baseMipLevel   = 0;
    lInfo.subresourceRange.levelCount     = 1;
    lInfo.subresourceRange.baseArrayLayer = 0;
    lInfo.subresourceRange.layerCount     = 1;

    lRes = vkCreateImageView(vDevice_vk, &lInfo, nullptr, &vSwapchainViews_vk.back());
    if (lRes) {
      eLOG("'vkCreateImageView' returned ", uEnum2Str::toStr(lRes));
      return 4;
    }

    cmdChangeImageLayout(
        _buf, lInfo.image, lInfo.subresourceRange, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
  }

  return 0;
}
}
