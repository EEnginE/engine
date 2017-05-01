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
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"
#include "rWorld.hpp"
#include <algorithm>
#include <string.h> // memcpy

#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

namespace e_engine {

rShaderBase::rShaderBase(iInit *_init) : vDevice_vk(_init->getDevice()), vInitPtr(_init) {}
rShaderBase::rShaderBase(rWorld *_tempWorld) : rShaderBase(_tempWorld->getInitPtr()) {}

using namespace internal;

/*!
 * \brief Compare operator
 *
 * Sort criteria:
 *  - vertex input first (looking for name and then location)
 *  - normal input second (looking for name and then location)
 *  - uv coords input third (looking for name and then location)
 */
bool rShaderBase::InOut::operator<(const InOut &rhs) {
  // Vertex
  for (auto const &i : gShaderInputVarNames[IN_VERTEX])
    if (name == i) return true;

  // Normal
  for (auto const &i : gShaderInputVarNames[IN_NORMAL]) {
    if (name == i) {
      for (auto const &j : gShaderInputVarNames[IN_VERTEX])
        if (rhs.name == j) return false;

      return true;
    }
  }

  // UV
  for (auto const &i : gShaderInputVarNames[IN_UV]) {
    if (name == i) {
      for (auto const &j : gShaderInputVarNames[IN_VERTEX])
        if (rhs.name == j) return false;

      for (auto const &j : gShaderInputVarNames[IN_NORMAL])
        if (rhs.name == j) return false;

      return true;
    }
  }

  return location < rhs.location;
}

rShaderBase::~rShaderBase() {
  for (auto i : vBuffers) vkDestroyBuffer(vDevice_vk, i, nullptr);

  for (auto i : vMemory) vkFreeMemory(vDevice_vk, i, nullptr);

  if (vVertModule_vk) vkDestroyShaderModule(vDevice_vk, vVertModule_vk, nullptr);

  if (vTescModule_vk) vkDestroyShaderModule(vDevice_vk, vTescModule_vk, nullptr);

  if (vTeseModule_vk) vkDestroyShaderModule(vDevice_vk, vTeseModule_vk, nullptr);

  if (vGeomModule_vk) vkDestroyShaderModule(vDevice_vk, vGeomModule_vk, nullptr);

  if (vFragModule_vk) vkDestroyShaderModule(vDevice_vk, vFragModule_vk, nullptr);

  if (vCompModule_vk) vkDestroyShaderModule(vDevice_vk, vCompModule_vk, nullptr);

  if (vModulesCreated) {
    vkDestroyDescriptorSetLayout(vDevice_vk, vDescLayout_vk, nullptr);
    vkDestroyPipelineLayout(vDevice_vk, vPipelineLayout_vk, nullptr);
    vkDestroyDescriptorPool(vDevice_vk, vDescPool_vk, nullptr);
  }
}

bool rShaderBase::createModule(VkShaderModule *_module, std::vector<unsigned char> _data) {
  VkShaderModuleCreateInfo lInfo = {};
  lInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  lInfo.pNext                    = nullptr;
  lInfo.flags                    = 0;
  lInfo.codeSize                 = _data.size();
  lInfo.pCode                    = reinterpret_cast<uint32_t *>(_data.data());

  auto lRes = vkCreateShaderModule(vDevice_vk, &lInfo, nullptr, _module);
  if (lRes) {
    eLOG("'vkCreateShaderModule' returned ", uEnum2Str::toStr(lRes));
    return false;
  }

  return true;
}

uint32_t rShaderBase::createUniformBuffer(uint32_t _size) {
  vBuffers.emplace_back();
  vMemory.emplace_back();

  uint32_t             lIndex;
  VkMemoryRequirements lMemReqs;

  VkBufferCreateInfo lBuffInfo    = {};
  lBuffInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  lBuffInfo.pNext                 = nullptr;
  lBuffInfo.flags                 = 0;
  lBuffInfo.size                  = _size;
  lBuffInfo.usage                 = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  lBuffInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
  lBuffInfo.queueFamilyIndexCount = 0;
  lBuffInfo.pQueueFamilyIndices   = nullptr;

  dVkLOG("    -- Creating uniform buffer, size = ", _size);

  auto lRes = vkCreateBuffer(vDevice_vk, &lBuffInfo, nullptr, &vBuffers.back());
  if (lRes) {
    eLOG("'vkCreateBuffer' returned ", uEnum2Str::toStr(lRes));
    return UINT32_MAX;
  }

  vkGetBufferMemoryRequirements(vDevice_vk, vBuffers.back(), &lMemReqs);

  lIndex = vInitPtr->getMemoryTypeIndexFromBitfield(lMemReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  if (lIndex == UINT32_MAX) {
    eLOG("Unable to find memory type");
    return UINT32_MAX;
  }

  VkMemoryAllocateInfo lAllocInfo = {};
  lAllocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  lAllocInfo.pNext                = nullptr;
  lAllocInfo.allocationSize       = lMemReqs.size;
  lAllocInfo.memoryTypeIndex      = lIndex;
  lRes                            = vkAllocateMemory(vDevice_vk, &lAllocInfo, nullptr, &vMemory.back());
  if (lRes) {
    eLOG("'vkAllocateMemory' returned ", uEnum2Str::toStr(lRes));
    return UINT32_MAX;
  }

  lRes = vkBindBufferMemory(vDevice_vk, vBuffers.back(), vMemory.back(), 0);
  if (lRes) {
    eLOG("'vkBindBufferMemory' returned ", uEnum2Str::toStr(lRes));
    return UINT32_MAX;
  }

  return vMemory.size() - 1;
}

VkDescriptorType rShaderBase::getDescriptorType(std::string _str) {
  if (_str == "image2D") return VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;

  if (_str == "sampler") return VK_DESCRIPTOR_TYPE_SAMPLER;

  if (_str == "texture2D") return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;

  if (_str == "sampler2D") return VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

  if (_str == "samplerBuffer") return VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;

  if (_str == "imageBuffer") return VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;

  if (_str == "subpassInput") return VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;

  return VK_DESCRIPTOR_TYPE_MAX_ENUM; // Error nor found
}

rShaderBase::UNIFORM_ROLE rShaderBase::guessRole(std::string _type, std::string _name) {
  if (_type == "mat4" || _type == "mat4x4") {
    for (auto const &i : gShaderInputVarNames[U_M_MVP])
      if (i == _name) return MODEL_VIEW_PROJECTION_MATRIX;

    for (auto const &i : gShaderInputVarNames[U_M_VP])
      if (i == _name) return VIEW_PROJECTION_MATRIX;

    for (auto const &i : gShaderInputVarNames[U_M_M])
      if (i == _name) return MODEL_MATRIX;
  }

  if (_type == "subpassInput") {
    for (auto const &i : gShaderInputVarNames[U_SP_POS])
      if (i == _name) return POSITION_SUBPASS_DATA;

    for (auto const &i : gShaderInputVarNames[U_SP_NORM])
      if (i == _name) return NORMAL_SUBPASS_DATA;

    for (auto const &i : gShaderInputVarNames[U_SP_ALBEDO])
      if (i == _name) return ALBEDO_SUBPASS_DATA;
  }

  return UNKONOWN;
}

/*!
 * \brief Does all the setup for uniforms
 */
void rShaderBase::addLayoutBindings(VkShaderStageFlagBits _stage, ShaderInfo _info) {
  dVkLOG("  -- stage ", uEnum2Str::toStr(_stage));

  // Handle Uniforms

  for (auto const &i : _info.uniforms) {
    VkDescriptorSetLayoutBinding lTemp = {};
    lTemp.binding                      = i.location;
    lTemp.descriptorType               = getDescriptorType(i.type);
    lTemp.descriptorCount              = i.arraySize;
    lTemp.stageFlags                   = _stage;
    lTemp.pImmutableSamplers           = nullptr; //! \todo Implement if found useful

    vUniformDesc.emplace_back();
    auto *lAlias        = &vUniformDesc.back();
    lAlias->stage       = _stage;
    lAlias->name        = i.name;
    lAlias->type        = i.type;
    lAlias->binding     = i.location;
    lAlias->guessedRole = guessRole(i.type, i.name);

    dVkLOG(
        "    -- Unifrom binding = ", i.location, " ", i.type, " ", i.name, " | ", uEnum2Str::toStr(lAlias->guessedRole));

    if (lTemp.descriptorType == VK_DESCRIPTOR_TYPE_MAX_ENUM) {
      wLOG("Unknown uniform descriptor type '", i.type, "' -- ignore");
      continue;
    }

    vLayoutBindings.emplace_back(lTemp);

    bool lFound = false;
    for (auto &j : vDescPoolSizes) {
      if (j.type == lTemp.descriptorType) {
        j.descriptorCount++;
        lFound = true;
        break;
      }
    }

    if (!lFound) vDescPoolSizes.push_back({lTemp.descriptorType, 1});
  }


  // Push Constants
  VkPushConstantRange lRange = {};
  lRange.stageFlags          = _stage;
  lRange.offset              = 0;
  lRange.size                = 0;

  vPushConstantDescs.reserve(_info.pushConstants.size());
  for (auto const &i : _info.pushConstants) {
    uint32_t lTempSize;
    VkFormat lTempF;

    if (!getGLSLTypeInfo(i.type, lTempSize, lTempF)) {
      wLOG("Unknown uniform type '", i.type, "' -- ignore");
      continue;
    }

    vPushConstantDescs.emplace_back();
    auto *lAlias = &vPushConstantDescs.back();

    lAlias->stage       = _stage;
    lAlias->name        = i.name;
    lAlias->type        = i.type;
    lAlias->offset      = lRange.size;
    lAlias->size        = lTempSize;
    lAlias->guessedRole = guessRole(i.type, i.name);

    lRange.size += lTempSize;
  }

  if (lRange.size != 0) vPushConstants.push_back(lRange);


  // Handle Uniform block

  for (auto const &i : _info.uniformBlocks) {
    VkDescriptorSetLayoutBinding lTemp = {};
    lTemp.binding                      = i.binding;
    lTemp.descriptorType               = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lTemp.descriptorCount              = 1;
    lTemp.stageFlags                   = _stage;
    lTemp.pImmutableSamplers           = nullptr; //! \todo Implement if found useful

    dVkLOG("    -- Unifrom Block binding = ", i.binding, " ", i.name);

    vLayoutBindings.emplace_back(lTemp);

    bool lFound = false;
    for (auto &j : vDescPoolSizes) {
      if (j.type == lTemp.descriptorType) {
        j.descriptorCount++;
        lFound = true;
        break;
      }
    }

    if (!lFound) vDescPoolSizes.push_back({lTemp.descriptorType, 1});

    uint32_t lSize     = 0;
    uint32_t lTempSize = 0;
    VkFormat lTempF;

    vUniformBufferDescs.emplace_back();
    vWriteDescData.emplace_back();

    bool lError = false;
    vDataBufferInfo.reserve(i.vars.size());
    for (auto const &j : i.vars) {
      if (!getGLSLTypeInfo(j.type, lTempSize, lTempF)) {
        wLOG("Unknown uniform type '", j.type, "' -- ignore");
        lError = true;
      }

      vUniformBufferDescs.back().vars.emplace_back();
      auto *lAlias = &vUniformBufferDescs.back().vars.back();

      lAlias->offset      = lSize;
      lAlias->type        = j.type;
      lAlias->name        = j.name;
      lAlias->size        = lTempSize * j.arraySize;
      lAlias->guessedRole = guessRole(j.type, j.name);

      lSize += lTempSize * j.arraySize;
      dVkLOG("      - ", j.type, " ", j.name, " | ", uEnum2Str::toStr(lAlias->guessedRole));
    }

    if (lError) continue;

    auto lIndex = createUniformBuffer(lSize);
    if (lIndex == UINT32_MAX) continue;

    vDataBufferInfo.emplace_back();
    auto *lAlias2 = &vDataBufferInfo.back();
    auto *lAlias1 = &vWriteDescData.back();

    lAlias2->buffer = vBuffers[lIndex];
    lAlias2->offset = 0;
    lAlias2->range  = lSize;

    lAlias1->sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    lAlias1->pNext            = nullptr;
    lAlias1->dstSet           = nullptr; // Set further down
    lAlias1->dstBinding       = i.binding;
    lAlias1->dstArrayElement  = 0;
    lAlias1->descriptorCount  = 1;
    lAlias1->descriptorType   = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    lAlias1->pImageInfo       = nullptr;
    lAlias1->pBufferInfo      = lAlias2;
    lAlias1->pTexelBufferView = nullptr;

    vUniformBufferDescs.back().stage = _stage;
    vUniformBufferDescs.back().size  = lSize;
    vUniformBufferDescs.back().mem   = vMemory[lIndex];

    for (auto &j : vUniformBufferDescs.back().vars) j.mem = vMemory[lIndex];
  }
}

bool rShaderBase::getGLSLTypeInfo(std::string _name, uint32_t &_size, VkFormat &_format) {
  if (_name == "vec4") {
    _size   = sizeof(float) * 4;
    _format = VK_FORMAT_R32G32B32A32_SFLOAT;
    return true;
  }

  if (_name == "vec3") {
    _size   = sizeof(float) * 3;
    _format = VK_FORMAT_R32G32B32_SFLOAT;
    return true;
  }

  if (_name == "vec2") {
    _size   = sizeof(float) * 2;
    _format = VK_FORMAT_R32G32_SFLOAT;
    return true;
  }

  if (_name == "mat4") {
    _size   = sizeof(float) * 4 * 4;
    _format = VK_FORMAT_R32G32B32A32_SFLOAT;
    return true;
  }

  if (_name == "mat3") {
    _size   = sizeof(float) * 3 * 3;
    _format = VK_FORMAT_R32G32B32_SFLOAT;
    return true;
  }

  if (_name == "mat2") {
    _size   = sizeof(float) * 2 * 2;
    _format = VK_FORMAT_R32G32_SFLOAT;
    return true;
  }

  //! \todo expand list

  return false;
}

/*!
 * \brief creates all vulkan shader stuff to create a pipeline
 * \todo Evaluate support for multiple descriptor sets
 */
bool rShaderBase::init() {
  if (vModulesCreated) {
    wLOG("Shader modules already created!");
    return false;
  }

  dVkLOG("Initilizing shader ", getName());

  bool lStatus = true;

  VkPipelineShaderStageCreateInfo lInfo;
  lInfo.sType               = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  lInfo.pNext               = nullptr;
  lInfo.flags               = 0;
  lInfo.stage               = VK_SHADER_STAGE_ALL; // Set further down
  lInfo.module              = nullptr;             // Set further down
  lInfo.pName               = "main";              // There is no point in using another name
  lInfo.pSpecializationInfo = nullptr;             //! \todo evaluate if this feature is should be supported

  if (has_vert()) {
    lStatus      = lStatus && createModule(&vVertModule_vk, getRawData_vert());
    lInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
    lInfo.module = vVertModule_vk;
    vShaderStageInfo.push_back(lInfo);
    addLayoutBindings(VK_SHADER_STAGE_VERTEX_BIT, getInfo_vert());

    vInputBindingDesc.binding   = 0;
    vInputBindingDesc.stride    = 0;
    vInputBindingDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    auto lInputInfo = getInfo_vert().input;
    std::sort(lInputInfo.begin(), lInputInfo.end());

    for (auto const &i : lInputInfo) {
      vInputDescs.emplace_back();
      uint32_t lSize = 0;

      if (!getGLSLTypeInfo(i.type, lSize, vInputDescs.back().format)) {
        eLOG("Format ", i.type, " currently not supported! [skip]");
        return false;
      }

      dVkLOG("    -- Input location = ", i.location, " ", i.type, " ", i.name, " (", lSize, ")");

      vInputDescs.back().binding  = 0;
      vInputDescs.back().location = i.location;
      vInputDescs.back().offset   = vInputBindingDesc.stride;
      vInputBindingDesc.stride += lSize;
    }
  }

  if (has_tesc()) {
    lStatus      = lStatus && createModule(&vTescModule_vk, getRawData_tesc());
    lInfo.stage  = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
    lInfo.module = vTescModule_vk;
    vShaderStageInfo.push_back(lInfo);
    addLayoutBindings(VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, getInfo_tesc());
  }

  if (has_tese()) {
    lStatus      = lStatus && createModule(&vTeseModule_vk, getRawData_tese());
    lInfo.stage  = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
    lInfo.module = vTeseModule_vk;
    vShaderStageInfo.push_back(lInfo);
    addLayoutBindings(VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, getInfo_tese());
  }

  if (has_geom()) {
    lStatus      = lStatus && createModule(&vGeomModule_vk, getRawData_geom());
    lInfo.stage  = VK_SHADER_STAGE_GEOMETRY_BIT;
    lInfo.module = vGeomModule_vk;
    vShaderStageInfo.push_back(lInfo);
    addLayoutBindings(VK_SHADER_STAGE_GEOMETRY_BIT, getInfo_geom());
  }

  if (has_frag()) {
    lStatus      = lStatus && createModule(&vFragModule_vk, getRawData_frag());
    lInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
    lInfo.module = vFragModule_vk;
    vShaderStageInfo.push_back(lInfo);
    addLayoutBindings(VK_SHADER_STAGE_FRAGMENT_BIT, getInfo_frag());
  }

  if (has_comp()) {
    lStatus      = lStatus && createModule(&vCompModule_vk, getRawData_comp());
    lInfo.stage  = VK_SHADER_STAGE_COMPUTE_BIT;
    lInfo.module = vCompModule_vk;
    vShaderStageInfo.push_back(lInfo);
    addLayoutBindings(VK_SHADER_STAGE_COMPUTE_BIT, getInfo_comp());
  }

  if (!lStatus) return false;

  VkDescriptorSetLayoutCreateInfo lSetInfo;
  lSetInfo.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  lSetInfo.pNext        = nullptr;
  lSetInfo.flags        = 0;
  lSetInfo.bindingCount = vLayoutBindings.size();
  lSetInfo.pBindings    = vLayoutBindings.data();

  auto lRes = vkCreateDescriptorSetLayout(vDevice_vk, &lSetInfo, nullptr, &vDescLayout_vk);
  if (lRes) {
    eLOG("'vkCreateDescriptorSetLayout' returend ", uEnum2Str::toStr(lRes));
    return false;
  }

  VkPipelineLayoutCreateInfo lPipeInfo;
  lPipeInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  lPipeInfo.pNext                  = nullptr;
  lPipeInfo.flags                  = 0;
  lPipeInfo.setLayoutCount         = 1;
  lPipeInfo.pSetLayouts            = &vDescLayout_vk;
  lPipeInfo.pushConstantRangeCount = vPushConstants.size();
  lPipeInfo.pPushConstantRanges    = vPushConstants.data();

  lRes = vkCreatePipelineLayout(vDevice_vk, &lPipeInfo, nullptr, &vPipelineLayout_vk);
  if (lRes) {
    eLOG("'vkCreatePipelineLayout' returend ", uEnum2Str::toStr(lRes));
    return false;
  }

  VkDescriptorPoolCreateInfo lDescPoolInfo;
  lDescPoolInfo.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  lDescPoolInfo.pNext         = nullptr;
  lDescPoolInfo.flags         = 0;
  lDescPoolInfo.maxSets       = NUM_MAX_DESCRIPTOR_SETS;
  lDescPoolInfo.poolSizeCount = vDescPoolSizes.size();
  lDescPoolInfo.pPoolSizes    = vDescPoolSizes.data();

  lRes = vkCreateDescriptorPool(vDevice_vk, &lDescPoolInfo, nullptr, &vDescPool_vk);
  if (lRes) {
    eLOG("'vkCreateDescriptorPool' returend ", uEnum2Str::toStr(lRes));
    return false;
  }

  vModulesCreated = true;
  return true;
}

/*!
 * \brief Updates the uniform memory buffer
 * \note This function does NO MEMORY SYNCHRONISATION!
 */
bool rShaderBase::updateUniform(UniformBuffer::Var const &_var, void const *_data) {
  void *lData;
  auto  lRes = vkMapMemory(vDevice_vk, _var.mem, _var.offset, _var.size, 0, &lData);
  if (lRes) {
    eLOG("'vkMapMemory' returned ", uEnum2Str::toStr(lRes));
    return false;
  }

  memcpy(lData, _data, _var.size);

  vkUnmapMemory(vDevice_vk, _var.mem);

  return true;
}

/*!
 * \brief Updates a push constant
 * \vkIntern
 * \warning This function does no error checking
 *
 * _var MUST BE acquired from this object!
 */
void rShaderBase::cmdUpdatePushConstant(VkCommandBuffer _buf, PushConstantVar const &_var, void const *_data) {
  vkCmdPushConstants(_buf, vPipelineLayout_vk, _var.stage, _var.offset, _var.size, _data);
}


/*!
 * \brief Binds the descriptor set associated with _materialPtr
 * \vkIntern
 * \warning This function does no error checking
 *
 * _materialPtr functions only as an ID. The material itself will not be accessed, so nullptr is a
 * valid value.
 */
void rShaderBase::cmdBindDescriptorSets(VkCommandBuffer     _buf,
                                        VkPipelineBindPoint _bindPoint,
                                        rMaterial const *   _materialPtr) {
  VkDescriptorSet lSet = getDescriptorSet(_materialPtr);

  if (lSet == nullptr) {
    eLOG("Failed to aquire descriptor set");
    return;
  }

  vkCmdBindDescriptorSets(_buf, _bindPoint, vPipelineLayout_vk, 0, 1, &lSet, 0, nullptr);
}

std::vector<VkPipelineShaderStageCreateInfo> rShaderBase::getShaderStageInfo() {
  if (!vModulesCreated)
    if (!init()) return {};

  return vShaderStageInfo;
}

/*!
 * \brief Get a descriptor set for a material
 * \param[in] _materialPtr The pointer to a material (nullptr is valid)
 *
 * _materialPtr functions only as an ID. The material itself will not be accessed, so nullptr is a
 * valid value.
 *
 * \returns nullptr on error
 */
VkDescriptorSet rShaderBase::getDescriptorSet(rMaterial const *_materialPtr) {
  if (!vModulesCreated)
    if (!init()) return nullptr;

  if (vDescSetMap.find(_materialPtr) != vDescSetMap.end()) return vDescSetMap[_materialPtr];

  VkDescriptorSet lDescSet = nullptr;

  VkDescriptorSetAllocateInfo lAllocInfo;
  lAllocInfo.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  lAllocInfo.pNext              = nullptr;
  lAllocInfo.descriptorPool     = vDescPool_vk;
  lAllocInfo.descriptorSetCount = 1;
  lAllocInfo.pSetLayouts        = &vDescLayout_vk;

  dVkLOG("Allocating new descriptor set for shader ", getName());

  auto lRes = vkAllocateDescriptorSets(vDevice_vk, &lAllocInfo, &lDescSet);
  if (lRes) {
    eLOG("'vkAllocateDescriptorSets' returend ", uEnum2Str::toStr(lRes));
    return nullptr;
  }

  // Update uniform buffers descriptor set
  for (auto &i : vWriteDescData) i.dstSet = lDescSet;

  vkUpdateDescriptorSets(vDevice_vk, vWriteDescData.size(), vWriteDescData.data(), 0, nullptr);

  vDescSetMap[_materialPtr] = lDescSet;

  return lDescSet;
}

/*!
 * \brief Updates a descriptor set for a material
 * \param[in] _data        What to update (a VkImage, ...)
 * \param[in] _materialPtr The pointer to a material (nullptr is valid)
 *
 * _materialPtr functions only as an ID. The material itself will not be accessed, so nullptr is a
 * valid value.
 *
 * \returns false on error
 */
bool rShaderBase::updateDescriptorSet(UniformVar const &_var,
                                      void *            _data,
                                      rMaterial const * _materialPtr,
                                      uint32_t          _elemet) {
  VkDescriptorSet lDescSet = getDescriptorSet(_materialPtr);
  if (lDescSet == nullptr) {
    eLOG("Failed to acquire descriptor set");
    return false;
  }

  VkDescriptorImageInfo lImageInfo = {nullptr, nullptr, VK_IMAGE_LAYOUT_UNDEFINED};
  //    VkDescriptorBufferInfo lBufferInfo = {nullptr, 0, 0};

  bool lFound = false;

  VkWriteDescriptorSet lWrite;
  lWrite.sType            = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
  lWrite.pNext            = nullptr;
  lWrite.dstSet           = lDescSet;
  lWrite.dstBinding       = _var.binding;
  lWrite.dstArrayElement  = _elemet;
  lWrite.descriptorCount  = 1;
  lWrite.descriptorType   = VK_DESCRIPTOR_TYPE_MAX_ENUM;
  lWrite.pImageInfo       = nullptr;
  lWrite.pBufferInfo      = nullptr;
  lWrite.pTexelBufferView = nullptr;

  if (_var.guessedRole == POSITION_SUBPASS_DATA || _var.guessedRole == NORMAL_SUBPASS_DATA ||
      _var.guessedRole == ALBEDO_SUBPASS_DATA) {
    lFound                 = true;
    lImageInfo.imageView   = reinterpret_cast<VkImageView>(_data);
    lImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    lWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    lWrite.pImageInfo     = &lImageInfo;
  }

  if (!lFound) {
    eLOG("Unknown type ", _var.type, " and/or role ", uEnum2Str::toStr(_var.guessedRole));
    eLOG("DEVELOP: TODO IMPLEMENT!");
    return false;
  }

#if D_LOG_VULKAN
  dLOG("Updating descriptor set for shader ", getName());
  dLOG("  -- descriptorType:          ", uEnum2Str::toStr(lWrite.descriptorType));
  if (lWrite.pImageInfo) {
    dLOG("  -- pImageInfo->sampler:     ", reinterpret_cast<uint64_t>(lWrite.pImageInfo->sampler));
    dLOG("  -- pImageInfo->imageView:   ", reinterpret_cast<uint64_t>(lWrite.pImageInfo->imageView));
    dLOG("  -- pImageInfo->imageLayout: ", uEnum2Str::toStr(lWrite.pImageInfo->imageLayout));
  }
#endif

  vkUpdateDescriptorSets(vDevice_vk, 1, &lWrite, 0, nullptr);
  return true;
}

/*!
 * \returns nullptr on error
 */
VkDescriptorSetLayout rShaderBase::getDescriptorSetLayout() {
  if (!vModulesCreated)
    if (!init()) return nullptr;

  return vDescLayout_vk;
}

/*!
 * \returns nullptr on error
 */
VkPipelineLayout rShaderBase::getPipelineLayout() {
  if (!vModulesCreated)
    if (!init()) return nullptr;

  return vPipelineLayout_vk;
}

VkVertexInputBindingDescription rShaderBase::getVertexInputBindingDescription() {
  if (!vModulesCreated)
    if (!init()) return {};

  return vInputBindingDesc;
}

std::vector<VkVertexInputAttributeDescription> rShaderBase::getVertexInputAttribureDescriptions() {
  if (!vModulesCreated)
    if (!init()) return {};

  return vInputDescs;
}

std::vector<rShaderBase::PushConstantVar> rShaderBase::getPushConstants(VkShaderStageFlagBits _stage) {
  if (!vModulesCreated)
    if (!init()) return {};

  std::vector<rShaderBase::PushConstantVar> lTemp;

  for (auto const &i : vPushConstantDescs)
    if (i.stage == _stage) lTemp.push_back(i);

  return lTemp;
}

std::vector<rShaderBase::UniformVar> rShaderBase::getUniforms() {
  if (!vModulesCreated)
    if (!init()) return {};

  return vUniformDesc;
}

/*!
 * \brief get shader stage unifrom buffer information
 * \returns nullptr on failure
 */
rShaderBase::UniformBuffer const *rShaderBase::getUniformBuffer(VkShaderStageFlagBits _stage) {
  if (!vModulesCreated)
    if (!init()) return nullptr;

  for (auto const &i : vUniformBufferDescs)
    if (i.stage == _stage) return &i;

  return nullptr;
}


bool rShaderBase::isInitialized() { return vModulesCreated; }



// Uniform handling

/*!
 * \brief Reserves modifying a uniform variable
 *
 * All uniform variables in a uniform block are constant during a render pass. Therefore they only
 * have to be modified once.
 *
 * This function is supposed to be called from an implementation of rObjectBase::signalRenderReset()
 *
 * The reserved uniforms will be automatically reset during the init phase of the render loop.
 *
 * \param _var The variable to reserve
 * \returns whether the variable is already reserved by another object (true == not reserved)
 *
 * \note This function does not check if the variable exists!
 */
bool rShaderBase::tryReserveUniform(UniformBuffer::Var const &_var) {
  for (auto const &i : vReservedUniforms)
    if (i == _var) return false;

  vReservedUniforms.push_back(_var);
  return true;
}

/*!
 * \brief Clears reserved uniforms
 * \note This function is only supposed to be called by rRender::renderLoop during the init phase
 */
void rShaderBase::signalRenderReset() { vReservedUniforms.clear(); }
}
