/*!
 * \file rRendererBase.cpp
 * \brief \b Classes: \a rRendererBase
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

#include "rRendererBase.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"
#include "iInit.hpp"
#include "rObjectBase.hpp"
#include "rPipeline.hpp"
#include "rScene.hpp"
#include "rShaderBase.hpp"
#include "rWorld.hpp"


#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

#if D_LOG_RENDER_LOOP
#define dRLOG(...) dLOG(__VA_ARGS__)
#else
#define dRLOG(...)
#endif

namespace e_engine {
namespace internal {

rRendererBase::rRendererBase(rWorld *_root, std::wstring _id) : vID(_id), vWorldPtr(_root) {
  vDevice    = vWorldPtr->getDevicePTR();
  vDevice_vk = **vDevice;

  vCmdRecordInfo.lRPInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  vCmdRecordInfo.lRPInfo.pNext                    = nullptr;
  vCmdRecordInfo.lRPInfo.renderPass               = nullptr;
  vCmdRecordInfo.lRPInfo.framebuffer              = nullptr;
  vCmdRecordInfo.lRPInfo.renderArea.extent.width  = GlobConf.win.width;
  vCmdRecordInfo.lRPInfo.renderArea.extent.height = GlobConf.win.height;
  vCmdRecordInfo.lRPInfo.renderArea.offset        = {0, 0};
  vCmdRecordInfo.lRPInfo.clearValueCount          = 0;
  vCmdRecordInfo.lRPInfo.pClearValues             = nullptr;

  vCmdRecordInfo.lViewPort.x        = 0;
  vCmdRecordInfo.lViewPort.y        = 0;
  vCmdRecordInfo.lViewPort.width    = GlobConf.win.width;
  vCmdRecordInfo.lViewPort.height   = GlobConf.win.height;
  vCmdRecordInfo.lViewPort.minDepth = 0.0f;
  vCmdRecordInfo.lViewPort.maxDepth = 1.0f;

  vCmdRecordInfo.lScissors.offset.x      = 0;
  vCmdRecordInfo.lScissors.offset.y      = 0;
  vCmdRecordInfo.lScissors.extent.width  = GlobConf.win.width;
  vCmdRecordInfo.lScissors.extent.height = GlobConf.win.height;

  vCmdRecordInfo.lInherit.sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  vCmdRecordInfo.lInherit.pNext                = nullptr;
  vCmdRecordInfo.lInherit.renderPass           = nullptr;
  vCmdRecordInfo.lInherit.framebuffer          = nullptr; // set in loop
  vCmdRecordInfo.lInherit.occlusionQueryEnable = VK_FALSE;
  vCmdRecordInfo.lInherit.queryFlags           = 0;
  vCmdRecordInfo.lInherit.pipelineStatistics   = 0;
}

rRendererBase::~rRendererBase() {}

int rRendererBase::init() {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  if (vIsSetup)
    return -3;

  auto                     lImages = vWorldPtr->getSwapchainImageViews();
  std::vector<VkImageView> lViews;

  lViews.reserve(lImages.size());
  vFramebuffers_vk.resize(lImages.size());
  for (auto &i : lImages) {
    lViews.emplace_back(i.iv);
  }

  for (size_t i = 0; i < lImages.size(); ++i) {
    vFramebuffers_vk[i].img   = lImages[i].img;
    vFramebuffers_vk[i].iv    = lImages[i].iv;
    vFramebuffers_vk[i].index = static_cast<uint32_t>(i);
  }

  if (initRenderer(lViews, vWorldPtr->getSwapchainFormat()))
    return 2;

  if (!initRendererData())
    return 4;

  vIsSetup = true;

  if (!vUserDisabledRendering)
    vEnableRendering = true;

  return 0;
}

void rRendererBase::destroy() {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  if (!vIsSetup)
    return;

  vEnableRendering = false;

  dVkLOG("Destroying old render pass data [renderer ", vID, "]");

  dVkLOG("  -- freeing old renderer data [renderer ", vID, "]");
  freeRendererData();

  dVkLOG("  -- destroying old renderpass [renderer ", vID, "]");
  destroyRenderer();

  vFramebuffers_vk.clear();
  vIsSetup = false;
}

void rRendererBase::getDepthFormat(VkFormat &_format, VkImageTiling &_tiling, VkImageAspectFlags &_aspect) {
  vDevice->getDepthFormat(_format, _tiling, _aspect);
}

void rRendererBase::recordCmdBuffersWrapper(Framebuffer_vk &_fb, RECORD_TARGET _toRender) {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  auto lClearValues = getClearValues();

  vCmdRecordInfo.lRPInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  vCmdRecordInfo.lRPInfo.pNext                    = nullptr;
  vCmdRecordInfo.lRPInfo.renderPass               = getRenderPass();
  vCmdRecordInfo.lRPInfo.framebuffer              = getFrameBuffer(_fb.index);
  vCmdRecordInfo.lRPInfo.renderArea.extent.width  = GlobConf.win.width;
  vCmdRecordInfo.lRPInfo.renderArea.extent.height = GlobConf.win.height;
  vCmdRecordInfo.lRPInfo.renderArea.offset        = {0, 0};
  vCmdRecordInfo.lRPInfo.clearValueCount          = static_cast<uint32_t>(lClearValues.size());
  vCmdRecordInfo.lRPInfo.pClearValues             = lClearValues.data();

  vCmdRecordInfo.lViewPort.x        = 0;
  vCmdRecordInfo.lViewPort.y        = 0;
  vCmdRecordInfo.lViewPort.width    = GlobConf.win.width;
  vCmdRecordInfo.lViewPort.height   = GlobConf.win.height;
  vCmdRecordInfo.lViewPort.minDepth = 0.0f;
  vCmdRecordInfo.lViewPort.maxDepth = 1.0f;

  vCmdRecordInfo.lScissors.offset.x      = 0;
  vCmdRecordInfo.lScissors.offset.y      = 0;
  vCmdRecordInfo.lScissors.extent.width  = GlobConf.win.width;
  vCmdRecordInfo.lScissors.extent.height = GlobConf.win.height;

  vCmdRecordInfo.lInherit.sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
  vCmdRecordInfo.lInherit.pNext                = nullptr;
  vCmdRecordInfo.lInherit.renderPass           = getRenderPass();
  vCmdRecordInfo.lInherit.framebuffer          = getFrameBuffer(_fb.index);
  vCmdRecordInfo.lInherit.occlusionQueryEnable = VK_FALSE;
  vCmdRecordInfo.lInherit.queryFlags           = 0;
  vCmdRecordInfo.lInherit.pipelineStatistics   = 0;

  recordCmdBuffers(_fb, _toRender);
}

void rRendererBase::updateRenderer() {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  // Destroy old pipelines
  for (auto &i : vObjects) {
    rPipeline *  lPipe   = i->getPipeline();
    rShaderBase *lShader = i->getShader();
    if (lPipe != nullptr) {
      if (lPipe->getIsCreated()) {
        lPipe->destroy();
      }
    }

    // Clear reserved uniforms, part of making sure not to change one uniform more than once
    if (lShader != nullptr) {
      lShader->signalRenderReset();
    }
  }

  // Create new pipelines
  for (auto &i : vObjects) {
    rPipeline *lPipe = i->getPipeline();
    if (lPipe != nullptr) {
      if (!lPipe->getIsCreated()) {
        lPipe->create(vDevice_vk, getRenderPass(), 0);
      }
    }

    // Setup object for rendering (preparing uniforms)
    i->signalRenderReset(this);
  }

  // Record all command buffers
  for (auto &i : vFramebuffers_vk)
    recordCmdBuffersWrapper(i, RECORD_ALL);
}

void rRendererBase::updateUniforms() {
  for (auto i : vObjects)
    i->updateUniforms();
}


void rRendererBase::initFrameCommandBuffers(vkuCommandPool *_pool) {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  VkImageSubresourceRange lSubResRange = {};
  lSubResRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
  lSubResRange.baseMipLevel            = 0;
  lSubResRange.levelCount              = 1;
  lSubResRange.baseArrayLayer          = 0;
  lSubResRange.layerCount              = 1;

  for (auto &i : vFramebuffers_vk) {
    i.preRender.init(_pool);
    i.render.init(_pool);
    i.postRender.init(_pool);

    i.preRender.begin();
    vWorldPtr->cmdChangeImageLayout(*i.preRender,
                                    i.img,
                                    lSubResRange,
                                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    i.preRender.end();

    i.postRender.begin();
    vWorldPtr->cmdChangeImageLayout(*i.postRender,
                                    i.img,
                                    lSubResRange,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
    i.postRender.end();
  }
}

void rRendererBase::freeFrameCommandBuffers() {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  for (auto &i : vFramebuffers_vk) {
    i.preRender.destroy();
    i.render.destroy();
    i.postRender.destroy();
  }
}

void rRendererBase::initAllCmdBuffers(vkuCommandPool *_pool) {
  initFrameCommandBuffers(_pool);
  initCmdBuffers(_pool);
}

void rRendererBase::freeAllCmdBuffers() {
  freeCmdBuffers();
  freeFrameCommandBuffers();
}

/*!
 * \brief Adds all objects form a scene to the renderer
 */
bool rRendererBase::renderScene(rSceneBase *_scene) {
  for (auto const &i : _scene->getObjects()) {
    addObject(i);
  }
  return true;
}

/*!
 * \brief Adds object to be rendererd
 * \param _obj Object to be rendererd
 */
bool rRendererBase::addObject(std::shared_ptr<rObjectBase> _obj) {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  vObjects.emplace_back(_obj);
  return true;
}

bool rRendererBase::resetObjects() {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);

  vObjects.clear();
  return true;
}

void rRendererBase::updatePushConstants(uint32_t _framebuffer) {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);
  recordCmdBuffersWrapper(vFramebuffers_vk[_framebuffer], RECORD_PUSH_CONST_ONLY);
}

/*!
 * \brief Get pointers to command buffers
 */
rRendererBase::CommandBuffers rRendererBase::getCommandBuffers(uint32_t _framebuffer) {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);
  CommandBuffers                        lBuffers = {};

  lBuffers.pre             = &vFramebuffers_vk[_framebuffer].preRender.get();
  lBuffers.render          = &vFramebuffers_vk[_framebuffer].render.get();
  lBuffers.post            = &vFramebuffers_vk[_framebuffer].postRender.get();
  lBuffers.enableRendering = &vEnableRendering;

  return lBuffers;
}

/*!
 * \brief Disables rendering for this renderer
 *
 * Also prevents init() from automatically enabling the renderer.
 *
 * \note This will not disable updating uniforms, recording buffers, etc. Only the render loop won't
 * \note execute the command buffers
 */
void rRendererBase::disableRendering() {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);
  vUserDisabledRendering = true;
  vEnableRendering       = false;
}

/*!
 * \brief Enables rendering for this renderer
 * \returns false if the renderer is not setup
 * \note After calling this function, init() will enable rendering automatically (default)
 */
bool rRendererBase::enableRendering() {
  std::lock_guard<std::recursive_mutex> lGuard(vMutexRecordData);
  vUserDisabledRendering = false;

  if (!vIsSetup)
    return false;

  vEnableRendering = true;
  return true;
}

/*!
 * \returns if rendering is enabled
 */
bool rRendererBase::getIsRenderingEnabled() const { return vEnableRendering; }

bool rRendererBase::getIsInit() const { return vIsSetup; }
void rRendererBase::setClearColor(VkClearColorValue _clearColor) { vClearColor = _clearColor; }

/*!
 * \brief Get the number of framebuffers
 * \returns The number of framebuffers or UINT32_MAX if not init
 */
uint32_t rRendererBase::getNumFramebuffers() const {
  if (!vIsSetup)
    return UINT32_MAX;

  // assert(vWorldPtr->getNumFramebuffers() == vFramebuffers_vk.size());
  return static_cast<uint32_t>(vFramebuffers_vk.size());
}
}
}
