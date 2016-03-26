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
#include "rWorld.hpp"
#include "rObjectBase.hpp"
#include "iInit.hpp"
#include "uLog.hpp"
#include "uEnum2Str.hpp"


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

uint64_t rRenderer::vRenderedFrames = 0;

rRenderer::rRenderer( iInit *_init, rWorld *_root, std::wstring _id )
    : vInitPtr( _init ), vWorldPtr( _root ), vID( _id ) {
   vDevice_vk = vInitPtr->getDevice();

   vRenderPass_vk.attachments.resize( 2 );
   vRenderPass_vk.attachmentViews.resize( 2 );
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
   dVkLOG( "Destroying old render pass data" );

   dVkLOG( "  -- destroying old framebuffers" );
   for ( auto &i : vFramebuffers_vk )
      if ( i.fb )
         vkDestroyFramebuffer( vDevice_vk, i.fb, nullptr );

   dVkLOG( "  -- destroying old renderpass" );
   vkDestroyRenderPass( vDevice_vk, vRenderPass_vk.renderPass, nullptr );

   dVkLOG( "  -- destroying old depth and stencil buffer" );
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

      std::vector<VkCommandBuffer> lBuffers;

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

      iLOG( "Starting the render loop" );
      while ( vRunRenderLoop ) {
         B_SLEEP( milliseconds, 20 ); // Do not kill the CPU
         if ( !vIsSetup ) {
            eLOG( "FATAL ERROR NOT SETUP" );
         }
         //! \todo RENDER HERE
         vRenderedFrames++;
      }
      iLOG( "Render loop stopped" );




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

bool rRenderer::addObject( rObjectBase *_obj ) {
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

      VkAttachmentReference lTemp;
      lTemp.attachment = i;
      lTemp.layout     = vRenderPass_vk.attachments[i].initialLayout;

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

      lData->depth.attachment = _deptStencil;
      lData->depth.layout     = vRenderPass_vk.attachments[_deptStencil].initialLayout;

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
}
