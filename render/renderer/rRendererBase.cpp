/*!
 * \file rRendererBase.cpp
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

#include "rRendererBase.hpp"
#include "iInit.hpp"
#include "rObjectBase.hpp"
#include "rPipeline.hpp"
#include "rShaderBase.hpp"
#include "rWorld.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"


#if D_LOG_VULKAN
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

#if D_LOG_RENDER_LOOP
#define dRLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dRLOG( ... )
#endif

namespace e_engine {
namespace internal {

uint64_t rRendererBase::vRenderedFrames = 0;

rRendererBase::rRendererBase( iInit *_init, rWorld *_root, std::wstring _id )
    : vID( _id ), vInitPtr( _init ), vWorldPtr( _root ) {
   vDevice_vk = vInitPtr->getDevice();

   vRenderThread = std::thread( &rRendererBase::renderLoop, this );

   vCmdRecordInfo.lRPInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
   vCmdRecordInfo.lRPInfo.pNext                    = nullptr;
   vCmdRecordInfo.lRPInfo.renderPass               = nullptr;
   vCmdRecordInfo.lRPInfo.framebuffer              = nullptr;
   vCmdRecordInfo.lRPInfo.renderArea.extent.width  = GlobConf.win.width;
   vCmdRecordInfo.lRPInfo.renderArea.extent.height = GlobConf.win.height;
   vCmdRecordInfo.lRPInfo.renderArea.offset        = {0, 0};
   vCmdRecordInfo.lRPInfo.clearValueCount          = 0;
   vCmdRecordInfo.lRPInfo.pClearValues             = nullptr;

   vCmdRecordInfo.lViewPort.x        = 0;
   vCmdRecordInfo.lViewPort.y        = 0;
   vCmdRecordInfo.lViewPort.width    = GlobConf.win.width;
   vCmdRecordInfo.lViewPort.height   = GlobConf.win.height;
   vCmdRecordInfo.lViewPort.minDepth = 0.0f;
   vCmdRecordInfo.lViewPort.maxDepth = 1.0f;

   vCmdRecordInfo.lScissors.offset.x      = 0;
   vCmdRecordInfo.lScissors.offset.y      = 0;
   vCmdRecordInfo.lScissors.extent.width  = GlobConf.win.width;
   vCmdRecordInfo.lScissors.extent.height = GlobConf.win.height;

   vCmdRecordInfo.lInherit.sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
   vCmdRecordInfo.lInherit.pNext                = nullptr;
   vCmdRecordInfo.lInherit.renderPass           = nullptr;
   vCmdRecordInfo.lInherit.framebuffer          = nullptr; // set in loop
   vCmdRecordInfo.lInherit.occlusionQueryEnable = VK_FALSE;
   vCmdRecordInfo.lInherit.queryFlags           = 0;
   vCmdRecordInfo.lInherit.pipelineStatistics   = 0;
}

rRendererBase::~rRendererBase() {
   vRunRenderThread = false;
   if ( vRunRenderLoop )
      stop();

   vVarStartRecording.notify_all();
   vVarStartLogLoop.notify_one();

   if ( vRenderThread.joinable() )
      vRenderThread.join();
}

int rRendererBase::init() {
   if ( vIsSetup )
      return -3;

   uint32_t lQueueFamily;
   VkQueue lQueue       = vInitPtr->getQueue( VK_QUEUE_TRANSFER_BIT, 0.0, &lQueueFamily );
   VkCommandPool lPool  = vWorldPtr->getCommandPool( lQueueFamily );
   VkCommandBuffer lBuf = vWorldPtr->createCommandBuffer( lPool );
   VkFence lFence       = vWorldPtr->createFence();

   if ( !lBuf )
      return -1;

   if ( vWorldPtr->beginCommandBuffer( lBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ) )
      return -2;

   if ( initImageBuffers( lBuf ) )
      return 1;

   // vRenderPass_vk.attachments resized in 'initImageBuffers'
   VkAttachmentDescription *lAttachment = &vRenderPass_vk.attachments[FRAMEBUFFER_ATTACHMENT_INDEX];
   lAttachment->flags                   = 0;
   lAttachment->format                  = vWorldPtr->getSwapchainFormat().format;
   lAttachment->samples                 = GlobConf.vk.samples;
   lAttachment->loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
   lAttachment->storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
   lAttachment->stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   lAttachment->stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   lAttachment->initialLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   lAttachment->finalLayout             = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

   if ( vRenderPass_vk.subpasses.empty() )
      setupSubpasses();

   if ( initRenderPass() )
      return 2;

   if ( initFramebuffers() )
      return 3;

   vkEndCommandBuffer( lBuf );

   VkSubmitInfo lInfo;
   lInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
   lInfo.pNext                = nullptr;
   lInfo.waitSemaphoreCount   = 0;
   lInfo.pWaitSemaphores      = nullptr;
   lInfo.pWaitDstStageMask    = nullptr;
   lInfo.commandBufferCount   = 1;
   lInfo.pCommandBuffers      = &lBuf;
   lInfo.signalSemaphoreCount = 0;
   lInfo.pSignalSemaphores    = nullptr;

   {
      std::lock_guard<std::mutex> lGuard( vInitPtr->getQueueMutex( lQueue ) );
      vkQueueSubmit( lQueue, 1, &lInfo, lFence );
   }

   auto lRes = vkWaitForFences( vDevice_vk, 1, &lFence, VK_TRUE, UINT64_MAX );
   if ( lRes ) {
      eLOG( "'vkWaitForFences' returned ", uEnum2Str::toStr( lRes ) );
      return 3;
   }

   vkDestroyFence( vDevice_vk, lFence, nullptr );
   vkFreeCommandBuffers( vDevice_vk, lPool, 1, &lBuf );

   vIsSetup = true;
   return 0;
}

void rRendererBase::destroy() {
   if ( vRunRenderLoop )
      stop();
   if ( !vIsSetup )
      return;
   dVkLOG( "Destroying old render pass data [renderer ", vID, "]" );

   dVkLOG( "  -- destroying old framebuffers [renderer ", vID, "]" );
   for ( auto &i : vFramebuffers_vk )
      if ( i.fb )
         vkDestroyFramebuffer( vDevice_vk, i.fb, nullptr );

   dVkLOG( "  -- destroying old renderpass [renderer ", vID, "]" );
   vkDestroyRenderPass( vDevice_vk, vRenderPass_vk.renderPass, nullptr );

   dVkLOG( "  -- destroying old image buffer(s) [renderer ", vID, "]" );
   for ( auto &i : vBuffers ) {
      vkDestroyImageView( vDevice_vk, i.iv, nullptr );
      vkDestroyImage( vDevice_vk, i.img, nullptr );
      vkFreeMemory( vDevice_vk, i.mem, nullptr );
   }

   vRenderPass_vk.renderPass = nullptr;

   vFramebuffers_vk.clear();
   vBuffers.clear();
   vRenderPass_vk.attachmentViews.clear();
   vRenderPass_vk.attachmentBuffers.clear();
   vIsSetup = false;
}

void rRendererBase::getDepthFormat( VkFormat &_format,
                                    VkImageTiling &_tiling,
                                    VkImageAspectFlags &_aspect ) {
   _format = VK_FORMAT_UNDEFINED;
   _tiling = VK_IMAGE_TILING_MAX_ENUM;
   _aspect = VK_IMAGE_ASPECT_FLAG_BITS_MAX_ENUM;

   if ( !vDevice_vk ) {
      eLOG( "Device not created!" );
      return;
   }

   static const VkFormat lDepthFormats[] = {
         VK_FORMAT_D32_SFLOAT_S8_UINT,
         VK_FORMAT_D24_UNORM_S8_UINT,
         VK_FORMAT_D16_UNORM_S8_UINT,
         VK_FORMAT_D32_SFLOAT,
         VK_FORMAT_X8_D24_UNORM_PACK32,
         VK_FORMAT_D16_UNORM,
   };

   for ( auto i : lDepthFormats ) {
      if ( vInitPtr->formatSupportsFeature(
                 i, VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_IMAGE_TILING_LINEAR ) ) {
         _format = i;
         _tiling = VK_IMAGE_TILING_LINEAR;
         break;
      } else if ( vInitPtr->formatSupportsFeature( i,
                                                   VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT,
                                                   VK_IMAGE_TILING_OPTIMAL ) ) {
         _format = i;
         _tiling = VK_IMAGE_TILING_OPTIMAL;
         break;
      }
   }

   if ( _format == VK_FORMAT_UNDEFINED ) {
      eLOG( "Unable to find depth format for the depth buffer" );
      return;
   }

   vHasStencilBuffer = _format == VK_FORMAT_D32_SFLOAT_S8_UINT ||
                       _format == VK_FORMAT_D24_UNORM_S8_UINT ||
                       _format == VK_FORMAT_D16_UNORM_S8_UINT;

   _aspect = VK_IMAGE_ASPECT_DEPTH_BIT;
   _aspect |= vHasStencilBuffer ? VK_IMAGE_ASPECT_STENCIL_BIT : 0;
}

void rRendererBase::recordCmdBuffersWrapper( Framebuffer_vk &_fb, RECORD_TARGET _toRender ) {
   VkClearValue *lClearValue = &vRenderPass_vk.clearValues[FRAMEBUFFER_ATTACHMENT_INDEX];
   lClearValue->depthStencil = {1.0f, 0};

   lClearValue->color = {{vClearColor.float32[0],
                          vClearColor.float32[1],
                          vClearColor.float32[2],
                          vClearColor.float32[3]}};

   vCmdRecordInfo.lRPInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
   vCmdRecordInfo.lRPInfo.pNext                    = nullptr;
   vCmdRecordInfo.lRPInfo.renderPass               = vRenderPass_vk.renderPass;
   vCmdRecordInfo.lRPInfo.framebuffer              = _fb.fb;
   vCmdRecordInfo.lRPInfo.renderArea.extent.width  = GlobConf.win.width;
   vCmdRecordInfo.lRPInfo.renderArea.extent.height = GlobConf.win.height;
   vCmdRecordInfo.lRPInfo.renderArea.offset        = {0, 0};
   vCmdRecordInfo.lRPInfo.clearValueCount          = vRenderPass_vk.clearValues.size();
   vCmdRecordInfo.lRPInfo.pClearValues             = vRenderPass_vk.clearValues.data();

   vCmdRecordInfo.lViewPort.x        = 0;
   vCmdRecordInfo.lViewPort.y        = 0;
   vCmdRecordInfo.lViewPort.width    = GlobConf.win.width;
   vCmdRecordInfo.lViewPort.height   = GlobConf.win.height;
   vCmdRecordInfo.lViewPort.minDepth = 0.0f;
   vCmdRecordInfo.lViewPort.maxDepth = 1.0f;

   vCmdRecordInfo.lScissors.offset.x      = 0;
   vCmdRecordInfo.lScissors.offset.y      = 0;
   vCmdRecordInfo.lScissors.extent.width  = GlobConf.win.width;
   vCmdRecordInfo.lScissors.extent.height = GlobConf.win.height;

   vCmdRecordInfo.lInherit.sType                = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
   vCmdRecordInfo.lInherit.pNext                = nullptr;
   vCmdRecordInfo.lInherit.renderPass           = vRenderPass_vk.renderPass;
   vCmdRecordInfo.lInherit.framebuffer          = _fb.fb;
   vCmdRecordInfo.lInherit.occlusionQueryEnable = VK_FALSE;
   vCmdRecordInfo.lInherit.queryFlags           = 0;
   vCmdRecordInfo.lInherit.pipelineStatistics   = 0;

   recordCmdBuffers( _fb, _toRender );
}

void rRendererBase::renderLoop() {
   LOG.nameThread( L"ren_" + vID );
   iLOG( "Starting render thread" );

   while ( vRunRenderThread ) {
      std::unique_lock<std::mutex> lWait1( vMutexStartRecording );
      std::unique_lock<std::mutex> lWait2( vMutexStartLogLoop );

      dRLOG( "Waiting for command buffer recording signal" );
      vVarStartRecording.wait( lWait1 );

      // Check for deconstructor
      if ( !vRunRenderThread )
         return;

      //    _____      _ _
      //   |_   _|    (_) |
      //     | | _ __  _| |_
      //     | || '_ \| | __|
      //    _| || | | | | |_
      //    \___/_| |_|_|\__|
      //

      const static uint32_t NUM_FENCES   = 3;
      const static uint32_t FENCE_RENDER = 0;
      const static uint32_t FENCE_IMG_1  = 1;
      const static uint32_t FENCE_IMG_2  = 2;

      uint32_t lQueueFamily = 0;

      VkSwapchainKHR lSwapchain_vk = vWorldPtr->getSwapchain();
      VkQueue lQueue             = vInitPtr->getQueue( VK_QUEUE_GRAPHICS_BIT, 1.0, &lQueueFamily );
      VkCommandPool lCommandPool = vWorldPtr->getCommandPool( lQueueFamily );
      VkSemaphore lSemPresent    = vWorldPtr->createSemaphore();
      VkSemaphore lSemAcquireImg = vWorldPtr->createSemaphore();
      VkFence lFences[NUM_FENCES];

      for ( uint32_t i = 0; i < NUM_FENCES; i++ ) {
         lFences[i] = vWorldPtr->createFence();
      }


      initCmdBuffers( lCommandPool, vFramebuffers_vk.size() );
      initFrameCommandBuffers( lCommandPool );

      VkPipelineStageFlags lSubmitWaitFlags =
            VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT | VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;

      VkSubmitInfo lRenderSubmit[3];
      lRenderSubmit[0].sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
      lRenderSubmit[0].pNext                = nullptr;
      lRenderSubmit[0].waitSemaphoreCount   = 0;
      lRenderSubmit[0].pWaitSemaphores      = nullptr;
      lRenderSubmit[0].pWaitDstStageMask    = nullptr;
      lRenderSubmit[0].commandBufferCount   = 1;
      lRenderSubmit[0].pCommandBuffers      = nullptr; // set in render loop
      lRenderSubmit[0].signalSemaphoreCount = 0;
      lRenderSubmit[0].pSignalSemaphores    = nullptr;

      lRenderSubmit[1] = lRenderSubmit[0];
      lRenderSubmit[2] = lRenderSubmit[0];

      lRenderSubmit[0].waitSemaphoreCount   = 1;
      lRenderSubmit[0].pWaitSemaphores      = &lSemAcquireImg;
      lRenderSubmit[0].pWaitDstStageMask    = &lSubmitWaitFlags;
      lRenderSubmit[2].signalSemaphoreCount = 1;
      lRenderSubmit[2].pSignalSemaphores    = &lSemPresent;

      VkPresentInfoKHR lPresentInfo   = {};
      lPresentInfo.sType              = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
      lPresentInfo.pNext              = nullptr;
      lPresentInfo.waitSemaphoreCount = 1;
      lPresentInfo.pWaitSemaphores    = &lSemPresent;
      lPresentInfo.swapchainCount     = 1;
      lPresentInfo.pSwapchains        = &lSwapchain_vk;
      lPresentInfo.pImageIndices      = nullptr; // set in render loop
      lPresentInfo.pResults           = nullptr;


      // ======================
      // Rocord command buffers
      // ======================

      // Destroy old pipelines
      for ( auto &i : vObjects ) {
         rPipeline *lPipe     = i->getPipeline();
         rShaderBase *lShader = i->getShader();
         if ( lPipe != nullptr ) {
            if ( lPipe->getIsCreated() ) {
               lPipe->destroy();
            }
         }

         // Clear reserved uniforms, part of making sure not to change one uniform more than once
         if ( lShader != nullptr ) {
            lShader->signalRenderReset();
         }
      }

      // Create new pipelines
      for ( auto &i : vObjects ) {
         rPipeline *lPipe = i->getPipeline();
         if ( lPipe != nullptr ) {
            if ( !lPipe->getIsCreated() ) {
               lPipe->create( vDevice_vk, vRenderPass_vk.renderPass, 0 );
            }
         }

         // Setup object for rendering (preparing uniforms)
         i->signalRenderReset( this );
      }

      // Record all command buffers
      for ( auto &i : vFramebuffers_vk )
         recordCmdBuffersWrapper( i, RECORD_ALL );



      // Notifying other thread to continue
      {
         std::lock_guard<std::mutex> lGuard( vMutexFinishedRecording );
         vFinishedRecording = true;
      }
      vVarFinishedRecording.notify_all();

      dRLOG( "Waiting for start render loop signal" );
      vVarStartLogLoop.wait( lWait2 );

      // Check for deconstructor
      if ( !vRunRenderThread )
         return;

      //   ______               _             _
      //   | ___ \             | |           | |
      //   | |_/ /___ _ __   __| | ___ _ __  | |     ___   ___  _ __
      //   |    // _ \ '_ \ / _` |/ _ \ '__| | |    / _ \ / _ \| '_ \
      //   | |\ \  __/ | | | (_| |  __/ |    | |___| (_) | (_) | |_) |
      //   \_| \_\___|_| |_|\__,_|\___|_|    \_____/\___/ \___/| .__/
      //                                                       | |
      //                                                       |_|

      uint32_t lNextImg;

      // Init Uniforms
      for ( auto i : vObjects )
         i->updateUniforms();

      iLOG( "Starting the render loop" );
      while ( vRunRenderLoop ) {
         if ( !vIsSetup ) {
            eLOG( "FATAL ERROR NOT SETUP" ); // Should never execute
            break;
         }

         // Get present image (this command blocks)
         auto lRes = vkAcquireNextImageKHR(
               vDevice_vk, lSwapchain_vk, UINT64_MAX, lSemAcquireImg, VK_NULL_HANDLE, &lNextImg );
         if ( lRes ) {
            eLOG( "'vkAcquireNextImageKHR' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // Rerecord command buffers to update push constants
         recordCmdBuffersWrapper( vFramebuffers_vk[lNextImg], RECORD_PUSH_CONST_ONLY );

         // Set CMD buffers
         lRenderSubmit[0].pCommandBuffers = &vFramebuffers_vk[lNextImg].preRender;
         lRenderSubmit[1].pCommandBuffers = &vFramebuffers_vk[lNextImg].render;
         lRenderSubmit[2].pCommandBuffers = &vFramebuffers_vk[lNextImg].postRender;

         // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  -->  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
         lRes = vkQueueSubmit( lQueue, 1, &lRenderSubmit[0], lFences[FENCE_IMG_1] );
         if ( lRes ) {
            eLOG( "'vkQueueSubmit' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // Render
         lRes = vkQueueSubmit( lQueue, 1, &lRenderSubmit[1], lFences[FENCE_RENDER] );
         if ( lRes ) {
            eLOG( "'vkQueueSubmit' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  -->  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
         lRes = vkQueueSubmit( lQueue, 1, &lRenderSubmit[2], lFences[FENCE_IMG_2] );
         if ( lRes ) {
            eLOG( "'vkQueueSubmit' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         lPresentInfo.pImageIndices = &lNextImg;
         lRes                       = vkQueuePresentKHR( lQueue, &lPresentInfo );
         if ( lRes ) {
            eLOG( "'vkQueuePresentKHR' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // Wait until rendering is done
         vkWaitForFences( vDevice_vk, 1, &lFences[FENCE_RENDER], VK_TRUE, UINT64_MAX );

         // Update Uniforms
         for ( auto i : vObjects )
            i->updateUniforms();

         vkWaitForFences( vDevice_vk, 1, &lFences[FENCE_IMG_1], VK_TRUE, UINT64_MAX );
         vkWaitForFences( vDevice_vk, 1, &lFences[FENCE_IMG_2], VK_TRUE, UINT64_MAX );

         vkResetFences( vDevice_vk, NUM_FENCES, lFences );

         vWorldPtr->signalRenderdFrame();
         vRenderedFrames++;
      }
      iLOG( "Render loop stopped" );


      //    _____ _
      //   /  __ \ |
      //   | /  \/ | ___  __ _ _ __  _   _ _ __
      //   | |   | |/ _ \/ _` | '_ \| | | | '_ \
      //   | \__/\ |  __/ (_| | | | | |_| | |_) |
      //    \____/_|\___|\__,_|_| |_|\__,_| .__/
      //                                  | |
      //                                  |_|

      auto lRes = vkDeviceWaitIdle( vDevice_vk );
      if ( lRes ) {
         eLOG( "'vkDeviceWaitIdle' returned ", uEnum2Str::toStr( lRes ) );
      }

      freeCmdBuffers( lCommandPool );
      freeFrameCommandBuffers( lCommandPool );

      vkDestroySemaphore( vDevice_vk, lSemPresent, nullptr );
      for ( uint32_t i = 0; i < NUM_FENCES; i++ )
         vkDestroyFence( vDevice_vk, lFences[i], nullptr );


      std::lock_guard<std::mutex> lGuard1( vMutexStopLogLoop );
      std::lock_guard<std::mutex> lGuard2( vMutexFinishedRecording );
      vVarStopLogLoop.notify_all();
      vFinishedRecording = false;
   }

   iLOG( "Stopping render thread" );
}

bool rRendererBase::start() {
   if ( vRunRenderLoop ) {
      wLOG( "Render loop already running! [renderer ", vID, "]" );
      return false;
   }

   if ( !vIsSetup ) {
      eLOG( "Can not start uninitialized renderer! [renderer ", vID, "]" );
      return false;
   }

   dRLOG( "Sending start render loop to [renderer ", vID, "]" );

   std::unique_lock<std::mutex> lWait( vMutexFinishedRecording );
   std::lock_guard<std::mutex> lGuard( vMutexStartLogLoop );

   if ( !vFinishedRecording )
      vVarFinishedRecording.wait( lWait );

   vRunRenderLoop = true;
   vVarStartLogLoop.notify_all();
   return true;
}

bool rRendererBase::stop() {
   if ( !vRunRenderLoop ) {
      wLOG( "Render loop already stopped! [renderer ", vID, "]" );
      return false;
   }

   dRLOG( "Sending stop render loop to [renderer ", vID, "]" );

   vRunRenderLoop = false;

   std::unique_lock<std::mutex> lWait( vMutexStopLogLoop );

   vVarStopLogLoop.wait( lWait );
   return true;
}

bool rRendererBase::applyChanges() {
   dRLOG( "Applying changes to [renderer ", vID, "]" );

   {
      std::lock_guard<std::mutex> lGuard( vMutexStartRecording );
      if ( vFinishedRecording ) {
         wLOG( "Already recorded [renderer ", vID, "]" );
         return false;
      }
   }

   std::unique_lock<std::mutex> lWait( vMutexFinishedRecording );
   vVarStartRecording.notify_one();
   vVarFinishedRecording.wait( lWait );
   return true;
}


void rRendererBase::initFrameCommandBuffers( VkCommandPool _pool ) {
   VkImageSubresourceRange lSubResRange = {};
   lSubResRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
   lSubResRange.baseMipLevel            = 0;
   lSubResRange.levelCount              = 1;
   lSubResRange.baseArrayLayer          = 0;
   lSubResRange.layerCount              = 1;

   for ( auto &i : vFramebuffers_vk ) {
      i.preRender  = vWorldPtr->createCommandBuffer( _pool );
      i.render     = vWorldPtr->createCommandBuffer( _pool );
      i.postRender = vWorldPtr->createCommandBuffer( _pool );

      vWorldPtr->beginCommandBuffer( i.preRender );
      vWorldPtr->cmdChangeImageLayout( i.preRender,
                                       i.img,
                                       lSubResRange,
                                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                       VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT );
      vkEndCommandBuffer( i.preRender );

      vWorldPtr->beginCommandBuffer( i.postRender );
      vWorldPtr->cmdChangeImageLayout( i.postRender,
                                       i.img,
                                       lSubResRange,
                                       VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
                                       VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                       VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT );
      vkEndCommandBuffer( i.postRender );
   }
}

void rRendererBase::freeFrameCommandBuffers( VkCommandPool _pool ) {
   for ( auto &i : vFramebuffers_vk ) {
      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.preRender );
      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.render );
      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.postRender );

      i.preRender  = nullptr;
      i.render     = nullptr;
      i.postRender = nullptr;
   }
}



bool rRendererBase::addObject( std::shared_ptr<rObjectBase> _obj ) {
   vObjects.emplace_back( _obj );
   return true;
}

bool rRendererBase::resetObjects() {
   std::lock_guard<std::mutex> lGuard( vMutexStartRecording );
   if ( vRunRenderLoop ) {
      wLOG( "Can not clear objects while render loop is running!" );
      return false;
   }

   vObjects.clear();
   return true;
}

/*!
 * \brief Creates a suppass description, used to create a rennderPass
 *
 * \param _bindPoint   The vulkan pipeline bindpoint
 * \param _deptStencil Index of the depth stencil attachment (UINT32_MAX to ignore)
 * \param _color       Indexes of the color attachments (UINT32_MAX for default color attachment)
 * \param _input       Indexes of the input attachments
 * \param _preserve    Indexes of the attachments to preserve
 * \param _resolve     Indexes of the color attachments to resolve
 * \param _layoutMap   Overwrites the default layout for an attachment
 *
 * \returns the created sbpass index (UINT32_MAX on error)
 */
uint32_t rRendererBase::addSubpass( VkPipelineBindPoint _bindPoint,
                                    uint32_t _deptStencil,
                                    std::vector<uint32_t> _color,
                                    std::vector<uint32_t> _input,
                                    std::vector<uint32_t> _preserve,
                                    std::vector<uint32_t> _resolve,
                                    std::unordered_map<uint32_t, VkImageLayout> _layoutMap ) {
   vRenderPass_vk.data.emplace_back( std::make_unique<RenderPass_vk::SubPassData>() );
   auto &lData     = vRenderPass_vk.data.back();
   lData->preserve = _preserve;

   for ( uint32_t i : _color ) {
      if ( i == UINT32_MAX )
         i = FRAMEBUFFER_ATTACHMENT_INDEX;

      if ( i > vRenderPass_vk.attachments.size() ) {
         eLOG( "Invalid attachment ID ", i, "!" );
         return UINT32_MAX;
      }

      VkAttachmentReference lTemp;
      lTemp.attachment = i;
      lTemp.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

      if ( _layoutMap.count( i ) > 0 )
         lTemp.layout = _layoutMap[i];

      lData->color.emplace_back( lTemp );
   }

   for ( uint32_t i : _input ) {
      if ( i > vRenderPass_vk.attachments.size() ) {
         eLOG( "Invalid attachment ID ", i, "!" );
         return UINT32_MAX;
      }

      VkAttachmentReference lTemp;
      lTemp.attachment = i;
      lTemp.layout     = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

      if ( _layoutMap.count( i ) > 0 )
         lTemp.layout = _layoutMap[i];

      lData->input.emplace_back( lTemp );
   }

   lData->resolve.resize( lData->color.size() );
   for ( auto &i : lData->resolve ) {
      i = {VK_ATTACHMENT_UNUSED, VK_IMAGE_LAYOUT_UNDEFINED};
   }

   for ( auto i : _resolve ) {
      if ( i > lData->color.size() ) {
         eLOG( "Invalid resolve index ", i, "! Read the vulkan doc!" );
         return UINT32_MAX;
      }

      lData->resolve[i] = lData->color[i];
   }

   if ( _deptStencil != UINT32_MAX ) {
      if ( _deptStencil > vRenderPass_vk.attachments.size() ) {
         eLOG( "Invalid attachment ID ", _deptStencil, "!" );
         return UINT32_MAX;
      }

      lData->depth.attachment = _deptStencil;
      lData->depth.layout     = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

      if ( _layoutMap.count( _deptStencil ) > 0 )
         lData->depth.layout = _layoutMap[_deptStencil];
   }

   VkSubpassDescription lDesc;
   lDesc.flags                   = 0;
   lDesc.pipelineBindPoint       = _bindPoint;
   lDesc.inputAttachmentCount    = lData->input.size();
   lDesc.pInputAttachments       = lData->input.data();
   lDesc.colorAttachmentCount    = lData->color.size();
   lDesc.pColorAttachments       = lData->color.data();
   lDesc.pResolveAttachments     = lData->resolve.data();
   lDesc.pDepthStencilAttachment = &lData->depth;
   lDesc.preserveAttachmentCount = lData->preserve.size();
   lDesc.pPreserveAttachments    = lData->preserve.data();

   vRenderPass_vk.subpasses.push_back( lDesc );

   return 0;
}

/*!
 * \brief Adds a Vulkan subpass dependecy
 */
void rRendererBase::addSubpassDependecy( uint32_t _srcSubPass,
                                         uint32_t _dstSubPass,
                                         uint32_t _srcStageMask,
                                         uint32_t _dstStageMask,
                                         uint32_t _srcAccessMask,
                                         uint32_t _dstAccessMask,
                                         uint32_t _dependencyFlags ) {
   vRenderPass_vk.dependecies.emplace_back();
   auto *lAlias            = &vRenderPass_vk.dependecies.back();
   lAlias->srcSubpass      = _srcSubPass;
   lAlias->dstSubpass      = _dstSubPass;
   lAlias->srcStageMask    = _srcStageMask;
   lAlias->dstStageMask    = _dstStageMask;
   lAlias->srcAccessMask   = _srcAccessMask;
   lAlias->dstAccessMask   = _dstAccessMask;
   lAlias->dependencyFlags = _dependencyFlags;
}

uint64_t *rRendererBase::getRenderedFramesPtr() { return &vRenderedFrames; }
bool rRendererBase::getIsRunning() const { return vRunRenderLoop; }
bool rRendererBase::getIsInit() const { return vIsSetup; }
void rRendererBase::setClearColor( VkClearColorValue _clearColor ) { vClearColor = _clearColor; }
}
}
