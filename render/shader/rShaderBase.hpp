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

#pragma once

#include "defines.hpp"
#include <vulkan.h>
#include <vector>
#include <string>

namespace e_engine {

class iInit;
class rWorld;

class rShaderBase {
 public:
   typedef struct InOut {
      std::string type;
      std::string name;
      uint32_t location;
      uint32_t arraySize;
   } InOut;

   typedef struct Uniform {
      std::string name;
      uint32_t binding;

      std::vector<InOut> vars;
   } Uniform;

   typedef struct ShaderInfo {
      std::vector<InOut> input;
      std::vector<InOut> output;
      std::vector<InOut> uniforms;
      std::vector<Uniform> uniformBlocks;
   } ShaderInfo;

 private:
   VkDevice vDevice_vk;

   VkShaderModule vVertModule_vk = nullptr;
   VkShaderModule vTescModule_vk = nullptr;
   VkShaderModule vTeseModule_vk = nullptr;
   VkShaderModule vGeomModule_vk = nullptr;
   VkShaderModule vFragModule_vk = nullptr;
   VkShaderModule vCompModule_vk = nullptr;

   VkDescriptorSetLayout vDescLayout_vk = nullptr;
   VkDescriptorPool vDescPool_vk        = nullptr;
   VkDescriptorSet vDescSet_vk          = nullptr;
   VkPipelineLayout vPipelineLayout_vk  = nullptr;

   std::vector<VkPipelineShaderStageCreateInfo> vShaderStageInfo;
   std::vector<VkDescriptorSetLayoutBinding> vLayoutBindings;
   std::vector<VkDescriptorPoolSize> vDescPoolSizes;

   bool vModulesCreated = false;

   bool createModule( VkShaderModule *_module, std::vector<unsigned char> _data );
   VkDescriptorType getDescriptorType( std::string _str );
   void addLayoutBindings( VkShaderStageFlags _stage, ShaderInfo _info );

 public:
   rShaderBase() = delete;
   rShaderBase( VkDevice _device );
   rShaderBase( iInit *_tempInit );
   rShaderBase( rWorld *_tempWorld );
   ~rShaderBase();

   bool init();
   bool isInitialized();
   std::vector<VkPipelineShaderStageCreateInfo> getShaderStageInfo();
   VkDescriptorSet getDescriptorSet();
   VkDescriptorSetLayout getDescriptorSetLayout();
   VkPipelineLayout getPipelineLayout();

   virtual bool has_vert() const = 0;
   virtual bool has_tesc() const = 0;
   virtual bool has_tese() const = 0;
   virtual bool has_geom() const = 0;
   virtual bool has_frag() const = 0;
   virtual bool has_comp() const = 0;

   virtual ShaderInfo getInfo_vert() const = 0;
   virtual ShaderInfo getInfo_tesc() const = 0;
   virtual ShaderInfo getInfo_tese() const = 0;
   virtual ShaderInfo getInfo_geom() const = 0;
   virtual ShaderInfo getInfo_frag() const = 0;
   virtual ShaderInfo getInfo_comp() const = 0;

   virtual std::vector<unsigned char> getRawData_vert() const = 0;
   virtual std::vector<unsigned char> getRawData_tesc() const = 0;
   virtual std::vector<unsigned char> getRawData_tese() const = 0;
   virtual std::vector<unsigned char> getRawData_geom() const = 0;
   virtual std::vector<unsigned char> getRawData_frag() const = 0;
   virtual std::vector<unsigned char> getRawData_comp() const = 0;
};
}
