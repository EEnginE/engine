/*!
 * \file rRendererBasic.cpp
 * \brief \b Classes : \a rRendererBasic
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

#include "defines.hpp"
#include "uConfig.hpp"
#include "rRendererBasic.hpp"
#include "rWorld.hpp"
#include "rPipeline.hpp"
#include "rObjectBase.hpp"
#include "uLog.hpp"
#include "uEnum2Str.hpp"

namespace e_engine {

void rRendererBasic::setupSubpasses() {
   addSubpass( VK_PIPELINE_BIND_POINT_GRAPHICS,
               DEPTH_STENCIL_ATTACHMENT_INDEX,
               {FRAMEBUFFER_ATTACHMENT_INDEX} );
}

std::vector<rRendererBasic::AttachmentInfo> rRendererBasic::getAttachmentInfos() {
   VkFormat lDepthStencilFormat;
   VkImageTiling lTiling;
   VkImageAspectFlags lAspectFlags;

   getDepthFormat( lDepthStencilFormat, lTiling, lAspectFlags );

   return {{
         lDepthStencilFormat,                              // format
         VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,      // usage
         VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // layout
         lTiling,                                          // tiling
         lAspectFlags,                                     // aspect
         DEPTH_STENCIL_ATTACHMENT_INDEX,                   // attachID
   }};
}

VkImageView rRendererBasic::getAttachmentView( ATTACHMENT_ROLE _role ) {
   switch ( _role ) {
      case DEPTH_STENCIL: return vRenderPass_vk.attachmentViews[DEPTH_STENCIL_ATTACHMENT_INDEX];
      default: return nullptr;
   }
}

void rRendererBasic::initCmdBuffers( VkCommandPool _pool, uint32_t _numFramebuffers ) {
   for ( auto i : vObjects ) {
      if ( i.get() == nullptr ) {
         eLOG( "FATAL ERROR: nullptr in object list!" );
         return;
      }

      if ( i->isMesh() ) {
         vRenderObjects.emplace_back( i );
      }
   }

   vFbData.resize( _numFramebuffers );
   for ( auto &i : vFbData ) {
      i.buffers.resize( vRenderObjects.size() );
      for ( auto &j : i.buffers ) {
         j = vWorldPtr->createCommandBuffer( _pool, VK_COMMAND_BUFFER_LEVEL_SECONDARY );
      }
   }
}

void rRendererBasic::freeCmdBuffers( VkCommandPool _pool ) {
   for ( auto &i : vFbData ) {
      if ( i.buffers.size() > 0 )
         vkFreeCommandBuffers( vDevice_vk, _pool, i.buffers.size(), i.buffers.data() );
   }
   vFbData.clear();
   vPuschConstObjects.clear();
}


/*!
 * \brief Records the Vulkan command buffers, for a framebuffer
 * \note _toRender.size() MUST BE EQUAL TO _fb.secondary.size()
 * Elements in _toRender can be skipped by setting them to nullptr
 */
void rRendererBasic::recordCmdBuffers( Framebuffer_vk &_fb, RECORD_TARGET _toRender ) {
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

      vWorldPtr->beginCommandBuffer( vFbData[_fb.index].buffers[i],
                                     VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT,
                                     &vCmdRecordInfo.lInherit );

      if ( lPipe->getNumViewpors() > 0 )
         vkCmdSetViewport( vFbData[_fb.index].buffers[i], 0, 1, &vCmdRecordInfo.lViewPort );

      if ( lPipe->getNumScissors() > 0 )
         vkCmdSetScissor( vFbData[_fb.index].buffers[i], 0, 1, &vCmdRecordInfo.lScissors );

      vRenderObjects[i]->record( vFbData[_fb.index].buffers[i] );
      vkEndCommandBuffer( vFbData[_fb.index].buffers[i] );
   }

   vkCmdExecuteCommands(
         _fb.render, vFbData[_fb.index].buffers.size(), vFbData[_fb.index].buffers.data() );
   vkCmdEndRenderPass( _fb.render );

   auto lRes = vkEndCommandBuffer( _fb.render );
   if ( lRes ) {
      eLOG( "'vkEndCommandBuffer' returned ", uEnum2Str::toStr( lRes ) );
      //! \todo Handle this somehow (practically this code must not execute)
   }
}
}
