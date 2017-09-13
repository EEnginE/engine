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
#include "vkuFrameBuffer.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"

using namespace e_engine;

vkuFrameBuffer::~vkuFrameBuffer() { destroy(); }

vkuFrameBuffer::vkuFrameBuffer(vkuFrameBuffer &&_old) {
  vFrameBuffer  = _old.vFrameBuffer;
  vRenderPass   = _old.vRenderPass;
  vDevice       = _old.vDevice;
  vAttachments  = _old.vAttachments;
  vImageBuffers = std::move(_old.vImageBuffers);

  // Invalidate old object
  _old.vFrameBuffer = VK_NULL_HANDLE;
  _old.vRenderPass  = VK_NULL_HANDLE;
  _old.vDevice      = nullptr;

  _old.vAttachments.clear();
  _old.vImageBuffers.clear();
}

vkuFrameBuffer &vkuFrameBuffer::operator=(vkuFrameBuffer &&_old) {
  vFrameBuffer  = _old.vFrameBuffer;
  vRenderPass   = _old.vRenderPass;
  vDevice       = _old.vDevice;
  vAttachments  = _old.vAttachments;
  vImageBuffers = std::move(_old.vImageBuffers);

  // Invalidate old object
  _old.vFrameBuffer = VK_NULL_HANDLE;
  _old.vRenderPass  = VK_NULL_HANDLE;
  _old.vDevice      = nullptr;

  _old.vAttachments.clear();
  _old.vImageBuffers.clear();

  return *this;
}

/*!
 * \brief Sets the format of the framebuffer for a specific render pass
 * \param _renderPass The render pass to use
 */
bool vkuFrameBuffer::setup(vkuRenderPass &_renderPass) {
  if (!_renderPass) {
    eLOG(L"Render pass is not created");
    return false;
  }

  if (isCreated()) {
    eLOG(L"Can not chanche framebuffer serup, while framebuffers are created");
    return false;
  }

  vDevice      = _renderPass.getDevice();
  vRenderPass  = *_renderPass;
  vAttachments = _renderPass.getConfig().attachments;

  return true;
}

/*!
 * \brief (Re)creates the framebuffer(s) based on a (new) configuration
 * \param _cfg The new configuration to use
 *
 * \note The configuration MUST be compatible with the renderpass attachment description
 */
VkResult vkuFrameBuffer::reCreateFrameBuffers(vkuFrameBuffer::Config _cfg) {
  // Error checking
  if (vRenderPass == VK_NULL_HANDLE) {
    eLOG(L"Framebuffer is not serup yet. Call setup() first");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  // Destroying old stuff
  if (vFrameBuffer != VK_NULL_HANDLE)
    vkDestroyFramebuffer(**vDevice, vFrameBuffer, nullptr);

  vImageBuffers.clear();

  // Creating new framebuffer
  std::vector<VkImageView> lImageViews;
  lImageViews.resize(vAttachments.size(), VK_NULL_HANDLE);

  for (auto &i : _cfg.data) {
    if (i.attachmentID >= vAttachments.size()) {
      wLOG(L"Invalid attachmentID ", i.attachmentID, " (value to big)");
      continue;
    }

    if (!vAttachments[i.attachmentID].imageCfg.useExternalImageView) {
      wLOG(L"External views not supported for attachment ", i.attachmentID);
      continue;
    }

    lImageViews[i.attachmentID] = i.view;
  }

  for (size_t i = 0; i < vAttachments.size(); ++i) {
    auto &conf = vAttachments[i];

    if (conf.imageCfg.useExternalImageView)
      continue;

    vkuImageBuffer &buff   = *vImageBuffers.emplace(vDevice);
    buff->type             = conf.bufferCreateCfg.type;
    buff->format           = conf.desc.format;
    buff->extent           = _cfg.size;
    buff->mipLevels        = conf.bufferCreateCfg.mipLevels;
    buff->arrayLayers      = conf.bufferCreateCfg.arrayLayers;
    buff->samples          = conf.desc.samples;
    buff->tiling           = conf.bufferCreateCfg.tiling;
    buff->usage            = conf.bufferCreateCfg.usage;
    buff->initialLayout    = conf.desc.initialLayout;
    buff->sharingMode      = conf.bufferCreateCfg.sharingMode;
    buff->subresourceRange = conf.bufferCreateCfg.subresourceRange;
    buff->components       = conf.bufferCreateCfg.components;
    buff->imageCreateFlags = conf.bufferCreateCfg.imageCreateFlags;

    if (buff.init() != VK_SUCCESS) {
      eLOG(L"Failed to create image buffer --> can not create framebuffer");
      vImageBuffers.clear();
      return VK_ERROR_INITIALIZATION_FAILED;
    }

    lImageViews[i] = *buff;
  }

  for (auto i : lImageViews) {
    if (i == VK_NULL_HANDLE) {
      vImageBuffers.clear();
      eLOG(L"Not all image views are set --> can not create framebuffer");
      return VK_ERROR_INITIALIZATION_FAILED;
    }
  }

  VkFramebufferCreateInfo lCreateInfo;
  lCreateInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
  lCreateInfo.pNext           = nullptr;
  lCreateInfo.flags           = 0;
  lCreateInfo.renderPass      = vRenderPass;
  lCreateInfo.attachmentCount = static_cast<uint32_t>(lImageViews.size());
  lCreateInfo.pAttachments    = lImageViews.data();
  lCreateInfo.width           = _cfg.size.width;
  lCreateInfo.height          = _cfg.size.height;
  lCreateInfo.layers          = _cfg.layers;

  auto lRes = vkCreateFramebuffer(**vDevice, &lCreateInfo, nullptr, &vFrameBuffer);

  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to create framebuffer: vkCreateFramebuffer returened ", uEnum2Str::toStr(lRes));
    vFrameBuffer = VK_NULL_HANDLE;
  }

  return lRes;
}



void vkuFrameBuffer::destroy() {
  if (!isCreated())
    return;

  vkDestroyFramebuffer(**vDevice, vFrameBuffer, nullptr);

  vFrameBuffer = VK_NULL_HANDLE;
  vRenderPass  = VK_NULL_HANDLE;
  vDevice      = nullptr;
}
