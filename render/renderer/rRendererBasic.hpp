/*!
 * \file rRendererBasic.hpp
 * \brief \b Classes: \a rRendererBasic
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

#pragma once

#include "defines.hpp"
#include "vkuCommandBuffer.hpp"
#include "vkuFrameBuffer.hpp"
#include "vkuImageBuffer.hpp"
#include "vkuRenderPass.hpp"
#include "rRendererBase.hpp"

namespace e_engine {

class rRendererBasic final : public rRendererBase {
  struct FB_DATA {
    std::vector<vkuCommandBuffer> buffers;
    vkuFrameBuffer                frameBuffer;
  };

 private:
  std::vector<FB_DATA> vFbData;

  vkuRenderPass  vRenderPass;
  vkuImageBuffer vDepthBuffer;

  OBJECTS vRenderObjects;

  vkuRenderPass::Config getRenderPassDescription(VkSurfaceFormatKHR _surfaceFormat);

 protected:
  VkResult initRenderer(std::vector<VkImageView> _images, VkSurfaceFormatKHR _surfaceFormat) override;
  void     destroyRenderer() override;

  void recordCmdBuffers(Framebuffer_vk &_fb, RECORD_TARGET _toRender) override;

  VkRenderPass              getRenderPass() override { return *vRenderPass; }
  VkFramebuffer             getFrameBuffer(uint32_t _fbIndex) override { return *vFbData[_fbIndex].frameBuffer; }
  std::vector<VkClearValue> getClearValues() override { return vRenderPass.getClearValues(); }

  void initCmdBuffers(vkuCommandPool *_pool) override;
  void freeCmdBuffers() override;

 public:
  static const uint32_t DEPTH_STENCIL_ATTACHMENT_INDEX = FIRST_FREE_ATTACHMENT_INDEX + 0;

  VkImageView getAttachmentView(ATTACHMENT_ROLE _role) override;

  rRendererBasic() = delete;
  rRendererBasic(rWorld *_root, std::wstring _id) : rRendererBase(_root, _id) {}
};
} // namespace e_engine
