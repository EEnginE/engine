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

#include "rBuffer.hpp"
#include "rMatrixObjectBase.hpp"
#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include "rShaderBase.hpp"
#include <string>

namespace e_engine {

class rSimpleMesh final : public rMatrixObjectBase<float>, public rObjectBase {
 private:
  rBuffer vIndex;
  rBuffer vVertex;

  rShaderBase *  vShader      = nullptr;
  UNIFORM_BUFFER vVertUniform = nullptr;

  UNIFORM_VAR   vMatrixMVPVar      = {};
  UNIFORM_VAR   vMatrixVPVar       = {};
  PUSH_CONSTANT vMatrixModelVar_PC = {};
  bool          vHasMVPMatrix      = false;
  bool          vHasVPMatrix       = false;
  bool          vHasModelMatrix_PC = false;

  std::vector<rBuffer *> setData_IMPL(VkCommandBuffer              _buf,
                                      const std::vector<uint32_t> &_index,
                                      const std::vector<float> &   _data) override;

  VERTEX_DATA_LAYOUT getDataLayout() const override { return POS_NORM; }
  MESH_TYPES         getMeshType() const override { return MESH_3D; }

 public:
  rSimpleMesh(rMatrixSceneBase<float> *_scene, std::string _name);

  virtual ~rSimpleMesh() {}

  rSimpleMesh()               = delete;
  rSimpleMesh(rSimpleMesh &&) = default;
  rSimpleMesh &operator=(const rSimpleMesh &) = delete;
  rSimpleMesh &operator=(rSimpleMesh &&) = default;

  bool isMesh() override { return true; }
  bool supportsPushConstants() override { return vHasModelMatrix_PC; }
  void record(VkCommandBuffer _buf) override;
  void updateUniforms() override;
  void signalRenderReset(internal::rRendererBase *) override;

  uint32_t getMatrix(rMat4f **_mat, rObjectBase::MATRIX_TYPES _type) override;
  uint32_t getMatrix(rMat3f **_mat, rObjectBase::MATRIX_TYPES _type) override;
  bool checkIsCompatible(rPipeline *_pipe) override;
};
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
