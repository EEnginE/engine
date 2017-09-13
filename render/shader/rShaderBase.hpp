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
#include "vkuDevice.hpp"
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan.h>

#include "rMaterial.hpp"

namespace e_engine {

namespace internal {
class rRendererBase;


static const std::vector<std::vector<std::string>> gShaderInputVarNames = {
    {"iVertex", "iVertices", "iVert"},                // Input Verex
    {"iNormal", "iNormals", "iNorm"},                 // Input Normal
    {"iUV", "iUVs"},                                  // Input UV
    {"uMVP", "MVP", "mvp"},                           // Matrix Model View Projection
    {"uViewProject", "uView", "viewProject", "view"}, // View Projection matrix
    {"uModel", "Model", "model"},                     // Model matrix
    {"uPos", "uPosition", "pos", "position"},         // Subpass position data
    {"uNormal", "normal"},                            // Subpass normal data
    {"uColor", "uAlbedo", "color", "albedo"},         // Subpass color / albedo data
    {}};

enum SHADER_INPUT_NAME_INDEX {
  IN_VERTEX   = 0,
  IN_NORMAL   = 1,
  IN_UV       = 2,
  U_M_MVP     = 3,
  U_M_VP      = 4,
  U_M_M       = 5,
  U_SP_POS    = 6,
  U_SP_NORM   = 7,
  U_SP_ALBEDO = 8
};
}

class rShaderBase {
 public:
  enum UNIFORM_ROLE {
    MODEL_VIEW_PROJECTION_MATRIX,
    VIEW_PROJECTION_MATRIX,
    MODEL_MATRIX,
    POSITION_SUBPASS_DATA,
    NORMAL_SUBPASS_DATA,
    ALBEDO_SUBPASS_DATA,
    UNKONOWN
  };

  typedef struct InOut {
    std::string type;
    std::string name;
    uint32_t    location;
    uint32_t    arraySize;

    bool operator<(const InOut &rhs);
  } InOut;

  typedef struct Uniform {
    std::string name;
    uint32_t    binding;

    std::vector<InOut> vars;
  } Uniform;

  typedef struct ShaderInfo {
    std::vector<InOut>   input;
    std::vector<InOut>   output;
    std::vector<InOut>   uniforms;
    std::vector<InOut>   pushConstants;
    std::vector<Uniform> uniformBlocks;
  } ShaderInfo;

  struct UniformBuffer {
    struct Var {
      std::string name;
      std::string type;
      uint32_t    offset;
      uint32_t    size;

      UNIFORM_ROLE   guessedRole = UNKONOWN;
      VkDeviceMemory mem;

      bool operator==(const Var &rhs) const { return mem == rhs.mem && offset == rhs.offset; }
    };

    VkShaderStageFlags stage;
    uint32_t           size;
    VkDeviceMemory     mem;

    std::vector<Var> vars;
  };

  struct PushConstantVar {
    VkShaderStageFlagBits stage;

    std::string name;
    std::string type;
    uint32_t    offset;
    uint32_t    size;

    UNIFORM_ROLE guessedRole = UNKONOWN;
  };

  struct UniformVar {
    VkShaderStageFlagBits stage;

    std::string name;
    std::string type;
    uint32_t    binding;
    uint32_t    arraySize;

    UNIFORM_ROLE guessedRole = UNKONOWN;
  };

 private:
  vkuDevicePTR vDevice;
  VkDevice     vDevice_vk; //!< \brief Shortcut for **vDevice \todo Evaluate elimenating this.

  VkShaderModule vVertModule_vk = nullptr;
  VkShaderModule vTescModule_vk = nullptr;
  VkShaderModule vTeseModule_vk = nullptr;
  VkShaderModule vGeomModule_vk = nullptr;
  VkShaderModule vFragModule_vk = nullptr;
  VkShaderModule vCompModule_vk = nullptr;

  VkDescriptorSetLayout vDescLayout_vk     = nullptr;
  VkDescriptorPool      vDescPool_vk       = nullptr;
  VkPipelineLayout      vPipelineLayout_vk = nullptr;

  VkVertexInputBindingDescription                vInputBindingDesc = {};
  std::vector<VkVertexInputAttributeDescription> vInputDescs;

  std::vector<VkPipelineShaderStageCreateInfo> vShaderStageInfo;
  std::vector<VkDescriptorSetLayoutBinding>    vLayoutBindings;
  std::vector<VkDescriptorPoolSize>            vDescPoolSizes;
  std::vector<VkWriteDescriptorSet>            vWriteDescData;
  std::vector<VkPushConstantRange>             vPushConstants;
  std::vector<VkBuffer>                        vBuffers;
  std::vector<VkDeviceMemory>                  vMemory;
  std::vector<UniformVar>                      vUniformDesc;
  std::vector<UniformBuffer>                   vUniformBufferDescs;
  std::vector<PushConstantVar>                 vPushConstantDescs;

  std::unordered_map<rMaterial const *, VkDescriptorSet> vDescSetMap;

  // Data storage
  std::vector<VkDescriptorBufferInfo> vDataBufferInfo;

  bool vModulesCreated = false;

  bool createModule(VkShaderModule *_module, std::vector<unsigned char> _data);
  VkDescriptorType getDescriptorType(std::string _str);
  UNIFORM_ROLE guessRole(std::string _type, std::string _name);
  void addLayoutBindings(VkShaderStageFlagBits _stage, ShaderInfo _info);

  uint32_t createUniformBuffer(uint32_t _size);


  // Uniform handling

  std::vector<UniformBuffer::Var> vReservedUniforms;

  void signalRenderReset();

 public:
  rShaderBase() = delete;
  rShaderBase(vkuDevicePTR _device);
  virtual ~rShaderBase();

  bool                                         init();
  void                                         destroy();
  bool                                         isInitialized();
  std::vector<VkPipelineShaderStageCreateInfo> getShaderStageInfo();
  VkDescriptorSet getDescriptorSet(rMaterial const *_materialPtr = nullptr);
  VkDescriptorSetLayout                          getDescriptorSetLayout();
  VkPipelineLayout                               getPipelineLayout();
  VkVertexInputBindingDescription                getVertexInputBindingDescription();
  std::vector<VkVertexInputAttributeDescription> getVertexInputAttribureDescriptions();

  static bool getGLSLTypeInfo(std::string _name, uint32_t &_size, VkFormat &_format);


  // Uniform handling

  UniformBuffer const *getUniformBuffer(VkShaderStageFlagBits _stage);
  bool updateUniform(UniformBuffer::Var const &_var, void const *_data);
  bool tryReserveUniform(UniformBuffer::Var const &_var);
  std::vector<PushConstantVar> getPushConstants(VkShaderStageFlagBits _stage);
  std::vector<UniformVar> getUniforms();

  bool updateDescriptorSet(UniformVar const &_var,
                           void *            _data,
                           rMaterial const * _materialPtr = nullptr,
                           uint32_t          _elemet      = 0);

  void cmdUpdatePushConstant(VkCommandBuffer _buf, PushConstantVar const &_var, void const *_data);

  void cmdBindDescriptorSets(VkCommandBuffer     _buf,
                             VkPipelineBindPoint _bindPoint,
                             rMaterial const *   _materialPtr = nullptr);

  virtual std::string getName() = 0;

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

  friend class internal::rRendererBase;
};
}

template std::vector<e_engine::rShaderBase::InOut>::~vector();
template std::vector<e_engine::rShaderBase::Uniform>::~vector();
template std::vector<e_engine::rShaderBase::ShaderInfo>::~vector(); // Clang needs this for some reason
