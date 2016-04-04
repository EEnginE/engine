/*!
 * \file rPipeline.hpp
 * \brief \b Classes: \a rPipeline
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
#include <vulkan.h>

#include "rShaderBase.hpp"

namespace e_engine {

class iInit;
class rWorld;

/*!
 * \brief Main pipeline Handling class
 *
 * Default setp:
 *     Description           | Value
 *   ----------------------- | ------------------------------------
 *   input Topology          | VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST
 *   PrimitiveRestart        | OFF
 *   Viewports               | 1
 *   Scissors                | 1
 *   Polygon mode            | VK_POLYGON_MODE_FILL
 *   Culling                 | OFF
 *   depthClamp              | OFF
 *   depthTest               | OFF
 *   depthBias               | OFF
 *   depthBoundTest          | OFF
 *   stencilTest             | OFF
 *
 * \todo Blending
 * \todo Better multi-sample control
 * \todo Tesselation
 * \todo depth bias
 * \todo Setting all other values
 */
class rPipeline {
 private:
   VkDevice vDevice_vk     = nullptr;
   VkPipeline vPipeline_vk = nullptr;

   rShaderBase *vShader = nullptr;

   VkPipelineVertexInputStateCreateInfo vVertex          = {};
   VkPipelineInputAssemblyStateCreateInfo vAssembly      = {};
   VkPipelineTessellationStateCreateInfo vTessellation   = {};
   VkPipelineViewportStateCreateInfo vViewport           = {};
   VkPipelineRasterizationStateCreateInfo vRasterization = {};
   VkPipelineMultisampleStateCreateInfo vMultisample     = {};
   VkPipelineDepthStencilStateCreateInfo vDepthStencil   = {};
   VkPipelineColorBlendStateCreateInfo vColorBlend       = {};
   VkPipelineDynamicStateCreateInfo vDynamic             = {};

   bool vIsCreated = false;

 public:
   rPipeline();
   rPipeline( const rPipeline &_obj ) = delete;
   rPipeline( rPipeline && ) = delete;
   rPipeline &operator=( const rPipeline & ) = delete;
   rPipeline &operator=( rPipeline && ) = delete;

   bool isReadyToCreate() const;

   void setShader( rShaderBase *_shader );
   rShaderBase *getShader() { return vShader; }

   rPipeline *setTopology( VkPrimitiveTopology _val = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST );
   rPipeline *enablePrimitiveRestart();
   rPipeline *disablePrimitiveRestart();
   rPipeline *setDynamicViewports( uint32_t _val = 1 );
   rPipeline *setDynamicScissors( uint32_t _val = 1 );
   rPipeline *setPolygonMode( VkPolygonMode _val = VK_POLYGON_MODE_FILL );
   rPipeline *enableCulling( VkFrontFace _front = VK_FRONT_FACE_COUNTER_CLOCKWISE );
   rPipeline *disableCulling();
   rPipeline *enableDepthClamp();
   rPipeline *disableDepthClamp();
   rPipeline *enableDepthTest( VkCompareOp _op = VK_COMPARE_OP_LESS_OR_EQUAL );
   rPipeline *disableDepthTest();
   rPipeline *enableDepthBoundTest();
   rPipeline *disableDepthBoundTest();
   rPipeline *enableDepthBias();
   rPipeline *disableDepthBias();
   rPipeline *enableStencilTest();
   rPipeline *disableStencilTest();

   bool create( VkDevice _device,
                VkRenderPass _renderPass,
                uint32_t _subPass,
                VkPipelineCache _cache = VK_NULL_HANDLE );

   bool destroy();
   VkPipeline getPipeline();

   bool cmdBindPipeline( VkCommandBuffer _buf, VkPipelineBindPoint _bindPoint );

   uint32_t getVertexBindPoint();
   uint32_t getNumViewpors() { return vViewport.viewportCount; }
   uint32_t getNumScissors() { return vViewport.scissorCount; }

   struct InputDesc {
      uint32_t num;
      uint32_t size;
   };

   bool checkInputCompatible( std::vector<InputDesc> _inputs );
   bool getIsCreated() { return vIsCreated; }

   virtual ~rPipeline();
};
}
