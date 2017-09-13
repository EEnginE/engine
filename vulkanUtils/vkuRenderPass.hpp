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
#include <vector>
#include <vulkan.h>

namespace e_engine {

/*!
 * \brief Container for a vulkan render pass
 *
 * A render pass represents a collection of attachments, subpasses, and dependencies between the
 * subpasses, and describes how the attachments are used over the course of the subpasses. The use of
 * a render pass in a command buffer is a render pass instance.
 *
 * It consists of at multiple (least one) subpasses, which represents a rendering phase.
 *
 * The render pass itself only describes the attachments that will be used. The attachments itself are stored in
 * a compatible framebufer.
 *
 * \todo Support VkRenderPassMultiviewKHX
 *
 * \note All objects of this class must be destroyed BEFORE the vulkan device is destroyed
 * \warning This class does no synchronisation
 */
class vkuRenderPass final {
 public:
  struct SubpassDescription {
    VkSubpassDescriptionFlags          flags;
    VkPipelineBindPoint                pipelineBindPoint;
    std::vector<VkAttachmentReference> inputAttachments;
    std::vector<VkAttachmentReference> colorAttachments;
    std::vector<VkAttachmentReference> resolveAttachments;
    VkAttachmentReference              depthStencilAttachment = {VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED};
    std::vector<uint32_t>              preserveAttachments;
  };

  struct Config {
    VkRenderPassCreateInfo               createInfo;
    std::vector<VkAttachmentDescription> attachments;
    std::vector<SubpassDescription>      subpasses;
    std::vector<VkSubpassDependency>     dependencies;
  };

 private:
  VkRenderPass vRenderPass = VK_NULL_HANDLE;
  VkDevice     vDevice     = VK_NULL_HANDLE;

  Config cfg;

 public:
  vkuRenderPass() : vkuRenderPass(VK_NULL_HANDLE) {}
  vkuRenderPass(VkDevice _device);
  ~vkuRenderPass();

  vkuRenderPass(vkuRenderPass const &) = delete;
  vkuRenderPass(vkuRenderPass &&)      = delete;

  vkuRenderPass &operator=(const vkuRenderPass &) = delete;
  vkuRenderPass &operator=(vkuRenderPass &&) = delete;

  VkResult init(VkDevice _device = VK_NULL_HANDLE) noexcept;
  void destroy() noexcept;

  void resetConfig() noexcept;
  void addAttachment(VkAttachmentDescription _attachment) noexcept;
  void addSubpass(SubpassDescription _subpass) noexcept;
  void addDependency(VkSubpassDependency _dependency) noexcept;

  inline VkRenderPass get() const noexcept { return vRenderPass; }
  inline VkDevice     getDevice() const noexcept { return vDevice; }
  inline Config       getConfig() const noexcept { return cfg; }
  inline bool         isCreated() const noexcept { return vRenderPass != VK_NULL_HANDLE; }

  inline VkRenderPass operator*() const noexcept { return vRenderPass; }

  inline bool operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};
}
