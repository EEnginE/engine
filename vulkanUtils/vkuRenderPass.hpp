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
#include "vkuDevice.hpp"
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
  struct AttachmentDescription {
    VkAttachmentDescription desc = {
        0,                                // flags
        VK_FORMAT_R8G8B8A8_UNORM,         // format
        VK_SAMPLE_COUNT_1_BIT,            // samples
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // loadOp
        VK_ATTACHMENT_STORE_OP_DONT_CARE, // storeOp
        VK_ATTACHMENT_LOAD_OP_DONT_CARE,  // stencilLoadOp
        VK_ATTACHMENT_STORE_OP_DONT_CARE, // stencilStoreOp
        VK_IMAGE_LAYOUT_UNDEFINED,        // initialLayout
        VK_IMAGE_LAYOUT_UNDEFINED         // finalLayout
    };

    VkClearValue clear = {{{0.0f, 0.0f, 0.0f, 1.0f}}};

    struct {
      VkImageType             type             = VK_IMAGE_TYPE_2D;
      uint32_t                mipLevels        = 1;
      uint32_t                arrayLayers      = 1;
      VkImageTiling           tiling           = VK_IMAGE_TILING_OPTIMAL;
      VkImageUsageFlags       usage            = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
      VkSharingMode           sharingMode      = VK_SHARING_MODE_EXCLUSIVE;
      VkImageSubresourceRange subresourceRange = {0, 0, 1, 0, 1};
      VkComponentMapping      components       = {
          VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};

      VkImageCreateFlags imageCreateFlags = 0;
    } bufferCreateCfg; //!< Will only be used when useExternalImageView is false

    struct {
      bool useExternalImageView = false; //!< The image buffer wont be automatically created and must be provided
    } imageCfg;
  };

  struct SubpassDescription {
    VkSubpassDescriptionFlags          flags             = 0;
    VkPipelineBindPoint                pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    std::vector<VkAttachmentReference> inputAttachments;
    std::vector<VkAttachmentReference> colorAttachments;
    std::vector<VkAttachmentReference> resolveAttachments;
    VkAttachmentReference              depthStencilAttachment = {VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED};
    std::vector<uint32_t>              preserveAttachments;
  };

  struct Config {
    std::vector<AttachmentDescription> attachments;
    std::vector<SubpassDescription>    subpasses;
    std::vector<VkSubpassDependency>   dependencies;
  };

 private:
  VkRenderPass vRenderPass = VK_NULL_HANDLE;
  vkuDevicePTR vDevice     = nullptr;

  Config                 cfg;
  VkRenderPassCreateInfo vCreateInfo;


 public:
  vkuRenderPass() : vkuRenderPass(nullptr) {}
  vkuRenderPass(vkuDevicePTR _device);
  ~vkuRenderPass();

  vkuRenderPass(vkuRenderPass const &) = delete;
  vkuRenderPass &operator=(const vkuRenderPass &) = delete;

  vkuRenderPass(vkuRenderPass &&);
  vkuRenderPass &operator=(vkuRenderPass &&);

  VkResult init(vkuDevicePTR _device = nullptr) noexcept;
  void destroy() noexcept;

  void resetConfig() noexcept;
  void addAttachment(AttachmentDescription _attachment) noexcept;
  void addSubpass(SubpassDescription _subpass) noexcept;
  void addDependency(VkSubpassDependency _dependency) noexcept;

  void setup(Config _newCfg) noexcept;

  std::vector<VkClearValue> getClearValues();

  inline VkRenderPass get() const noexcept { return vRenderPass; }
  inline vkuDevicePTR getDevice() const noexcept { return vDevice; }
  inline Config       getConfig() const noexcept { return cfg; }
  inline bool         isCreated() const noexcept { return vRenderPass != VK_NULL_HANDLE; }

  inline VkRenderPass operator*() const noexcept { return vRenderPass; }

  inline bool operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};
}
