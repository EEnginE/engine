/*!
 * \file rRenderNormal_3_3.hpp
 * \brief \b Classes: \a rRenderNormal_3_3
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

#ifndef RRENDERNORMAL_3_3_HPP
#define RRENDERNORMAL_3_3_HPP

#include "defines.hpp"

#include <GL/glew.h>
#include "rRenderBase.hpp"
#include "rObjectBase.hpp"
#include "rMatrixMath.hpp"

namespace e_engine {

/*!
 * \brief Very basic OpenGL 3.3 renderer
 *
 * ID: render_OGL_3_3_Normal_Basic_1S_1D
 */
class rRenderNormal_3_3 : public rRenderBase {
 private:
   GLuint vVertexBufferObj_OGL;
   GLuint vIndexBufferObj_OGL;

   GLuint vShader_OGL;

   GLint vInputLocation_OGL;
   GLint vUniformLocation_OGL;

   GLuint vDataSize_uI;

   rMat4f *vMatrix;

 public:
   rRenderNormal_3_3();
   virtual ~rRenderNormal_3_3() {}

   virtual void render();
   virtual RENDERER_ID getRendererID() const { return render_OGL_3_3_Normal_Basic_1S_1D; }
   virtual void setDataFromShader( rShader *_s );
   virtual void setDataFromObject( rObjectBase *_obj );

   static bool testShader( rShader *_shader );
   static bool testObject( rObjectBase *_obj );
};
}

#endif // RRENDERNORMAL_3_3_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
