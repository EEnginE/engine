/*!
 * \file rRenderer_vkInit.cpp
 * \brief \b Classes: \a rRenderer
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

#include "rRenderer.hpp"
#include "rWorld.hpp"
#include "iInit.hpp"
#include "uLog.hpp"
#include "uEnum2Str.hpp"

#if D_LOG_VULKAN
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

namespace e_engine {

/*!
 * \brief creteates the depth and stencil buffer
 *
 * Destroies old buffers (if exist)
 *
 * \todo Stencil buffer implementation
 */
int rRenderer::initDepthAndStencilBuffer( VkCommandBuffer _buf ) {
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
   VkClearValue *lClearValue            = &vRenderPass_vk.clearValues[vRenderPass_vk.depthAttachID];

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

   lClearValue->color = {{vClearColor.float32[0],
                          vClearColor.float32[1],
                          vClearColor.float32[2],
                          vClearColor.float32[3]}};
   lClearValue->depthStencil = {1.0f, 0};

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

#if D_LOG_VULKAN
   dLOG( "Creating Depth buffer [renderer ", vID, "]" );
   dLOG( "  -- lHasStencil:     ", vHasStencilBuffer );
   dLOG( "  -- imageType:       ", uEnum2Str::toStr( lImageCreate.imageType ) );
   dLOG( "  -- fromat:          ", uEnum2Str::toStr( lImageCreate.format ) );
   dLOG( "  -- tiling:          ", uEnum2Str::toStr( lImageCreate.tiling ) );
   dLOG( "  -- memoryTypeIndex: ", lMemoryAlloc.memoryTypeIndex );
   dLOG( "  -- extent:          ", lImageCreate.extent.width, "x", lImageCreate.extent.height );
#endif

   vWorldPtr->cmdChangeImageLayout( _buf,
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
int rRenderer::initRenderPass() {
   if ( !vDevice_vk )
      return 1;

   if ( vRenderPass_vk.subpasses.empty() ) {
      eLOG( "Can not create a renderPass without at least one subpass!" );
      return 2;
   }

#if D_LOG_VULKAN
   uint32_t lCounter = 0;
   dLOG( "Creating a renderPass: [renderer ", vID, "]" );
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

int rRenderer::initFramebuffers() {
   auto lTempViews = vWorldPtr->getSwapchainImageViews();
   for ( auto i : lTempViews ) {
      vFramebuffers_vk.emplace_back();
      vFramebuffers_vk.back().img = i.img;
      vFramebuffers_vk.back().iv = i.iv;
   }

   if ( vRenderPass_vk.attachmentViews.size() != vRenderPass_vk.attachments.size() ) {
      eLOG( "Invalid number of framebuffer attachments" );
      return 2;
   }

   VkFramebufferCreateInfo lInfo;
   lInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
   lInfo.pNext           = nullptr;
   lInfo.flags           = 0;
   lInfo.renderPass      = vRenderPass_vk.renderPass;
   lInfo.attachmentCount = vRenderPass_vk.attachmentViews.size();
   lInfo.pAttachments    = vRenderPass_vk.attachmentViews.data();
   lInfo.width           = GlobConf.win.width;
   lInfo.height          = GlobConf.win.height;
   lInfo.layers          = 1;

   vRenderPass_vk.attachmentViews[vRenderPass_vk.depthAttachID] = vDepthStencilBuf_vk.iv;

   dVkLOG( "Creating framebuffers: [renderer ", vID, "]" );
   dVkLOG( "  -- extent: ", lInfo.width, "x", lInfo.height );

   for ( auto &i : vFramebuffers_vk ) {
      vRenderPass_vk.attachmentViews[vRenderPass_vk.frameAttachID] = i.iv;

      auto lRes = vkCreateFramebuffer( vDevice_vk, &lInfo, nullptr, &i.fb );
      if ( lRes ) {
         eLOG( "'vkCreateFramebuffer' returned ", uEnum2Str::toStr( lRes ) );
         return 1;
      }
   }
   return 0;
}
}
