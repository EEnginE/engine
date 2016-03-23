/*!
 * \file rRoot.cpp
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

#include "rRoot.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"

#if D_LOG_VULKAN
#define dVkLOG( ... ) dLOG( __VA_ARGS__ )
#else
#define dVkLOG( ... )
#endif

namespace e_engine {

void rRoot::handleResize( iEventInfo const & ) {
   if ( initBasic() )
      return;

   initRenderPass();
}

rRoot::rRoot( iInit *_init ) : vInitPtr( _init ), vResizeSlot( &rRoot::handleResize, this ) {
   vDevice_vk  = vInitPtr->getDevice();
   vSurface_vk = vInitPtr->getVulkanSurface();

   vRenderPass_vk.attachments.resize( 2 );
   vRenderPass_vk.frameAttachID = 0;
   vRenderPass_vk.depthAttachID = 1;
   vAttachmentImageViews.resize( 2 );
}

rRoot::~rRoot() {
   dVkLOG( "Vulkan cleanup [rRoot]:" );
   dVkLOG( "  -- Destroying command pools..." );

   std::lock_guard<std::mutex> lLock( vCommandPoolsMutex );
   vkDeviceWaitIdle( vInitPtr->getDevice() );
   for ( auto &i : vCmdPools_vk ) {
      dVkLOG( "    -- tID: ", i.first.tID, "; queue family: ", i.first.qfIndex );
      if ( i.second == nullptr ) {
         wLOG( "Command pool already destroyed" );
         continue;
      }

      vkDestroyCommandPool( vInitPtr->getDevice(), i.second, nullptr );
   }

   dVkLOG( "  -- destroying framebuffers" );
   for ( auto &i : vFramebuffers_vk )
      if ( i.fb )
         vkDestroyFramebuffer( vDevice_vk, i.fb, nullptr );

   dVkLOG( "  -- destroying renderpass" );
   if ( vRenderPass_vk.renderPass )
      vkDestroyRenderPass( vDevice_vk, vRenderPass_vk.renderPass, nullptr );

   dVkLOG( "  -- destroying depth and stencil buffer" );
   if ( vDepthStencilBuf_vk.iv )
      vkDestroyImageView( vDevice_vk, vDepthStencilBuf_vk.iv, nullptr );

   if ( vDepthStencilBuf_vk.img )
      vkDestroyImage( vDevice_vk, vDepthStencilBuf_vk.img, nullptr );

   if ( vDepthStencilBuf_vk.mem )
      vkFreeMemory( vDevice_vk, vDepthStencilBuf_vk.mem, nullptr );

   dVkLOG( "  -- destroying swapchain image views" );
   for ( auto &i : vFramebuffers_vk )
      vkDestroyImageView( vDevice_vk, i.iv, nullptr );

   dVkLOG( "  -- destroying swapchain" );
   if ( vSwapchain_vk )
      vkDestroySwapchainKHR( vDevice_vk, vSwapchain_vk, nullptr );
}


/*!
 * \brief initializes the swapchain and the depth and stencil buffer
 *
 * Frees old objects (if they exist) and then recreates them with the new settings
 *
 * \returns 0 on success
 * \note this function will be automatically called every time the window is resized
 */
int rRoot::initBasic() {
   dVkLOG( "Destroying old render pass data" );

   dVkLOG( "  -- destroying old framebuffers" );
   for ( auto &i : vFramebuffers_vk )
      if ( i.fb )
         vkDestroyFramebuffer( vDevice_vk, i.fb, nullptr );

   dVkLOG( "  -- destroying old renderpass" );
   if ( vRenderPass_vk.renderPass )
      vkDestroyRenderPass( vDevice_vk, vRenderPass_vk.renderPass, nullptr );

   dVkLOG( "  -- destroying old depth and stencil buffer" );
   if ( vDepthStencilBuf_vk.iv )
      vkDestroyImageView( vDevice_vk, vDepthStencilBuf_vk.iv, nullptr );

   if ( vDepthStencilBuf_vk.img )
      vkDestroyImage( vDevice_vk, vDepthStencilBuf_vk.img, nullptr );

   if ( vDepthStencilBuf_vk.mem )
      vkFreeMemory( vDevice_vk, vDepthStencilBuf_vk.mem, nullptr );

   dVkLOG( "  -- destroying old swapchain image views" );
   for ( auto &i : vFramebuffers_vk )
      vkDestroyImageView( vDevice_vk, i.iv, nullptr );

   vRenderPass_vk.renderPass = nullptr;
   vDepthStencilBuf_vk.iv    = nullptr;
   vDepthStencilBuf_vk.img   = nullptr;
   vDepthStencilBuf_vk.mem   = nullptr;

   vFramebuffers_vk.clear();

   uint32_t lQueueFamily;
   VkQueue lQueue       = vInitPtr->getQueue( VK_QUEUE_TRANSFER_BIT, 0.0, &lQueueFamily );
   VkCommandPool lPool  = getCommandPool( lQueueFamily );
   VkCommandBuffer lBuf = createCommandBuffer( lPool );
   VkFence lFence       = createFence();

   if ( !lBuf )
      return -1;

   if ( beginCommandBuffer( lBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT ) )
      return -2;

   if ( recreateSwapchain() )
      return 1;

   if ( recreateDepthAndStencilBuffer( lBuf ) )
      return 2;

   if ( recreateSwapchainImages( lBuf ) )
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

   dVkLOG( "Submitting image layout change command buffer..." );

   {
      std::lock_guard<std::mutex> lGuard( vInitPtr->getQueueMutex( lQueue ) );
      vkQueueSubmit( lQueue, 1, &lInfo, lFence );
   }

   auto lRes = vkWaitForFences( vDevice_vk, 1, &lFence, VK_TRUE, UINT64_MAX );
   if ( lRes ) {
      eLOG( "'vkWaitForFences' returned ", uEnum2Str::toStr( lRes ) );
      return 4;
   }

   vkDestroyFence( vDevice_vk, lFence, nullptr );
   vkFreeCommandBuffers( vDevice_vk, lPool, 1, &lBuf );
   dVkLOG( "...DONE" );

   return 0;
}


/*!
 * \brief initializes the renderpass and framebuffers
 *
 * \returns 0 on success
 * \note this function will be automatically called every time the window is resized
 */
int rRoot::initRenderPass() {
   if ( recreateRenderPass() )
      return 4;

   if ( recreateFramebuffers() )
      return 5;

   if ( !vIsResizeSlotSetup )
      vInitPtr->addResizeSlot( &vResizeSlot );

   vIsResizeSlotSetup = true;
   return 0;
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
uint32_t rRoot::addSubpass( VkPipelineBindPoint _bindPoint,
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

      wLOG( i );

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
void rRoot::defaultSetup() {
   addSubpass( VK_PIPELINE_BIND_POINT_GRAPHICS, vRenderPass_vk.depthAttachID );
}


void rRoot::cmdChangeImageLayout( VkCommandBuffer _cmdBuffer,
                                  VkImage _img,
                                  VkImageSubresourceRange _imgSubres,
                                  VkImageLayout _src,
                                  VkImageLayout _dst ) {
   VkImageMemoryBarrier lBarriar;
   lBarriar.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
   lBarriar.pNext               = nullptr;
   lBarriar.oldLayout           = _src;
   lBarriar.newLayout           = _dst;
   lBarriar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
   lBarriar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
   lBarriar.image               = _img;
   lBarriar.subresourceRange    = _imgSubres;

   switch ( _src ) {
      case VK_IMAGE_LAYOUT_PREINITIALIZED:
         lBarriar.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
         break;
      case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
         lBarriar.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
         break;
      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
         lBarriar.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
         break;
      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
         lBarriar.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
         break;
      case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
         lBarriar.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
         break;
      case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
         lBarriar.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
         break;
      case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
         lBarriar.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
         break;
      default: lBarriar.srcAccessMask = 0; break;
   }

   switch ( _dst ) {
      case VK_IMAGE_LAYOUT_PREINITIALIZED:
         lBarriar.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
         break;
      case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
         lBarriar.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
         break;
      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
         lBarriar.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT |
                                  VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
         break;
      case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
         lBarriar.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
         break;
      case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
         lBarriar.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
         break;
      case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
         lBarriar.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
         break;
      case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
         lBarriar.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
         break;
      default: lBarriar.dstAccessMask = 0; break;
   }

   vkCmdPipelineBarrier( _cmdBuffer,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &lBarriar );
}


/*!
 * \brief Get a command pool
 *
 * This function selects (or generates if needed) a command pool for the current thread (commad
 * pools are not thread safe ==> one command pool for every thread).
 *
 * \param _queueFamilyIndex The queue family index
 * \param __flags           command pool flags
 * \returns a command buffer (or nullptr)
 */
VkCommandPool rRoot::getCommandPool( uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flags ) {
   PoolInfo lTemp;
   lTemp.tID     = std::this_thread::get_id();
   lTemp.qfIndex = _queueFamilyIndex;
   lTemp.flags   = _flags;

   std::lock_guard<std::mutex> lLock( vCommandPoolsMutex );

   if ( vCmdPools_vk.count( lTemp ) > 0 )
      return vCmdPools_vk[lTemp];

   // Command pool does not exists
   dVkLOG( "Creating command pool for thread ", lTemp.tID, ", queue family ", lTemp.qfIndex );

   VkCommandPool lPool;
   VkCommandPoolCreateInfo lInfo;
   lInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
   lInfo.pNext            = nullptr;
   lInfo.flags            = _flags;
   lInfo.queueFamilyIndex = _queueFamilyIndex;

   auto lRes = vkCreateCommandPool( vInitPtr->getDevice(), &lInfo, nullptr, &lPool );
   if ( lRes ) {
      eLOG( "vkCreateCommandPool returned ", uEnum2Str::toStr( lRes ) );
   }

   vCmdPools_vk[lTemp] = lPool;

   return lPool;
}

VkCommandBuffer rRoot::createCommandBuffer( VkCommandPool _pool, VkCommandBufferLevel _level ) {
   VkCommandBuffer lBuf;
   VkCommandBufferAllocateInfo lInfo;
   lInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
   lInfo.pNext              = nullptr;
   lInfo.commandPool        = _pool;
   lInfo.level              = _level;
   lInfo.commandBufferCount = 1;

   auto lRes = vkAllocateCommandBuffers( vDevice_vk, &lInfo, &lBuf );
   if ( lRes ) {
      eLOG( "'vkAllocateCommandBuffers' returned ", uEnum2Str::toStr( lRes ) );
      return nullptr;
   }

   return lBuf;
}

VkResult rRoot::beginCommandBuffer( VkCommandBuffer _buf, VkCommandBufferUsageFlags _flags ) {
   VkCommandBufferBeginInfo lBegin;
   lBegin.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
   lBegin.pNext            = nullptr;
   lBegin.flags            = _flags;
   lBegin.pInheritanceInfo = nullptr;

   VkResult lRes = vkBeginCommandBuffer( _buf, &lBegin );
   if ( lRes ) {
      eLOG( "'vkBeginCommandBuffer' returned ", uEnum2Str::toStr( lRes ) );
   }

   return lRes;
}

/*!
 * \brief Get a command pool
 *
 * This function selects (or generates if needed) a command pool for the current thread (commad
 * pools are not thread safe ==> one command pool for every thread).
 *
 * \param _qFlags Flags the queue family MUST support
 * \param __flags command pool flags
 * \returns a command buffer (or nullptr)
 *
 * \note Wrapper for rRoot::getCommandPool
 */
VkCommandPool rRoot::getCommandPoolFlags( VkQueueFlags _qFlags, VkCommandPoolCreateFlags _flags ) {
   uint32_t lFamilyIndex = vInitPtr->getQueueFamily( _qFlags );
   if ( lFamilyIndex == UINT32_MAX ) {
      return nullptr;
   }

   return getCommandPool( lFamilyIndex, _flags );
}

VkFence rRoot::createFence( VkFenceCreateFlags _flags ) {
   VkFence lFence;

   VkFenceCreateInfo lInfo;
   lInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
   lInfo.pNext = nullptr;
   lInfo.flags = _flags;

   auto lRes = vkCreateFence( vDevice_vk, &lInfo, nullptr, &lFence );
   if ( lRes ) {
      eLOG( "'vkCreateFence' returned ", uEnum2Str::toStr( lRes ) );
      return nullptr;
   }

   return lFence;
}


uint32_t rRoot::getDepthBufferAttachmentIndex() const { return vRenderPass_vk.depthAttachID; }
uint32_t rRoot::getFrameBufferAttachmentIndex() const { return vRenderPass_vk.frameAttachID; }
}
