/*!
 * \file rShaderBase.hpp
 * \brief \b Classes: \a rShaderBase
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

#include "rShaderBase.hpp"
#include "iInit.hpp"
#include "rWorld.hpp"
#include "uLog.hpp"
#include "uEnum2Str.hpp"

namespace e_engine {

rShaderBase::rShaderBase( iInit *_tempInit ) : rShaderBase( _tempInit->getDevice() ) {}
rShaderBase::rShaderBase( rWorld *_tempWorld ) : rShaderBase( _tempWorld->getDevice() ) {}

rShaderBase::rShaderBase( VkDevice _device ) : vDevice_vk( _device ) {}

rShaderBase::~rShaderBase() {
   if ( vVertModule_vk )
      vkDestroyShaderModule( vDevice_vk, vVertModule_vk, nullptr );

   if ( vTescModule_vk )
      vkDestroyShaderModule( vDevice_vk, vTescModule_vk, nullptr );

   if ( vTeseModule_vk )
      vkDestroyShaderModule( vDevice_vk, vTeseModule_vk, nullptr );

   if ( vGeomModule_vk )
      vkDestroyShaderModule( vDevice_vk, vGeomModule_vk, nullptr );

   if ( vFragModule_vk )
      vkDestroyShaderModule( vDevice_vk, vFragModule_vk, nullptr );

   if ( vCompModule_vk )
      vkDestroyShaderModule( vDevice_vk, vCompModule_vk, nullptr );

   if ( vModulesCreated ) {
      vkDestroyDescriptorSetLayout( vDevice_vk, vDescLayout_vk, nullptr );
      vkDestroyPipelineLayout( vDevice_vk, vPipelineLayout_vk, nullptr );
      vkDestroyDescriptorPool( vDevice_vk, vDescPool_vk, nullptr );
   }
}

bool rShaderBase::createModule( VkShaderModule *_module, std::vector<unsigned char> _data ) {
   VkShaderModuleCreateInfo lInfo = {};
   lInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
   lInfo.pNext                    = nullptr;
   lInfo.flags                    = 0;
   lInfo.codeSize                 = _data.size();
   lInfo.pCode                    = reinterpret_cast<uint32_t *>( _data.data() );

   auto lRes = vkCreateShaderModule( vDevice_vk, &lInfo, nullptr, _module );
   if ( lRes ) {
      eLOG( "'vkCreateShaderModule' returned ", uEnum2Str::toStr( lRes ) );
      return false;
   }

   return true;
}

VkDescriptorType rShaderBase::getDescriptorType( std::string _str ) {
   if ( _str == "image2D" )
      return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

   if ( _str == "sampler" )
      return VK_DESCRIPTOR_TYPE_SAMPLER;

   if ( _str == "texture2D" )
      return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

   if ( _str == "sampler2D" )
      return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

   if ( _str == "samplerBuffer" )
      return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

   if ( _str == "imageBuffer" )
      return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

   if ( _str == "subpassInput" )
      return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

   return VK_DESCRIPTOR_TYPE_MAX_ENUM; // Error nor found
}

void rShaderBase::addLayoutBindings( VkShaderStageFlags _stage, ShaderInfo _info ) {
   for ( auto const &i : _info.uniforms ) {
      VkDescriptorSetLayoutBinding lTemp = {};
      lTemp.binding                      = i.location;
      lTemp.descriptorType               = getDescriptorType( i.type );
      lTemp.descriptorCount              = i.arraySize;
      lTemp.stageFlags                   = _stage;
      lTemp.pImmutableSamplers           = nullptr; //! \todo Implement if found useful

      if ( lTemp.descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM ) {
         wLOG( "Unknown uniform descriptor type '", i.type, "' -- ignore" );
         continue;
      }

      vLayoutBindings.emplace_back( lTemp );

      bool lFound = false;
      for ( auto &i : vDescPoolSizes ) {
         if ( i.type == lTemp.descriptorType ) {
            i.descriptorCount++;
            lFound = true;
            break;
         }
      }

      if ( !lFound )
         vDescPoolSizes.push_back( {lTemp.descriptorType, 1} );
   }

   for ( auto const &i : _info.uniformBlocks ) {
      VkDescriptorSetLayoutBinding lTemp = {};
      lTemp.binding                      = i.binding;
      lTemp.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
      lTemp.descriptorCount              = 1;
      lTemp.stageFlags                   = _stage;
      lTemp.pImmutableSamplers           = nullptr; //! \todo Implement if found useful

      vLayoutBindings.emplace_back( lTemp );

      bool lFound = false;
      for ( auto &i : vDescPoolSizes ) {
         if ( i.type == lTemp.descriptorType ) {
            i.descriptorCount++;
            lFound = true;
            break;
         }
      }

      if ( !lFound )
         vDescPoolSizes.push_back( {lTemp.descriptorType, 1} );
   }
}

/*!
 * \brief creates all vulkan shader stuff to create a pipeline
 * \todo Evaluate support for multiple descriptor sets
 */
bool rShaderBase::init() {
   if ( vModulesCreated ) {
      wLOG( "Shader modules already created!" );
      return false;
   }

   bool lStatus = true;

   VkPipelineShaderStageCreateInfo lInfo;
   lInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
   lInfo.pNext               = nullptr;
   lInfo.flags               = 0;
   lInfo.stage               = VK_SHADER_STAGE_ALL; // Set further down
   lInfo.module              = nullptr;             // Set further down
   lInfo.pName               = "main";              // There is no point in using another name
   lInfo.pSpecializationInfo = nullptr; //! \todo evaluate if this feature is should be supported

   std::vector<VkPipelineShaderStageCreateInfo> lTemp;

   if ( has_vert() ) {
      lStatus      = lStatus && createModule( &vVertModule_vk, getRawData_vert() );
      lInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
      lInfo.module = vVertModule_vk;
      lTemp.push_back( lInfo );
      addLayoutBindings( VK_SHADER_STAGE_VERTEX_BIT, getInfo_vert() );
   }

   if ( has_tesc() ) {
      lStatus      = lStatus && createModule( &vTescModule_vk, getRawData_tesc() );
      lInfo.stage  = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
      lInfo.module = vTescModule_vk;
      lTemp.push_back( lInfo );
      addLayoutBindings( VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, getInfo_tesc() );
   }

   if ( has_tese() ) {
      lStatus      = lStatus && createModule( &vTeseModule_vk, getRawData_tese() );
      lInfo.stage  = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
      lInfo.module = vTeseModule_vk;
      lTemp.push_back( lInfo );
      addLayoutBindings( VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, getInfo_tese() );
   }

   if ( has_geom() ) {
      lStatus      = lStatus && createModule( &vGeomModule_vk, getRawData_geom() );
      lInfo.stage  = VK_SHADER_STAGE_GEOMETRY_BIT;
      lInfo.module = vGeomModule_vk;
      lTemp.push_back( lInfo );
      addLayoutBindings( VK_SHADER_STAGE_GEOMETRY_BIT, getInfo_geom() );
   }

   if ( has_frag() ) {
      lStatus      = lStatus && createModule( &vFragModule_vk, getRawData_frag() );
      lInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
      lInfo.module = vFragModule_vk;
      lTemp.push_back( lInfo );
      addLayoutBindings( VK_SHADER_STAGE_FRAGMENT_BIT, getInfo_frag() );
   }

   if ( has_comp() ) {
      lStatus      = lStatus && createModule( &vCompModule_vk, getRawData_comp() );
      lInfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
      lInfo.module = vCompModule_vk;
      lTemp.push_back( lInfo );
      addLayoutBindings( VK_SHADER_STAGE_COMPUTE_BIT, getInfo_comp() );
   }

   if ( !lStatus )
      return false;

   VkDescriptorSetLayoutCreateInfo lSetInfo;
   lSetInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
   lSetInfo.pNext        = nullptr;
   lSetInfo.flags        = 0;
   lSetInfo.bindingCount = vLayoutBindings.size();
   lSetInfo.pBindings    = vLayoutBindings.data();

   auto lRes = vkCreateDescriptorSetLayout( vDevice_vk, &lSetInfo, nullptr, &vDescLayout_vk );
   if ( lRes ) {
      eLOG( "'vkCreateDescriptorSetLayout' returend ", uEnum2Str::toStr( lRes ) );
      return false;
   }

   VkPipelineLayoutCreateInfo lPipeInfo;
   lPipeInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
   lPipeInfo.pNext                  = nullptr;
   lPipeInfo.flags                  = 0;
   lPipeInfo.setLayoutCount         = 1;
   lPipeInfo.pSetLayouts            = &vDescLayout_vk;
   lPipeInfo.pushConstantRangeCount = 0;
   lPipeInfo.pPushConstantRanges    = nullptr; //! \todo Add push constants support

   lRes = vkCreatePipelineLayout( vDevice_vk, &lPipeInfo, nullptr, &vPipelineLayout_vk );
   if ( lRes ) {
      eLOG( "'vkCreatePipelineLayout' returend ", uEnum2Str::toStr( lRes ) );
      return false;
   }

   VkDescriptorPoolCreateInfo lDescPoolInfo;
   lDescPoolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
   lDescPoolInfo.pNext         = nullptr;
   lDescPoolInfo.flags         = 0;
   lDescPoolInfo.maxSets       = 1;
   lDescPoolInfo.poolSizeCount = vDescPoolSizes.size();
   lDescPoolInfo.pPoolSizes    = vDescPoolSizes.data();

   lRes = vkCreateDescriptorPool( vDevice_vk, &lDescPoolInfo, nullptr, &vDescPool_vk );
   if ( lRes ) {
      eLOG( "'vkCreateDescriptorPool' returend ", uEnum2Str::toStr( lRes ) );
      return false;
   }

   VkDescriptorSetAllocateInfo lAllocInfo;
   lAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
   lAllocInfo.pNext              = nullptr;
   lAllocInfo.descriptorPool     = vDescPool_vk;
   lAllocInfo.descriptorSetCount = 1;
   lAllocInfo.pSetLayouts        = &vDescLayout_vk;

   lRes = vkAllocateDescriptorSets( vDevice_vk, &lAllocInfo, &vDescSet_vk );
   if ( lRes ) {
      eLOG( "'vkAllocateDescriptorSets' returend ", uEnum2Str::toStr( lRes ) );
      return false;
   }


   vModulesCreated = true;
   return true;
}


std::vector<VkPipelineShaderStageCreateInfo> rShaderBase::getShaderStageInfo() {
   if ( !vModulesCreated )
      if ( !init() )
         return {};

   return vShaderStageInfo;
}

VkDescriptorSet rShaderBase::getDescriptorSet() {
   if ( !vModulesCreated )
      if ( !init() )
         return nullptr;

   return vDescSet_vk;
}

VkDescriptorSetLayout rShaderBase::getDescriptorSetLayout() {
   if ( !vModulesCreated )
      if ( !init() )
         return nullptr;

   return vDescLayout_vk;
}

VkPipelineLayout rShaderBase::getPipelineLayout() {
   if ( !vModulesCreated )
      if ( !init() )
         return nullptr;

   return vPipelineLayout_vk;
}


bool rShaderBase::isInitialized() { return vModulesCreated; }
}
