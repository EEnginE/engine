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
#include "rRenderBase.hpp"
#include "rMatrixObjectBase.hpp"
#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"


namespace e_engine {

class rSimpleMesh final : public rMatrixObjectBase<float>, public rObjectBase {
 private:
   GLuint vVertexBufferObject;
   GLuint vIndexBufferObject;
   GLuint vNormalBufferObject;

   void setFlags();

   bool vHasNormals;

 public:
   rSimpleMesh( rMatrixSceneBase<float> *_scene,
                std::string _name,
                std::string _file,
                DATA_FILE_TYPE _type = AUTODETECT )
       : rMatrixObjectBase( _scene ), rObjectBase( _name, _file, _type ), vHasNormals( false ) {
      setFlags();
   }

   rSimpleMesh() = delete;

   virtual ~rSimpleMesh();

   int clearOGLData__();
   int setOGLData__();

   virtual uint32_t getVBO( uint32_t &_n );
   virtual uint32_t getIBO( uint32_t &_n );
   virtual uint32_t getNBO( uint32_t &_n );
   virtual uint32_t getMatrix( e_engine::rMat4f **_mat, rObjectBase::MATRIX_TYPES _type );
   virtual uint32_t getMatrix( e_engine::rMat3f **_mat, rObjectBase::MATRIX_TYPES _type );
};
}

#endif // R_NORMAL_OBJECT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
