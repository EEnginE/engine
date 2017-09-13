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
#include "uLog.hpp"

using namespace e_engine;

vkuFrameBuffer::~vkuFrameBuffer() { destroy(); }

/*!
 * \brief Creates the framebuffer for a specific render pass
 * \param _renderPass The render pass to use
 */
VkResult vkuFrameBuffer::init(vkuRenderPass &_renderPass) {
  if (!_renderPass) {
    eLOG(L"");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  vDevice     = _renderPass.getDevice();
  vRenderPass = *_renderPass;

  return VK_SUCCESS;
}

void vkuFrameBuffer::destroy() {
  if (!isCreated())
    return;

  vkDestroyFramebuffer(vDevice, vFrameBuffer, nullptr);
  vFrameBuffer = VK_NULL_HANDLE;
  vRenderPass  = VK_NULL_HANDLE;
}
