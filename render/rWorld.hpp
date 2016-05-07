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
#include <unordered_map>
#include <mutex>
#include <condition_variable>
#include <vulkan/vulkan.h>
#include "rWorld_structs.hpp"
#include "rRendererBase.hpp"
#include "uSignalSlot.hpp"

#include <type_traits>

namespace e_engine {

class iEventInfo;
class iInit;
class rSceneBase;

/*!
 * \brief root render class
 *
 * This class manages
 *   - the renderers
 *
 * This class is responsible for (Vulkan)
 *   - command pool handling
 *   - presenting images (todo)
 *   - eventually other stuff
 *
 * This class handles two render class instances. A front and a back renderer. This allows modifying
 * and setting up the back rendere without impacting the performance to mutch.
 *
 * \warning An object of this class must be destroyed BEFORE the vulkan context is destroyed (= the
 *          iInit object is destroyed)!!!
 *
 * \note When creating rWorld objects (or inheritance) use the (template) wrapper class
 * rWorldCreator
 */
class RENDER_API rWorld {
 public:
   typedef internal::CommandPoolInfo PoolInfo;

   struct ViewPort {
      bool vNeedUpdate_B;
      int x;
      int y;
      int width;
      int height;
   };

   struct ClearColor {
      bool vNeedUpdate_B;
      float r;
      float g;
      float b;
      float a;
   };

   struct SwapChainImg {
      VkImage img;
      VkImageView iv;
   };

 private:
   iInit *vInitPtr;

   VkDevice vDevice_vk;
   VkSurfaceKHR vSurface_vk;
   VkSwapchainKHR vSwapchain_vk = nullptr;

   std::vector<VkImage> vSwapchainImages_vk;
   std::vector<VkImageView> vSwapchainViews_vk;

   ViewPort vViewPort;
   ClearColor vClearColor;

   internal::rRendererBase *vFrontRenderer = nullptr;
   internal::rRendererBase *vBackRenderer  = nullptr;

   VkSurfaceFormatKHR vSwapchainFormat = {VK_FORMAT_UNDEFINED, VK_COLOR_SPACE_MAX_ENUM_KHR};

   std::unordered_map<PoolInfo, VkCommandPool> vCmdPools_vk;
   std::mutex vCommandPoolsMutex;
   std::mutex vRenderAccessMutex;
   std::condition_variable vRenderedFrameSignal;

   uSlot<void, rWorld, iEventInfo const &> vResizeSlot;

   bool vIsResizeSlotSetup = false;
   bool vIsSetup           = false;

   int recreateSwapchain();
   int recreateSwapchainImages( VkCommandBuffer _buf );

   void handleResize( iEventInfo const & );

   VkSwapchainKHR getSwapchain();
   std::vector<SwapChainImg> getSwapchainImageViews();
   VkSurfaceFormatKHR getSwapchainFormat();
   inline void signalRenderdFrame() { vRenderedFrameSignal.notify_all(); }

   virtual void setRendererPtr( internal::rRendererBase **_r1, internal::rRendererBase **_r2 ) = 0;

 public:
   rWorld() = delete;
   rWorld( iInit *_init );
   virtual ~rWorld();

   int init();
   bool renderScene( rSceneBase *_scene );
   void shutdown();

   bool isSetup() { return vIsSetup; }
   bool waitForFrame( std::mutex &_mutex );


   // Begin Low level Vulkan section

   VkCommandBuffer createCommandBuffer(
         VkCommandPool _pool, VkCommandBufferLevel _level = VK_COMMAND_BUFFER_LEVEL_PRIMARY );

   VkResult beginCommandBuffer( VkCommandBuffer _buf,
                                VkCommandBufferUsageFlags _flags      = 0,
                                VkCommandBufferInheritanceInfo *_info = nullptr );

   void cmdChangeImageLayout( VkCommandBuffer _cmdBuffer,
                              VkImage _img,
                              VkImageSubresourceRange _imgSubres,
                              VkImageLayout _src,
                              VkImageLayout _dst,
                              VkPipelineStageFlags _srcFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                              VkPipelineStageFlags _dstFlags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );

   VkCommandPool getCommandPool(
         uint32_t _queueFamilyIndex,
         VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

   VkCommandPool getCommandPoolFlags(
         VkQueueFlags _qFlags,
         VkCommandPoolCreateFlags _flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT );

   VkFence createFence( VkFenceCreateFlags _flags = 0 );
   VkSemaphore createSemaphore();

   void updateViewPort( int _x, int _y, int _width, int _height );
   void updateClearColor( float _r, float _g, float _b, float _a );
   uint64_t *getRenderedFramesPtr();
   VkDevice getDevice();
   iInit *getInitPtr();

   friend class internal::rRendererBase;
};


template <class RENDERER>
class rWorldCreator : public rWorld {
   static_assert( std::is_base_of<internal::rRendererBase, RENDERER>::value,
                  "RENDERER must be derived from internal::rRendererBase" );

 private:
   RENDERER vR1;
   RENDERER vR2;

   void setRendererPtr( internal::rRendererBase **_r1, internal::rRendererBase **_r2 ) override {
      *_r1 = &vR1;
      *_r2 = &vR2;
   }

 public:
   rWorldCreator() = delete;
   rWorldCreator( iInit *_init )
       : rWorld( _init ), vR1( _init, this, L"A" ), vR2( _init, this, L"B" ) {}

   virtual ~rWorldCreator() { shutdown(); }
};
}
