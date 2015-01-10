/*!
* \file rRenderMultipleLights_3_3.hpp
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

#ifndef R_RENDER_MULTIPLE_LIGHTS_3_3_HPP
#define R_RENDER_MULTIPLE_LIGHTS_3_3_HPP

#include "defines.hpp"

#include <GL/glew.h>
#include <vector>
#include "rRenderBase.hpp"
#include "rObjectBase.hpp"
#include "rMatrixMath.hpp"
#include "rLightSourceStructs.hpp"

namespace e_engine {

class rRenderMultipleLights_3_3 : public rRenderBase {
 private:
   GLuint vVertexBufferObj_OGL = NOT_SET_ui;
   GLuint vIndexBufferObj_OGL = NOT_SET_ui;
   GLuint vNormalBufferObj_OGL = NOT_SET_ui;

   GLuint vShader_OGL = NOT_SET_ui;

   GLuint vInputVertexLocation_OGL = NOT_SET_ui;
   GLuint vInputNormalsLocation_OGL = NOT_SET_ui;
   GLint vUniformMVP_OGL = NOT_SET;
   GLint vUniformModelView_OGL = NOT_SET;
   GLint vUniformNormal_OGL = NOT_SET;

   GLint vUniformNumLights = NOT_SET;

   struct sUniforms {
      GLint type = NOT_SET;
      GLint ambient = NOT_SET;
      GLint color = NOT_SET;
      GLint pos = NOT_SET;
      GLint attenuation = NOT_SET;
   };

   std::vector<sUniforms> vUniforms;

   GLsizei vDataSize_uI = 0;

   rMat4f *vModelViewProjection = nullptr;
   rMat4f *vModelView = nullptr;
   rMat3f *vNormal = nullptr;

   std::vector<rRenderDirectionalLight<float>> vDirectionalLight;
   std::vector<rRenderPointLight<float>> vPointLight;

 public:
   rRenderMultipleLights_3_3() {}
   virtual ~rRenderMultipleLights_3_3() {}

   virtual void render();
   virtual RENDERER_ID getRendererID() const { return render_OGL_3_3_Normal_Basic_1S_1D; }
   virtual void setDataFromShader( rShader *_s );
   virtual void setDataFromObject( rObjectBase *_obj );

   virtual void setDataFromAdditionalObjects( rObjectBase *_obj );

   virtual bool canRender();

   static bool testShader( rShader *_shader );
   static bool testObject( rObjectBase *_obj );
};
}

#endif // R_RENDER_MULTIPLE_LIGHTS_3_3_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
