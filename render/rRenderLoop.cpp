/*!
 * \file rRenderLoop.cpp
 * \brief \b Classes: \a rRenderLoop
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
#include "rRenderLoop.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"
#include "vkuSemaphore.hpp"
#include "iInit.hpp"
#include "rPipeline.hpp"
#include "rWorld.hpp"
#include <algorithm>

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

uint64_t rRenderLoop::vRenderedFrames = 0;

rRenderLoop::rRenderLoop(rWorld *_root) : vWorldPtr(_root) {
  vRenderThread = std::thread(&rRenderLoop::renderLoop, this);
  vDevice       = vWorldPtr->getDevice();
  vDevice_vk    = **vDevice;
}

rRenderLoop::~rRenderLoop() {
  if (vRunRenderLoop)
    stop();

  vRunRenderThread = false;

  if (vRenderThread.joinable())
    vRenderThread.join();
}

void rRenderLoop::renderLoop() {
  LOG.nameThread(L"RLoop");
  iLOG("Starting render thread");

  while (vRunRenderThread) {
    // Sync point 1: Sync with start()
    {
      std::unique_lock<std::mutex> lControl(vRenderLoopControlMutex);

      dRLOG("Waiting for command buffer recording signal");

      while (vLoopStartCommand != PASS) {
        vRenderLoopControl.wait_for(lControl, cfg.condWaitTimeout);

        if (!vRunRenderThread)
          return;
      }

      dRLOG("DONE Waiting for command buffer recording signal");

      vLoopStartCommand = PASSED;
      vRenderLoopResponse.notify_all();
    }

    //    _____      _ _
    //   |_   _|    (_) |
    //     | | _ __  _| |_
    //     | || '_ \| | __|
    //    _| || | | | | |_
    //    \___/_| |_|_|\__|
    //

    const static uint32_t NUM_FENCES   = 3;
    const static uint32_t FENCE_RENDER = 0;
    const static uint32_t FENCE_IMG_1  = 1;
    const static uint32_t FENCE_IMG_2  = 2;

    const static uint32_t NUM_SEMAPHORES = 2;
    const static uint32_t SEM_PRESENT    = 0;
    const static uint32_t SEM_ACQUIRE    = 1;

    uint32_t lQueueFamily = 0;

    vkuSwapChain *                lSwapchain   = vWorldPtr->getSwapChain();
    VkQueue                       lQueue       = vDevice->getQueue(VK_QUEUE_GRAPHICS_BIT, 1.0, &lQueueFamily);
    vkuCommandPool *              lCommandPool = vkuCommandPoolManager::get(vDevice_vk, lQueueFamily);
    vkuFences<NUM_FENCES>         lFences(vDevice_vk);
    vkuSemaphores<NUM_SEMAPHORES> lSemaphores(vDevice_vk);

    for (auto const &i : vRenderers) {
      if (lSwapchain->getNumImages() != i->getNumFramebuffers()) {
        eLOG("Internal error: number of framebuffers not equal in renderer and world!");
        eLOG("This might cause undefined behaviour!");
      }

      i->initAllCmdBuffers(lCommandPool);
    }

    VkPipelineStageFlags lSubmitWaitFlags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    VkSubmitInfo lRenderSubmit[3];
    lRenderSubmit[0].sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    lRenderSubmit[0].pNext                = nullptr;
    lRenderSubmit[0].waitSemaphoreCount   = 0;
    lRenderSubmit[0].pWaitSemaphores      = nullptr;
    lRenderSubmit[0].pWaitDstStageMask    = nullptr;
    lRenderSubmit[0].commandBufferCount   = 0;
    lRenderSubmit[0].pCommandBuffers      = nullptr; // set in render loop
    lRenderSubmit[0].signalSemaphoreCount = 0;
    lRenderSubmit[0].pSignalSemaphores    = nullptr;

    lRenderSubmit[1] = lRenderSubmit[0];
    lRenderSubmit[2] = lRenderSubmit[0];

    lRenderSubmit[0].waitSemaphoreCount   = 1;
    lRenderSubmit[0].pWaitSemaphores      = &lSemaphores[SEM_ACQUIRE];
    lRenderSubmit[0].pWaitDstStageMask    = &lSubmitWaitFlags;
    lRenderSubmit[2].signalSemaphoreCount = 1;
    lRenderSubmit[2].pSignalSemaphores    = &lSemaphores[SEM_PRESENT];

    VkPresentInfoKHR lPresentInfo   = {};
    lPresentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    lPresentInfo.pNext              = nullptr;
    lPresentInfo.waitSemaphoreCount = 1;
    lPresentInfo.pWaitSemaphores    = &lSemaphores[SEM_PRESENT];
    lPresentInfo.swapchainCount     = 1;
    lPresentInfo.pSwapchains        = nullptr;
    lPresentInfo.pImageIndices      = nullptr; // set in render loop
    lPresentInfo.pResults           = nullptr;


    // ==============================
    // Rocord and get command buffers
    // ==============================

    CommandBuffers lCmdBuffers;

    lCmdBuffers.pre.reserve(vRenderers.size());
    lCmdBuffers.render.reserve(vRenderers.size());
    lCmdBuffers.post.reserve(vRenderers.size());

    for (auto const &i : vRenderers) {
      i->updateRenderer();
      lCmdBuffers.pointers.emplace_back();

      for (uint32_t j = 0; j < lSwapchain->getNumImages(); j++) {
        lCmdBuffers.pointers.back().push_back(i->getCommandBuffers(j));
      }
    }


    {
      // Sync point 2
      std::unique_lock<std::mutex> lControl(vRenderLoopControlMutex);

      dRLOG("Notifying main thread that command buffers were recorded");

      vFinishedRecording = true;
      vRenderLoopResponse.notify_all(); // Notifying other thread to continue


      dRLOG("Waiting for start render loop signal");

      while (vStartRenderLoop != PASS) {
        vRenderLoopControl.wait_for(lControl, cfg.condWaitTimeout);

        if (!vRunRenderThread)
          return;
      }

      dRLOG("DONE Waiting for start render loop signal");

      vStartRenderLoop = PASSED;
      vRenderLoopResponse.notify_all();
    }

    //   ______               _             _
    //   | ___ \             | |           | |
    //   | |_/ /___ _ __   __| | ___ _ __  | |     ___   ___  _ __
    //   |    // _ \ '_ \ / _` |/ _ \ '__| | |    / _ \ / _ \| '_ \
    //   | |\ \  __/ | | | (_| |  __/ |    | |___| (_) | (_) | |_) |
    //   \_| \_\___|_| |_|\__,_|\___|_|    \_____/\___/ \___/| .__/
    //                                                       | |
    //                                                       |_|

    // Init Uniforms
    for (auto const &i : vRenderers)
      i->updateUniforms();

    iLOG("Starting the render loop");
    while (vRunRenderLoop) {

      // Get present image (this command blocks)
      auto lNextImg = lSwapchain->acquireNextImage(lSemaphores[SEM_ACQUIRE]);

      if (!lNextImg) {
        eLOG("'vkAcquireNextImageKHR' returned ", uEnum2Str::toStr(lNextImg.getError()));
        break;
      }

      rebuildCommandBuffersArray(&lCmdBuffers, *lNextImg);

      // Set CMD buffers
      lRenderSubmit[0].commandBufferCount = static_cast<uint32_t>(lCmdBuffers.pre.size());
      lRenderSubmit[0].pCommandBuffers    = lCmdBuffers.pre.data();
      lRenderSubmit[1].commandBufferCount = static_cast<uint32_t>(lCmdBuffers.render.size());
      lRenderSubmit[1].pCommandBuffers    = lCmdBuffers.render.data();
      lRenderSubmit[2].commandBufferCount = static_cast<uint32_t>(lCmdBuffers.post.size());
      lRenderSubmit[2].pCommandBuffers    = lCmdBuffers.post.data();

      // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  -->  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
      auto lRes = vkQueueSubmit(lQueue, 1, &lRenderSubmit[0], lFences[FENCE_IMG_1]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      // Render
      lRes = vkQueueSubmit(lQueue, 1, &lRenderSubmit[1], lFences[FENCE_RENDER]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  -->  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      lRes = vkQueueSubmit(lQueue, 1, &lRenderSubmit[2], lFences[FENCE_IMG_2]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      VkSwapchainKHR lTempSc   = **lSwapchain;
      uint32_t       lImgIndex = *lNextImg;

      lPresentInfo.pSwapchains   = &lTempSc;
      lPresentInfo.pImageIndices = &lImgIndex;
      lRes                       = vkQueuePresentKHR(lQueue, &lPresentInfo);
      if (lRes) {
        eLOG("'vkQueuePresentKHR' returned ", uEnum2Str::toStr(lRes));
        break;
      }


      // Wait until rendering is done
      lFences.wait(FENCE_RENDER, 1);

      // Update Uniforms
      for (auto const &i : vRenderers)
        i->updateUniforms();

      lFences.wait(FENCE_IMG_1, 1);
      lFences.wait(FENCE_IMG_2, 1);

      lFences.reset();

      vWorldPtr->signalRenderdFrame();
      vRenderedFrames++;
    }
    iLOG("Render loop stopped");


    //    _____ _
    //   /  __ \ |
    //   | /  \/ | ___  __ _ _ __  _   _ _ __
    //   | |   | |/ _ \/ _` | '_ \| | | | '_ \
    //   | \__/\ |  __/ (_| | | | | |_| | |_) |
    //    \____/_|\___|\__,_|_| |_|\__,_| .__/
    //                                  | |
    //                                  |_|

    auto lRes = vkDeviceWaitIdle(vDevice_vk);
    if (lRes) {
      eLOG("'vkDeviceWaitIdle' returned ", uEnum2Str::toStr(lRes));
    }

    for (auto const &i : vRenderers)
      i->freeAllCmdBuffers();

    // Sync point 3
    std::unique_lock<std::mutex> lControl(vRenderLoopControlMutex);

    dRLOG("Waiting for stop render loop signal");
    while (vStopRenderLoop != PASS) {
      vRenderLoopControl.wait_for(lControl, cfg.condWaitTimeout);

      if (!vRunRenderThread)
        return;
    }

    dRLOG("DONE Waiting for stop render loop signal");

    vStopRenderLoop    = PASSED;
    vFinishedRecording = false;
    vRenderLoopResponse.notify_all();
  }

  iLOG("Stopping render thread");
}

void rRenderLoop::rebuildCommandBuffersArray(CommandBuffers *_buffers, uint32_t _framebuffer) {
  for (auto const &i : vRenderers) {
    i->updatePushConstants(_framebuffer);
  }

  _buffers->pre.clear();
  _buffers->render.clear();
  _buffers->post.clear();

  for (auto const &i : _buffers->pointers) {
    if (!*i[_framebuffer].enableRendering)
      continue;

    _buffers->pre.emplace_back(*i[_framebuffer].pre);
    _buffers->render.emplace_back(*i[_framebuffer].render);
    _buffers->post.emplace_back(*i[_framebuffer].post);
  }
}

/**
 * \brief Initilizes all renderes (if neccessary)
 * \returns The sum of all error codes
 */
int rRenderLoop::init() {
  std::lock_guard<std::recursive_mutex> lGuard(vLoopAccessMutex);

  int errorCode = 0;
  for (auto const &i : vRenderers)
    if (!i->getIsInit())
      errorCode += i->init();

  return errorCode;
}

/**
 * \brief Destoyes all renderes (if neccessary)
 */
void rRenderLoop::destroy() {
  std::lock_guard<std::recursive_mutex> lGuard(vLoopAccessMutex);

  for (auto const &i : vRenderers)
    if (i->getIsInit())
      i->destroy();
}

/**
 * \brief Starts the render loop and waits until the rendering starts
 * \note This does not start a seperate thread. It will unpause the thread created in the constuctor
 */
bool rRenderLoop::start() {
  std::lock_guard<std::recursive_mutex> lGuard1(vLoopAccessMutex);
  std::unique_lock<std::mutex>          lControl(vRenderLoopControlMutex);

  if (vRunRenderLoop) {
    wLOG("Render loop already running!");
    return false;
  }

  dRLOG("Initializing renderers");

  for (auto const &i : vRenderers) {
    if (!i->getIsInit()) {
      i->init();
    }
  }


  // Sync point 1
  dRLOG("Start recording");
  vLoopStartCommand = PASS;
  vRenderLoopControl.notify_all();

  while (!vFinishedRecording || vLoopStartCommand != PASSED)
    vRenderLoopResponse.wait_for(lControl, cfg.condWaitTimeout);

  vLoopStartCommand = BLOCK; // Reset

  // Sync point 2
  dRLOG("Sending start render loop");
  vStartRenderLoop = PASS;
  vRunRenderLoop   = true;

  vRenderLoopControl.notify_all();
  while (vStartRenderLoop != PASSED)
    vRenderLoopResponse.wait_for(lControl, cfg.condWaitTimeout);

  vStartRenderLoop = BLOCK; // Reset
  return true;
}

/**
 * \brief Stops the render loop and waits until the rendering has stopped
 * \note This only pauses the thread. The thread will be destroyed in the destructor.
 */
bool rRenderLoop::stop() {
  std::lock_guard<std::recursive_mutex> lGuard(vLoopAccessMutex);
  std::unique_lock<std::mutex>          lControl(vRenderLoopControlMutex);

  if (!vRunRenderLoop) {
    wLOG("Render loop already stopped!");
    return false;
  }

  // Sync point 3
  dRLOG("Sending stop to render loop");

  vRunRenderLoop  = false;
  vStopRenderLoop = PASS;
  vRenderLoopControl.notify_all();

  while (vStopRenderLoop != PASSED)
    vRenderLoopResponse.wait_for(lControl, cfg.condWaitTimeout);

  vStopRenderLoop = BLOCK;
  return true;
}

/*!
 * \brief Sets the clear color for all renderers
 */
void rRenderLoop::updateGlobalClearColor(VkClearColorValue _clear) {
  std::lock_guard<std::recursive_mutex> lGuard(vLoopAccessMutex);

  for (auto const &i : vRenderers) {
    i->setClearColor(_clear);
  }
}

/*!
 * \brief Ads a renderer to the render loop
 * \note This will restart the render loop
 */
void rRenderLoop::addRenderer(std::shared_ptr<internal::rRendererBase> _renderer) {
  std::lock_guard<std::recursive_mutex> lGuard(vLoopAccessMutex);

  bool lStartRenderLoop = vRunRenderLoop;
  if (vRunRenderLoop)
    stop();

  vRenderers.push_back(_renderer);

  if (lStartRenderLoop)
    start();
}

/*!
 * \brief Removes a renderer to the render loop
 * \note This will restart the render loop
 */
void rRenderLoop::removeRenderer(std::shared_ptr<internal::rRendererBase> _renderer) {
  std::lock_guard<std::recursive_mutex> lGuard(vLoopAccessMutex);

  bool lStartRenderLoop = vRunRenderLoop;
  if (vRunRenderLoop)
    stop();

  vRenderers.erase(std::remove(vRenderers.begin(), vRenderers.end(), _renderer), vRenderers.end());

  if (lStartRenderLoop)
    start();
}

/*!
 * \brief Removes all renderers from the render loop
 * \note This will restart the render loop
 */
void rRenderLoop::clearRenderers() {
  std::lock_guard<std::recursive_mutex> lGuard(vLoopAccessMutex);

  bool lStartRenderLoop = vRunRenderLoop;
  if (vRunRenderLoop)
    stop();

  vRenderers.clear();

  if (lStartRenderLoop)
    start();
}

uint64_t *rRenderLoop::getRenderedFramesPtr() { return &vRenderedFrames; }
bool      rRenderLoop::getIsRunning() const { return vRunRenderLoop; }
} // namespace e_engine
