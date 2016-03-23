/*!
 * \file rRoot.cpp
 * \brief \b Classes: \a rRoot
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

#include "rRoot.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"

#if D_LOG_VULKAN
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

namespace e_engine {



/*!
 * \brief recreates the vulkan swapchain
 * \return 0 on success
 */
int rRoot::recreateSwapchain() {
   auto lSInfo = vInitPtr->getSurfaceInfo();

   VkPresentModeKHR lModelToUse = VK_PRESENT_MODE_MAX_ENUM;
   vSwapchainFormat.format      = VK_FORMAT_UNDEFINED;

   dVkLOG( "Surface device info:" );
   dVkLOG( "  -- Surface formats:" );
   for ( auto const &i : lSInfo.formats ) {
      if ( vSwapchainFormat.format == VK_FORMAT_UNDEFINED )
         vSwapchainFormat = i;

      if ( i.format == GlobConf.vk.preferedSurfaceFormat ) {
         dVkLOG( "    -- Format: ",
                 uEnum2Str::toStr( i.format ),
                 "; colorSpace: ",
                 uEnum2Str::toStr( i.colorSpace ),
                 " [PREFERED]" );
         vSwapchainFormat = i;
      } else {
         dVkLOG( "    -- Format: ",
                 uEnum2Str::toStr( i.format ),
                 "; colorSpace: ",
                 uEnum2Str::toStr( i.colorSpace ) );
      }
   }

   dVkLOG( "  -- Present models:" );
   for ( auto const &i : lSInfo.presentModels ) {
      if ( lModelToUse == VK_PRESENT_MODE_MAX_ENUM )
         lModelToUse = i;

      if ( GlobConf.vk.preferedSurfaceFormat ) {
         if ( i == VK_PRESENT_MODE_FIFO_KHR )
            lModelToUse = i;
      } else {
         if ( i == VK_PRESENT_MODE_IMMEDIATE_KHR )
            lModelToUse = i;
      }
      dVkLOG( "    -- model: ", uEnum2Str::toStr( i ) );
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

   VkSwapchainKHR lOldSwapchain = vSwapchain_vk;
   VkSwapchainKHR lNewSwapchain;

   uint32_t lNumImages = lSInfo.surfaceInfo.minImageCount + 1;
   if ( lNumImages > lSInfo.surfaceInfo.maxImageCount )
      lNumImages = lSInfo.surfaceInfo.maxImageCount;

   VkExtent2D lExtentToUse = lSInfo.surfaceInfo.currentExtent;
   lExtentToUse.width      = GlobConf.win.width;
   lExtentToUse.height     = GlobConf.win.height;

   if ( lExtentToUse.width < lSInfo.surfaceInfo.minImageExtent.width ||
        lExtentToUse.height < lSInfo.surfaceInfo.minImageExtent.height )
      lExtentToUse = lSInfo.surfaceInfo.minImageExtent;

   if ( lExtentToUse.width > lSInfo.surfaceInfo.maxImageExtent.width ||
        lExtentToUse.height > lSInfo.surfaceInfo.maxImageExtent.height )
      lExtentToUse = lSInfo.surfaceInfo.maxImageExtent;

   auto lUsageFlags = VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
   if ( ( lSInfo.surfaceInfo.supportedUsageFlags & lUsageFlags ) == 0 ) {
      lUsageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
   }

   if ( ( lSInfo.surfaceInfo.supportedUsageFlags & lUsageFlags ) == 0 ) {
      eLOG( "Surface does not support requierd image usage flags" );
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
   lCreateInfo.imageUsage            = lUsageFlags;
   lCreateInfo.imageSharingMode      = VK_SHARING_MODE_EXCLUSIVE;
   lCreateInfo.queueFamilyIndexCount = 0;
   lCreateInfo.pQueueFamilyIndices   = nullptr;
   lCreateInfo.preTransform          = lSInfo.surfaceInfo.currentTransform;
   lCreateInfo.compositeAlpha        = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
   lCreateInfo.presentMode           = lModelToUse;
   lCreateInfo.clipped               = VK_TRUE;
   lCreateInfo.oldSwapchain          = lOldSwapchain;

   iLOG( "Creating swapchain with:" );
   iLOG( "  -- minImageCount:   ", lCreateInfo.minImageCount );
   iLOG( "  -- imageFormat:     ", uEnum2Str::toStr( lCreateInfo.imageFormat ) );
   iLOG( "  -- imageColorSpace: ", uEnum2Str::toStr( lCreateInfo.imageColorSpace ) );
   iLOG( "  -- preTransform:    ", uEnum2Str::toStr( lCreateInfo.preTransform ) );
   iLOG( "  -- presentMode:     ", uEnum2Str::toStr( lCreateInfo.presentMode ) );
   iLOG( "  -- imageUsage:      ",
         uEnum2Str::toStr( static_cast<VkImageUsageFlagBits>( lCreateInfo.imageUsage ) ) );
   iLOG( "  -- imageExtent:     ",
         lCreateInfo.imageExtent.width,
         "x",
         lCreateInfo.imageExtent.height );

   vkDeviceWaitIdle( vDevice_vk );
   VkResult lRes = vkCreateSwapchainKHR( vDevice_vk, &lCreateInfo, nullptr, &lNewSwapchain );
   if ( lRes ) {
      eLOG( "'vkCreateSwapchainKHR' returned ", uEnum2Str::toStr( lRes ) );
      return 1;
   }

   vkDeviceWaitIdle( vDevice_vk );

   // Destroying old image views before destroying old swapchain
   for ( auto &i : vFramebuffers_vk )
      vkDestroyImageView( vDevice_vk, i.iv, nullptr );

   if ( lOldSwapchain ) {
      vkDestroySwapchainKHR( vDevice_vk, lOldSwapchain, nullptr );
   }

   vSwapchain_vk = lNewSwapchain;
   return 0;
}



/*!
 * \brief recreates swapchain image views
 * \returns 0 on success
 */
int rRoot::recreateSwapchainImages( VkCommandBuffer _buf ) {
   if ( !vDevice_vk )
      return 1;

   uint32_t lNum;
   auto lRes = vkGetSwapchainImagesKHR( vDevice_vk, vSwapchain_vk, &lNum, nullptr );
   if ( lRes ) {
      eLOG( "'vkGetSwapchainImagesKHR' returned ", uEnum2Str::toStr( lRes ) );
      return 1;
   }

   std::vector<VkImage> lSwapcainImages;
   lSwapcainImages.resize( lNum );

   lRes = vkGetSwapchainImagesKHR( vDevice_vk, vSwapchain_vk, &lNum, lSwapcainImages.data() );
   if ( lRes ) {
      eLOG( "'vkGetSwapchainImagesKHR' returned ", uEnum2Str::toStr( lRes ) );
      return 2;
   }

   for ( auto &i : lSwapcainImages ) {
      vFramebuffers_vk.emplace_back();
      vFramebuffers_vk.back().img = i;

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

      lRes = vkCreateImageView( vDevice_vk, &lInfo, nullptr, &vFramebuffers_vk.back().iv );
      if ( lRes ) {
         eLOG( "'vkCreateImageView' returned ", uEnum2Str::toStr( lRes ) );
         return 4;
      }

      cmdChangeImageLayout( _buf,
                            lInfo.image,
                            lInfo.subresourceRange,
                            VK_IMAGE_LAYOUT_UNDEFINED,
                            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );
   }

   VkAttachmentDescription *lAttachment = &vRenderPass_vk.attachments[vRenderPass_vk.frameAttachID];
   lAttachment->flags                   = 0;
   lAttachment->format                  = vSwapchainFormat.format;
   lAttachment->samples                 = GlobConf.vk.samples;
   lAttachment->loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
   lAttachment->storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
   lAttachment->stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   lAttachment->stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   lAttachment->initialLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   lAttachment->finalLayout             = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

   return 0;
}




/*!
 * \brief creteates the depth and stencil buffer
 *
 * Destroies old buffers (if exist)
 *
 * \todo Stencil buffer implementation
 */
int rRoot::recreateDepthAndStencilBuffer( VkCommandBuffer _buf ) {
   if ( !vDevice_vk ) {
      eLOG( "Device not created!" );
      return -1;
   }

   static const VkFormat lDepthFormats[] = {
         VK_FORMAT_D32_SFLOAT_S8_UINT,
         VK_FORMAT_D24_UNORM_S8_UINT,
         VK_FORMAT_D16_UNORM_S8_UINT,
         VK_FORMAT_D32_SFLOAT,
         VK_FORMAT_X8_D24_UNORM_PACK32,
         VK_FORMAT_D16_UNORM,
   };

   VkFormat lDepthStencilFormat = VK_FORMAT_UNDEFINED;
   VkImageTiling lTiling        = VK_IMAGE_TILING_MAX_ENUM;

   for ( auto i : lDepthFormats ) {
      if ( vInitPtr->formatSupportsFeature(
                 i, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_LINEAR ) ) {
         lDepthStencilFormat = i;
         lTiling             = VK_IMAGE_TILING_LINEAR;
         break;
      } else if ( vInitPtr->formatSupportsFeature( i,
                                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                   VK_IMAGE_TILING_OPTIMAL ) ) {
         lDepthStencilFormat = i;
         lTiling             = VK_IMAGE_TILING_OPTIMAL;
         break;
      }
   }

   if ( lDepthStencilFormat == VK_FORMAT_UNDEFINED ) {
      eLOG( "Unable to find depth format for the depth buffer" );
      return 1;
   }

   vHasStencilBuffer = lDepthStencilFormat == VK_FORMAT_D32_SFLOAT_S8_UINT ||
                       lDepthStencilFormat == VK_FORMAT_D24_UNORM_S8_UINT ||
                       lDepthStencilFormat == VK_FORMAT_D16_UNORM_S8_UINT;

   VkImageAspectFlags lAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
   lAspectFlags |= vHasStencilBuffer ? VK_IMAGE_ASPECT_STENCIL_BIT : 0;

   VkImageCreateInfo lImageCreate;
   VkMemoryAllocateInfo lMemoryAlloc;
   VkMemoryRequirements lRequirements;
   VkImageViewCreateInfo lImageViewCreate;
   VkAttachmentDescription *lAttachment = &vRenderPass_vk.attachments[vRenderPass_vk.depthAttachID];

   lImageCreate.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
   lImageCreate.pNext                 = nullptr;
   lImageCreate.flags                 = 0;
   lImageCreate.imageType             = VK_IMAGE_TYPE_2D;
   lImageCreate.format                = lDepthStencilFormat;
   lImageCreate.extent.width          = GlobConf.win.width;
   lImageCreate.extent.height         = GlobConf.win.height;
   lImageCreate.extent.depth          = 1;
   lImageCreate.mipLevels             = 1;
   lImageCreate.arrayLayers           = 1;
   lImageCreate.samples               = GlobConf.vk.samples;
   lImageCreate.tiling                = lTiling;
   lImageCreate.usage                 = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
   lImageCreate.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
   lImageCreate.queueFamilyIndexCount = 0;
   lImageCreate.pQueueFamilyIndices   = nullptr;
   lImageCreate.initialLayout         = VK_IMAGE_LAYOUT_UNDEFINED;

   lMemoryAlloc.sType           = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
   lMemoryAlloc.pNext           = nullptr;
   lMemoryAlloc.allocationSize  = 0; // Will be set further down
   lMemoryAlloc.memoryTypeIndex = 0;

   lImageViewCreate.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
   lImageViewCreate.pNext                           = nullptr;
   lImageViewCreate.flags                           = 0;
   lImageViewCreate.image                           = nullptr; // set further down
   lImageViewCreate.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
   lImageViewCreate.format                          = lDepthStencilFormat;
   lImageViewCreate.components.r                    = VK_COMPONENT_SWIZZLE_R;
   lImageViewCreate.components.g                    = VK_COMPONENT_SWIZZLE_G;
   lImageViewCreate.components.b                    = VK_COMPONENT_SWIZZLE_B;
   lImageViewCreate.components.a                    = VK_COMPONENT_SWIZZLE_A;
   lImageViewCreate.subresourceRange.aspectMask     = lAspectFlags;
   lImageViewCreate.subresourceRange.baseMipLevel   = 0;
   lImageViewCreate.subresourceRange.levelCount     = 1;
   lImageViewCreate.subresourceRange.baseArrayLayer = 0;
   lImageViewCreate.subresourceRange.layerCount     = 1;

   lAttachment->flags          = 0;
   lAttachment->format         = lDepthStencilFormat;
   lAttachment->samples        = GlobConf.vk.samples;
   lAttachment->loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
   lAttachment->storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
   lAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   lAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   lAttachment->initialLayout  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
   lAttachment->finalLayout    = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

   if ( vHasStencilBuffer ) {
      lAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
      lAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
   }

   auto lRes = vkCreateImage( vDevice_vk, &lImageCreate, nullptr, &vDepthStencilBuf_vk.img );
   if ( lRes ) {
      eLOG( "'vkCreateImage' returned ", uEnum2Str::toStr( lRes ) );
      return 2;
   }

   vkGetImageMemoryRequirements( vDevice_vk, vDepthStencilBuf_vk.img, &lRequirements );
   lMemoryAlloc.allocationSize = lRequirements.size;
   lMemoryAlloc.memoryTypeIndex =
         vInitPtr->getMemoryTypeIndexFromBitfield( lRequirements.memoryTypeBits );

   if ( lMemoryAlloc.memoryTypeIndex == UINT32_MAX ) {
      eLOG( "No valid memory type found!" );
      return 3;
   }

   lRes = vkAllocateMemory( vDevice_vk, &lMemoryAlloc, nullptr, &vDepthStencilBuf_vk.mem );
   if ( lRes ) {
      eLOG( "'vkAllocateMemory' returned ", uEnum2Str::toStr( lRes ) );
      return 4;
   }

   lRes = vkBindImageMemory( vDevice_vk, vDepthStencilBuf_vk.img, vDepthStencilBuf_vk.mem, 0 );
   if ( lRes ) {
      eLOG( "'vkBindImageMemory' returned ", uEnum2Str::toStr( lRes ) );
      return 5;
   }

   lImageViewCreate.image = vDepthStencilBuf_vk.img;
   lRes = vkCreateImageView( vDevice_vk, &lImageViewCreate, nullptr, &vDepthStencilBuf_vk.iv );
   if ( lRes ) {
      eLOG( "'vkCreateImageView' returned ", uEnum2Str::toStr( lRes ) );
      return 6;
   }

#if D_LOG_VULKAN_INIT
   dLOG( "Creating Depth buffer" );
   dLOG( "  -- lHasStencil:     ", vHasStencilBuffer );
   dLOG( "  -- imageType:       ", uEnum2Str::toStr( lImageCreate.imageType ) );
   dLOG( "  -- fromat:          ", uEnum2Str::toStr( lImageCreate.format ) );
   dLOG( "  -- tiling:          ", uEnum2Str::toStr( lImageCreate.tiling ) );
   dLOG( "  -- memoryTypeIndex: ", lMemoryAlloc.memoryTypeIndex );
#endif

   cmdChangeImageLayout( _buf,
                         lImageViewCreate.image,
                         lImageViewCreate.subresourceRange,
                         VK_IMAGE_LAYOUT_UNDEFINED,
                         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL );

   return 0;
}




/*!
 * \brief recreates the vulkan renderpass
 * \returns 0 on success
 */
int rRoot::recreateRenderPass() {
   if ( !vDevice_vk )
      return 1;

   if ( vRenderPass_vk.subpasses.empty() ) {
      eLOG( "Can not create a renderPass without at least one subpass!" );
      return 2;
   }

#if D_LOG_VULKAN
   uint32_t lCounter = 0;
   dLOG( "Creating a renderPass:" );
   dLOG( "  -- attachments:" );
   for ( auto const &i : vRenderPass_vk.attachments ) {
      dLOG( "    -- attachment ", lCounter++ );
      dLOG( "      - flags:          ", i.flags );
      dLOG( "      - format:         ", uEnum2Str::toStr( i.format ) );
      dLOG( "      - samples:        ", uEnum2Str::toStr( i.samples ) );
      dLOG( "      - loadOp:         ", uEnum2Str::toStr( i.loadOp ) );
      dLOG( "      - storeOp:        ", uEnum2Str::toStr( i.storeOp ) );
      dLOG( "      - stencilLoadOp:  ", uEnum2Str::toStr( i.stencilLoadOp ) );
      dLOG( "      - stencilStoreOp: ", uEnum2Str::toStr( i.stencilStoreOp ) );
      dLOG( "      - initialLayout:  ", uEnum2Str::toStr( i.initialLayout ) );
      dLOG( "      - finalLayout:    ", uEnum2Str::toStr( i.finalLayout ) );
   }
   lCounter = 0;
   dLOG( "  -- subpasses:" );
   for ( auto const &i : vRenderPass_vk.subpasses ) {
      dLOG( "    -- subpass ", lCounter++ );
      dLOG( "      - flags:             ", i.flags );
      dLOG( "      - pipelineBindPoint: ", uEnum2Str::toStr( i.pipelineBindPoint ) );
      dLOG( "      - inputAttachments:" );
      for ( uint32_t j = 0; j < i.inputAttachmentCount; j++ ) {
         auto lTemp = i.pInputAttachments[j];
         dLOG( "       - ", lTemp.attachment, "; ", uEnum2Str::toStr( lTemp.layout ) );
      }
      dLOG( "      - colorAttachments:" );
      for ( uint32_t j = 0; j < i.colorAttachmentCount; j++ ) {
         auto lTemp = i.pColorAttachments[j];
         dLOG( "       - ", lTemp.attachment, "; ", uEnum2Str::toStr( lTemp.layout ) );
      }
      dLOG( "      - resolveAttachments:" );
      for ( uint32_t j = 0; j < i.colorAttachmentCount; j++ ) {
         auto lTemp = i.pResolveAttachments[j];
         if ( lTemp.attachment == VK_ATTACHMENT_UNUSED )
            continue;
         dLOG( "        - ", lTemp.attachment, "; ", uEnum2Str::toStr( lTemp.layout ) );
      }
      auto lTemp = i.pDepthStencilAttachment;
      dLOG( "      - depthStencilAttachment: ",
            lTemp->attachment,
            "; ",
            uEnum2Str::toStr( lTemp->layout ) );
      dLOG( "      - preserveAttachments:" );
      for ( uint32_t j = 0; j < i.preserveAttachmentCount; j++ ) {
         auto lTemp = i.pPreserveAttachments[j];
         dLOG( "       - ", lTemp );
      }
      dLOG( "  -- subpass dependecies:" );
      for ( auto const &i : vRenderPass_vk.dependecies ) {
         dLOG( "    -- attachment ", lCounter++ );
         dLOG( "      - srcSubpass:      ", i.srcSubpass );
         dLOG( "      - dstSubpass:      ", i.dstSubpass );
         dLOG( "      - srcStageMask:    ", i.srcStageMask );
         dLOG( "      - dstStageMask:    ", i.dstStageMask );
         dLOG( "      - srcAccessMask:   ", i.srcAccessMask );
         dLOG( "      - dstAccessMask:   ", i.dstAccessMask );
         dLOG( "      - dependencyFlags: ", i.dependencyFlags );
      }
   }
#endif

   VkRenderPassCreateInfo lInfo;
   lInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
   lInfo.pNext           = nullptr;
   lInfo.flags           = 0;
   lInfo.attachmentCount = vRenderPass_vk.attachments.size();
   lInfo.pAttachments    = vRenderPass_vk.attachments.data();
   lInfo.subpassCount    = vRenderPass_vk.subpasses.size();
   lInfo.pSubpasses      = vRenderPass_vk.subpasses.data();
   lInfo.dependencyCount = vRenderPass_vk.dependecies.size();
   lInfo.pDependencies   = vRenderPass_vk.dependecies.data();

   auto lRes = vkCreateRenderPass( vDevice_vk, &lInfo, nullptr, &vRenderPass_vk.renderPass );
   if ( lRes ) {
      eLOG( "'vkCreateRenderPass' returned ", uEnum2Str::toStr( lRes ) );
      return 3;
   }

   return 0;
}

int rRoot::recreateFramebuffers() {
   if ( vAttachmentImageViews.size() != vRenderPass_vk.attachments.size() ) {
      eLOG( "Invalid number of framebuffer attachments" );
      return 2;
   }

   VkFramebufferCreateInfo lInfo;
   lInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   lInfo.pNext           = nullptr;
   lInfo.flags           = 0;
   lInfo.renderPass      = vRenderPass_vk.renderPass;
   lInfo.attachmentCount = vAttachmentImageViews.size();
   lInfo.pAttachments    = vAttachmentImageViews.data();
   lInfo.width           = GlobConf.win.width;
   lInfo.height          = GlobConf.win.height;
   lInfo.layers          = 1;

   vAttachmentImageViews[vRenderPass_vk.depthAttachID] = vDepthStencilBuf_vk.iv;

   dVkLOG( "Creating framebuffers:" );

   for ( auto i : vFramebuffers_vk ) {
      vAttachmentImageViews[vRenderPass_vk.frameAttachID] = i.iv;

      auto lRes = vkCreateFramebuffer( vDevice_vk, &lInfo, nullptr, &i.fb );
      if ( lRes ) {
         eLOG( "'vkCreateFramebuffer' returned ", uEnum2Str::toStr( lRes ) );
         return 1;
      }
   }
   return 0;
}
}
