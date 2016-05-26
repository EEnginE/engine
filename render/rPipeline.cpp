/*!
 * \file rPipeline.cpp
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

#include "rPipeline.hpp"

#include "rWorld.hpp"
#include "iInit.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"

namespace e_engine {

rPipeline::rPipeline() {
   // Setting up create info structs

   vVertex.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vVertex.pNext = nullptr;
   vVertex.flags = 0;
   vVertex.vertexBindingDescriptionCount   = 0;
   vVertex.pVertexBindingDescriptions      = nullptr;
   vVertex.vertexAttributeDescriptionCount = 0;
   vVertex.pVertexAttributeDescriptions    = nullptr;
   vAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   vAssembly.pNext                  = nullptr;
   vAssembly.flags                  = 0;
   vAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
   vAssembly.primitiveRestartEnable = VK_FALSE;
   vTessellation.sType              = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
   vTessellation.pNext              = nullptr;
   vTessellation.flags              = 0;
   vTessellation.patchControlPoints = 0;
   vViewport.sType                  = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   vViewport.pNext                  = nullptr;
   vViewport.flags                  = 0;
   vViewport.viewportCount          = 1;
   vViewport.pViewports             = nullptr;
   vViewport.scissorCount           = 1;
   vViewport.pScissors              = nullptr;
   vRasterization.sType             = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   vRasterization.pNext             = nullptr;
   vRasterization.flags             = 0;
   vRasterization.depthClampEnable  = VK_FALSE;
   vRasterization.rasterizerDiscardEnable = VK_FALSE;
   vRasterization.polygonMode             = VK_POLYGON_MODE_FILL;
   vRasterization.cullMode                = VK_CULL_MODE_NONE;
   vRasterization.frontFace               = VK_FRONT_FACE_COUNTER_CLOCKWISE;
   vRasterization.depthBiasEnable         = VK_FALSE;
   vRasterization.depthBiasConstantFactor = 0.0f;
   vRasterization.depthBiasClamp          = 0.0f;
   vRasterization.depthBiasSlopeFactor    = 0.0f;
   vRasterization.lineWidth               = 1.0f;
   vMultisample.sType                  = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   vMultisample.pNext                  = nullptr;
   vMultisample.flags                  = 0;
   vMultisample.rasterizationSamples   = GlobConf.vk.samples;
   vMultisample.sampleShadingEnable    = VK_FALSE;
   vMultisample.minSampleShading       = 0.0f;
   vMultisample.pSampleMask            = nullptr;
   vMultisample.alphaToCoverageEnable  = VK_FALSE;
   vMultisample.alphaToOneEnable       = VK_FALSE;
   vDepthStencil.sType                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   vDepthStencil.pNext                 = nullptr;
   vDepthStencil.flags                 = 0;
   vDepthStencil.depthTestEnable       = VK_FALSE;
   vDepthStencil.depthWriteEnable      = VK_FALSE;
   vDepthStencil.depthCompareOp        = VK_COMPARE_OP_LESS_OR_EQUAL;
   vDepthStencil.depthBoundsTestEnable = VK_FALSE;
   vDepthStencil.stencilTestEnable     = VK_FALSE;
   vDepthStencil.front.failOp          = VK_STENCIL_OP_KEEP;
   vDepthStencil.front.passOp          = VK_STENCIL_OP_KEEP;
   vDepthStencil.front.compareOp       = VK_COMPARE_OP_LESS_OR_EQUAL;
   vDepthStencil.front.compareMask     = 0;
   vDepthStencil.front.writeMask       = 0;
   vDepthStencil.front.reference       = 0;
   vDepthStencil.back                  = vDepthStencil.front;
   vDepthStencil.minDepthBounds        = 0.0f;
   vDepthStencil.maxDepthBounds        = 0.0f;
   vColorBlend.sType                   = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   vColorBlend.pNext                   = nullptr;
   vColorBlend.flags                   = 0;
   vColorBlend.logicOpEnable           = VK_FALSE;
   vColorBlend.logicOp                 = VK_LOGIC_OP_COPY;
   vColorBlend.attachmentCount         = 0;
   vColorBlend.pAttachments            = nullptr;
   vColorBlend.blendConstants[0]       = 0.0f;
   vColorBlend.blendConstants[1]       = 0.0f;
   vColorBlend.blendConstants[2]       = 0.0f;
   vColorBlend.blendConstants[3]       = 0.0f;
   vDynamic.sType                      = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   vDynamic.pNext                      = nullptr;
   vDynamic.flags                      = 0;
   vDynamic.dynamicStateCount          = 0;
   vDynamic.pDynamicStates             = nullptr;
}

rPipeline::~rPipeline() {
   if ( vPipeline_vk )
      vkDestroyPipeline( vDevice_vk, vPipeline_vk, nullptr );
}


/*!
 * \brief creates the pipeline
 * \note The renderer will call this function
 * \vkIntern
 */
bool rPipeline::create( VkDevice        _device,
                        VkRenderPass    _renderPass,
                        uint32_t        _subPass,
                        VkPipelineCache _cache ) {
   if ( !isReadyToCreate() ) {
      eLOG( "Pipeline not setup yet!" );
      return false;
   }

   if ( vIsCreated ) {
      eLOG( "Pipeline already created!" );
      return false;
   }

   vDevice_vk = _device;

   std::vector<VkDynamicState> lDynStates;

   if ( vViewport.viewportCount > 0 )
      lDynStates.emplace_back( VK_DYNAMIC_STATE_VIEWPORT );

   if ( vViewport.scissorCount > 0 )
      lDynStates.emplace_back( VK_DYNAMIC_STATE_SCISSOR );

   if ( vDepthStencil.depthBoundsTestEnable == VK_TRUE )
      lDynStates.emplace_back( VK_DYNAMIC_STATE_DEPTH_BOUNDS );

   if ( vRasterization.depthBiasEnable == VK_TRUE )
      lDynStates.emplace_back( VK_DYNAMIC_STATE_DEPTH_BIAS );

   if ( vDepthStencil.stencilTestEnable == VK_TRUE ) {
      lDynStates.emplace_back( VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK );
      lDynStates.emplace_back( VK_DYNAMIC_STATE_STENCIL_REFERENCE );
      lDynStates.emplace_back( VK_DYNAMIC_STATE_STENCIL_WRITE_MASK );
   }

   //    lDynStates.emplace_back( VK_DYNAMIC_STATE_LINE_WIDTH );

   auto lShaderCreateInfo = vShader->getShaderStageInfo();
   auto lVertexInfo1      = vShader->getVertexInputBindingDescription();
   auto lVertexInfo2      = vShader->getVertexInputAttribureDescriptions();

   vVertex.vertexBindingDescriptionCount   = 1;
   vVertex.pVertexBindingDescriptions      = &lVertexInfo1;
   vVertex.vertexAttributeDescriptionCount = lVertexInfo2.size();
   vVertex.pVertexAttributeDescriptions    = lVertexInfo2.data();

   vDynamic.dynamicStateCount = lDynStates.size();
   vDynamic.pDynamicStates    = lDynStates.data();

   VkGraphicsPipelineCreateInfo lInfo = {};
   lInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   lInfo.pNext                        = nullptr;
   lInfo.flags               = 0; //! \todo Handle this flags if needed (currently irrelevant)
   lInfo.stageCount          = lShaderCreateInfo.size();
   lInfo.pStages             = lShaderCreateInfo.data();
   lInfo.pVertexInputState   = &vVertex;
   lInfo.pInputAssemblyState = &vAssembly;
   lInfo.pTessellationState  = &vTessellation;
   lInfo.pViewportState      = &vViewport;
   lInfo.pRasterizationState = &vRasterization;
   lInfo.pMultisampleState   = &vMultisample;
   lInfo.pDepthStencilState  = &vDepthStencil;
   lInfo.pColorBlendState    = &vColorBlend;
   lInfo.pDynamicState       = &vDynamic;
   lInfo.layout              = vShader->getPipelineLayout();
   lInfo.renderPass          = _renderPass;
   lInfo.subpass             = _subPass;
   lInfo.basePipelineHandle  = VK_NULL_HANDLE;
   lInfo.basePipelineIndex   = -1;


   auto lRes = vkCreateGraphicsPipelines( vDevice_vk, _cache, 1, &lInfo, nullptr, &vPipeline_vk );
   if ( lRes ) {
      eLOG( "'vkCreateGraphicsPipelines' returned ", uEnum2Str::toStr( lRes ) );
      return false;
   }

   vIsCreated = true;
   return true;
}

/*!
 * \brief destroyes the pipeline
 * \note The renderer will call this function
 * \vkIntern
 */
bool rPipeline::destroy() {
   if ( !vIsCreated ) {
      eLOG( "Pipeline already destroyed" );
      return false;
   }

   if ( vPipeline_vk )
      vkDestroyPipeline( vDevice_vk, vPipeline_vk, nullptr );

   vPipeline_vk = nullptr;
   vIsCreated   = false;

   return true;
}


/*!
 * \brief Checks if the pipeline is ready to be created
 */
bool rPipeline::isReadyToCreate() const {
   if ( vShader == nullptr )
      return false;

   return true;
}

/*!
 * \brief Checks if the input interface matches
 */
bool rPipeline::checkInputCompatible( std::vector<InputDesc> _inputs ) {
   if ( vShader == nullptr ) {
      eLOG( "Shader not yet set!" );
      return false;
   }

   uint32_t lSum = 0;

   auto lVertexInfo1 = vShader->getVertexInputBindingDescription();
   auto lVertexInfo2 = vShader->getVertexInputAttribureDescriptions();

   if ( _inputs.size() != lVertexInfo2.size() )
      return false;

   for ( uint32_t i = 0; i < _inputs.size(); i++ ) {
      if ( lVertexInfo2[i].offset != lSum )
         return false;

      lSum += _inputs[i].num * _inputs[i].size;
   }

   if ( lVertexInfo1.stride != lSum )
      return false;

   return true;
}

bool rPipeline::checkUniformCompatible( std::vector<rShaderBase::UNIFORM_ROLE> _uniforms ) {
   if ( vShader == nullptr ) {
      eLOG( "Shader not yet set!" );
      return false;
   }

   auto lUniforms = vShader->getUniforms();

   for ( auto const &i : _uniforms ) {
      bool lFound = false;

      for ( auto const &j : lUniforms ) {
         if ( i == j.guessedRole ) {
            lFound = true;
            break;
         }
      }

      if ( !lFound )
         return false;
   }

   return true;
}

/*!
 * \brief Binds the pipeline and descriptor set to the command buffer
 * \vkIntern
 */
bool rPipeline::cmdBindPipeline( VkCommandBuffer _buf, VkPipelineBindPoint _bindPoint ) {
   if ( !vIsCreated ) {
      eLOG( "Pipeline not created yet" );
      return false;
   }

   if ( !vPipeline_vk ) {
      eLOG( "Fatal error while creating pipeline" );
      return false;
   }

   vkCmdBindPipeline( _buf, _bindPoint, vPipeline_vk );
   return true;
}

/*!
 * \returns The vertex bind point
 * \vkIntern
 */
uint32_t rPipeline::getVertexBindPoint() {
   return vShader->getVertexInputBindingDescription().binding;
}

/*!
 * \returns nullptr on error / pipeline not created yet
 * \vkIntern
 */
VkPipeline rPipeline::getPipeline() {
   if ( !vIsCreated ) {
      eLOG( "Pipeline not created yet!" );
      return nullptr;
   }

   return vPipeline_vk;
}

/*!
 * \brief Sets the shader for the pipeline
 */
void rPipeline::setShader( rShaderBase *_shader ) {
   vShader = _shader;
   if ( !vShader->isInitialized() )
      vShader->init();
}


rPipeline *rPipeline::setTopology( VkPrimitiveTopology _val ) {
   vAssembly.topology = _val;
   return this;
}

rPipeline *rPipeline::enablePrimitiveRestart() {
   vAssembly.primitiveRestartEnable = VK_TRUE;
   return this;
}

rPipeline *rPipeline::disablePrimitiveRestart() {
   vAssembly.primitiveRestartEnable = VK_FALSE;
   return this;
}

rPipeline *rPipeline::setDynamicViewports( uint32_t _val ) {
   vViewport.viewportCount = _val;
   return this;
}
rPipeline *rPipeline::setDynamicScissors( uint32_t _val ) {
   vViewport.scissorCount = _val;
   return this;
}

rPipeline *rPipeline::setPolygonMode( VkPolygonMode _val ) {
   vRasterization.polygonMode = _val;
   return this;
}

rPipeline *rPipeline::enableCulling( VkFrontFace _front, VkCullModeFlags _mode ) {
   vRasterization.cullMode  = _mode;
   vRasterization.frontFace = _front;
   return this;
}

rPipeline *rPipeline::disableCulling() {
   vRasterization.cullMode = VK_CULL_MODE_NONE;
   return this;
}

rPipeline *rPipeline::enableDepthClamp() {
   vRasterization.depthClampEnable = VK_TRUE;
   return this;
}

rPipeline *rPipeline::disableDepthClamp() {
   vRasterization.depthClampEnable = VK_FALSE;
   return this;
}

rPipeline *rPipeline::enableDepthTest( VkCompareOp _op ) {
   vDepthStencil.depthTestEnable  = VK_TRUE;
   vDepthStencil.depthWriteEnable = VK_TRUE;
   vDepthStencil.depthCompareOp   = _op;
   return this;
}

rPipeline *rPipeline::disableDepthTest() {
   vDepthStencil.depthTestEnable  = VK_FALSE;
   vDepthStencil.depthWriteEnable = VK_FALSE;
   return this;
}

rPipeline *rPipeline::enableDepthBoundTest() {
   vDepthStencil.depthBoundsTestEnable = VK_TRUE;
   return this;
}

rPipeline *rPipeline::disableDepthBoundTest() {
   vDepthStencil.depthBoundsTestEnable = VK_FALSE;
   return this;
}

rPipeline *rPipeline::enableDepthBias() {
   vRasterization.depthBiasEnable = VK_TRUE;
   return this;
}

rPipeline *rPipeline::disableDepthBias() {
   vRasterization.depthBiasEnable = VK_FALSE;
   return this;
}

rPipeline *rPipeline::enableStencilTest() {
   vDepthStencil.stencilTestEnable = VK_TRUE;
   return this;
}

rPipeline *rPipeline::disableStencilTest() {
   vDepthStencil.stencilTestEnable = VK_FALSE;
   return this;
}
}
