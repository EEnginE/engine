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

#include <string>
#include "rMatrixObjectBase.hpp"
#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include "rBuffer.hpp"
#include "rShaderBase.hpp"

namespace e_engine {

class RENDER_API rSimpleMesh final : public rMatrixObjectBase<float>, public rObjectBase {
 private:
   rBuffer vIndex;
   rBuffer vVertex;

   rShaderBase *vShader                           = nullptr;
   const rShaderBase::UniformBuffer *vVertUniform = nullptr;

   rShaderBase::UniformBuffer::Var vMatrixMVPVar = {};
   bool vHasMVPMatrix                            = false;

   std::vector<rBuffer *> setData_IMPL( VkCommandBuffer _buf,
                                        const std::vector<uint32_t> &_index,
                                        const std::vector<float> &_pos,
                                        const std::vector<float> &_norm,
                                        const std::vector<float> &_uv ) override;

 public:
   rSimpleMesh( rMatrixSceneBase<float> *_scene, std::string _name );

   virtual ~rSimpleMesh() {}

   rSimpleMesh() = delete;
   rSimpleMesh( rSimpleMesh && ) = default;
   rSimpleMesh &operator=( const rSimpleMesh & ) = delete;
   rSimpleMesh &operator=( rSimpleMesh && ) = default;

   bool canRecord() override { return true; }
   void record( VkCommandBuffer _buf ) override;
   void updateUniforms() override;
   void signalRenderReset() override;

   uint32_t getMatrix( rMat4f **_mat, rObjectBase::MATRIX_TYPES _type ) override;
   uint32_t getMatrix( rMat3f **_mat, rObjectBase::MATRIX_TYPES _type ) override;
   bool checkIsCompatible( rPipeline *_pipe ) override;
};
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
