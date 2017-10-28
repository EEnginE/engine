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
#include <mutex>
#include <thread>
#include <vulkan.h>

namespace e_engine {

class rWorld;

namespace internal {

struct CommandBufferReferences {
  struct CommandBuffers {
    std::vector<VkCommandBuffer> pre;
    std::vector<VkCommandBuffer> render;
    std::vector<VkCommandBuffer> post;
  };

  std::vector<CommandBuffers> frames;
};

} // namespace internal

class rRenderLoop {
 public:
  enum LoopCommand { BLOCK, PASS, PASSED };


 private:
  static uint64_t                   vRenderedFrames;
  internal::CommandBufferReferences vCommandBufferRefs;

  rWorld *vWorldPtr;

  VkDevice     vDevice_vk; //!< \brief Shortcut for **vDevice \todo Evaluate elimenating this.
  vkuDevicePTR vDevice;

  bool vRunRenderLoop   = false;
  bool vRunRenderThread = true;
  bool vBlockRenderLoop = false;

  VkQueue  vQueue            = VK_NULL_HANDLE;
  uint32_t vQueueFamilyIndex = 0;

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
  rRenderLoop(rWorld *_root);
  rRenderLoop(const rRenderLoop &_obj) = delete;
  rRenderLoop(rRenderLoop &&)          = delete;
  rRenderLoop &operator=(const rRenderLoop &) = delete;
  rRenderLoop &operator=(rRenderLoop &&) = delete;
  virtual ~rRenderLoop();

  bool start();
  bool stop();
  bool getIsRunning() const;

  uint64_t *      getRenderedFramesPtr();
  inline uint32_t getQueueFamilyIndex() const noexcept { return vQueueFamilyIndex; }

  internal::CommandBufferReferences *getCommandBufferReferences() noexcept;
  std::unique_lock<std::mutex>       getRenderLoopLock() noexcept;
};

} // namespace e_engine
