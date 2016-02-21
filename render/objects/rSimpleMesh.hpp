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

#ifndef R_NORMAL_OBJECT_HPP
#define R_NORMAL_OBJECT_HPP

#include "defines.hpp"

#include <string>
#include "rMatrixObjectBase.hpp"
#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include "rObjectBase_data.hpp"


namespace e_engine {

class RENDER_API rSimpleMesh final : public rMatrixObjectBase<float>,
                                     public rObjectBase,
                                     public rObjectBase_data<float, unsigned short> {
 private:
   unsigned vVertexBufferObject;
   unsigned vIndexBufferObject;

   void setFlags();

   int clearOGLData__();
   int setOGLData__();

 public:
   rSimpleMesh( rMatrixSceneBase<float> *_scene, DATA _data )
       : rMatrixObjectBase( _scene ),
         rObjectBase( _data->vName ),
         rObjectBase_data( _data, vObjectHints, vObjects ) {
      setFlags();
   }

   virtual ~rSimpleMesh() { clearOGLData(); }

   rSimpleMesh() = delete;
   rSimpleMesh( rSimpleMesh && ) = default;

   virtual uint32_t getMatrix( rMat4f **_mat, rObjectBase::MATRIX_TYPES _type );
   virtual uint32_t getMatrix( rMat3f **_mat, rObjectBase::MATRIX_TYPES _type );
};
}

#endif // R_NORMAL_OBJECT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
