/*!
 * \file rRendererBasic.cpp
 * \brief \b Classes : \a rRendererBasic
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

#include "defines.hpp"
#include "rRendererBasic.hpp"
#include "uConfig.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "rPipeline.hpp"
#include "rObjectBase.hpp"
#include "rWorld.hpp"

using namespace e_engine;

VkResult rRendererBasic::initRenderer(std::vector<VkImageView> _images, VkSurfaceFormatKHR _surfaceFormat) {
  vFbData.resize(_images.size());
  vRenderPass.setup(getRenderPassDescription(_surfaceFormat));

  auto lRes = vRenderPass.init(vDevice);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to init render pass. Can not initialize renderer");
    return lRes;
  }

  for (size_t i = 0; i < vFbData.size(); ++i) {
    vFbData[i].frameBuffer.setup(vRenderPass);
    lRes = vFbData[i].frameBuffer.reCreateFrameBuffers(getFrameBufferDescription(_images[i]));
  }

  return lRes;
}

void rRendererBasic::destroyRenderer() {
  vRenderPass.destroy();
  vFbData.clear();
}



void rRendererBasic::initCmdBuffers(vkuCommandPool *_pool) {
  for (auto i : vObjects) {
    if (i.get() == nullptr) {
      eLOG("FATAL ERROR: nullptr in object list!");
      return;
    }

    if (i->isMesh()) {
      vRenderObjects.emplace_back(i);
    }
  }

  for (auto &i : vFbData) {
    i.buffers.resize(vRenderObjects.size());
    for (auto &j : i.buffers) {
      j.init(_pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY);
    }
  }
}

void rRendererBasic::freeCmdBuffers() {
  for (auto &i : vFbData)
    i.buffers.clear();

  vRenderObjects.clear();
}

VkImageView rRendererBasic::getAttachmentView(internal::rRendererBase::ATTACHMENT_ROLE _role) {
  switch (_role) {
    case DEPTH_STENCIL: return VK_NULL_HANDLE;
    default: return VK_NULL_HANDLE;
  }
}



/*!
 * \brief Records the Vulkan command buffers, for a framebuffer
 * \note _toRender.size() MUST BE EQUAL TO _fb.secondary.size()
 * Elements in _toRender can be skipped by setting them to nullptr
 */
void rRendererBasic::recordCmdBuffers(Framebuffer_vk &_fb, RECORD_TARGET _toRender) {
  _fb.render.begin();

  vkCmdBeginRenderPass(*_fb.render, &vCmdRecordInfo.lRPInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

  for (uint32_t i = 0; i < vRenderObjects.size(); i++) {
    if (_toRender == RECORD_PUSH_CONST_ONLY)
      if (!vRenderObjects[i]->supportsPushConstants())
        continue;

    auto *lPipe = vRenderObjects[i]->getPipeline();
    if (!lPipe) {
      eLOG("Object ", vRenderObjects[i]->getName(), " has no pipeline!");
      continue;
    }

    vFbData[_fb.index].buffers[i].begin(VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &vCmdRecordInfo.lInherit);

    if (lPipe->getNumViewpors() > 0)
      vkCmdSetViewport(*vFbData[_fb.index].buffers[i], 0, 1, &vCmdRecordInfo.lViewPort);

    if (lPipe->getNumScissors() > 0)
      vkCmdSetScissor(*vFbData[_fb.index].buffers[i], 0, 1, &vCmdRecordInfo.lScissors);

    vRenderObjects[i]->record(*vFbData[_fb.index].buffers[i]);
    vFbData[_fb.index].buffers[i].end();
  }

  for (auto &i : vFbData[_fb.index].buffers) {
    vkCmdExecuteCommands(*_fb.render, 1, &i.get());
  }

  vkCmdEndRenderPass(*_fb.render);

  auto lRes = vkEndCommandBuffer(*_fb.render);
  if (lRes) {
    eLOG("'vkEndCommandBuffer' returned ", uEnum2Str::toStr(lRes));
    //! \todo Handle this somehow (practically this code must not execute)
  }
}
