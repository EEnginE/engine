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
  vQueue        = vDevice->getQueue(VK_QUEUE_GRAPHICS_BIT, 1.0, &vQueueFamilyIndex);
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

    vkuSwapChain *                lSwapchain = vWorldPtr->getSwapChain();
    vkuFences<NUM_FENCES>         lFences(vDevice_vk);
    vkuSemaphores<NUM_SEMAPHORES> lSemaphores(vDevice_vk);

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

    //   ______               _             _
    //   | ___ \             | |           | |
    //   | |_/ /___ _ __   __| | ___ _ __  | |     ___   ___  _ __
    //   |    // _ \ '_ \ / _` |/ _ \ '__| | |    / _ \ / _ \| '_ \
    //   | |\ \  __/ | | | (_| |  __/ |    | |___| (_) | (_) | |_) |
    //   \_| \_\___|_| |_|\__,_|\___|_|    \_____/\___/ \___/| .__/
    //                                                       | |
    //                                                       |_|


    iLOG(L"Render loop started");
    while (vRunRenderLoop) {
      {
        // Sync point 2 (makes sure that the lock thread will acquire the mutex ())
        std::unique_lock<std::mutex> lControl(vRenderLoopControlMutex);
        if (vBlockRenderLoop) {
          dRLOG(L"Blocking render loop...");

          while (vBlockRenderLoop)
            vRenderLoopControl.wait_for(lControl, cfg.condWaitTimeout);
        }
      }

      std::unique_lock<std::mutex> lCmdAccessLock(vRenderLoopLockMutex);

      // Get present image (this command blocks)
      auto lNextImg = lSwapchain->acquireNextImage(lSemaphores[SEM_ACQUIRE]);

      if (!lNextImg) {
        eLOG(L"'vkAcquireNextImageKHR' returned ", uEnum2Str::toStr(lNextImg.getError()));
        continue;
      }

      //! \todo Update push constants here

      // Set CMD buffers
      lRenderSubmit[0].commandBufferCount = static_cast<uint32_t>(vCommandBufferRefs.frames[*lNextImg].pre.size());
      lRenderSubmit[0].pCommandBuffers    = vCommandBufferRefs.frames[*lNextImg].pre.data();
      lRenderSubmit[1].commandBufferCount = static_cast<uint32_t>(vCommandBufferRefs.frames[*lNextImg].render.size());
      lRenderSubmit[1].pCommandBuffers    = vCommandBufferRefs.frames[*lNextImg].render.data();
      lRenderSubmit[2].commandBufferCount = static_cast<uint32_t>(vCommandBufferRefs.frames[*lNextImg].post.size());
      lRenderSubmit[2].pCommandBuffers    = vCommandBufferRefs.frames[*lNextImg].post.data();

      // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  -->  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
      auto lRes = vkQueueSubmit(vQueue, 1, &lRenderSubmit[0], lFences[FENCE_IMG_1]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      // Render
      lRes = vkQueueSubmit(vQueue, 1, &lRenderSubmit[1], lFences[FENCE_RENDER]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  -->  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      lRes = vkQueueSubmit(vQueue, 1, &lRenderSubmit[2], lFences[FENCE_IMG_2]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      VkSwapchainKHR lTempSc   = **lSwapchain;
      uint32_t       lImgIndex = *lNextImg;

      lPresentInfo.pSwapchains   = &lTempSc;
      lPresentInfo.pImageIndices = &lImgIndex;
      lRes                       = vkQueuePresentKHR(vQueue, &lPresentInfo);
      if (lRes) {
        eLOG("'vkQueuePresentKHR' returned ", uEnum2Str::toStr(lRes));
        //         break;
      }


      // Wait until rendering is done
      lFences.wait(FENCE_RENDER, 1);

      //       // Update Uniforms
      //       for (auto const &i : vRenderers)
      //         i->updateUniforms();

      lFences.wait(FENCE_IMG_1, 1);
      lFences.wait(FENCE_IMG_2, 1);

      lFences.reset();

      lCmdAccessLock.unlock();


      vWorldPtr->signalRenderdFrame();
      vRenderedFrames++;
    }
    iLOG(L"Render loop stopped");


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

    // Sync point 3
    std::unique_lock<std::mutex> lControl(vRenderLoopControlMutex);

    dRLOG("Waiting for stop render loop signal");
    while (vStopRenderLoop != PASS) {
      vRenderLoopControl.wait_for(lControl, cfg.condWaitTimeout);

      if (!vRunRenderThread)
        return;
    }

    dRLOG("DONE Waiting for stop render loop signal");

    vStopRenderLoop = PASSED;
    vRenderLoopResponse.notify_all();
  }

  iLOG("Stopping render thread");
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

  // Sync point 1
  dRLOG("Start recording");
  vLoopStartCommand = PASS;
  vRunRenderLoop    = true;
  vRenderLoopControl.notify_all();

  while (vLoopStartCommand != PASSED)
    vRenderLoopResponse.wait_for(lControl, cfg.condWaitTimeout);

  vLoopStartCommand = BLOCK; // Reset
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

std::unique_lock<std::mutex> rRenderLoop::getRenderLoopLock() noexcept {
  std::unique_lock<std::mutex> lControl(vRenderLoopControlMutex);
  dRLOG(L"Telling the render loop to block");

  vBlockRenderLoop = true;
  std::unique_lock<std::mutex> lLock(vRenderLoopLockMutex); // Get the mutex first

  vBlockRenderLoop = false;
  vRenderLoopControl.notify_all(); // Tell the render thread to continue (if required)
  return lLock;
}

/*!
 * \brief Returns a pointer to the struct storing the command buffers used for rendering
 *
 * This function should be used to read / write the command buffers that should be rendered
 *
 * \note This function must be extrernally synchronized with getRenderLoopLock()
 */
internal::CommandBufferReferences *rRenderLoop::getCommandBufferReferences() noexcept { return &vCommandBufferRefs; }

uint64_t *rRenderLoop::getRenderedFramesPtr() { return &vRenderedFrames; }
bool      rRenderLoop::getIsRunning() const { return vRunRenderLoop; }
} // namespace e_engine
