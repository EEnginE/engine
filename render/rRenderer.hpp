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
#include <vulkan.h>

namespace e_engine {

class iInit;
class rRoot;

class rRenderer {
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
      std::vector<VkSubpassDescription> subpasses;
      std::vector<VkSubpassDependency> dependecies;

      std::vector<VkImageView> attachmentViews;

      uint32_t frameAttachID;
      uint32_t depthAttachID;

      VkRenderPass renderPass = nullptr;
   } RenderPass_vk;

   typedef struct Framebuffer_vk : Buffer_vk { VkFramebuffer fb = nullptr; } Framebuffer_vk;

 private:
   iInit *vInitPtr;
   rRoot *vRootPtr;

   VkDevice vDevice_vk;
   Buffer_vk vDepthStencilBuf_vk;
   RenderPass_vk vRenderPass_vk;
   std::vector<Framebuffer_vk> vFramebuffers_vk;

   bool vHasStencilBuffer = false;
   bool vIsSetup          = false;

   int initDepthAndStencilBuffer( VkCommandBuffer _buf );
   int initRenderPass();
   int initFramebuffers();

   void renderLoop();

 public:
   rRenderer() = delete;
   rRenderer( iInit *_init, rRoot *_root );

   void defaultSetup();

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
};
}
