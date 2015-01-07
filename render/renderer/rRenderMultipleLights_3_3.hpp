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
   GLuint vVertexBufferObj_OGL = 0;
   GLuint vIndexBufferObj_OGL = 0;
   GLuint vNormalBufferObj_OGL = 0;

   GLuint vShader_OGL = 0;

   GLint vInputVertexLocation_OGL = -1;
   GLint vInputNormalsLocation_OGL = -1;
   GLint vUniformMVP_OGL = -1;
   GLint vUniformModelView_OGL = -1;
   GLint vUniformNormal_OGL = -1;

   GLint vUniformNumLights = -1;

   struct sUniforms {
      GLint type = -1;
      GLint ambient = -1;
      GLint color = -1;
      GLint pos = -1;
      GLint attenuation = -1;
   };

   std::vector<sUniforms> vUniforms;

   GLuint vDataSize_uI = 0;

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
