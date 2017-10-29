/*!
 * \file rWorld.cpp
 * \brief \b Classes: \a rWorld
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

#include "rWorld.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"
#include "iInit.hpp"
#include "rScene.hpp"

#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

namespace e_engine {

void rWorld::handleResize(iEventInfo const &) {
  init(); // Will reset the swapchain and update the renderers
}

/*!
 * \brief Constructor
 * \note The pointer _init must be valid over the lifetime of the object!
 */
rWorld::rWorld(iInit *_init)
    : vInitPtr(_init),
      vDevice(_init->getDevicePTR()),
      vDevice_vk(**vDevice),
      vRenderLoop(this),
      vResizeSlot(&rWorld::handleResize, this) {
  vSurface_vk = vInitPtr->getVulkanSurface();

  vViewPort.vNeedUpdate_B = false;
  vViewPort.x             = 0;
  vViewPort.y             = 0;
  vViewPort.width         = 0;
  vViewPort.height        = 0;

  vClearColor.vNeedUpdate_B = false;
  vClearColor.r             = 0;
  vClearColor.g             = 0;
  vClearColor.b             = 0;
  vClearColor.a             = 1;
}

rWorld::~rWorld() { shutdown(); }


/*!
 * \brief initializes the swapchain and the depth and stencil buffer
 *
 * Frees old objects (if they exist) and then recreates them with the new settings.
 * Call this function manually if you want to update a global setting like multi sampling.
 *
 * \returns 0 on success
 * \note this function will be automatically called every time the window is resized
 */
int rWorld::init() {
  if (!vInitPtr->getIsSetup())
    return -100;

  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();
  iLOG(L"(Re)initializing renderers");

  destroyRenderers();

  vSurface_vk = vInitPtr->getVulkanSurface();

  if (!vSwapChain.init(vDevice, vSurface_vk))
    return 1;

  initRenderers();
  rebuildSubmitInfos();

  if (!vIsResizeSlotSetup)
    vInitPtr->addResizeSlot(&vResizeSlot);

  vIsResizeSlotSetup = true;
  vIsSetup           = true;
  iLOG(L"Done (re)initializing renderers");
  return 0;
}

/*!
 * \brief Stops all render loops
 */
void rWorld::shutdown() {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  if (!vIsSetup || !vInitPtr->getIsSetup())
    return;

  if (vRenderLoop.getIsRunning())
    vRenderLoop.stop();

  dVkLOG("Vulkan cleanup [rWorld]:");
  destroyRenderers();
  vSwapChain.destroy();

  vIsSetup = false;
}

/*!
 * \brief Recrecords command buffers and writes them to the renderloop
 */
void rWorld::rebuildRenderers() {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();
  rebuildSubmitInfos();
}


/*!
 * \brief Non thread safe private implementation of rebuildRenderers
 * \note Requires external synchronisation with the Render Loop Lock
 */
void rWorld::rebuildSubmitInfos() {
  auto *lBufferRef = vRenderLoop.getCommandBufferReferences();

  for (auto const &i : vRenderers) {
    if (!i->getIsInit()) {
      continue;
    }

    i->updateRenderer();
  }

  lBufferRef->frames.resize(vSwapChain.getNumImages());

  for (uint32_t i = 0; i < vSwapChain.getNumImages(); ++i) {
    lBufferRef->frames[i].inf.clear();

    for (auto &j : vRenderers) {
      if (!j->getIsInit()) {
        continue;
      }

      if (!j->getIsRenderingEnabled())
        continue;

      auto lSubmitInfo = j->getVulkanSubmitInfos();

      auto &lShared = lSubmitInfo.shared.submitInfos;
      auto &fb      = lSubmitInfo.fb[i].submitInfos;
      std::copy(lShared.begin(), lShared.end(), std::back_inserter(lBufferRef->frames[i].inf));
      std::copy(fb.begin(), fb.end(), std::back_inserter(lBufferRef->frames[i].inf));
    }
  }
}


/**
 * \brief Initilizes all renderes (if neccessary)
 * \returns The sum of all error codes
 * \note Requires external synchronisation with the Render Loop Lock
 */
int rWorld::initRenderers() {
  vkuCommandPool *lPool     = vkuCommandPoolManager::get(vDevice_vk, vRenderLoop.getQueueFamilyIndex());
  int             errorCode = 0;
  for (auto const &i : vRenderers)
    if (!i->getIsInit())
      errorCode += i->init(lPool);

  rebuildSubmitInfos();

  return errorCode;
}

/**
 * \brief Destoyes all renderes (if neccessary)
 * \note Requires external synchronisation with the Render Loop Lock
 */
void rWorld::destroyRenderers() {
  for (auto const &i : vRenderers)
    if (i->getIsInit())
      i->destroy();

  rebuildSubmitInfos();
}

/*!
 * \brief Waits until a frame is rendered or a timeout occurs
 *
 * The timeout is 500ms
 *
 * \returns false on timeout
 */
bool rWorld::waitForFrame(std::mutex &_mutex) {
  std::unique_lock<std::mutex> lLock(_mutex);
  if (vRenderedFrameSignal.wait_for(lLock, std::chrono::milliseconds(500)) == std::cv_status::timeout)
    return false;
  else
    return true;
}

/*!
 * \brief Ads a renderer to the render loop
 * \note This will NOT initialize the renderer
 */
void rWorld::addRenderer(std::shared_ptr<rRendererBase> _renderer) {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();

  vRenderers.push_back(_renderer);
  rebuildSubmitInfos();
}

/*!
 * \brief Removes a renderer to the render loop
 */
void rWorld::removeRenderer(std::shared_ptr<rRendererBase> _renderer) {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();

  vRenderers.erase(std::remove(vRenderers.begin(), vRenderers.end(), _renderer), vRenderers.end());
  rebuildSubmitInfos();
}

/*!
 * \brief Removes all renderers from the render loop
 */
void rWorld::clearRenderers() {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();

  vRenderers.clear();
  rebuildSubmitInfos();
}

void rWorld::updateViewPort(int _x, int _y, int _width, int _height) {
  vViewPort.vNeedUpdate_B = true;
  vViewPort.x             = _x;
  vViewPort.y             = _y;
  vViewPort.width         = _width;
  vViewPort.height        = _height;
}

/*!
 * \brief Updates the clear color
 * \note This function will only take effect once the render loop is restarted!
 */
void rWorld::updateClearColor(float _r, float _g, float _b, float _a) {
  VkClearColorValue lClear = {{_r, _g, _b, _a}};

  for (auto const &i : vRenderers) {
    i->setClearColor(lClear);
  }
}

/*!
 * \returns A pointer to the integer counting the number of rendered frames
 */
uint64_t *rWorld::getRenderedFramesPtr() { return vRenderLoop.getRenderedFramesPtr(); }

/*!
 * \returns A shared pointer to the vulkan device wrapper
 * \vkIntern
 */
vkuDevicePTR rWorld::getDevice() { return vDevice; }

/*!
 * \returns A pointer to the swapchain in use
 * \vkIntern
 */
vkuSwapChain *rWorld::getSwapChain() { return &vSwapChain; }

/*!
 * \returns the internaly used iInit pointer
 */
iInit *rWorld::getInitPtr() { return vInitPtr; }

/*!
 * \returns a reverence to the render loop
 */
rRenderLoop *rWorld::getRenderLoop() { return &vRenderLoop; }
} // namespace e_engine
