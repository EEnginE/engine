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
#include "vkuImageBuffer.hpp"
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

struct PresentImageLayoutChange {
  struct Info {
    VkSubmitInfo         submitInfo;
    vkuCommandBuffer     cmdBuffer;
    VkImageMemoryBarrier barrier;
  };

  std::array<Info, 2>  infos;
  VkPipelineStageFlags lSubmitWaitFlags = VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
};

enum class Semaphores : uint32_t { ACQUIRE = 0, PRESENT, NUM };
enum class Fences : uint32_t { RENDER = 0, IMG1, IMG2, NUM };

typedef vkuSemaphores<static_cast<uint32_t>(Semaphores::NUM)> LoopSemaphores;
typedef vkuFences<static_cast<uint32_t>(Fences::NUM)>         LoopFences;

std::vector<PresentImageLayoutChange> rebuildPresentInfo(vkuCommandPool *_pool,
                                                         vkuSwapChain *  _swapChain,
                                                         LoopSemaphores &_semaphores,
                                                         uint32_t        _renderQueue,
                                                         uint32_t        _presetnQueue);

rRenderLoop::rRenderLoop(vkuDevicePTR  _device,
                         vkuSwapChain *_swapChain,
                         CallBackVoid  _renderedFrame,
                         CallBackInt   _updatePC) {
  vRenderThread          = std::thread(&rRenderLoop::renderLoop, this);
  vDevice                = _device;
  vDevice_vk             = **vDevice;
  vSwapChain             = _swapChain;
  vRenderedFrameCB       = _renderedFrame;
  vUpdatePushConstantsCB = _updatePC;
  vQueue                 = vDevice->getQueue(VK_QUEUE_GRAPHICS_BIT, 1.0, &vQueueIndex);
  vPresentQueue          = vDevice->getQueue(0, 0.25, &vPresentQueueIndex, true);
}

rRenderLoop::~rRenderLoop() {
  if (vRunRenderLoop)
    stop();

  vRunRenderThread = false;

  if (vRenderThread.joinable())
    vRenderThread.join();
}

std::vector<PresentImageLayoutChange> rebuildPresentInfo(vkuCommandPool *_pool,
                                                         vkuSwapChain *  _swapChain,
                                                         LoopSemaphores &_semaphores,
                                                         uint32_t        _renderQueue,
                                                         uint32_t        _presetnQueue) {
  std::vector<PresentImageLayoutChange> lPresentInfo;
  auto                                  lImages  = _swapChain->getImages();
  auto                                  lSwapCfg = _swapChain->getConfig();
  lPresentInfo.resize(lImages.size());

  for (uint32_t i = 0; i < lImages.size(); ++i) {
    PresentImageLayoutChange::Info &acquire = lPresentInfo[i].infos[static_cast<uint32_t>(Semaphores::ACQUIRE)];
    PresentImageLayoutChange::Info &present = lPresentInfo[i].infos[static_cast<uint32_t>(Semaphores::PRESENT)];

    acquire.cmdBuffer.init(_pool);
    present.cmdBuffer.init(_pool);

    acquire.barrier = vkuImageBuffer::generateLayoutChangeBarrier(lImages[i].img,
                                                                  lSwapCfg.subResRange,
                                                                  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                                  _presetnQueue,
                                                                  _renderQueue);

    present.barrier = vkuImageBuffer::generateLayoutChangeBarrier(lImages[i].img,
                                                                  lSwapCfg.subResRange,
                                                                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                                                  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                                                  _renderQueue,
                                                                  _presetnQueue);

    VkPipelineStageFlags lFlags = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

    acquire.cmdBuffer.begin();
    vkCmdPipelineBarrier(*acquire.cmdBuffer, lFlags, lFlags, 0, 0, nullptr, 0, nullptr, 1, &acquire.barrier);
    acquire.cmdBuffer.end();

    present.cmdBuffer.begin();
    vkCmdPipelineBarrier(*present.cmdBuffer, lFlags, lFlags, 0, 0, nullptr, 0, nullptr, 1, &present.barrier);
    present.cmdBuffer.end();

    acquire.submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    acquire.submitInfo.pNext                = nullptr;
    acquire.submitInfo.waitSemaphoreCount   = 1;
    acquire.submitInfo.pWaitSemaphores      = &_semaphores[static_cast<uint32_t>(Semaphores::ACQUIRE)];
    acquire.submitInfo.pWaitDstStageMask    = &lPresentInfo[i].lSubmitWaitFlags;
    acquire.submitInfo.commandBufferCount   = 1;
    acquire.submitInfo.pCommandBuffers      = &acquire.cmdBuffer.get();
    acquire.submitInfo.signalSemaphoreCount = 0;
    acquire.submitInfo.pSignalSemaphores    = nullptr;

    present.submitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    present.submitInfo.pNext                = nullptr;
    present.submitInfo.waitSemaphoreCount   = 0;
    present.submitInfo.pWaitSemaphores      = nullptr;
    present.submitInfo.pWaitDstStageMask    = nullptr;
    present.submitInfo.commandBufferCount   = 1;
    present.submitInfo.pCommandBuffers      = &present.cmdBuffer.get();
    present.submitInfo.signalSemaphoreCount = 1;
    present.submitInfo.pSignalSemaphores    = &_semaphores[static_cast<uint32_t>(Semaphores::PRESENT)];
  }

  return lPresentInfo;
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

    LoopFences      lFences(vDevice_vk);
    LoopSemaphores  lSemaphores(vDevice_vk);
    vkuCommandPool *lPool = vkuCommandPoolManager::get(vDevice_vk, vQueueIndex);

    std::vector<PresentImageLayoutChange> lLayoutChangeSubmitInfo;

    VkPresentInfoKHR lPresentInfo   = {};
    lPresentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    lPresentInfo.pNext              = nullptr;
    lPresentInfo.waitSemaphoreCount = 1;
    lPresentInfo.pWaitSemaphores    = &lSemaphores[static_cast<uint32_t>(Semaphores::PRESENT)];
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
      auto lNextImg = vSwapChain->acquireNextImage(lSemaphores[static_cast<uint32_t>(Semaphores::ACQUIRE)]);

      if (!lNextImg) {
        eLOG(L"'vkAcquireNextImageKHR' returned ", uEnum2Str::toStr(lNextImg.getError()));
        continue;
      }

      // Check if the layout chang info is up to data
      if (lLayoutChangeSubmitInfo.size() != vSwapChain->getNumImages() ||
          lLayoutChangeSubmitInfo[*lNextImg].infos[0].barrier.image != vSwapChain->getImage(*lNextImg).img) {

        dVkLOG(L"Resetting present layout change structures");
        lLayoutChangeSubmitInfo = rebuildPresentInfo(lPool, vSwapChain, lSemaphores, vQueueIndex, vPresentQueueIndex);
      }

      auto &lLayoutAcquire = lLayoutChangeSubmitInfo[*lNextImg].infos[static_cast<uint32_t>(Semaphores::ACQUIRE)];
      auto &lLayoutPresent = lLayoutChangeSubmitInfo[*lNextImg].infos[static_cast<uint32_t>(Semaphores::PRESENT)];


      vUpdatePushConstantsCB(*lNextImg);


      // Render everything here

      // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  -->  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
      auto lRes = vkQueueSubmit(vQueue, 1, &lLayoutAcquire.submitInfo, lFences[static_cast<uint32_t>(Fences::IMG1)]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      // Render
      uint32_t      lNumSubmitInfo = static_cast<uint32_t>(vSubmitInfos.frames[*lNextImg].inf.size());
      VkSubmitInfo *lSubmitInfo    = vSubmitInfos.frames[*lNextImg].inf.data();
      lRes = vkQueueSubmit(vQueue, lNumSubmitInfo, lSubmitInfo, lFences[static_cast<uint32_t>(Fences::RENDER)]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }

      // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  -->  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
      lRes = vkQueueSubmit(vQueue, 1, &lLayoutPresent.submitInfo, lFences[static_cast<uint32_t>(Fences::IMG2)]);
      if (lRes) {
        eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
        break;
      }



      VkSwapchainKHR lTempSc   = **vSwapChain;
      uint32_t       lImgIndex = *lNextImg;

      lPresentInfo.pSwapchains   = &lTempSc;
      lPresentInfo.pImageIndices = &lImgIndex;
      lRes                       = vkQueuePresentKHR(vPresentQueue, &lPresentInfo);
      if (lRes) {
        eLOG("'vkQueuePresentKHR' returned ", uEnum2Str::toStr(lRes));
        //         break;
      }


      // Wait until rendering is done
      lFences.wait(static_cast<uint32_t>(Fences::RENDER), 1);

      //       // Update Uniforms
      //       for (auto const &i : vRenderers)
      //         i->updateUniforms();

      lFences.wait(static_cast<uint32_t>(Fences::IMG1), 1);
      lFences.wait(static_cast<uint32_t>(Fences::IMG2), 1);

      lFences.reset();

      lCmdAccessLock.unlock();


      vRenderedFrameCB();
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
internal::SubmitInfos *rRenderLoop::getCommandBufferReferences() noexcept { return &vSubmitInfos; }

uint64_t *rRenderLoop::getRenderedFramesPtr() { return &vRenderedFrames; }
bool      rRenderLoop::getIsRunning() const { return vRunRenderLoop; }
} // namespace e_engine
