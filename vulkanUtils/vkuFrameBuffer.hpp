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
#include "vkuRenderPass.hpp"
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
 private:
  VkFramebuffer vFrameBuffer = VK_NULL_HANDLE;
  VkDevice      vDevice      = VK_NULL_HANDLE;
  VkRenderPass  vRenderPass  = VK_NULL_HANDLE;

 public:
  vkuFrameBuffer() : vkuFrameBuffer(VK_NULL_HANDLE) {}
  vkuFrameBuffer(VkDevice _device);
  ~vkuFrameBuffer();

  vkuFrameBuffer(vkuFrameBuffer const &) = delete;
  vkuFrameBuffer(vkuFrameBuffer &&)      = delete;

  vkuFrameBuffer &operator=(const vkuFrameBuffer &) = delete;
  vkuFrameBuffer &operator=(vkuFrameBuffer &&) = delete;

  VkResult init(vkuRenderPass &_renderPass);
  void destroy();

  inline VkFramebuffer get() const noexcept { return vFrameBuffer; }
  inline VkRenderPass  getRenderPass() const noexcept { return vRenderPass; }
  inline VkDevice      getDevice() const noexcept { return vDevice; }
  inline bool          isCreated() const noexcept { return vFrameBuffer != VK_NULL_HANDLE; }

  inline VkFramebuffer operator*() const noexcept { return vFrameBuffer; }

  inline bool operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};
}
