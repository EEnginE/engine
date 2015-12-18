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
#include <cstddef>

namespace e_engine {


void rRenderMultipleLights_3_3::render() {
   glUseProgram( vShader_OGL );

   glUniformMatrix4fv( vUniformMVP_OGL, 1, false, vModelViewProjection->getMatrix() );
   glUniformMatrix4fv( vUniformModelView_OGL, 1, false, vModelView->getMatrix() );
   glUniformMatrix3fv( vUniformNormal_OGL, 1, false, vNormal->getMatrix() );

   glUniform1i( vUniformNumLights,
                static_cast<GLint>( vDirectionalLight.size() + vPointLight.size() ) );

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
   glEnableVertexAttribArray( vInputNormalsLocation_OGL );
   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObj_OGL );
   glVertexAttribPointer( vInputVertexLocation_OGL,                    // Location
                          3,                                           // Num elements per vertex
                          GL_FLOAT,                                    // Data type
                          GL_FALSE,                                    // Is normalized
                          vVertexStride,                               // Data stride
                          reinterpret_cast<void *>( vVertexOffset ) ); // Offset

   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObj_OGL );
   glVertexAttribPointer( vInputNormalsLocation_OGL,                   // Location
                          3,                                           // Num elements per vertex
                          GL_FLOAT,                                    // Data type
                          GL_FALSE,                                    // Is normalized
                          vNormalStride,                               // Data stride
                          reinterpret_cast<void *>( vNormalOffset ) ); // Offset

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObj_OGL );
   glDrawElements(
         GL_TRIANGLES, vDataSize_uI, GL_UNSIGNED_SHORT, reinterpret_cast<void *>( vIndexOffset ) );

   glDisableVertexAttribArray( vInputNormalsLocation_OGL );
   glDisableVertexAttribArray( vInputVertexLocation_OGL );
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
   int64_t lMeshs, lFlags, lMatrices, lLightModel;

   _obj->getHints( rObjectBase::NUM_MESHS,
                   lMeshs,
                   rObjectBase::FLAGS,
                   lFlags,
                   rObjectBase::MATRICES,
                   lMatrices,
                   rObjectBase::LIGHT_MODEL,
                   lLightModel );

   if ( !( lFlags & MESH_OBJECT ) )
      return false;

   if ( lMeshs < 1 )
      return false;

   if ( !( lMatrices & MODEL_VIEW_PROJECTION_MATRIX_FLAG && lMatrices & MODEL_VIEW_MATRIX_FLAG &&
           lMatrices & NORMAL_MATRIX_FLAG ) )
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
                      vVertexOffset,
                      L"Vertex Offset",
                      vVertexStride,
                      L"Vertex stride",
                      vNormalOffset,
                      L"Normal Offset",
                      vNormalStride,
                      L"Normal stride",
                      vIndexOffset,
                      L"Index offset",
                      vIndexStride,
                      L"Index stride",
                      vDataSize_uI,
                      L"Num Indexes" ) )
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
   vInputVertexLocation_OGL  = static_cast<GLuint>( _s->getLocation( rShader::VERTEX_INPUT ) );
   vInputNormalsLocation_OGL = static_cast<GLuint>( _s->getLocation( rShader::NORMALS_INPUT ) );

   vUniformModelView_OGL = _s->getLocation( rShader::MODEL_VIEW_MATRIX );
   vUniformNormal_OGL    = _s->getLocation( rShader::NORMAL_MATRIX );
   vUniformMVP_OGL       = _s->getLocation( rShader::M_V_P_MATRIX );

   vUniformNumLights = _s->getLocation( rShader::NUM_LIGHTS );

   auto lNumMaxLights = _s->getUniformArraySize( rShader::LIGHT_COLOR );

   vUniforms.resize( lNumMaxLights );

   unsigned int lCounter = 0;
   for ( auto &i : vUniforms ) {
      i.type        = _s->getLocation( rShader::LIGHT_TYPE, lCounter );
      i.ambient     = _s->getLocation( rShader::AMBIENT_COLOR, lCounter );
      i.color       = _s->getLocation( rShader::LIGHT_COLOR, lCounter );
      i.pos         = _s->getLocation( rShader::LIGHT_POSITION, lCounter );
      i.attenuation = _s->getLocation( rShader::LIGHT_ATTENUATION, lCounter );
      ++lCounter;
   }

   _s->getProgram( vShader_OGL );
}

void rRenderMultipleLights_3_3::setDataFromObject( rObjectBase *_obj ) {
   _obj->getMatrix( &vModelViewProjection, rObjectBase::MODEL_VIEW_PROJECTION );
   _obj->getMatrix( &vModelView, rObjectBase::MODEL_VIEW_MATRIX );
   _obj->getMatrix( &vNormal, rObjectBase::NORMAL_MATRIX );

   int64_t lVBO, lIBO, lVO, lVS, lNO, lNS, lIO, lIS, lDS_ui;
   lVBO = lIBO = lVO = lVS = lNO = lNS = lIO = lIS = lDS_ui = -1;

   if ( !_obj->getHints( rObjectBase::DATA_BUFFER, lVBO, rObjectBase::INDEX_BUFFER, lIBO ) )
      eLOG( "BIG BIG DATA RENDER ERROR - base!!! (please consider creating an issue)" );

   if ( !_obj->getHintsOBJ( MESH_3D,
                            0,
                            rObjectBase::VERTEX_OFFSET,
                            lVO,
                            rObjectBase::VERTEX_STRIDE,
                            lVS,
                            rObjectBase::NORMAL_OFFSET,
                            lNO,
                            rObjectBase::NORMAL_STRIDE,
                            lNS,
                            rObjectBase::INDEX_OFFSET,
                            lIO,
                            rObjectBase::INDEX_STRIDE,
                            lIS,
                            rObjectBase::NUM_INDEXES,
                            lDS_ui ) )
      eLOG( "BIG BIG DATA RENDER ERROR - obj!!! (please consider creating an issue)" );

   vVertexBufferObj_OGL = lVBO >= 0 ? static_cast<GLuint>( lVBO ) : NOT_SET_ui;
   vIndexBufferObj_OGL  = lIBO >= 0 ? static_cast<GLuint>( lIBO ) : NOT_SET_ui;

   vVertexOffset = lVO >= 0 ? static_cast<GLsizei>( lVO ) : NOT_SET_ui;
   vVertexStride = lVS >= 0 ? static_cast<GLsizei>( lVS ) : NOT_SET_ui; // NOT_SET_ui;
   vNormalOffset = lNO >= 0 ? static_cast<GLsizei>( lNO ) : NOT_SET_ui; // NOT_SET_ui;
   vNormalStride = lNS >= 0 ? static_cast<GLsizei>( lNS ) : NOT_SET_ui; // NOT_SET_ui;
   vIndexOffset  = lIO >= 0 ? static_cast<GLsizei>( lIO ) : NOT_SET_ui;
   vIndexStride  = lIS >= 0 ? static_cast<GLsizei>( lIS ) : NOT_SET_ui; // NOT_SET_ui;
   vDataSize_uI  = lDS_ui > 0 ? static_cast<GLsizei>( lDS_ui ) : NOT_SET_ui;
}

void rRenderMultipleLights_3_3::setDataFromAdditionalObjects( rObjectBase *_obj ) {
   int64_t lLightType;

   _obj->getHints( rObjectBase::FLAGS, lLightType );

   auto lMax = vUniforms.size();
   auto lCur = vPointLight.size() + vDirectionalLight.size();

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
