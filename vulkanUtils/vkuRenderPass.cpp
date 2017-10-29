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
#include "vkuRenderPass.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"

using namespace e_engine;

vkuRenderPass::vkuRenderPass(vkuDevicePTR _device) {
  vDevice = _device;

  vCreateInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  vCreateInfo.pNext           = nullptr;
  vCreateInfo.flags           = 0;
  vCreateInfo.attachmentCount = 0;
  vCreateInfo.pAttachments    = nullptr;
  vCreateInfo.subpassCount    = 0;
  vCreateInfo.pSubpasses      = nullptr;
  vCreateInfo.dependencyCount = 0;
  vCreateInfo.pDependencies   = nullptr;
}


vkuRenderPass::~vkuRenderPass() { destroy(); }

vkuRenderPass::vkuRenderPass(vkuRenderPass &&_old) {
  vRenderPass = _old.vRenderPass;
  vDevice     = _old.vDevice;
  cfg         = _old.cfg;
  vCreateInfo = _old.vCreateInfo;

  // Invalidate old object
  _old.vRenderPass = VK_NULL_HANDLE;
  _old.vDevice     = nullptr;
}

vkuRenderPass &vkuRenderPass::operator=(vkuRenderPass &&_old) {
  destroy(); // Destroy old renderpass

  vRenderPass = _old.vRenderPass;
  vDevice     = _old.vDevice;
  cfg         = _old.cfg;
  vCreateInfo = _old.vCreateInfo;

  // Invalidate old object
  _old.vRenderPass = VK_NULL_HANDLE;
  _old.vDevice     = nullptr;

  return *this;
}

/*!
 * \brief Creates the render pass
 * \param _device The device to use
 *
 * This function will do nothing if the render pass is already created.
 * The object can no longer be configured after calling this function.
 *
 * \note If _device is VK_NULL_HANDLE the last valid device will be used
 */
VkResult vkuRenderPass::init(vkuDevicePTR _device) noexcept {
  if (isCreated()) {
    wLOG(L"Render pass already created");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (_device)
    vDevice = _device;

  if (!vDevice) {
    eLOG("vDevice is VK_NULL_HANDLE");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  std::vector<VkAttachmentDescription> lAttachments;
  std::vector<VkSubpassDescription>    lSubpasses;
  lSubpasses.resize(cfg.subpasses.size());
  for (size_t i = 0; i < cfg.subpasses.size(); ++i) {
    lSubpasses[i].flags                   = cfg.subpasses[i].flags;
    lSubpasses[i].pipelineBindPoint       = cfg.subpasses[i].pipelineBindPoint;
    lSubpasses[i].inputAttachmentCount    = static_cast<uint32_t>(cfg.subpasses[i].inputAttachments.size());
    lSubpasses[i].pInputAttachments       = cfg.subpasses[i].inputAttachments.data();
    lSubpasses[i].colorAttachmentCount    = static_cast<uint32_t>(cfg.subpasses[i].colorAttachments.size());
    lSubpasses[i].pColorAttachments       = cfg.subpasses[i].colorAttachments.data();
    lSubpasses[i].preserveAttachmentCount = static_cast<uint32_t>(cfg.subpasses[i].preserveAttachments.size());
    lSubpasses[i].pPreserveAttachments    = cfg.subpasses[i].preserveAttachments.data();
    lSubpasses[i].pDepthStencilAttachment = &cfg.subpasses[i].depthStencilAttachment;

    if (cfg.subpasses[i].resolveAttachments.empty()) {
      lSubpasses[i].pResolveAttachments = nullptr;
    } else {
      lSubpasses[i].pResolveAttachments = cfg.subpasses[i].resolveAttachments.data();
    }
  }

  for (auto &i : cfg.attachments) {
    lAttachments.push_back(i.desc);
  }


  vCreateInfo.attachmentCount = static_cast<uint32_t>(lAttachments.size());
  vCreateInfo.pAttachments    = lAttachments.data();
  vCreateInfo.subpassCount    = static_cast<uint32_t>(lSubpasses.size());
  vCreateInfo.pSubpasses      = lSubpasses.data();
  vCreateInfo.dependencyCount = static_cast<uint32_t>(cfg.dependencies.size());
  vCreateInfo.pDependencies   = cfg.dependencies.data();


#if D_LOG_VULKAN_UTILS
  uint32_t lCounter = 0;
  dLOG(L"Initializing Render Pass with config:");
  dLOG(L"  -- attachments:");
  for (auto const &i : cfg.attachments) {
    dLOG(L"    -- attachment ", lCounter++);
    dLOG(L"      - flags:             ", uEnum2Str::VkAttachmentDescriptionFlagBits_toStr(i.desc.flags));
    dLOG(L"      - format:            ", uEnum2Str::toStr(i.desc.format));
    dLOG(L"      - samples:           ", uEnum2Str::toStr(i.desc.samples));
    dLOG(L"      - loadOp:            ", uEnum2Str::toStr(i.desc.loadOp));
    dLOG(L"      - storeOp:           ", uEnum2Str::toStr(i.desc.storeOp));
    dLOG(L"      - stencilLoadOp:     ", uEnum2Str::toStr(i.desc.stencilLoadOp));
    dLOG(L"      - stencilStoreOp:    ", uEnum2Str::toStr(i.desc.stencilStoreOp));
    dLOG(L"      - initialLayout:     ", uEnum2Str::toStr(i.desc.initialLayout));
    dLOG(L"      - finalLayout:       ", uEnum2Str::toStr(i.desc.finalLayout));
    dLOG(L"      - Image create config:");
    dLOG(L"        - mipLevels:        ", i.bufferCreateCfg.mipLevels);
    dLOG(L"        - arrayLayers:      ", i.bufferCreateCfg.arrayLayers);
    dLOG(L"        - tiling:           ", uEnum2Str::toStr(i.bufferCreateCfg.tiling));
    dLOG(L"        - usage:            ", uEnum2Str::VkImageUsageFlagBits_toStr(i.bufferCreateCfg.usage));
    dLOG(L"        - sharingMode:      ", uEnum2Str::toStr(i.bufferCreateCfg.sharingMode));
    dLOG(L"        - imageCreateFlags: ", uEnum2Str::VkImageCreateFlagBits_toStr(i.bufferCreateCfg.imageCreateFlags));
  }
  lCounter = 0;
  dLOG(L"  -- subpasses:");
  for (auto const &i : cfg.subpasses) {
    dLOG(L"    -- subpass ", lCounter++);
    dLOG(L"      - flags:             ", i.flags);
    dLOG(L"      - pipelineBindPoint: ", uEnum2Str::toStr(i.pipelineBindPoint));
    dLOG(L"      - inputAttachments:");
    for (auto const &j : i.inputAttachments) {
      dLOG(L"       - ", j.attachment, L"; ", uEnum2Str::toStr(j.layout));
    }
    dLOG(L"      - colorAttachments:");
    for (auto const &j : i.colorAttachments) {
      dLOG(L"       - ", j.attachment, L"; ", uEnum2Str::toStr(j.layout));
    }
    dLOG(L"      - resolveAttachments:");
    for (auto const &j : i.resolveAttachments) {
      dLOG(L"        - ", j.attachment, L"; ", uEnum2Str::toStr(j.layout));
    }
    auto lTemp = i.depthStencilAttachment;
    dLOG(L"      - depthStencilAttachment: ", lTemp.attachment, L"; ", uEnum2Str::toStr(lTemp.layout));
    dLOG(L"      - preserveAttachments:");
    for (auto j : i.preserveAttachments) {
      dLOG(L"       - ", j);
    }
  }

  lCounter = 0;
  dLOG(L"  -- subpass dependencies:");
  for (auto const &i : cfg.dependencies) {
    std::wstring src = i.srcSubpass == VK_SUBPASS_EXTERNAL ? L"VK_SUBPASS_EXTERNAL" : std::to_wstring(i.srcSubpass);
    std::wstring dst = i.dstSubpass == VK_SUBPASS_EXTERNAL ? L"VK_SUBPASS_EXTERNAL" : std::to_wstring(i.dstSubpass);

    dLOG(L"    -- dependecy ", lCounter++);
    dLOG(L"      - srcSubpass:      ", src);
    dLOG(L"      - dstSubpass:      ", dst);
    dLOG(L"      - srcStageMask:    ", uEnum2Str::VkPipelineStageFlagBits_toStr(i.srcStageMask));
    dLOG(L"      - dstStageMask:    ", uEnum2Str::VkPipelineStageFlagBits_toStr(i.dstStageMask));
    dLOG(L"      - srcAccessMask:   ", uEnum2Str::VkAccessFlagBits_toStr(i.srcAccessMask));
    dLOG(L"      - dstAccessMask:   ", uEnum2Str::VkAccessFlagBits_toStr(i.dstAccessMask));
    dLOG(L"      - dependencyFlags: ", uEnum2Str::VkDependencyFlagBits_toStr(i.dependencyFlags));
  }
#endif

  auto lRes = vkCreateRenderPass(**vDevice, &vCreateInfo, nullptr, &vRenderPass);
  if (lRes != VK_SUCCESS) {
    vRenderPass = VK_NULL_HANDLE;
    eLOG(L"Failed to create render pass: vkCreateRenderPass returned ", uEnum2Str::toStr(lRes));
  }

  return lRes;
}

/*!
 * \brief Genreates an image for a specific attatchment
 * \param _desc The attachment description
 * \param _size The required image size
 */
vkuImageBuffer vkuRenderPass::generateImageBufferFromAttachment(AttachmentDescription _desc, VkExtent3D _size) {
  vkuImageBuffer buff(vDevice);
  buff->type             = _desc.bufferCreateCfg.type;
  buff->format           = _desc.desc.format;
  buff->extent           = _size;
  buff->mipLevels        = _desc.bufferCreateCfg.mipLevels;
  buff->arrayLayers      = _desc.bufferCreateCfg.arrayLayers;
  buff->samples          = _desc.desc.samples;
  buff->tiling           = _desc.bufferCreateCfg.tiling;
  buff->usage            = _desc.bufferCreateCfg.usage;
  buff->startLayout      = _desc.desc.initialLayout;
  buff->sharingMode      = _desc.bufferCreateCfg.sharingMode;
  buff->subresourceRange = _desc.bufferCreateCfg.subresourceRange;
  buff->components       = _desc.bufferCreateCfg.components;
  buff->imageCreateFlags = _desc.bufferCreateCfg.imageCreateFlags;

  auto lRet = buff.init();
  if (lRet != VK_SUCCESS) {
    eLOG(L"Failed to create image buffer (return code ", uEnum2Str::toStr(lRet), ")");
  }

  return buff;
}

/*!
 * \brief Genreates an image for a specific attatchment
 * \param _attachmentID The ID / index of the attachment
 * \param _size         The required image size
 *
 * \note This is a wrapper for generateImageBufferFromAttachment()
 */
vkuImageBuffer vkuRenderPass::generateImageBufferFromAttachment(uint32_t _attachmentID, VkExtent3D _size) {
  if (_attachmentID >= cfg.attachments.size()) {
    eLOG(L"Invlaid attachment ID ", _attachmentID);
    return vkuImageBuffer();
  }

  return generateImageBufferFromAttachment(cfg.attachments[_attachmentID], _size);
}

/*!
 * \brief Destroys the current render pass
 *
 * After calling this function the configuration functions may be called.
 *
 * \note This function will NOT reset the configuration (use resetConfig())
 */
void vkuRenderPass::destroy() noexcept {
  if (!(*this))
    return;

  vkDestroyRenderPass(**vDevice, vRenderPass, nullptr);
  vRenderPass = VK_NULL_HANDLE;
}

/*!
 * \brief Adds an attachment to the render pass
 * \param _attachment The attachment to add
 *
 * \note This function does nothing when the render pass is already created
 */
void vkuRenderPass::addAttachment(AttachmentDescription _attachment) noexcept {
  if (isCreated()) {
    wLOG(L"Render pass can NOT be configured after initialization");
    return;
  }

  cfg.attachments.push_back(_attachment);
}

/*!
 * \brief Adds a subpass to the render pass
 * \param _subpass The subpass to add
 *
 * \note This function does nothing when the render pass is already created
 */
void vkuRenderPass::addSubpass(vkuRenderPass::SubpassDescription _subpass) noexcept {
  if (isCreated()) {
    wLOG(L"Render pass can NOT be configured after initialization");
    return;
  }

  cfg.subpasses.push_back(_subpass);
}

/*!
 * \brief Adds a subpass dependency to the render pass
 * \param _dependency The subpass dependency to add
 *
 * \note This function does nothing when the render pass is already created
 */
void vkuRenderPass::addDependency(VkSubpassDependency _dependency) noexcept {
  if (isCreated()) {
    wLOG(L"Render pass can NOT be configured after initialization");
    return;
  }

  cfg.dependencies.push_back(_dependency);
}

/*!
 * \brief Resets / clears the current configuration
 *
 * \note This function does nothing when the render pass is already created
 */
void vkuRenderPass::resetConfig() noexcept {
  if (isCreated()) {
    wLOG(L"Render pass can NOT be configured after initialization");
    return;
  }

  cfg.attachments.clear();
  cfg.subpasses.clear();
  cfg.dependencies.clear();
}

/*!
 * \brief Sets all config options
 *
 * This will REPLACE the current configuration
 *
 * \note This function does nothing when the render pass is already created
 */
void vkuRenderPass::setup(Config _newCfg) noexcept {
  if (isCreated()) {
    wLOG(L"Render pass can NOT be configured after initialization");
    return;
  }

  cfg = _newCfg;
}


std::vector<VkClearValue> vkuRenderPass::getClearValues() {
  std::vector<VkClearValue> lValues;
  lValues.reserve(cfg.attachments.size());
  for (auto &i : cfg.attachments)
    lValues.push_back(i.clear);

  return lValues;
}
