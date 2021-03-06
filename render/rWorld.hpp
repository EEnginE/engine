/*!
 * \file rWorld.hpp
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

#pragma once

#include "defines.hpp"
#include "uSignalSlot.hpp"
#include "vkuDevice.hpp"
#include "vkuSwapChain.hpp"
#include "rRenderLoop.hpp"
#include "rRendererBase.hpp"
#include <condition_variable>
#include <mutex>
#include <unordered_map>
#include <vulkan.h>

#include <type_traits>

namespace e_engine {

struct iEventInfo;
class iInit;
class rSceneBase;

/*!
 * \brief root render class
 *
 * This class manages
 *   - the renderers
 *   - the render loop
 *
 * This class handles two render class instances. A front and a back renderer. This allows modifying
 * and setting up the back rendere without impacting the performance to mutch.
 *
 * \warning An object of this class must be destroyed BEFORE the vulkan context is destroyed (= the
 *          iInit object is destroyed)!!!
 */
class rWorld {
 public:
  struct ViewPort {
    bool vNeedUpdate_B;
    int  x;
    int  y;
    int  width;
    int  height;
  };

  struct ClearColor {
    bool  vNeedUpdate_B;
    float r;
    float g;
    float b;
    float a;
  };

 private:
  iInit *vInitPtr;

  vkuDevicePTR vDevice;
  VkDevice     vDevice_vk; //!< \brief Shortcut for **vDevice \todo Evaluate elimenating this.
  VkSurfaceKHR vSurface_vk;

  std::vector<std::shared_ptr<rRendererBase>> vRenderers;

  vkuSwapChain vSwapChain;

  ViewPort   vViewPort;
  ClearColor vClearColor;

  rRenderLoop vRenderLoop;

  std::mutex              vRenderAccessMutex;
  std::condition_variable vRenderedFrameSignal;

  uSlot<void, rWorld, iEventInfo const &> vResizeSlot;

  bool vIsResizeSlotSetup = false;
  bool vIsSetup           = false;

  int  initRenderers();
  void destroyRenderers();
  void rebuildSubmitInfos();

  void handleResize(iEventInfo const &);

 public:
  rWorld() = delete;
  rWorld(iInit *_init);
  virtual ~rWorld();

  int  init();
  void shutdown();
  void rebuildRenderers();

  bool isSetup() { return vIsSetup; }
  bool waitForFrame(std::mutex &_mutex);

  void addRenderer(std::shared_ptr<rRendererBase> _renderer);
  void removeRenderer(std::shared_ptr<rRendererBase> _renderer);
  void clearRenderers();

  // Begin Low level Vulkan section

  void          updateViewPort(int _x, int _y, int _width, int _height);
  void          updateClearColor(float _r, float _g, float _b, float _a);
  uint64_t *    getRenderedFramesPtr();
  vkuDevicePTR  getDevice();
  iInit *       getInitPtr();
  rRenderLoop * getRenderLoop();
  vkuSwapChain *getSwapChain();
};
} // namespace e_engine
