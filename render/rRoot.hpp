/*!
 * \file rRoot.hpp
 * \brief \b Classes: \a rRoot
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
#include <unordered_map>
#include <mutex>
#include <vulkan/vulkan.h>
#include "rRoot_structs.hpp"
#include "uSignalSlot.hpp"

namespace e_engine {

class iEventInfo;
class iInit;

/*!
 * \brief root render class
 *
 * This class is responsible for
 *   - command pool handling
 *   - presenting images (todo)
 *   - eventually other stuff
 *
 * \warning An object of this class must be destroyed BEFORE the vulkan context is destroyed (= the
 *          iInit object is destroyed)!!!
 */
class RENDER_API rRoot {
 public:
   typedef internal::CommandPoolInfo PoolInfo;
   typedef std::unordered_map<uint32_t, VkImageLayout> AttachmentLayoutMap;

   typedef struct Buffer_vk {
      VkImage img        = nullptr;
      VkImageView iv     = nullptr;
      VkDeviceMemory mem = nullptr;
   } Buffer_vk;

   typedef struct Framebuffer_vk : Buffer_vk { VkFramebuffer fb = nullptr; } Framebuffer_vk;

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

      uint32_t frameAttachID;
      uint32_t depthAttachID;

      VkRenderPass renderPass = nullptr;
   } RenderPass_vk;

 private:
   iInit *vInitPtr;

   VkDevice vDevice_vk;
   VkSurfaceKHR vSurface_vk;
   VkSwapchainKHR vSwapchain_vk = nullptr;

   Buffer_vk vDepthStencilBuf_vk;
   RenderPass_vk vRenderPass_vk;

   std::vector<VkImageView> vAttachmentImageViews;
   std::vector<Framebuffer_vk> vFramebuffers_vk;

   VkSurfaceFormatKHR vSwapchainFormat = {VK_FORMAT_UNDEFINED, VK_COLORSPACE_MAX_ENUM};

   std::unordered_map<PoolInfo, VkCommandPool> vCmdPools_vk;
   std::mutex vCommandPoolsMutex;

   uSlot<void, rRoot, iEventInfo const &> vResizeSlot;

   bool vHasStencilBuffer  = false;
   bool vIsResizeSlotSetup = false;

   int recreateSwapchain();
   int recreateSwapchainImages( VkCommandBuffer _buf );
   int recreateDepthAndStencilBuffer( VkCommandBuffer _buf );
   int recreateRenderPass();
   int recreateFramebuffers();

   void handleResize( iEventInfo const & );

 public:
   rRoot() = delete;
   rRoot( iInit *_init );
   virtual ~rRoot();

   int initBasic();
   int initRenderPass();
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

   VkCommandBuffer createCommandBuffer(
         VkCommandPool _pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY );

   VkResult beginCommandBuffer( VkCommandBuffer _buf, VkCommandBufferUsageFlags _flags = 0 );

   void cmdChangeImageLayout( VkCommandBuffer _cmdBuffer,
                              VkImage _img,
                              VkImageSubresourceRange _imgSubres,
                              VkImageLayout _src,
                              VkImageLayout _dst );

   VkCommandPool getCommandPool(
         uint32_t _queueFamilyIndex,
         VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

   VkCommandPool getCommandPoolFlags(
         VkQueueFlags _qFlags,
         VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

   VkFence createFence( VkFenceCreateFlags _flags = 0 );
};
}
