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
#include "vkuImageBuffer.hpp"
#include "vkuRenderPass.hpp"
#include <plf_colony.h>
#include <vulkan.h>

namespace e_engine {

/*!
 * \brief Container for a vulkan framebuffer
 *
 * The framebuffer manages the actual, specific resources described in a render pass.
 *
 * Therefore, a framebuffer is (in general) only compatible with the render pass it was created for.
 *
 * \todo support multiview
 */
class vkuFrameBuffer {
 public:
  typedef vkuRenderPass::AttachmentDescription AttachmentDesc;

  struct Config {
    VkExtent3D size; //!< Size of the framebuffer

    struct AttachmentData {
      uint32_t attachmentID = 0;

      /*!
       * \brief Stores the image views to use when useExternalImageView is true in the attachment description
       */
      VkImageView view = VK_NULL_HANDLE;
    };

    std::vector<AttachmentData> data = {}; //!< \brief Image views for the new framebuffer, provided externally

    uint32_t layers = 1; //!< The number of layers for multiview
  };

 private:
  VkFramebuffer vFrameBuffer = VK_NULL_HANDLE;
  VkRenderPass  vRenderPass  = VK_NULL_HANDLE;
  vkuDevicePTR  vDevice      = nullptr;

  std::vector<AttachmentDesc> vAttachments;
  plf::colony<vkuImageBuffer> vImageBuffers; // Can not use vector because vkuImageBuffer can not be copied or moved

 public:
  vkuFrameBuffer() = default;
  ~vkuFrameBuffer();

  vkuFrameBuffer(vkuFrameBuffer const &) = delete;
  vkuFrameBuffer &operator=(const vkuFrameBuffer &) = delete;

  vkuFrameBuffer(vkuFrameBuffer &&);
  vkuFrameBuffer &operator=(vkuFrameBuffer &&);

  bool setup(vkuRenderPass &_renderPass);
  VkResult reCreateFrameBuffers(Config _cfg);
  void destroy();

  inline VkRenderPass getRenderPass() const noexcept { return vRenderPass; }
  inline vkuDevicePTR getDevice() const noexcept { return vDevice; }
  inline bool         isCreated() const noexcept { return vFrameBuffer != VK_NULL_HANDLE; }

  inline VkFramebuffer get() const noexcept { return vFrameBuffer; }
  inline VkFramebuffer operator*() const noexcept { return vFrameBuffer; }

  inline bool operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};
}
