/*!
 * \file rRenderer.hpp
 * \brief \b Classes: \a rRenderer
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

#include <vector>
#include <unordered_map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vulkan.h>

namespace e_engine {

class iInit;
class rWorld;
class rObjectBase;

namespace internal {

/*!
 * \brief Main render class
 *
 * \todo Multiple viewports scissors
 */
class RENDER_API rRenderer {
 public:
   typedef std::unordered_map<uint32_t, VkImageLayout> AttachmentLayoutMap;

   typedef struct Buffer_vk {
      VkImage img        = nullptr;
      VkImageView iv     = nullptr;
      VkDeviceMemory mem = nullptr;
   } Buffer_vk;

   typedef struct RenderPass_vk {
      struct SubPassData {
         std::vector<uint32_t> preserve;
         std::vector<VkAttachmentReference> color;
         std::vector<VkAttachmentReference> input;
         std::vector<VkAttachmentReference> resolve;
         VkAttachmentReference depth = {VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED};
      };

      std::vector<SubPassData> data;
      std::vector<VkAttachmentDescription> attachments;
      std::vector<VkClearValue> clearValues;
      std::vector<VkSubpassDescription> subpasses;
      std::vector<VkSubpassDependency> dependecies;

      std::vector<VkImageView> attachmentViews;

      uint32_t frameAttachID;
      uint32_t depthAttachID;

      VkRenderPass renderPass = nullptr;
   } RenderPass_vk;

   typedef struct Framebuffer_vk : Buffer_vk {
      VkFramebuffer fb           = nullptr;
      VkCommandBuffer preRender  = nullptr;
      VkCommandBuffer render     = nullptr;
      VkCommandBuffer postRender = nullptr;

      std::vector<VkCommandBuffer> secondary;
   } Framebuffer_vk;

   typedef struct RecordInfo_vk {
      VkRenderPassBeginInfo lRPInfo           = {};
      VkViewport lViewPort                    = {};
      VkRect2D lScissors                      = {};
      VkCommandBufferInheritanceInfo lInherit = {};
   } RecordInfo_vk;

   using OBJECTS = std::vector<std::shared_ptr<rObjectBase>>;

 private:
   static uint64_t vRenderedFrames;

   iInit *vInitPtr;
   rWorld *vWorldPtr;

   std::wstring vID;

   VkDevice vDevice_vk;
   Buffer_vk vDepthStencilBuf_vk;
   RenderPass_vk vRenderPass_vk;
   std::vector<Framebuffer_vk> vFramebuffers_vk;

   RecordInfo_vk vCmdRecordInfo;
   OBJECTS vObjects;

   std::thread vRenderThread;

   std::mutex vMutexStartRecording;
   std::mutex vMutexFinishedRecording;
   std::mutex vMutexStartLogLoop;
   std::mutex vMutexStopLogLoop;

   std::condition_variable vVarStartRecording;
   std::condition_variable vVarFinishedRecording;
   std::condition_variable vVarStartLogLoop;
   std::condition_variable vVarStopLogLoop;

   VkClearColorValue vClearColor = {{0.0f, 0.0f, 0.0f, 1.0f}};

   bool vHasStencilBuffer  = false;
   bool vIsSetup           = false;
   bool vFinishedRecording = false;
   bool vRunRenderThread   = true;
   bool vRunRenderLoop     = false;

   int initDepthAndStencilBuffer( VkCommandBuffer _buf );
   int initRenderPass();
   int initFramebuffers();

   void initFrameCommandBuffers( VkCommandPool _pool, uint32_t _numSecondary );
   void freeFrameCommandBuffers( VkCommandPool _pool );

   void recordCmdBuffers( Framebuffer_vk &_fb, OBJECTS &_toRender );

   void renderLoop();

 public:
   rRenderer() = delete;
   rRenderer( iInit *_init, rWorld *_root, std::wstring _id );
   rRenderer( const rRenderer &_obj ) = delete;
   rRenderer( rRenderer && ) = delete;
   rRenderer &operator=( const rRenderer & ) = delete;
   rRenderer &operator=( rRenderer && ) = delete;
   virtual ~rRenderer();

   void defaultSetup();
   bool addObject( std::shared_ptr<rObjectBase> _obj );
   bool resetObjects();

   uint32_t getDepthBufferAttachmentIndex() const;
   uint32_t getFrameBufferAttachmentIndex() const;

   uint32_t addSubpass( VkPipelineBindPoint _bindPoint,
                        uint32_t _deptStencil           = UINT32_MAX,
                        std::vector<uint32_t> _color    = {UINT32_MAX},
                        std::vector<uint32_t> _input    = {},
                        std::vector<uint32_t> _preserve = {},
                        std::vector<uint32_t> _resolve  = {},
                        AttachmentLayoutMap _layoutMap = {} );

   int init();
   void destroy();

   bool applyChanges();

   bool start();
   bool stop();
   bool getIsRunning() const;
   bool getIsInit() const;

   void setClearColor( VkClearColorValue _clearColor );

   uint64_t *getRenderedFramesPtr();
};
}
}
