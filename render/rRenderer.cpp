/*!
 * \file rRenderer.cpp
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

#include "rRenderer.hpp"
#include "rPipeline.hpp"
#include "rWorld.hpp"
#include "rObjectBase.hpp"
#include "iInit.hpp"
#include "uLog.hpp"
#include "uEnum2Str.hpp"
#include "rShaderBase.hpp"


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

uint64_t rRenderer::vRenderedFrames = 0;

rRenderer::rRenderer( iInit *_init, rWorld *_root, std::wstring _id )
    : vInitPtr( _init ), vWorldPtr( _root ), vID( _id ) {
   vDevice_vk = vInitPtr->getDevice();

   vRenderPass_vk.attachments.resize( 2 );
   vRenderPass_vk.attachmentViews.resize( 2 );
   vRenderPass_vk.clearValues.resize( 2 );
   vRenderPass_vk.frameAttachID = 0;
   vRenderPass_vk.depthAttachID = 1;

   vRenderThread = std::thread( &rRenderer::renderLoop, this );

   defaultSetup();
}

rRenderer::~rRenderer() {
   vRunRenderThread = false;
   if ( vRunRenderLoop )
      stop();

   vVarStartRecording.notify_all();
   vVarStartLogLoop.notify_one();

   if ( vRenderThread.joinable() )
      vRenderThread.join();
}

int rRenderer::init() {
   if ( vIsSetup )
      return -3;

   VkAttachmentDescription *lAttachment = &vRenderPass_vk.attachments[vRenderPass_vk.frameAttachID];
   lAttachment->flags                   = 0;
   lAttachment->format                  = vWorldPtr->getSwapchainFormat().format;
   lAttachment->samples                 = GlobConf.vk.samples;
   lAttachment->loadOp                  = VK_ATTACHMENT_LOAD_OP_CLEAR;
   lAttachment->storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
   lAttachment->stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
   lAttachment->stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
   lAttachment->initialLayout           = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
   lAttachment->finalLayout             = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

   uint32_t lQueueFamily;
   VkQueue lQueue       = vInitPtr->getQueue( VK_QUEUE_TRANSFER_BIT, 0.0, &lQueueFamily );
   VkCommandPool lPool  = vWorldPtr->getCommandPool( lQueueFamily );
   VkCommandBuffer lBuf = vWorldPtr->createCommandBuffer( lPool );
   VkFence lFence       = vWorldPtr->createFence();

   if ( !lBuf )
      return -1;

   if ( vWorldPtr->beginCommandBuffer( lBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ) )
      return -2;

   if ( initDepthAndStencilBuffer( lBuf ) )
      return 1;

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

void rRenderer::destroy() {
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

   dVkLOG( "  -- destroying old depth and stencil buffer [renderer ", vID, "]" );
   vkDestroyImageView( vDevice_vk, vDepthStencilBuf_vk.iv, nullptr );
   vkDestroyImage( vDevice_vk, vDepthStencilBuf_vk.img, nullptr );
   vkFreeMemory( vDevice_vk, vDepthStencilBuf_vk.mem, nullptr );

   vRenderPass_vk.renderPass = nullptr;
   vDepthStencilBuf_vk.iv    = nullptr;
   vDepthStencilBuf_vk.img   = nullptr;
   vDepthStencilBuf_vk.mem   = nullptr;

   vFramebuffers_vk.clear();
   vIsSetup = false;
}

void rRenderer::renderLoop() {
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

      uint32_t lQueueFamily = 0;

      VkSwapchainKHR lSwapchain_vk = vWorldPtr->getSwapchain();
      VkQueue lQueue               = vInitPtr->getQueue( VK_QUEUE_GRAPHICS_BIT, 1.0, &lQueueFamily );
      VkCommandPool lCommandPool   = vWorldPtr->getCommandPool( lQueueFamily );

      OBJECTS lRenderObjects;
      for ( auto i : vObjects ) {
         if ( i->canRecord() ) {
            lRenderObjects.emplace_back( i );
         }
      }

      initFrameCommandBuffers( lCommandPool, lRenderObjects.size() );

      VkSemaphore lSemPresent  = vWorldPtr->createSemaphore();
      VkFence lWaitRenderFence = vWorldPtr->createFence();

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

      VkClearValue *lClearValue = &vRenderPass_vk.clearValues[vRenderPass_vk.frameAttachID];
      lClearValue->depthStencil = {1.0f, 0};

      lClearValue->color = {{vClearColor.float32[0],
                             vClearColor.float32[1],
                             vClearColor.float32[2],
                             vClearColor.float32[3]}};

      VkRenderPassBeginInfo lRPInfo    = {};
      lRPInfo.sType                    = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
      lRPInfo.pNext                    = nullptr;
      lRPInfo.renderPass               = vRenderPass_vk.renderPass;
      lRPInfo.framebuffer              = nullptr; // set in the loop below
      lRPInfo.renderArea.extent.width  = GlobConf.win.width;
      lRPInfo.renderArea.extent.height = GlobConf.win.height;
      lRPInfo.renderArea.offset        = {0, 0};
      lRPInfo.clearValueCount          = vRenderPass_vk.clearValues.size();
      lRPInfo.pClearValues             = vRenderPass_vk.clearValues.data();

      VkViewport lViewPort = {};
      lViewPort.x          = 0;
      lViewPort.y          = 0;
      lViewPort.width      = GlobConf.win.width;
      lViewPort.height     = GlobConf.win.height;
      lViewPort.minDepth   = 0.0f;
      lViewPort.maxDepth   = 1.0f;

      VkRect2D lScissors      = {};
      lScissors.offset.x      = 0;
      lScissors.offset.y      = 0;
      lScissors.extent.width  = GlobConf.win.width;
      lScissors.extent.height = GlobConf.win.height;


      // ======================
      // Rocord command buffers
      // ======================


      VkCommandBufferInheritanceInfo lInherit = {};
      lInherit.sType                          = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
      lInherit.pNext                          = nullptr;
      lInherit.renderPass                     = vRenderPass_vk.renderPass;
      lInherit.framebuffer                    = nullptr; // set in loop
      lInherit.occlusionQueryEnable           = VK_FALSE;
      lInherit.queryFlags                     = 0;
      lInherit.pipelineStatistics             = 0;

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
         i->signalRenderReset();
      }

      for ( auto &i : vFramebuffers_vk ) {
         vWorldPtr->beginCommandBuffer( i.render );

         lInherit.framebuffer = i.fb;
         lRPInfo.framebuffer  = i.fb;

         vkCmdBeginRenderPass( i.render, &lRPInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

         for ( uint32_t j = 0; j < lRenderObjects.size(); j++ ) {
            auto *lPipe = lRenderObjects[j]->getPipeline();
            if ( !lPipe ) {
               eLOG( "Object ", lRenderObjects[j]->getName(), " has no pipeline!" );
               continue;
            }

            vWorldPtr->beginCommandBuffer(
                  i.secondary[j], VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT, &lInherit );

            if ( lPipe->getNumViewpors() > 0 )
               vkCmdSetViewport( i.render, 0, 1, &lViewPort );

            if ( lPipe->getNumScissors() > 0 )
               vkCmdSetScissor( i.render, 0, 1, &lScissors );

            lRenderObjects[j]->record( i.render );
            vkEndCommandBuffer( i.secondary[j] );
         }

         vkCmdExecuteCommands( i.render, i.secondary.size(), i.secondary.data() );
         vkCmdEndRenderPass( i.render );

         auto lRes = vkEndCommandBuffer( i.render );
         if ( lRes ) {
            eLOG( "'vkEndCommandBuffer' returned ", uEnum2Str::toStr( lRes ) );
            //! \todo Handle this somehow (practically this code must not execute)
         }
      }





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
               vDevice_vk, lSwapchain_vk, UINT64_MAX, VK_NULL_HANDLE, VK_NULL_HANDLE, &lNextImg );
         if ( lRes ) {
            eLOG( "'vkAcquireNextImageKHR' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // Set CMD buffers
         lRenderSubmit[0].pCommandBuffers = &vFramebuffers_vk[lNextImg].preRender;
         lRenderSubmit[1].pCommandBuffers = &vFramebuffers_vk[lNextImg].render;
         lRenderSubmit[2].pCommandBuffers = &vFramebuffers_vk[lNextImg].postRender;

         // VK_IMAGE_LAYOUT_PRESENT_SRC_KHR  -->  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
         lRes = vkQueueSubmit( lQueue, 1, &lRenderSubmit[0], VK_NULL_HANDLE );
         if ( lRes ) {
            eLOG( "'vkQueueSubmit' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // Render
         lRes = vkQueueSubmit( lQueue, 1, &lRenderSubmit[1], lWaitRenderFence );
         if ( lRes ) {
            eLOG( "'vkQueueSubmit' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  -->  VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
         lRes = vkQueueSubmit( lQueue, 1, &lRenderSubmit[2], VK_NULL_HANDLE );
         if ( lRes ) {
            eLOG( "'vkQueueSubmit' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         lPresentInfo.pImageIndices = &lNextImg;
         lRes = vkQueuePresentKHR( lQueue, &lPresentInfo );
         if ( lRes ) {
            eLOG( "'vkQueuePresentKHR' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // Wait until rendering is done
         lRes = vkWaitForFences( vDevice_vk, 1, &lWaitRenderFence, VK_TRUE, UINT64_MAX );
         if ( lRes ) {
            eLOG( "'vkWaitForFences' returned ", uEnum2Str::toStr( lRes ) );
            break;
         }

         // Update Uniforms
         for ( auto i : vObjects )
            i->updateUniforms();


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

      vkDeviceWaitIdle( vDevice_vk );
      vkDestroySemaphore( vDevice_vk, lSemPresent, nullptr );
      vkDestroyFence( vDevice_vk, lWaitRenderFence, nullptr );

      freeFrameCommandBuffers( lCommandPool );


      std::lock_guard<std::mutex> lGuard1( vMutexStopLogLoop );
      std::lock_guard<std::mutex> lGuard2( vMutexFinishedRecording );
      vVarStopLogLoop.notify_all();
      vFinishedRecording = false;
   }

   iLOG( "Stopping render thread" );
}

bool rRenderer::start() {
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

bool rRenderer::stop() {
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

bool rRenderer::applyChanges() {
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


void rRenderer::initFrameCommandBuffers( VkCommandPool _pool, uint32_t _numSecondary ) {
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

      i.secondary.resize( _numSecondary );
      for ( auto &j : i.secondary )
         j = vWorldPtr->createCommandBuffer( _pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY );

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

void rRenderer::freeFrameCommandBuffers( VkCommandPool _pool ) {
   for ( auto &i : vFramebuffers_vk ) {
      if ( i.secondary.size() > 0 )
         vkFreeCommandBuffers( vDevice_vk, _pool, i.secondary.size(), i.secondary.data() );

      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.preRender );
      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.render );
      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.postRender );

      i.preRender  = nullptr;
      i.render     = nullptr;
      i.postRender = nullptr;
      i.secondary.clear();
   }
}



bool rRenderer::addObject( std::shared_ptr<rObjectBase> _obj ) {
   vObjects.emplace_back( _obj );
   return true;
}

bool rRenderer::resetObjects() {
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
uint32_t rRenderer::addSubpass( VkPipelineBindPoint _bindPoint,
                                uint32_t _deptStencil,
                                std::vector<uint32_t> _color,
                                std::vector<uint32_t> _input,
                                std::vector<uint32_t> _preserve,
                                std::vector<uint32_t> _resolve,
                                std::unordered_map<uint32_t, VkImageLayout> _layoutMap ) {
   vRenderPass_vk.data.emplace_back();
   auto *lData     = &vRenderPass_vk.data.back();
   lData->preserve = _preserve;

   for ( uint32_t i : _color ) {
      if ( i == UINT32_MAX )
         i = vRenderPass_vk.frameAttachID;

      if ( i > vRenderPass_vk.attachments.size() ) {
         eLOG( "Invalid attachment ID ", i, "!" );
         return UINT32_MAX;
      }

      VkImageLayout lLayout = vRenderPass_vk.attachments[i].initialLayout;
      VkAttachmentReference lTemp;
      lTemp.attachment = i;
      lTemp.layout     = lLayout != VK_IMAGE_LAYOUT_UNDEFINED
                           ? lLayout
                           : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

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
      lTemp.layout     = vRenderPass_vk.attachments[i].initialLayout;

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

      VkImageLayout lLayout   = vRenderPass_vk.attachments[_deptStencil].initialLayout;
      lData->depth.attachment = _deptStencil;
      lData->depth.layout     = lLayout != VK_IMAGE_LAYOUT_UNDEFINED
                                  ? lLayout
                                  : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

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
 * \brief Adds some stuff so that it works
 * \todo remove this hack!
 */
void rRenderer::defaultSetup() {
   addSubpass( VK_PIPELINE_BIND_POINT_GRAPHICS, vRenderPass_vk.depthAttachID );
}

uint32_t rRenderer::getDepthBufferAttachmentIndex() const { return vRenderPass_vk.depthAttachID; }
uint32_t rRenderer::getFrameBufferAttachmentIndex() const { return vRenderPass_vk.frameAttachID; }
uint64_t *rRenderer::getRenderedFramesPtr() { return &vRenderedFrames; }
bool rRenderer::getIsRunning() const { return vRunRenderLoop; }
bool rRenderer::getIsInit() const { return vIsSetup; }
void rRenderer::setClearColor( VkClearColorValue _clearColor ) { vClearColor = _clearColor; }
}
}
