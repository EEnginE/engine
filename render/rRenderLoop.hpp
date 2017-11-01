/*!
 * \file rRenderLoop.hpp
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

#pragma once

#include "defines.hpp"
#include "vkuDevice.hpp"
#include "rRendererBase.hpp"
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <vulkan.h>

namespace e_engine {

namespace internal {

struct SubmitInfos {
  struct Infos {
    std::vector<VkSubmitInfo> inf;
  };

  std::vector<Infos> frames;
};

} // namespace internal

class rRenderLoop {
 public:
  enum LoopCommand { BLOCK, PASS, PASSED };
  typedef std::function<void(uint32_t)> CallBackInt;
  typedef std::function<void()>         CallBackVoid;


 private:
  uint64_t              vRenderedFrames = 0;
  internal::SubmitInfos vSubmitInfos;

  VkDevice     vDevice_vk; //!< \brief Shortcut for **vDevice \todo Evaluate elimenating this.
  vkuDevicePTR vDevice;

  vkuSwapChain *vSwapChain;

  CallBackVoid vRenderedFrameCB;
  CallBackInt  vUpdatePushConstantsCB;

  bool vRunRenderLoop   = false;
  bool vRunRenderThread = true;
  bool vBlockRenderLoop = false;

  VkQueue  vQueue             = VK_NULL_HANDLE;
  VkQueue  vPresentQueue      = VK_NULL_HANDLE;
  uint32_t vQueueIndex        = 0;
  uint32_t vPresentQueueIndex = 0;

  std::thread vRenderThread;

  std::recursive_mutex vLoopAccessMutex;
  std::mutex           vRenderLoopControlMutex;
  std::mutex           vRenderLoopLockMutex;

  std::condition_variable vRenderLoopControl;
  std::condition_variable vRenderLoopResponse;

  LoopCommand vLoopStartCommand = BLOCK;
  LoopCommand vStopRenderLoop   = BLOCK;

  struct Config {
    std::chrono::milliseconds condWaitTimeout = std::chrono::milliseconds(100);
  } cfg;

  void renderLoop();

 public:
  rRenderLoop() = delete;
  rRenderLoop(vkuDevicePTR _device, vkuSwapChain *_swapChain, CallBackVoid _renderedFrame, CallBackInt _updatePC);
  rRenderLoop(const rRenderLoop &_obj) = delete;
  rRenderLoop(rRenderLoop &&)          = delete;
  rRenderLoop &operator=(const rRenderLoop &) = delete;
  rRenderLoop &operator=(rRenderLoop &&) = delete;
  virtual ~rRenderLoop();

  bool start();
  bool stop();
  bool getIsRunning() const;

  uint64_t *      getRenderedFramesPtr();
  inline uint32_t getQueueFamilyIndex() const noexcept { return vQueueIndex; }

  internal::SubmitInfos *      getCommandBufferReferences() noexcept;
  std::unique_lock<std::mutex> getRenderLoopLock() noexcept;
};

} // namespace e_engine
