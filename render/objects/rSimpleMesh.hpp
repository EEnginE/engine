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

namespace e_engine {

class RENDER_API rSimpleMesh final : public rMatrixObjectBase<float>, public rObjectBase {
 private:
   rBuffer vIndex;
   rBuffer vVertex;

 public:
   rSimpleMesh( rMatrixSceneBase<float> *_scene, std::string _name );

   virtual ~rSimpleMesh() {}

   rSimpleMesh() = delete;
   rSimpleMesh( rSimpleMesh && ) = default;
   rSimpleMesh &operator=( const rSimpleMesh & ) = delete;
   rSimpleMesh &operator=( rSimpleMesh && ) = default;

   bool setData( VkCommandBuffer _buf,
                 std::vector<uint32_t> const &_index,
                 std::vector<float> const &_pos,
                 std::vector<float> const &_norm,
                 std::vector<float> const & ) override;

   bool finishData() override;

   uint32_t getMatrix( rMat4f **_mat, rObjectBase::MATRIX_TYPES _type ) override;
   uint32_t getMatrix( rMat3f **_mat, rObjectBase::MATRIX_TYPES _type ) override;
};
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
