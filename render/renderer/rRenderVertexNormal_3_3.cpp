/*!
 * \file rRenderVertexNormal_3_3.cpp
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

#include "rRenderVertexNormal_3_3.hpp"

namespace e_engine {


void rRenderVertexNormal_3_3::render() {
   glUseProgram( vShader_OGL );

   glUniformMatrix4fv( vUniformMVP_OGL, 1, false, vModelViewProjection->getMatrix() );

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


bool rRenderVertexNormal_3_3::testShader( rShader *_shader ) {
   if ( !_shader->getIsLinked() )
      return false;

   return require( _shader, rShader::VERTEX_INPUT, rShader::NORMALS_INPUT, rShader::M_V_P_MATRIX );
}

bool rRenderVertexNormal_3_3::testObject( rObjectBase *_obj ) {
   int64_t lMeshs, lFlags, lMatrices;

   _obj->getHints( rObjectBase::NUM_MESHS,
                   lMeshs,
                   rObjectBase::FLAGS,
                   lFlags,
                   rObjectBase::MATRICES,
                   lMatrices );

   if ( !( lFlags & MESH_OBJECT ) )
      return false;

   if ( lMeshs < 1 )
      return false;

   if ( !( lMatrices & MODEL_VIEW_PROJECTION_MATRIX_FLAG ) )
      return false;

   return true;
}

bool rRenderVertexNormal_3_3::canRender() {
   if ( !testUnifrom( vInputVertexLocation_OGL,
                      L"Input Vertex",
                      vInputNormalsLocation_OGL,
                      L"Input Normals",
                      vUniformMVP_OGL,
                      L"Model View Projection Matrix",
                      vShader_OGL,
                      L"The shader",
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

   if ( !testPointer( vModelViewProjection, L"Model View Projection Matrix" ) )
      return false;

   return true;
}




void rRenderVertexNormal_3_3::setDataFromShader( rShader *_s ) {
   vInputVertexLocation_OGL  = static_cast<GLuint>( _s->getLocation( rShader::VERTEX_INPUT ) );
   vInputNormalsLocation_OGL = static_cast<GLuint>( _s->getLocation( rShader::NORMALS_INPUT ) );

   vUniformMVP_OGL = _s->getLocation( rShader::M_V_P_MATRIX );

   _s->getProgram( vShader_OGL );
}

void rRenderVertexNormal_3_3::setDataFromObject( rObjectBase *_obj ) {
   _obj->getMatrix( &vModelViewProjection, rObjectBase::MODEL_VIEW_PROJECTION );

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
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
