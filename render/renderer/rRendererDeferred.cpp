/*!
 * \file rRendererDeferred.cpp
 * \brief \b Classes: \a rRendererDeferred
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


#include "rRendererDeferred.hpp"

#include "rObjectBase.hpp"
#include "rPipeline.hpp"
#include "rWorld.hpp"
#include "iInit.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"

namespace e_engine {
void rRendererDeferred::setupSubpasses() {
   addSubpass( VK_PIPELINE_BIND_POINT_GRAPHICS,
               DEPTH_STENCIL_ATTACHMENT_INDEX, // Depth / Stencil
               {DEFERRED_POS_ATTACHMENT_INDEX, // Color Output
                DEFERRED_NORMAL_ATTACHMENT_INDEX,
                DEFERRED_ALBEDO_ATTACHMENT_INDEX},
               {} // Input
               );

   addSubpass( VK_PIPELINE_BIND_POINT_GRAPHICS,
               UINT32_MAX,                     // Depth / Stencil
               {FRAMEBUFFER_ATTACHMENT_INDEX}, // Color Output
               {DEFERRED_POS_ATTACHMENT_INDEX, // Input
                DEFERRED_NORMAL_ATTACHMENT_INDEX,
                DEFERRED_ALBEDO_ATTACHMENT_INDEX} );

   addSubpassDependecy( 0,                                    // src subPass
                        1,                                    // dst subPass
                        VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,   // src Stage mask
                        VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT,   // dst Stage mask
                        VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT | // src Access flags
                              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                              VK_ACCESS_UNIFORM_READ_BIT |
                              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT,
                        VK_ACCESS_INPUT_ATTACHMENT_READ_BIT | // dst Access flags
                              VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT |
                              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                              VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT );
}

std::vector<rRendererDeferred::AttachmentInfo> rRendererDeferred::getAttachmentInfos() {
   VkFormat           lDepthStencilFormat;
   VkImageTiling      lTiling;
   VkImageAspectFlags lAspectFlags;

   getDepthFormat( lDepthStencilFormat, lTiling, lAspectFlags );

   return {

         // Depth Stencil buffer
         {
               lDepthStencilFormat,                              // format
               VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,      // usage
               VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // layout
               lTiling,                                          // tiling
               lAspectFlags,                                     // aspect
               DEPTH_STENCIL_ATTACHMENT_INDEX,                   // attachID
         },

         // Deferred Position
         {
               VK_FORMAT_R16G16B16A16_SFLOAT,                                             // format
               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, // usage
               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                                  // layout
               VK_IMAGE_TILING_OPTIMAL,                                                   // tiling
               VK_IMAGE_ASPECT_COLOR_BIT,                                                 // aspect
               DEFERRED_POS_ATTACHMENT_INDEX,                                             // ID
         },

         // Deferred Normal
         {
               VK_FORMAT_R16G16B16A16_SFLOAT,                                             // format
               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, // usage
               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                                  // layout
               VK_IMAGE_TILING_OPTIMAL,                                                   // tiling
               VK_IMAGE_ASPECT_COLOR_BIT,                                                 // aspect
               DEFERRED_NORMAL_ATTACHMENT_INDEX,                                          // ID
         },

         // Deferred Albedo
         {
               VK_FORMAT_R8G8B8A8_UNORM,                                                  // format
               VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, // usage
               VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,                                  // layout
               VK_IMAGE_TILING_OPTIMAL,                                                   // tiling
               VK_IMAGE_ASPECT_COLOR_BIT,                                                 // aspect
               DEFERRED_ALBEDO_ATTACHMENT_INDEX,                                          // ID
         }

   };
}

VkImageView rRendererDeferred::getAttachmentView( ATTACHMENT_ROLE _role ) {
   switch ( _role ) {
      case DEPTH_STENCIL: return vRenderPass_vk.attachmentViews[DEPTH_STENCIL_ATTACHMENT_INDEX];
      case DEFERRED_POSITION: return vRenderPass_vk.attachmentViews[DEFERRED_POS_ATTACHMENT_INDEX];
      case DEFERRED_NORMAL: return vRenderPass_vk.attachmentViews[DEFERRED_NORMAL_ATTACHMENT_INDEX];
      case DEFERRED_ALBEDO: return vRenderPass_vk.attachmentViews[DEFERRED_ALBEDO_ATTACHMENT_INDEX];
      default: return nullptr;
   }
}

/*!
 * \brief Initializes vertex and Index buffer
 *
 * The buffers have 2 triangles, filling the entire normalized space (-1,-1) - (1,1)
 */
void rRendererDeferred::initBuffers() {
   uint32_t        lQueueFamily;
   VkQueue         lQueue = vInitPtr->getQueue( VK_QUEUE_TRANSFER_BIT, 0.0, &lQueueFamily );
   VkCommandPool   lPool  = vWorldPtr->getCommandPool( lQueueFamily );
   VkCommandBuffer lBuf   = vWorldPtr->createCommandBuffer( lPool );
   VkFence         lFence = vWorldPtr->createFence();

   std::vector<float>    lDefBufferData  = {1.0f, 1.0f, -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, -1.0f};
   std::vector<uint32_t> lDefBufferIndex = {0, 1, 2, 2, 3, 0};

   vWorldPtr->beginCommandBuffer( lBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT );

   vDeferredDataBuffer.cmdInit( lDefBufferData, lBuf, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT );
   vDeferredIndexBuffer.cmdInit( lDefBufferIndex, lBuf, VK_BUFFER_USAGE_INDEX_BUFFER_BIT );

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

   vkEndCommandBuffer( lBuf );

   {
      std::lock_guard<std::mutex> lGuard( vInitPtr->getQueueMutex( lQueue ) );
      vkQueueSubmit( lQueue, 1, &lInfo, lFence );
   }

   auto lRes = vkWaitForFences( vDevice_vk, 1, &lFence, VK_TRUE, UINT64_MAX );
   if ( lRes ) {
      eLOG( "'vkWaitForFences' returned ", uEnum2Str::toStr( lRes ) );
   }

   vkDestroyFence( vDevice_vk, lFence, nullptr );
   vkFreeCommandBuffers( vDevice_vk, lPool, 1, &lBuf );

   vDeferredDataBuffer.doneCopying();
   vDeferredIndexBuffer.doneCopying();
}

void rRendererDeferred::initCmdBuffers( VkCommandPool _pool, uint32_t _numFramebuffers ) {
   for ( auto i : vObjects ) {
      if ( i.get() == nullptr ) {
         eLOG( "FATAL ERROR: nullptr in object list!" );
         return;
      }

      if ( i->isMesh() ) {
         vRenderObjects.emplace_back( i );
      } else {
         vLightObjects.emplace_back( i );
      }
   }

   vFbData.resize( _numFramebuffers );
   for ( auto &i : vFbData ) {
      i.objects.resize( vRenderObjects.size() );
      i.lights.resize( vLightObjects.size() );

      for ( auto &j : i.objects )
         j = vWorldPtr->createCommandBuffer( _pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY );

      for ( auto &j : i.lights )
         j = vWorldPtr->createCommandBuffer( _pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY );

      i.layoutChange1 = vWorldPtr->createCommandBuffer( _pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
      i.layoutChange2 = vWorldPtr->createCommandBuffer( _pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
   }
}

void rRendererDeferred::freeCmdBuffers( VkCommandPool _pool ) {
   for ( auto &i : vFbData ) {
      if ( i.objects.size() > 0 )
         vkFreeCommandBuffers( vDevice_vk, _pool, i.objects.size(), i.objects.data() );

      if ( i.lights.size() > 0 )
         vkFreeCommandBuffers( vDevice_vk, _pool, i.lights.size(), i.lights.data() );

      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.layoutChange1 );
      vkFreeCommandBuffers( vDevice_vk, _pool, 1, &i.layoutChange2 );
   }
   vFbData.clear();
   vRenderObjects.clear();
   vLightObjects.clear();
}


/*!
 * \brief Records the Vulkan command buffers, for a framebuffer
 * \note _toRender.size() MUST BE EQUAL TO _fb.secondary.size()
 * Elements in _toRender can be skipped by setting them to nullptr
 */
void rRendererDeferred::recordCmdBuffers( Framebuffer_vk &_fb, RECORD_TARGET _toRender ) {
   vWorldPtr->beginCommandBuffer( _fb.render );

   vkCmdBeginRenderPass(
         _fb.render, &vCmdRecordInfo.lRPInfo, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

   for ( uint32_t i = 0; i < vRenderObjects.size(); i++ ) {
      if ( _toRender == RECORD_PUSH_CONST_ONLY )
         if ( !vRenderObjects[i]->supportsPushConstants() )
            continue;

      auto *lPipe = vRenderObjects[i]->getPipeline();
      if ( !lPipe ) {
         eLOG( "Object ", vRenderObjects[i]->getName(), " has no pipeline!" );
         continue;
      }

      vWorldPtr->beginCommandBuffer( vFbData[_fb.index].objects[i],
                                     VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
                                     &vCmdRecordInfo.lInherit );

      if ( lPipe->getNumViewpors() > 0 )
         vkCmdSetViewport( vFbData[_fb.index].objects[i], 0, 1, &vCmdRecordInfo.lViewPort );

      if ( lPipe->getNumScissors() > 0 )
         vkCmdSetScissor( vFbData[_fb.index].objects[i], 0, 1, &vCmdRecordInfo.lScissors );

      vRenderObjects[i]->record( vFbData[_fb.index].objects[i] );
      vkEndCommandBuffer( vFbData[_fb.index].objects[i] );
   }

   vkCmdExecuteCommands(
         _fb.render, vFbData[_fb.index].objects.size(), vFbData[_fb.index].objects.data() );

   vWorldPtr->beginCommandBuffer( vFbData[_fb.index].layoutChange1,
                                  VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
                                  &vCmdRecordInfo.lInherit );

   VkImageSubresourceRange lRange = {};
   lRange.aspectMask              = VK_IMAGE_ASPECT_COLOR_BIT;
   lRange.baseMipLevel            = 0;
   lRange.levelCount              = 1;
   lRange.baseArrayLayer          = 0;
   lRange.layerCount              = 1;

   auto *lBuffPos    = vRenderPass_vk.attachmentBuffers[DEFERRED_POS_ATTACHMENT_INDEX];
   auto *lBuffNormal = vRenderPass_vk.attachmentBuffers[DEFERRED_NORMAL_ATTACHMENT_INDEX];
   auto *lBuffAlbedo = vRenderPass_vk.attachmentBuffers[DEFERRED_ALBEDO_ATTACHMENT_INDEX];

   vWorldPtr->cmdChangeImageLayout( vFbData[_fb.index].layoutChange1,
                                    lBuffPos->img,
                                    lRange,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

   vWorldPtr->cmdChangeImageLayout( vFbData[_fb.index].layoutChange1,
                                    lBuffNormal->img,
                                    lRange,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

   vWorldPtr->cmdChangeImageLayout( vFbData[_fb.index].layoutChange1,
                                    lBuffAlbedo->img,
                                    lRange,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );

   vkEndCommandBuffer( vFbData[_fb.index].layoutChange1 );
   vkCmdExecuteCommands( _fb.render, 1, &vFbData[_fb.index].layoutChange1 );
   vkCmdNextSubpass( _fb.render, VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS );

   for ( uint32_t i = 0; i < vLightObjects.size(); i++ ) {
      if ( _toRender == RECORD_PUSH_CONST_ONLY )
         if ( !vLightObjects[i]->supportsPushConstants() )
            continue;

      auto *lPipe = vLightObjects[i]->getPipeline();
      if ( !lPipe ) {
         eLOG( "Object ", vLightObjects[i]->getName(), " has no pipeline!" );
         continue;
      }

      vWorldPtr->beginCommandBuffer( vFbData[_fb.index].lights[i],
                                     VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
                                     &vCmdRecordInfo.lInherit );

      if ( lPipe->getNumViewpors() > 0 )
         vkCmdSetViewport( vFbData[_fb.index].lights[i], 0, 1, &vCmdRecordInfo.lViewPort );

      if ( lPipe->getNumScissors() > 0 )
         vkCmdSetScissor( vFbData[_fb.index].lights[i], 0, 1, &vCmdRecordInfo.lScissors );

      vLightObjects[i]->recordLight(
            vFbData[_fb.index].lights[i], vDeferredDataBuffer, vDeferredIndexBuffer );
      vkEndCommandBuffer( vFbData[_fb.index].lights[i] );
   }

   vkCmdExecuteCommands(
         _fb.render, vFbData[_fb.index].lights.size(), vFbData[_fb.index].lights.data() );

   vWorldPtr->beginCommandBuffer( vFbData[_fb.index].layoutChange2,
                                  VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
                                  &vCmdRecordInfo.lInherit );

   vWorldPtr->cmdChangeImageLayout( vFbData[_fb.index].layoutChange2,
                                    lBuffPos->img,
                                    lRange,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );

   vWorldPtr->cmdChangeImageLayout( vFbData[_fb.index].layoutChange2,
                                    lBuffNormal->img,
                                    lRange,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );

   vWorldPtr->cmdChangeImageLayout( vFbData[_fb.index].layoutChange2,
                                    lBuffAlbedo->img,
                                    lRange,
                                    VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
                                    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL );

   vkEndCommandBuffer( vFbData[_fb.index].layoutChange2 );
   vkCmdExecuteCommands( _fb.render, 1, &vFbData[_fb.index].layoutChange2 );

   vkCmdEndRenderPass( _fb.render );

   auto lRes = vkEndCommandBuffer( _fb.render );
   if ( lRes ) {
      eLOG( "'vkEndCommandBuffer' returned ", uEnum2Str::toStr( lRes ) );
      //! \todo Handle this somehow (practically this code must not execute)
   }
}
}
