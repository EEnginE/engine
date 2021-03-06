/*!
 * \file rRendererBase.hpp
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

#pragma once

#include "defines.hpp"
#include "vkuCommandPool.hpp"
#include "vkuDevice.hpp"
#include "vkuSwapChain.hpp"
#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
#include <vulkan.h>

namespace e_engine {

class rWorld;
class rObjectBase;
class rRenderLoop;
class rSceneBase;

/*!
 * \brief Main render class
 *
 * \todo Multiple viewports scissors
 */
class rRendererBase {
 public:
  typedef std::unordered_map<uint32_t, VkImageLayout> AttachmentLayoutMap;
  typedef std::vector<vkuSwapChain::SwapChainImg>     SwapChainImages;

  typedef struct RecordInfo_vk {
    VkRenderPassBeginInfo          lRPInfo   = {};
    VkViewport                     lViewPort = {};
    VkRect2D                       lScissors = {};
    VkCommandBufferInheritanceInfo lInherit  = {};
  } RecordInfo_vk;

  struct SubmitInfo {
    struct FBInfo {
      std::vector<VkSubmitInfo> submitInfos;
    };

    std::vector<FBInfo> fb;     //!< Submit infos. Must be empty or have an element for each swapchain image
    FBInfo              shared; //!< Submit info for single framebuffer systems.
  };

  using OBJECTS = std::vector<std::shared_ptr<rObjectBase>>;

  enum RECORD_TARGET { RECORD_ALL, RECORD_PUSH_CONST_ONLY };
  enum ATTACHMENT_ROLE { DEPTH_STENCIL, DEFERRED_POSITION, DEFERRED_NORMAL, DEFERRED_ALBEDO };

 private:
  std::wstring vID;

  std::recursive_mutex vMutexRecordData;

  SwapChainImages vImages;

  VkClearColorValue vClearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};

  bool vIsSetup               = false;
  bool vEnableRendering       = false;
  bool vUserDisabledRendering = false; //!< User manually disabled rendering

  void recordCmdBuffersWrapper(uint32_t &_fbIndex, RECORD_TARGET _toRender);

 protected:
  enum PREDEFINED_ATTACHMENT_INDEXES { FRAMEBUFFER_ATTACHMENT_INDEX = 0, FIRST_FREE_ATTACHMENT_INDEX };

  rWorld *vWorldPtr;

  VkDevice     vDevice_vk; //!< \brief Shortcut for **vDevice \todo Evaluate elimenating this.
  vkuDevicePTR vDevice;

  RecordInfo_vk vCmdRecordInfo;

  OBJECTS vObjects;

  virtual VkResult initRenderer(SwapChainImages _images, VkSurfaceFormatKHR _surfaceFormat, vkuCommandPool *_pool) = 0;
  virtual void     destroyRenderer()                                                                               = 0;
  virtual void     recordCmdBuffers(uint32_t &_fbIndex, RECORD_TARGET _toRender)                                   = 0;

  virtual VkRenderPass              getRenderPass()                   = 0;
  virtual VkFramebuffer             getFrameBuffer(uint32_t _fbIndex) = 0;
  virtual std::vector<VkClearValue> getClearValues()                  = 0;

  virtual bool initRendererData() { return true; }
  virtual bool freeRendererData() { return true; }

 public:
  rRendererBase() = delete;
  rRendererBase(rWorld *_root, std::wstring _id);
  rRendererBase(const rRendererBase &_obj) = delete;
  rRendererBase(rRendererBase &&)          = delete;
  rRendererBase &operator=(const rRendererBase &) = delete;
  rRendererBase &operator=(rRendererBase &&) = delete;
  virtual ~rRendererBase();

  virtual VkImageView getAttachmentView(ATTACHMENT_ROLE _role) = 0;
  virtual SubmitInfo  getVulkanSubmitInfos()                   = 0;

  bool renderScene(rSceneBase *_scene);
  bool addObject(std::shared_ptr<rObjectBase> _obj);
  bool resetObjects();

  int  init(vkuCommandPool *_pool);
  void destroy();

  void disableRendering();
  bool enableRendering();
  bool getIsRenderingEnabled() const;

  void updateUniforms();

  bool getIsInit() const;

  void setClearColor(VkClearColorValue _clearColor);

  uint32_t getNumFramebuffers() const;

  void updateRenderer();
  void updatePushConstants(uint32_t _framebuffer);
};
} // namespace e_engine
