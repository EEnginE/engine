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

static const std::string gShaderVertexInputVarName[]   = {"iVertex", "iVertices", "iVert"};
static const std::string gShaderNormalsInputVarName[]  = {"iNormal", "iNormals", "iNorm"};
static const std::string gShaderUVInputVarName[]       = {"iUV", "iUVs"};
static const std::string gShaderUniformMVPMatrixName[] = {"uMVP", "MVP"};

class rShaderBase {
 public:
   enum UNIFORM_ROLE { MODEL_VIEW_PROJECTION_MATRIX, UNKONOWN };

   typedef struct InOut {
      std::string type;
      std::string name;
      uint32_t location;
      uint32_t arraySize;

      bool operator<( const InOut &rhs );
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

   typedef struct UniformBuffer {
      typedef struct Var {
         std::string name;
         std::string type;
         uint32_t offset;
         uint32_t size;

         UNIFORM_ROLE guessedRole = UNKONOWN;
         VkDeviceMemory mem;
      } Var;

      VkShaderStageFlags stage;
      uint32_t size;
      VkDeviceMemory mem;

      std::vector<Var> vars;
   } UniformBuffer;

 private:
   VkDevice vDevice_vk;
   iInit *vInitPtr;

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

   VkVertexInputBindingDescription vInputBindingDesc = {};
   std::vector<VkVertexInputAttributeDescription> vInputDescs;

   std::vector<VkPipelineShaderStageCreateInfo> vShaderStageInfo;
   std::vector<VkDescriptorSetLayoutBinding> vLayoutBindings;
   std::vector<VkDescriptorPoolSize> vDescPoolSizes;
   std::vector<VkWriteDescriptorSet> vWriteDescData;
   std::vector<VkBuffer> vBuffers;
   std::vector<VkDeviceMemory> vMemory;
   std::vector<UniformBuffer> vUniformBufferDescs;

   // Data storage
   std::vector<VkDescriptorBufferInfo> vDataBufferInfo;

   bool vModulesCreated = false;

   bool createModule( VkShaderModule *_module, std::vector<unsigned char> _data );
   VkDescriptorType getDescriptorType( std::string _str );
   void addLayoutBindings( VkShaderStageFlags _stage, ShaderInfo _info );

   uint32_t createUniformBuffer( uint32_t _size );

 public:
   rShaderBase() = delete;
   rShaderBase( iInit *_init );
   rShaderBase( rWorld *_tempWorld );
   ~rShaderBase();

   bool init();
   bool isInitialized();
   std::vector<VkPipelineShaderStageCreateInfo> getShaderStageInfo();
   VkDescriptorSet getDescriptorSet();
   VkDescriptorSetLayout getDescriptorSetLayout();
   VkPipelineLayout getPipelineLayout();
   VkVertexInputBindingDescription getVertexInputBindingDescription();
   std::vector<VkVertexInputAttributeDescription> getVertexInputAttribureDescriptions();
   UniformBuffer const *getUniformBuffer( VkShaderStageFlagBits _stage );

   static bool getGLSLTypeInfo( std::string _name, uint32_t &_size, VkFormat &_format );

   bool updateUniform( UniformBuffer::Var const &_var, void const *_data );

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
