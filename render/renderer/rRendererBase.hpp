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

#include <condition_variable>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>
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
class RENDER_API rRendererBase {
 public:
   typedef std::unordered_map<uint32_t, VkImageLayout> AttachmentLayoutMap;

   typedef struct Buffer_vk {
      VkImage img        = nullptr;
      VkImageView iv     = nullptr;
      VkDeviceMemory mem = nullptr;
   } Buffer_vk;

   struct AttachmentInfo {
      VkFormat format;
      VkImageUsageFlags usage;
      VkImageLayout layout;
      VkImageTiling tiling;
      VkImageAspectFlags aspect;
      uint32_t attachmentID;
   };

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
      std::vector<Buffer_vk *> attachmentBuffers; //!< Correctly indexed reference to vBuffers

      VkRenderPass renderPass = nullptr;
   } RenderPass_vk;

   typedef struct Framebuffer_vk : Buffer_vk {
      uint32_t index             = 0;
      VkFramebuffer fb           = nullptr;
      VkCommandBuffer preRender  = nullptr;
      VkCommandBuffer render     = nullptr;
      VkCommandBuffer postRender = nullptr;
   } Framebuffer_vk;

   typedef struct RecordInfo_vk {
      VkRenderPassBeginInfo lRPInfo           = {};
      VkViewport lViewPort                    = {};
      VkRect2D lScissors                      = {};
      VkCommandBufferInheritanceInfo lInherit = {};
   } RecordInfo_vk;

   using OBJECTS = std::vector<std::shared_ptr<rObjectBase>>;

   enum RECORD_TARGET { RECORD_ALL, RECORD_PUSH_CONST_ONLY };
   enum ATTACHMENT_ROLE { DEPTH_STENCIL, DEFERRED_POSITION, DEFERRED_NORMAL, DEFERRED_ALBEDO };

 private:
   static uint64_t vRenderedFrames;

   std::wstring vID;

   std::vector<Framebuffer_vk> vFramebuffers_vk;
   std::vector<Buffer_vk> vBuffers;

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

   int initImageBuffers( VkCommandBuffer _buf );
   int initRenderPass();
   int initFramebuffers();

   void initFrameCommandBuffers( VkCommandPool _pool );
   void freeFrameCommandBuffers( VkCommandPool _pool );

   void recordCmdBuffersWrapper( Framebuffer_vk &_fb, RECORD_TARGET _toRender );

   void renderLoop();

 protected:
   enum PREDEFINED_ATTACHMENT_INDEXES {
      FRAMEBUFFER_ATTACHMENT_INDEX = 0,
      FIRST_FREE_ATTACHMENT_INDEX
   };

   iInit *vInitPtr;
   rWorld *vWorldPtr;

   RenderPass_vk vRenderPass_vk;

   VkDevice vDevice_vk;

   RecordInfo_vk vCmdRecordInfo;

   OBJECTS vObjects;

   virtual std::vector<AttachmentInfo> getAttachmentInfos() = 0;
   virtual void setupSubpasses()                            = 0;
   uint32_t addSubpass( VkPipelineBindPoint _bindPoint,
                        uint32_t _deptStencil           = UINT32_MAX,
                        std::vector<uint32_t> _color    = {UINT32_MAX},
                        std::vector<uint32_t> _input    = {},
                        std::vector<uint32_t> _preserve = {},
                        std::vector<uint32_t> _resolve  = {},
                        AttachmentLayoutMap _layoutMap  = {} );

   void addSubpassDependecy( uint32_t _srcSubPass,
                             uint32_t _dstSubPass,
                             uint32_t _srcStageMask    = 0,
                             uint32_t _dstStageMask    = 0,
                             uint32_t _srcAccessMask   = 0,
                             uint32_t _dstAccessMask   = 0,
                             uint32_t _dependencyFlags = 0 );

   virtual void initCmdBuffers( VkCommandPool _pool, uint32_t _numFramebuffers ) = 0;
   virtual void freeCmdBuffers( VkCommandPool _pool ) = 0;
   virtual void recordCmdBuffers( Framebuffer_vk &_fb, RECORD_TARGET _toRender ) = 0;

 public:
   rRendererBase() = delete;
   rRendererBase( iInit *_init, rWorld *_root, std::wstring _id );
   rRendererBase( const rRendererBase &_obj ) = delete;
   rRendererBase( rRendererBase && )          = delete;
   rRendererBase &operator=( const rRendererBase & ) = delete;
   rRendererBase &operator=( rRendererBase && ) = delete;
   virtual ~rRendererBase();

   virtual VkImageView getAttachmentView( ATTACHMENT_ROLE _role ) = 0;

   bool addObject( std::shared_ptr<rObjectBase> _obj );
   bool resetObjects();

   int init();
   void destroy();

   bool applyChanges();

   bool start();
   bool stop();
   bool getIsRunning() const;
   bool getIsInit() const;

   void setClearColor( VkClearColorValue _clearColor );

   void getDepthFormat( VkFormat &_format, VkImageTiling &_tiling, VkImageAspectFlags &_aspect );

   uint64_t *getRenderedFramesPtr();
};
}
}
