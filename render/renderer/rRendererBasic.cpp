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

VkResult rRendererBasic::initRenderer(SwapChainImages    _images,
                                      VkSurfaceFormatKHR _surfaceFormat,
                                      vkuCommandPool *   _pool) {
  vFbData.resize(_images.size());

  // ==> Setup render pass
  vRenderPass.setup(getRenderPassDescription(_surfaceFormat));

  auto lRes = vRenderPass.init(vDevice);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to init render pass. Can not initialize renderer");
    return lRes;
  }

  VkExtent3D lSize = {
      GlobConf.win.width,  // width
      GlobConf.win.height, // height
      1                    // depth
  };

  // ==> Setup framebuffer
  vDepthBuffer = vRenderPass.generateImageBufferFromAttachment(1, lSize);

  if (!vDepthBuffer) {
    eLOG(L"Failed to create depth buffer ==> can not create framebuffer");
  }

  for (size_t i = 0; i < vFbData.size(); ++i) {
    vFbData[i].frameBuffer.setup(vRenderPass);
    lRes = vFbData[i].frameBuffer.reCreateFrameBuffers({

        // Size
        lSize,

        // Data
        {

            // ATTACHMENT 0 ----- swapchain image
            {0, _images[i].iv},

            // ATTACHMENT 1 ----- swapchain image
            {1, *vDepthBuffer}

        }

    });
  }

  // ==> Setup command buffer

  //   -- Root command buffer
  for (auto &i : vFbData) {
    i.cmdBuffer.init(_pool);
  }

  //   -- Child object buffers
  for (auto i : vObjects) {
    if (i.get() == nullptr) {
      wLOG("WARNING: nullptr in object list! (skipping)");
      continue;
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

  return lRes;
}

void rRendererBasic::destroyRenderer() {
  vRenderPass.destroy();
  vFbData.clear();
  vDepthBuffer.destroy();

  for (auto &i : vFbData)
    i.buffers.clear();

  vRenderObjects.clear();
}


VkImageView rRendererBasic::getAttachmentView(rRendererBase::ATTACHMENT_ROLE _role) {
  switch (_role) {
    case DEPTH_STENCIL: return vDepthBuffer.get();
    default: return VK_NULL_HANDLE;
  }
}

rRendererBase::SubmitInfo rRendererBasic::getVulkanSubmitInfos() {
  SubmitInfo lInfo;

  for (auto &i : vFbData) {
    VkSubmitInfo lSubInfo;
    lSubInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    lSubInfo.pNext                = nullptr;
    lSubInfo.waitSemaphoreCount   = 0;
    lSubInfo.pWaitSemaphores      = nullptr;
    lSubInfo.pWaitDstStageMask    = nullptr;
    lSubInfo.commandBufferCount   = 1;
    lSubInfo.pCommandBuffers      = &i.cmdBuffer.get();
    lSubInfo.signalSemaphoreCount = 0;
    lSubInfo.pSignalSemaphores    = nullptr;

    lInfo.fb.push_back({{lSubInfo}});
  }

  return lInfo;
}



/*!
 * \brief Records the Vulkan command buffers, for a framebuffer
 * \note _toRender.size() MUST BE EQUAL TO _fb.secondary.size()
 * Elements in _toRender can be skipped by setting them to nullptr
 */
void rRendererBasic::recordCmdBuffers(uint32_t &_fbIndex, RECORD_TARGET _toRender) {
  auto &fb = vFbData[_fbIndex];
  fb.cmdBuffer.begin();

  vkCmdBeginRenderPass(*fb.cmdBuffer, &vCmdRecordInfo.lRPInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);

  for (uint32_t i = 0; i < vRenderObjects.size(); i++) {
    if (_toRender == RECORD_PUSH_CONST_ONLY)
      if (!vRenderObjects[i]->supportsPushConstants())
        continue;

    auto *lPipe = vRenderObjects[i]->getPipeline();
    if (!lPipe) {
      eLOG("Object ", vRenderObjects[i]->getName(), " has no pipeline!");
      continue;
    }

    vFbData[_fbIndex].buffers[i].begin(VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &vCmdRecordInfo.lInherit);

    if (lPipe->getNumViewpors() > 0)
      vkCmdSetViewport(*vFbData[_fbIndex].buffers[i], 0, 1, &vCmdRecordInfo.lViewPort);

    if (lPipe->getNumScissors() > 0)
      vkCmdSetScissor(*vFbData[_fbIndex].buffers[i], 0, 1, &vCmdRecordInfo.lScissors);

    vRenderObjects[i]->record(*vFbData[_fbIndex].buffers[i]);
    vFbData[_fbIndex].buffers[i].end();
  }

  for (auto &i : vFbData[_fbIndex].buffers) {
    vkCmdExecuteCommands(*fb.cmdBuffer, 1, &i.get());
  }

  vkCmdEndRenderPass(*fb.cmdBuffer);

  auto lRes = vkEndCommandBuffer(*fb.cmdBuffer);
  if (lRes) {
    eLOG("'vkEndCommandBuffer' returned ", uEnum2Str::toStr(lRes));
    //! \todo Handle this somehow (practically this code must not execute)
  }
}
