/*
 * Copyright (C) 2017 EEnginE project
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

using namespace e_engine;

/*!
 * \brief Returns a complete description of the render pass (+ framebuffer image formats)
 * \param _surfaceFormat The surface format of the swapchain
 */
vkuRenderPass::Config rRendererBasic::getRenderPassDescription(VkSurfaceFormatKHR _surfaceFormat) {
  // Query some vaiables
  VkFormat           lDepthFormat;
  VkImageTiling      lTiling;
  VkImageAspectFlags lAspectFlags;
  vDevice->getDepthFormat(lDepthFormat, lTiling, lAspectFlags);

  VkClearValue lColorClear;
  VkClearValue lDepthClear;

  lColorClear.color        = {{0.0f, 0.0f, 0.0f, 1.0f}};
  lDepthClear.depthStencil = {
      1.0f, // depth
      0     // stencil
  };

  return {

      // ===============
      // = Attachments =
      // ===============

      {

          // ====== ATTACHMENT 0 ----- Swapchain image ======
          {

              // Attachment Description
              {
                  0,                                        // flags
                  _surfaceFormat.format,                    // format
                  VK_SAMPLE_COUNT_1_BIT,                    // samples
                  VK_ATTACHMENT_LOAD_OP_CLEAR,              // loadOp
                  VK_ATTACHMENT_STORE_OP_STORE,             // storeOp
                  VK_ATTACHMENT_LOAD_OP_DONT_CARE,          // stencilLoadOp
                  VK_ATTACHMENT_STORE_OP_DONT_CARE,         // stencilStoreOp
                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL, // initialLayout
                  VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL  // finalLayout
              },

              // Clear color
              lColorClear,

              // Buffer create info
              {},

              // Buffer config
              {
                  true // useExternalImageView
              }

          },

          // ====== ATTACHMENT 1 ----- Depth Stencil buffer ======
          {

              // Attachment Description
              {
                  0,                                                // flags
                  lDepthFormat,                                     // format
                  VK_SAMPLE_COUNT_1_BIT,                            // samples
                  VK_ATTACHMENT_LOAD_OP_CLEAR,                      // loadOp
                  VK_ATTACHMENT_STORE_OP_STORE,                     // storeOp
                  VK_ATTACHMENT_LOAD_OP_CLEAR,                      // stencilLoadOp
                  VK_ATTACHMENT_STORE_OP_STORE,                     // stencilStoreOp
                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL, // initialLayout
                  VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL  // finalLayout
              },

              // Clear color
              lDepthClear,

              // Buffer create info
              {
                  VK_IMAGE_TYPE_2D,                            // type
                  1,                                           // mipLevels
                  1,                                           // arrayLayers
                  VK_IMAGE_TILING_OPTIMAL,                     // tiling
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, // usage
                  VK_SHARING_MODE_EXCLUSIVE,                   // sharingMode
                  {lAspectFlags, 0, 1, 0, 1}                   // subresourceRange
              },

              // Buffer config
              {}

          }

      },

      // =============
      // = Subpasses =
      // =============
      {

          // ====== SUBPASS 0 ----- Normal rendering ======
          {

              0,                               // flags
              VK_PIPELINE_BIND_POINT_GRAPHICS, // pipelineBindPoint

              // inputAttachments
              {},

              // colorAttachments
              {
                  {0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL} // ATTACHMENT 0
              },

              // resolveAttachments
              {},

              // depthStencilAttachment (ATTACHMENT 1)
              {1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL},

              // preserveAttachments
              {}

          }

      },

      // ========================
      // = Subpass dependencies =
      // ========================
      {

          // ====== DEPENDENCY 0 ======
          {
              VK_SUBPASS_EXTERNAL,                                                        // srcSubpass
              0,                                                                          // dstSubpass
              VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,                                       // srcStageMask
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,                              // dstStageMask
              VK_ACCESS_MEMORY_READ_BIT,                                                  // srcAccessMask
              VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // dstAccessMask
              VK_DEPENDENCY_BY_REGION_BIT                                                 // dependencyFlags
          },

          // ====== DEPENDENCY 1 ======
          {
              0,                                                                          // srcSubpass
              VK_SUBPASS_EXTERNAL,                                                        // dstSubpass
              VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,                              // srcStageMask
              VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,                                       // dstStageMask
              VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT, // srcAccessMask
              VK_ACCESS_MEMORY_READ_BIT,                                                  // dstAccessMask
              VK_DEPENDENCY_BY_REGION_BIT                                                 // dependencyFlags
          },

      }

  };
}

vkuFrameBuffer::Config rRendererBasic::getFrameBufferDescription(VkImageView _swapChainImageView) {
  return {

      // Size
      {
          GlobConf.win.width,  // width
          GlobConf.win.height, // height
          1                    // depth
      },

      // Data
      {

          // ATTACHMENT 0 ----- swapchain image
          {
              0,                  // attachmentID
              _swapChainImageView // view
          }

      }

  };
}
