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

vkuRenderPass::vkuRenderPass(VkDevice _device) {
  vDevice = _device;

  cfg.createInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  cfg.createInfo.pNext           = nullptr;
  cfg.createInfo.flags           = 0;
  cfg.createInfo.attachmentCount = 0;
  cfg.createInfo.pAttachments    = nullptr;
  cfg.createInfo.subpassCount    = 0;
  cfg.createInfo.pSubpasses      = nullptr;
  cfg.createInfo.dependencyCount = 0;
  cfg.createInfo.pDependencies   = nullptr;
}


vkuRenderPass::~vkuRenderPass() { destroy(); }

/*!
 * \brief Creates the render pass
 * \param _device The device to use
 *
 * This function will do nothing if the render pass is already created.
 * The object can no longer be configured after calling this function.
 *
 * \note If _device is VK_NULL_HANDLE the last valid device will be used
 */
VkResult vkuRenderPass::init(VkDevice _device) noexcept {
  if (isCreated()) {
    wLOG(L"Render pass already created");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (_device != VK_NULL_HANDLE)
    vDevice = _device;

  if (vDevice == VK_NULL_HANDLE) {
    eLOG("vDevice is VK_NULL_HANDLE");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  std::vector<VkSubpassDescription> lSubpasses;
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

  cfg.createInfo.attachmentCount = static_cast<uint32_t>(cfg.attachments.size());
  cfg.createInfo.pAttachments    = cfg.attachments.data();
  cfg.createInfo.subpassCount    = static_cast<uint32_t>(lSubpasses.size());
  cfg.createInfo.pSubpasses      = lSubpasses.data();
  cfg.createInfo.dependencyCount = static_cast<uint32_t>(cfg.dependencies.size());
  cfg.createInfo.pDependencies   = cfg.dependencies.data();


#if D_LOG_VULKAN_UTILS
  uint32_t lCounter = 0;
  dLOG(L"Initializing Render Pass with config:");
  dLOG(L"  -- attachments:");
  for (auto const &i : cfg.attachments) {
    dLOG(L"    -- attachment ", lCounter++);
    dLOG(L"      - flags:          ", i.flags);
    dLOG(L"      - format:         ", uEnum2Str::toStr(i.format));
    dLOG(L"      - samples:        ", uEnum2Str::toStr(i.samples));
    dLOG(L"      - loadOp:         ", uEnum2Str::toStr(i.loadOp));
    dLOG(L"      - storeOp:        ", uEnum2Str::toStr(i.storeOp));
    dLOG(L"      - stencilLoadOp:  ", uEnum2Str::toStr(i.stencilLoadOp));
    dLOG(L"      - stencilStoreOp: ", uEnum2Str::toStr(i.stencilStoreOp));
    dLOG(L"      - initialLayout:  ", uEnum2Str::toStr(i.initialLayout));
    dLOG(L"      - finalLayout:    ", uEnum2Str::toStr(i.finalLayout));
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

  auto lRes = vkCreateRenderPass(vDevice, &cfg.createInfo, nullptr, &vRenderPass);
  if (lRes != VK_SUCCESS) {
    vRenderPass = VK_NULL_HANDLE;
    eLOG(L"Failed to create render pass: vkCreateRenderPass returned ", uEnum2Str::toStr(lRes));
  }

  return lRes;
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

  vkDestroyRenderPass(vDevice, vRenderPass, nullptr);
  vRenderPass = VK_NULL_HANDLE;
}

/*!
 * \brief Adds an attachment to the render pass
 * \param _attachment The attachment to add
 *
 * \note This function does nothing when the render pass is already created
 */
void vkuRenderPass::addAttachment(VkAttachmentDescription _attachment) noexcept {
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
