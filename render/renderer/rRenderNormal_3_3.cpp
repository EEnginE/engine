/*!
 * \file rRenderNormal_3_3.cpp
 * \brief \b Classes: \a rRenderNormal_3_3
 */

#include <vector>
#include "rRenderNormal_3_3.hpp"
#include "uLog.hpp"

namespace e_engine {

rRenderNormal_3_3::rRenderNormal_3_3() {
   vVertexBufferObj_OGL = 0;
   vIndexBufferObj_OGL  = 0;
   vShader_OGL          = 0;
   vInputLocation_OGL   = 0;
   vUniformLocation_OGL = 0;
   vDataSize_uI         = 0;

   vMatrix = nullptr;
}

void rRenderNormal_3_3::render() {
   glUseProgram( vShader_OGL );

   if ( vNeedUpdateUniforms_B || vAlwaysUpdateUniforms_B || true ) {
      glUniformMatrix4fv( vUniformLocation_OGL, 1, false, vMatrix->getMatrix() );
      vNeedUpdateUniforms_B = false;
   }

   glEnableVertexAttribArray( vInputLocation_OGL );

   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObj_OGL );
   glVertexAttribPointer( vInputLocation_OGL, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObj_OGL );
   glDrawElements( GL_TRIANGLES, vDataSize_uI, GL_UNSIGNED_INT, 0 );

   glDisableVertexAttribArray( vInputLocation_OGL );
}


bool rRenderNormal_3_3::testShader( rShader *_shader ) {
   if ( !_shader->getIsLinked() )
      return false;

   auto *info = _shader->getShaderInfo();

   // We need exactly one input
   if ( info->vInputInfo.size() != 1 )
      return false;

   if ( info->vInputInfo[0].type != GL_FLOAT_VEC3 )
      return false;

   // We need exactly one uniform
   if ( info->vUniformInfo.size() != 1 )
      return false;

   if ( info->vUniformInfo[0].type != GL_FLOAT_MAT4 )
      return false;

   // And none of these
   if ( info->vUniformBlockInfo.size() != 0 )
      return false;

   return true;
}

bool rRenderNormal_3_3::testObject( rObjectBase *_obj ) {
   int lVert, lFlags, lMatrices, lnVBO, lnIBO;

   _obj->getHints(
      rObjectBase::NUM_INDEXES, lVert,
      rObjectBase::FLAGS,       lFlags,
      rObjectBase::MATRICES,    lMatrices,
      rObjectBase::NUM_VBO,     lnVBO,
      rObjectBase::NUM_IBO,     lnIBO
   );

   if ( !( lFlags & MESH_OBJECT ) )
      return false;

   if ( lVert < 3 )
      return false;

   if ( !( lMatrices & FINAL_MATRIX_FLAG ) )
      return false;

   if ( lnVBO != 1 )
      return false;

   if ( lnIBO != 1 )
      return false;

   return true;
}


void rRenderNormal_3_3::setDataFromShader( rShader *_s ) {
   if ( !_s->getProgram( vShader_OGL ) )
      return;

   auto *info = _s->getShaderInfo();

   vInputLocation_OGL   = info->vInputInfo[0].location;
   vUniformLocation_OGL = info->vUniformInfo[0].location;
}

void rRenderNormal_3_3::setDataFromObject( rObjectBase *_obj ) {
   vVertexBufferObj_OGL = vIndexBufferObj_OGL = 0;
   _obj->getVBO( vVertexBufferObj_OGL );
   _obj->getIBO( vIndexBufferObj_OGL );
   _obj->getMatrix( &vMatrix, rObjectBase::FINAL );

   int lTemp;

   _obj->getHints( rObjectBase::NUM_INDEXES, lTemp );

   vDataSize_uI = ( GLuint ) lTemp;
}



}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;





