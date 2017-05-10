/*!
 * \file rObjectBase.hpp
 */
/*
 * Copyright (C) 2015 EEnginE project
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
 */

#pragma once

#include "defines.hpp"

#include "rBuffer.hpp"
#include "rShaderBase.hpp"
#include <array>
#include <assimp/scene.h>
#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vulkan.h>

namespace e_engine {

// Object Flags:
#define MESH_OBJECT (1 << 0)

#define LIGHT_SOURCE (1 << 1)
#define DIRECTIONAL_LIGHT (1 << 2)
#define POINT_LIGHT (1 << 3)
#define SPOT_LIGHT (1 << 4)

// Matrix Flags:
#define SCALE_MATRIX_FLAG (1 << 0)
#define ROTATION_MATRIX_FLAG (1 << 1)
#define TRANSLATION_MATRIX_FLAG (1 << 2)
#define VIEW_MATRIX_FLAG (1 << 3)
#define PROJECTION_MATRIX_FLAG (1 << 4)
#define CAMERA_MATRIX_FLAG (1 << 5)
#define MODEL_MATRIX_FLAG (1 << 6)
#define MODEL_VIEW_MATRIX_FLAG (1 << 7)
#define MODEL_VIEW_PROJECTION_MATRIX_FLAG (1 << 8)
#define NORMAL_MATRIX_FLAG (1 << 9)

class rPipeline;

namespace internal {
class rRendererBase;
}

/*!
 * \brief Base class for creating objects
 *
 * You MUST set some hints in your constructor about the object and create a clearOGLData__
 * and setOGLData__ function.
 *
 * The data will be loaded into RAM from this class and can than be accessed via vLoaderData
 *
 * You should also return 1 if everything went fine. Values < 0 mean there where errors but data
 * can be safely set / cleared. Values > 1 mean that the data can still be used and may be
 * successfully
 * cleared later. Value = 0 means that this object is completely broken!
 *
 * \warning Always inherit this class FIRST
 */
class rObjectBase {
 public:
  enum ERROR_FLAGS {
    ALL_OK                             = 0,
    FUNCTION_NOT_VALID_FOR_THIS_OBJECT = (1 << 0),
    INDEX_OUT_OF_RANGE                 = (1 << 1),
    DATA_NOT_LOADED                    = (1 << 2),
    UNSUPPORTED_TYPE                   = (1 << 3),
  };

  enum MATRIX_TYPES {
    SCALE,
    ROTATION,
    TRANSLATION,
    VIEW_MATRIX,
    PROJECTION_MATRIX,
    CAMERA_MATRIX,
    MODEL_MATRIX,
    MODEL_VIEW_MATRIX,
    MODEL_VIEW_PROJECTION,
    NORMAL_MATRIX
  };

  enum VECTOR_TYPES { AMBIENT_COLOR, LIGHT_COLOR, POSITION, POSITION_MODEL_VIEW, DIRECTION, ATTENUATION };

  enum LIGHT_MODEL_T { NO_LIGHTS = 0, SIMPLE_ADS_LIGHT };

  enum VERTEX_DATA_LAYOUT {
    POS,
    POS_NORM,
    POS_COLOR,
    POS_UV,
    POS_NORM_COLOR,
    POS_NORM_UV,
    POS_NORM_UV_COLOR,
    UNDEFINED
  };

  using UNIFORM_BUFFER = const rShaderBase::UniformBuffer *;
  using UNIFORM_VAR    = rShaderBase::UniformBuffer::Var;
  using PUSH_CONSTANT  = rShaderBase::PushConstantVar;

 private:
  std::vector<rBuffer *> vLoadBuffers;

 protected:
  std::string vName_str;

  bool       vPartialLoaded_B = false;
  bool       vIsLoaded_B      = false;
  rPipeline *vPipeline        = nullptr;

  virtual std::vector<rBuffer *> setData_IMPL(VkCommandBuffer,
                                              std::vector<uint32_t> const &,
                                              std::vector<float> const &) {
    return {};
  }

  bool setupVertexData_PN(aiMesh const *_mesh, std::vector<float> &_out);
  virtual void destroy_IMPL() {}

 public:
  rObjectBase(std::string _name) : vName_str(_name) {}
  rObjectBase() = delete;

  virtual VERTEX_DATA_LAYOUT getDataLayout() const { return UNDEFINED; }
  virtual MESH_TYPES         getMeshType() const { return UNDEFINED_3D; }

  // Forbid copying
  rObjectBase(const rObjectBase &) = delete;
  rObjectBase &operator=(const rObjectBase &) = delete;

  // Allow moving
  rObjectBase(rObjectBase &&) = default;
  rObjectBase &operator=(rObjectBase &&) = default;

  virtual ~rObjectBase();

  bool setData(VkCommandBuffer _buf, aiMesh const *_mesh);
  void destroy();

  bool finishData();

  virtual bool checkIsCompatible(rPipeline *_pipe) = 0;
  virtual bool isMesh()                            = 0;
  virtual void updateUniforms() {}
  virtual void record(VkCommandBuffer) {}
  virtual void recordLight(VkCommandBuffer, rBuffer &, rBuffer &) {}
  virtual void signalRenderReset(internal::rRendererBase *) {}
  virtual bool supportsPushConstants() { return false; }

  rPipeline *  getPipeline() { return vPipeline; }
  rShaderBase *getShader();
  bool         getIsDataLoaded() const { return vIsLoaded_B; }
  std::string  getName() const { return vName_str; }
  bool setPipeline(rPipeline *_pipe);

  virtual uint32_t getMatrix(glm::mat4 **_mat, MATRIX_TYPES _type);
  virtual uint32_t getMatrix(glm::dmat4 **_mat, MATRIX_TYPES _type);

  virtual uint32_t getMatrix(glm::mat3 **_mat, MATRIX_TYPES _type);
  virtual uint32_t getMatrix(glm::dmat3 **_mat, MATRIX_TYPES _type);

  virtual uint32_t getVector(glm::vec4 **_vec, VECTOR_TYPES _type);
  virtual uint32_t getVector(glm::dvec4 **_vec, VECTOR_TYPES _type);

  virtual uint32_t getVector(glm::vec3 **_vec, VECTOR_TYPES _type);
  virtual uint32_t getVector(glm::dvec3 **_vec, VECTOR_TYPES _type);
};



} // e_engine



// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
