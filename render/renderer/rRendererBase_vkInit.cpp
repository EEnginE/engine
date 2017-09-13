/*!
 * \file rRendererBase_vkInit.cpp
 * \brief \b Classes: \a rRendererBase
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

#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"
#include "rRendererBase.hpp"
#include "rWorld.hpp"

#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

namespace e_engine {
namespace internal {

struct AttachmentInfoWorker {
  VkFormat                  format;
  VkImageUsageFlags         usage;
  VkImageLayout             layout;
  VkImageTiling             tiling;
  VkImageAspectFlags        aspect;
  uint32_t                  attachmentID;
  rRendererBase::Buffer_vk *buff;
};

/*!
 * \brief creteates the depth and stencil buffer
 *
 * Destroies old buffers (if exist)
 *
 * \todo Stencil buffer implementation
 */
int rRendererBase::initImageBuffers(VkCommandBuffer _buf) {
  if (!vDevice_vk) {
    eLOG("Device not created!");
    return -1;
  }

  VkImageCreateInfo     lImageCreate;
  VkMemoryAllocateInfo  lMemoryAlloc;
  VkMemoryRequirements  lRequirements;
  VkImageViewCreateInfo lImageViewCreate;

  auto lAttachmentInfosTemp = getAttachmentInfos();
  vBuffers.resize(lAttachmentInfosTemp.size()); // No more changes in size after this call

  std::vector<AttachmentInfoWorker> lAttachmentInfos;
  for (uint32_t i = 0; i < lAttachmentInfosTemp.size(); i++) {
    lAttachmentInfos.push_back({
        lAttachmentInfosTemp[i].format,
        lAttachmentInfosTemp[i].usage,
        lAttachmentInfosTemp[i].layout,
        lAttachmentInfosTemp[i].tiling,
        lAttachmentInfosTemp[i].aspect,
        lAttachmentInfosTemp[i].attachmentID,
        &vBuffers[i] // Legal because no more resize in vBuffers
    });
  }

  uint32_t lNumAttachments = FIRST_FREE_ATTACHMENT_INDEX + static_cast<uint32_t>(lAttachmentInfosTemp.size());

  vRenderPass_vk.attachments.resize(lNumAttachments);
  vRenderPass_vk.attachmentViews.resize(lNumAttachments);
  vRenderPass_vk.attachmentBuffers.resize(lNumAttachments);
  vRenderPass_vk.clearValues.resize(lNumAttachments);

  memset(vRenderPass_vk.attachmentViews.data(),
         0,
         vRenderPass_vk.attachmentViews.size() * sizeof(vRenderPass_vk.attachmentViews[0]));

  memset(vRenderPass_vk.attachmentBuffers.data(),
         0,
         vRenderPass_vk.attachmentBuffers.size() * sizeof(vRenderPass_vk.attachmentBuffers[0]));


  lImageCreate.sType                 = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
  lImageCreate.pNext                 = nullptr;
  lImageCreate.flags                 = 0;
  lImageCreate.imageType             = VK_IMAGE_TYPE_2D;
  lImageCreate.format                = VK_FORMAT_UNDEFINED; // set in loop
  lImageCreate.extent.width          = GlobConf.win.width;
  lImageCreate.extent.height         = GlobConf.win.height;
  lImageCreate.extent.depth          = 1;
  lImageCreate.mipLevels             = 1;
  lImageCreate.arrayLayers           = 1;
  lImageCreate.samples               = GlobConf.vk.samples;
  lImageCreate.tiling                = VK_IMAGE_TILING_MAX_ENUM; // set in loop
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
  lImageViewCreate.image                           = nullptr; // set in loop
  lImageViewCreate.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
  lImageViewCreate.format                          = VK_FORMAT_UNDEFINED; // set in loop
  lImageViewCreate.components.r                    = VK_COMPONENT_SWIZZLE_R;
  lImageViewCreate.components.g                    = VK_COMPONENT_SWIZZLE_G;
  lImageViewCreate.components.b                    = VK_COMPONENT_SWIZZLE_B;
  lImageViewCreate.components.a                    = VK_COMPONENT_SWIZZLE_A;
  lImageViewCreate.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM; // loop
  lImageViewCreate.subresourceRange.baseMipLevel   = 0;
  lImageViewCreate.subresourceRange.levelCount     = 1;
  lImageViewCreate.subresourceRange.baseArrayLayer = 0;
  lImageViewCreate.subresourceRange.layerCount     = 1;

  for (auto &i : lAttachmentInfos) {
    VkAttachmentDescription *lAttachment = &vRenderPass_vk.attachments[i.attachmentID];
    VkClearValue *           lClearValue = &vRenderPass_vk.clearValues[i.attachmentID];

    lImageCreate.format = i.format;
    lImageCreate.tiling = i.tiling;
    lImageCreate.usage  = i.usage;

    lImageViewCreate.format                      = i.format;
    lImageViewCreate.subresourceRange.aspectMask = i.aspect;

    lAttachment->flags          = 0;
    lAttachment->format         = i.format;
    lAttachment->samples        = GlobConf.vk.samples;
    lAttachment->loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
    lAttachment->storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
    lAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    lAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    lAttachment->initialLayout  = i.layout;
    lAttachment->finalLayout    = i.layout;

    lClearValue->color = {
        {vClearColor.float32[0], vClearColor.float32[1], vClearColor.float32[2], vClearColor.float32[3]}};

    if (vHasStencilBuffer && i.usage == VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
      lAttachment->stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_CLEAR;
      lAttachment->stencilStoreOp = VK_ATTACHMENT_STORE_OP_STORE;
      lClearValue->depthStencil   = {1.0f, 0};
    }

    auto lRes = vkCreateImage(vDevice_vk, &lImageCreate, nullptr, &i.buff->img);
    if (lRes) {
      eLOG("'vkCreateImage' returned ", uEnum2Str::toStr(lRes));
      return 2;
    }

    vkGetImageMemoryRequirements(vDevice_vk, i.buff->img, &lRequirements);
    lMemoryAlloc.allocationSize  = lRequirements.size;
    lMemoryAlloc.memoryTypeIndex = vDevice->getMemoryTypeIndexFromBitfield(lRequirements.memoryTypeBits);

    if (lMemoryAlloc.memoryTypeIndex == UINT32_MAX) {
      eLOG("No valid memory type found!");
      return 3;
    }

    lRes = vkAllocateMemory(vDevice_vk, &lMemoryAlloc, nullptr, &i.buff->mem);
    if (lRes) {
      eLOG("'vkAllocateMemory' returned ", uEnum2Str::toStr(lRes));
      return 4;
    }

    lRes = vkBindImageMemory(vDevice_vk, i.buff->img, i.buff->mem, 0);
    if (lRes) {
      eLOG("'vkBindImageMemory' returned ", uEnum2Str::toStr(lRes));
      return 5;
    }

    lImageViewCreate.image = i.buff->img;
    lRes                   = vkCreateImageView(vDevice_vk, &lImageViewCreate, nullptr, &i.buff->iv);
    if (lRes) {
      eLOG("'vkCreateImageView' returned ", uEnum2Str::toStr(lRes));
      return 6;
    }

    vRenderPass_vk.attachmentViews[i.attachmentID]   = i.buff->iv;
    vRenderPass_vk.attachmentBuffers[i.attachmentID] = i.buff;

#if D_LOG_VULKAN
    dLOG("Creating Image buffer [renderer ", vID, "]");
    dLOG("  -- lHasStencil:     ", vHasStencilBuffer);
    dLOG("  -- imageType:       ", uEnum2Str::toStr(lImageCreate.imageType));
    dLOG("  -- usage:           ", uEnum2Str::toStr(static_cast<VkImageUsageFlagBits>(lImageCreate.usage)));
    dLOG("  -- fromat:          ", uEnum2Str::toStr(lImageCreate.format));
    dLOG("  -- tiling:          ", uEnum2Str::toStr(lImageCreate.tiling));
    dLOG("  -- memoryTypeIndex: ", lMemoryAlloc.memoryTypeIndex);
    dLOG("  -- extent:          ", lImageCreate.extent.width, "x", lImageCreate.extent.height);
    dLOG("  -- attachmentID:    ", i.attachmentID);
    dLOG("  -- image:           ", reinterpret_cast<uint64_t>(i.buff->img));
    dLOG("  -- image View:      ", reinterpret_cast<uint64_t>(i.buff->iv));
#endif

    vWorldPtr->cmdChangeImageLayout(
        _buf, lImageViewCreate.image, lImageViewCreate.subresourceRange, VK_IMAGE_LAYOUT_UNDEFINED, i.layout);
  }

  return 0;
}




/*!
 * \brief recreates the vulkan renderpass
 * \returns 0 on success
 */
int rRendererBase::initRenderPass() {
  if (!vDevice_vk)
    return 1;

  if (vRenderPass_vk.subpasses.empty()) {
    eLOG("Can not create a renderPass without at least one subpass!");
    return 2;
  }

#if D_LOG_VULKAN
  uint32_t lCounter = 0;
  dLOG("Creating a renderPass: [renderer ", vID, "]");
  dLOG("  -- attachments:");
  for (auto const &i : vRenderPass_vk.attachments) {
    dLOG("    -- attachment ", lCounter++);
    dLOG("      - flags:          ", i.flags);
    dLOG("      - format:         ", uEnum2Str::toStr(i.format));
    dLOG("      - samples:        ", uEnum2Str::toStr(i.samples));
    dLOG("      - loadOp:         ", uEnum2Str::toStr(i.loadOp));
    dLOG("      - storeOp:        ", uEnum2Str::toStr(i.storeOp));
    dLOG("      - stencilLoadOp:  ", uEnum2Str::toStr(i.stencilLoadOp));
    dLOG("      - stencilStoreOp: ", uEnum2Str::toStr(i.stencilStoreOp));
    dLOG("      - initialLayout:  ", uEnum2Str::toStr(i.initialLayout));
    dLOG("      - finalLayout:    ", uEnum2Str::toStr(i.finalLayout));
  }
  lCounter = 0;
  dLOG("  -- subpasses:");
  for (auto const &i : vRenderPass_vk.subpasses) {
    dLOG("    -- subpass ", lCounter++);
    dLOG("      - flags:             ", i.flags);
    dLOG("      - pipelineBindPoint: ", uEnum2Str::toStr(i.pipelineBindPoint));
    dLOG("      - inputAttachments:");
    for (uint32_t j = 0; j < i.inputAttachmentCount; j++) {
      auto lTemp = i.pInputAttachments[j];
      dLOG("       - ", lTemp.attachment, "; ", uEnum2Str::toStr(lTemp.layout));
    }
    dLOG("      - colorAttachments:");
    for (uint32_t j = 0; j < i.colorAttachmentCount; j++) {
      auto lTemp = i.pColorAttachments[j];
      dLOG("       - ", lTemp.attachment, "; ", uEnum2Str::toStr(lTemp.layout));
    }
    dLOG("      - resolveAttachments:");
    for (uint32_t j = 0; j < i.colorAttachmentCount; j++) {
      auto lTemp = i.pResolveAttachments[j];
      if (lTemp.attachment == VK_ATTACHMENT_UNUSED)
        continue;
      dLOG("        - ", lTemp.attachment, "; ", uEnum2Str::toStr(lTemp.layout));
    }
    auto lTemp = i.pDepthStencilAttachment;
    if (lTemp)
      dLOG("      - depthStencilAttachment: ", lTemp->attachment, "; ", uEnum2Str::toStr(lTemp->layout));
    dLOG("      - preserveAttachments:");
    for (uint32_t j = 0; j < i.preserveAttachmentCount; j++) {
      auto lTemp2 = i.pPreserveAttachments[j];
      dLOG("       - ", lTemp2);
    }
  }

  lCounter = 0;
  dLOG("  -- subpass dependecies:");
  for (auto const &i : vRenderPass_vk.dependecies) {
    dLOG("    -- dependecy ", lCounter++);
    dLOG("      - srcSubpass:      ", i.srcSubpass);
    dLOG("      - dstSubpass:      ", i.dstSubpass);
    dLOG("      - srcStageMask:    ", uEnum2Str::VkPipelineStageFlagBits_toStr(i.srcStageMask));
    dLOG("      - dstStageMask:    ", uEnum2Str::VkPipelineStageFlagBits_toStr(i.dstStageMask));
    dLOG("      - srcAccessMask:   ", uEnum2Str::VkAccessFlagBits_toStr(i.srcAccessMask));
    dLOG("      - dstAccessMask:   ", uEnum2Str::VkAccessFlagBits_toStr(i.dstAccessMask));
    dLOG("      - dependencyFlags: ", uEnum2Str::VkDependencyFlagBits_toStr(i.dependencyFlags));
  }
#endif

  VkRenderPassCreateInfo lInfo;
  lInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  lInfo.pNext           = nullptr;
  lInfo.flags           = 0;
  lInfo.attachmentCount = static_cast<uint32_t>(vRenderPass_vk.attachments.size());
  lInfo.pAttachments    = vRenderPass_vk.attachments.data();
  lInfo.subpassCount    = static_cast<uint32_t>(vRenderPass_vk.subpasses.size());
  lInfo.pSubpasses      = vRenderPass_vk.subpasses.data();
  lInfo.dependencyCount = static_cast<uint32_t>(vRenderPass_vk.dependecies.size());
  lInfo.pDependencies   = vRenderPass_vk.dependecies.data();

  auto lRes = vkCreateRenderPass(vDevice_vk, &lInfo, nullptr, &vRenderPass_vk.renderPass);
  if (lRes) {
    eLOG("'vkCreateRenderPass' returned ", uEnum2Str::toStr(lRes));
    return 3;
  }

  return 0;
}

int rRendererBase::initFramebuffers() {
  auto     lTempViews = vWorldPtr->getSwapchainImageViews();
  uint32_t lCounter   = 0;

  for (auto i : lTempViews) {
    vFramebuffers_vk.emplace_back();
    vFramebuffers_vk.back().index = lCounter++;
    vFramebuffers_vk.back().img   = i.img;
    vFramebuffers_vk.back().iv    = i.iv;
  }

  if (vRenderPass_vk.attachmentViews.size() != vRenderPass_vk.attachments.size()) {
    eLOG("Invalid number of framebuffer attachments");
    return 2;
  }

  VkFramebufferCreateInfo lInfo;
  lInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  lInfo.pNext           = nullptr;
  lInfo.flags           = 0;
  lInfo.renderPass      = vRenderPass_vk.renderPass;
  lInfo.attachmentCount = static_cast<uint32_t>(vRenderPass_vk.attachmentViews.size());
  lInfo.pAttachments    = vRenderPass_vk.attachmentViews.data();
  lInfo.width           = GlobConf.win.width;
  lInfo.height          = GlobConf.win.height;
  lInfo.layers          = 1;

  dVkLOG("Creating framebuffers: [renderer ", vID, "]");
  dVkLOG("  -- extent: ", lInfo.width, "x", lInfo.height);

  for (auto &i : vFramebuffers_vk) {
    vRenderPass_vk.attachmentViews[FRAMEBUFFER_ATTACHMENT_INDEX] = i.iv;

    auto lRes = vkCreateFramebuffer(vDevice_vk, &lInfo, nullptr, &i.fb);
    if (lRes) {
      eLOG("'vkCreateFramebuffer' returned ", uEnum2Str::toStr(lRes));
      return 1;
    }
  }
  return 0;
}
}
}
