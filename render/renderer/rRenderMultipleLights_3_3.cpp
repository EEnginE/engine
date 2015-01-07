/*!
* \file rRenderMultipleLights_3_3.cpp
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

#include "defines.hpp"

#include "rRenderMultipleLights_3_3.hpp"

namespace e_engine {


void rRenderMultipleLights_3_3::render() {
   glUseProgram( vShader_OGL );

   glUniformMatrix4fv( vUniformMVP_OGL, 1, false, vModelViewProjection->getMatrix() );
   glUniformMatrix4fv( vUniformModelView_OGL, 1, false, vModelView->getMatrix() );
   glUniformMatrix3fv( vUniformNormal_OGL, 1, false, vNormal->getMatrix() );

   glUniform1i( vUniformNumLights, vDirectionalLight.size() + vPointLight.size() );

   unsigned int lClounter = 0;

   for ( auto const &l : vDirectionalLight ) {
      glUniform1i( vUniforms[lClounter].type, 0 );
      glUniform3fv( vUniforms[lClounter].ambient, 1, l.ambient->getMatrix() );
      glUniform3fv( vUniforms[lClounter].color, 1, l.color->getMatrix() );
      glUniform3fv( vUniforms[lClounter].pos, 1, l.direction->getMatrix() );
      ++lClounter;
   }

   for ( auto const &l : vPointLight ) {
      glUniform1i( vUniforms[lClounter].type, 1 );
      glUniform3fv( vUniforms[lClounter].ambient, 1, l.ambient->getMatrix() );
      glUniform3fv( vUniforms[lClounter].color, 1, l.color->getMatrix() );
      glUniform3fv( vUniforms[lClounter].pos, 1, l.position->getMatrix() );
      glUniform3fv( vUniforms[lClounter].attenuation, 1, l.attenuation->getMatrix() );
      ++lClounter;
   }

   glEnableVertexAttribArray( vInputVertexLocation_OGL );
   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObj_OGL );
   glVertexAttribPointer( vInputVertexLocation_OGL, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

   glEnableVertexAttribArray( vInputNormalsLocation_OGL );
   glBindBuffer( GL_ARRAY_BUFFER, vNormalBufferObj_OGL );
   glVertexAttribPointer( vInputNormalsLocation_OGL, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObj_OGL );
   glDrawElements( GL_TRIANGLES, vDataSize_uI, GL_UNSIGNED_INT, nullptr );

   glDisableVertexAttribArray( vInputVertexLocation_OGL );
   glDisableVertexAttribArray( vInputNormalsLocation_OGL );
}


bool rRenderMultipleLights_3_3::testShader( rShader *_shader ) {
   if ( !_shader->getIsLinked() )
      return false;

   if ( _shader->getUniformArraySize( rShader::LIGHT_COLOR ) !=
              _shader->getUniformArraySize( rShader::LIGHT_POSITION ) &&
        _shader->getUniformArraySize( rShader::LIGHT_POSITION ) !=
              _shader->getUniformArraySize( rShader::AMBIENT_COLOR ) )
      return false;

   return require( _shader,
                   rShader::VERTEX_INPUT,
                   rShader::NORMALS_INPUT,
                   rShader::MODEL_VIEW_MATRIX,
                   rShader::NORMAL_MATRIX,
                   rShader::M_V_P_MATRIX,
                   rShader::LIGHT_TYPE,
                   rShader::AMBIENT_COLOR,
                   rShader::NUM_LIGHTS,
                   rShader::LIGHT_COLOR,
                   rShader::LIGHT_POSITION,
                   rShader::LIGHT_ATTENUATION );
}

bool rRenderMultipleLights_3_3::testObject( rObjectBase *_obj ) {
   int lVert, lFlags, lMatrices, lnVBO, lnIBO, lnNBO, lLightModel;

   _obj->getHints( rObjectBase::NUM_INDEXES,
                   lVert,
                   rObjectBase::FLAGS,
                   lFlags,
                   rObjectBase::MATRICES,
                   lMatrices,
                   rObjectBase::NUM_VBO,
                   lnVBO,
                   rObjectBase::NUM_IBO,
                   lnIBO,
                   rObjectBase::NUM_NBO,
                   lnNBO,
                   rObjectBase::LIGHT_MODEL,
                   lLightModel );

   if ( !( lFlags & MESH_OBJECT ) )
      return false;

   if ( lVert < 3 )
      return false;

   if ( !( lMatrices & MODEL_VIEW_PROJECTION_MATRIX_FLAG && lMatrices & MODEL_VIEW_MATRIX_FLAG &&
           lMatrices & NORMAL_MATRIX_FLAG ) )
      return false;

   if ( lnVBO != 1 )
      return false;

   if ( lnIBO != 1 )
      return false;

   if ( lnNBO != 1 )
      return false;

   if ( lLightModel != rObjectBase::SIMPLE_ADS_LIGHT )
      return false;

   return true;
}

bool rRenderMultipleLights_3_3::canRender() {
   if ( !testUnifrom( vInputVertexLocation_OGL,
                      L"Input Vertex",
                      vInputNormalsLocation_OGL,
                      L"Input Normals",
                      vUniformModelView_OGL,
                      L"Model View Matrix",
                      vUniformMVP_OGL,
                      L"Model View Projection Matrix",
                      vUniformNormal_OGL,
                      L"Normal Matrix",
                      vShader_OGL,
                      L"The shader",
                      vUniformNumLights,
                      L"Number of lights",
                      vVertexBufferObj_OGL,
                      L"Vertex buffer object",
                      vIndexBufferObj_OGL,
                      L"Index buffer object",
                      vNormalBufferObj_OGL,
                      L"Normal buffer object" ) )
      return false;

   for ( auto const &u : vUniforms ) {
      if ( !testUnifrom( u.type,
                         L"Light type",
                         u.ambient,
                         L"Ambient Light color",
                         u.color,
                         L"Light color",
                         u.pos,
                         L"Light position",
                         u.attenuation,
                         L"Light attenuation" ) ) {
         return false;
      }
   }

   if ( !testPointer( vModelView,
                      L"Model View Matrix",
                      vModelViewProjection,
                      L"Model View Projection Matrix",
                      vNormal,
                      L"Normal Matrix" ) )
      return false;

   for ( auto const &p : vDirectionalLight ) {
      if ( !testPointer( p.ambient,
                         L"Ambient Light color",
                         p.color,
                         L"Light color",
                         p.direction,
                         L"Light position" ) ) {
         return false;
      }
   }

   for ( auto const &p : vPointLight ) {
      if ( !testPointer( p.ambient,
                         L"Ambient Light color",
                         p.color,
                         L"Light color",
                         p.position,
                         L"Light position",
                         p.attenuation,
                         L"Light attenuation" ) ) {
         return false;
      }
   }

   return true;
}




void rRenderMultipleLights_3_3::setDataFromShader( rShader *_s ) {
   vInputVertexLocation_OGL = _s->getLocation( rShader::VERTEX_INPUT );
   vInputNormalsLocation_OGL = _s->getLocation( rShader::NORMALS_INPUT );

   vUniformModelView_OGL = _s->getLocation( rShader::MODEL_VIEW_MATRIX );
   vUniformNormal_OGL = _s->getLocation( rShader::NORMAL_MATRIX );
   vUniformMVP_OGL = _s->getLocation( rShader::M_V_P_MATRIX );

   vUniformNumLights = _s->getLocation( rShader::NUM_LIGHTS );

   auto lNumMaxLights = _s->getUniformArraySize( rShader::LIGHT_COLOR );

   vUniforms.resize( lNumMaxLights );

   unsigned int lCounter = 0;
   for ( auto &i : vUniforms ) {
      i.type = _s->getLocation( rShader::LIGHT_TYPE, lCounter );
      i.ambient = _s->getLocation( rShader::AMBIENT_COLOR, lCounter );
      i.color = _s->getLocation( rShader::LIGHT_COLOR, lCounter );
      i.pos = _s->getLocation( rShader::LIGHT_POSITION, lCounter );
      i.attenuation = _s->getLocation( rShader::LIGHT_ATTENUATION, lCounter );
      ++lCounter;
   }

   _s->getProgram( vShader_OGL );
}

void rRenderMultipleLights_3_3::setDataFromObject( rObjectBase *_obj ) {
   vVertexBufferObj_OGL = vIndexBufferObj_OGL = 0;
   _obj->getVBO( vVertexBufferObj_OGL );
   _obj->getIBO( vIndexBufferObj_OGL );
   _obj->getNBO( vNormalBufferObj_OGL );
   _obj->getMatrix( &vModelViewProjection, rObjectBase::MODEL_VIEW_PROJECTION );
   _obj->getMatrix( &vModelView, rObjectBase::MODEL_VIEW_MATRIX );
   _obj->getMatrix( &vNormal, rObjectBase::NORMAL_MATRIX );

   int lTemp;

   _obj->getHints( rObjectBase::NUM_INDEXES, lTemp );

   vDataSize_uI = (GLuint)lTemp;
}

void rRenderMultipleLights_3_3::setDataFromAdditionalObjects( rObjectBase *_obj ) {
   int lLightType;

   _obj->getHints( rObjectBase::FLAGS, lLightType );

   unsigned int lMax = vUniforms.size();
   unsigned int lCur = vPointLight.size() + vDirectionalLight.size();

   if ( lMax <= lCur ) {
      wLOG( "Can not set more lights, max ammount of lights is: ", lMax );
      return;
   }

   if ( lLightType & POINT_LIGHT ) {
      vPointLight.emplace_back( _obj );
      return;
   }

   if ( lLightType & DIRECTIONAL_LIGHT ) {
      vDirectionalLight.emplace_back( _obj );
      return;
   }

   wLOG( "Unsupported light type: ", lLightType );
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
