/*!
 * \file rRenderNormal_3_3.cpp
 * \brief \b Classes: \a rRenderNormal_3_3
 */

#include "rRenderNormal_3_3.hpp"

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

   glUniformMatrix4fv( vUniformLocation_OGL, 1, false, vMatrix->getMatrix() );

   glEnableVertexAttribArray( vInputLocation_OGL );

   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObj_OGL );
   glVertexAttribPointer( vInputLocation_OGL, 3, GL_FLOAT, GL_FALSE, 0, nullptr );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObj_OGL );
   glDrawElements( GL_TRIANGLES, vDataSize_uI, GL_UNSIGNED_INT, nullptr );

   glDisableVertexAttribArray( vInputLocation_OGL );
}


bool rRenderNormal_3_3::testShader( rShader *_shader ) {
   if( !_shader->getIsLinked() )
      return false;

   return require( _shader, rShader::VERTEX_INPUT, rShader::M_V_P_MATRIX, rShader::NORMAL_MATRIX );
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

   if( !( lFlags & MESH_OBJECT ) )
      return false;

   if( lVert < 3 )
      return false;

   if( !( lMatrices & MODEL_VIEW_PROJECTION_MATRIX_FLAG ) )
      return false;

   if( lnVBO != 1 )
      return false;

   if( lnIBO != 1 )
      return false;

   return true;
}


void rRenderNormal_3_3::setDataFromShader( rShader *_s ) {
   if( !_s->getProgram( vShader_OGL ) )
      return;

   vInputLocation_OGL   = _s->getLocation( rShader::VERTEX_INPUT );
   vUniformLocation_OGL = _s->getLocation( rShader::M_V_P_MATRIX );
}

void rRenderNormal_3_3::setDataFromObject( rObjectBase *_obj ) {
   vVertexBufferObj_OGL = vIndexBufferObj_OGL = 0;
   _obj->getVBO( vVertexBufferObj_OGL );
   _obj->getIBO( vIndexBufferObj_OGL );
   _obj->getMatrix( &vMatrix, rObjectBase::MODEL_VIEW_PROJECTION );

   int lTemp;

   _obj->getHints( rObjectBase::NUM_INDEXES, lTemp );

   vDataSize_uI = ( GLuint ) lTemp;
}



}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;





