/*!
 * \file rRenderBasicLight_3_3.cpp
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

#include "rRenderBasicLight_3_3.hpp"

namespace e_engine {


void rRenderBasicLight_3_3::render() {
   glUseProgram( vShader_OGL );

   glUniformMatrix4fv( vUniformMVP_OGL, 1, false, vModelViewProjection->getMatrix() );
   glUniformMatrix4fv( vUniformModelView_OGL, 1, false, vModelView->getMatrix() );
   glUniformMatrix3fv( vUniformNormal_OGL, 1, false, vNormal->getMatrix() );

   glUniform3fv( vUniformAmbient_OGL, 1, vAmbientLight.color->getMatrix() );
   glUniform3fv( vUniformLight_OGL, 1, vLightSource.color->getMatrix() );
   glUniform3fv( vUniformLightPos_OGL, 1, vLightSource.position->getMatrix() );

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


bool rRenderBasicLight_3_3::testShader( rShader *_shader ) {
   if ( !_shader->getIsLinked() )
      return false;

   return require( _shader,
                   rShader::VERTEX_INPUT,
                   rShader::NORMALS_INPUT,
                   rShader::MODEL_VIEW_MATRIX,
                   rShader::NORMAL_MATRIX,
                   rShader::M_V_P_MATRIX,
                   rShader::AMBIENT_COLOR,
                   rShader::LIGHT_COLOR,
                   rShader::LIGHT_POSITION );
}

bool rRenderBasicLight_3_3::testObject( rObjectBase *_obj ) {
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

bool rRenderBasicLight_3_3::canRender() {
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
                      vUniformAmbient_OGL,
                      L"Ambient collor",
                      vUniformLight_OGL,
                      L"Light collor",
                      vUniformLightPos_OGL,
                      L"Light position",
                      vShader_OGL,
                      L"The shader",
                      vVertexBufferObj_OGL,
                      L"Vertex buffer object",
                      vIndexBufferObj_OGL,
                      L"Index buffer object",
                      vNormalBufferObj_OGL,
                      L"Normal buffer object" ) )
      return false;

   if ( !testPointer( vModelView,
                      L"Model View Matrix",
                      vModelViewProjection,
                      L"Model View Projection Matrix",
                      vNormal,
                      L"Normal Matrix",
                      vAmbientLight.color,
                      L"Ambient collor",
                      vLightSource.color,
                      L"Light collor",
                      vLightSource.position,
                      L"Light position" ) )
      return false;

   return true;
}




void rRenderBasicLight_3_3::setDataFromShader( rShader *_s ) {
   vInputVertexLocation_OGL = _s->getLocation( rShader::VERTEX_INPUT );
   vInputNormalsLocation_OGL = _s->getLocation( rShader::NORMALS_INPUT );

   vUniformModelView_OGL = _s->getLocation( rShader::MODEL_VIEW_MATRIX );
   vUniformNormal_OGL = _s->getLocation( rShader::NORMAL_MATRIX );
   vUniformMVP_OGL = _s->getLocation( rShader::M_V_P_MATRIX );

   vUniformAmbient_OGL = _s->getLocation( rShader::AMBIENT_COLOR );
   vUniformLight_OGL = _s->getLocation( rShader::LIGHT_COLOR );
   vUniformLightPos_OGL = _s->getLocation( rShader::LIGHT_POSITION );

   _s->getProgram( vShader_OGL );
}

void rRenderBasicLight_3_3::setDataFromObject( rObjectBase *_obj ) {
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

void rRenderBasicLight_3_3::setDataFromAdditionalObjects( rObjectBase *_obj ) {
   int lLightType;

   _obj->getHints( rObjectBase::FLAGS, lLightType );

   if ( lLightType & AMBIENT_LIGHT ) {
      vAmbientLight.setAmbient( _obj );
      return;
   }

   if ( lLightType & LIGHT_SOURCE ) {
      vLightSource.setLight( _obj );
      return;
   }

   wLOG( "Unsupported light type: ", lLightType );
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
