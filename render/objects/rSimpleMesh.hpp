/*!
 * \file rSimpleMesh.hpp
 * \brief \b Classes: \a rSimpleMesh
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

#include "rTexture.hpp"
#include "rMatrixObjectBase.hpp"
#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include "rShaderBase.hpp"
#include <string>

namespace e_engine {

class rSimpleMesh final : public rMatrixObjectBase<float>, public rObjectBase {
 private:
  vkuBuffer vIndex;
  vkuBuffer vVertex;

  rShaderBase *                        vShader      = nullptr;
  UNIFORM_BUFFER                       vVertUniform = nullptr;
  std::vector<rShaderBase::UniformVar> vUniforms;

  UNIFORM_VAR             vMatrixMVPVar      = {};
  UNIFORM_VAR             vMatrixVPVar       = {};
  UNIFORM_VAR             vMatrixNormal      = {};
  UNIFORM_VAR             vLODBias           = {};
  PUSH_CONSTANT           vMatrixModelVar_PC = {};
  PUSH_CONSTANT           vMatrixMVP_PC      = {};
  bool                    vHasMVPMatrix      = false;
  bool                    vHasVPMatrix       = false;
  bool                    vHasMVPMatrix_PC   = false;
  bool                    vHasModelMatrix_PC = false;
  bool                    vHasNormalMatrix   = false;
  bool                    vHasLODBias        = false;
  bool                    vHasTexture        = false;
  rTexture *              vTexture           = nullptr;
  rShaderBase::UniformVar vTextureVar        = {};
  uint32_t                vIndexCount        = 0;

  std::vector<vkuBuffer *> setData_IMPL(vkuCommandBuffer &           _buf,
                                        const std::vector<uint32_t> &_index,
                                        const std::vector<float> &   _data) override;

  void destroy_IMPL() override;

  VERTEX_DATA_LAYOUT getDataLayout() const override { return POS_NORM_UV; }
  MESH_TYPES         getMeshType() const override { return MESH_3D; }

 public:
  rSimpleMesh(rMatrixSceneBase<float> *_scene, vkuDevicePTR _device, std::string _name);

  ~rSimpleMesh() override { destroy_IMPL(); }

  rSimpleMesh()               = delete;
  rSimpleMesh(rSimpleMesh &&) = default;
  rSimpleMesh &operator=(const rSimpleMesh &) = delete;
  rSimpleMesh &operator=(rSimpleMesh &&) = default;

  bool isMesh() override { return true; }
  bool supportsPushConstants() override { return vHasModelMatrix_PC || vHasMVPMatrix_PC; }
  void record(VkCommandBuffer _buf) override;
  void updateUniforms() override;
  void signalRenderReset(rRendererBase *) override;

  uint32_t getMatrix(glm::mat4 **_mat, rObjectBase::MATRIX_TYPES _type) override;
  uint32_t getMatrix(glm::mat3 **_mat, rObjectBase::MATRIX_TYPES _type) override;
  bool     checkIsCompatible(rPipeline *_pipe) override;
};
} // namespace e_engine


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
