/*!
 * \file rRenderVertexNormal_3_3.cpp
 */

#include "rRenderVertexNormal_3_3.hpp"

namespace e_engine {


void rRenderVertexNormal_3_3::render() {
   glUseProgram( vShader_OGL );

   glUniformMatrix4fv( vUniformMVP_OGL, 1, false, vModelViewProjection->getMatrix() );

   glEnableVertexAttribArray( vInputVertexLocation_OGL );
   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObj_OGL );
   glVertexAttribPointer( vInputVertexLocation_OGL, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glEnableVertexAttribArray( vInputNormalsLocation_OGL );
   glBindBuffer( GL_ARRAY_BUFFER, vNormalBufferObj_OGL );
   glVertexAttribPointer( vInputNormalsLocation_OGL, 3, GL_FLOAT, GL_FALSE, 0, 0 );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObj_OGL );
   glDrawElements( GL_TRIANGLES, vDataSize_uI, GL_UNSIGNED_INT, 0 );

   glDisableVertexAttribArray( vInputVertexLocation_OGL );
   glDisableVertexAttribArray( vInputNormalsLocation_OGL );
}


bool rRenderVertexNormal_3_3::testShader( rShader *_shader ) {
   if( !_shader->getIsLinked() )
      return false;

   return require(
         _shader,
         rShader::VERTEX_INPUT,
         rShader::NORMALS_INPUT,
         rShader::M_V_P_MATRIX
         );
}

bool rRenderVertexNormal_3_3::testObject( rObjectBase *_obj ) {
   int lVert, lFlags, lMatrices, lnVBO, lnIBO, lnNBO;

   _obj->getHints(
         rObjectBase::NUM_INDEXES, lVert,
         rObjectBase::FLAGS,       lFlags,
         rObjectBase::MATRICES,    lMatrices,
         rObjectBase::NUM_VBO,     lnVBO,
         rObjectBase::NUM_IBO,     lnIBO,
         rObjectBase::NUM_NBO,     lnNBO
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

   if( lnNBO != 1 )
      return false;

   return true;
}

bool rRenderVertexNormal_3_3::canRender() {
   if( !testUnifrom(
         vInputVertexLocation_OGL,   L"Input Vertex",
         vInputNormalsLocation_OGL,  L"Input Normals",
         vUniformMVP_OGL,            L"Model View Projection Matrix",
         vShader_OGL,                L"The shader",
         vVertexBufferObj_OGL,       L"Vertex buffer object",
         vIndexBufferObj_OGL,        L"Index buffer object",
         vNormalBufferObj_OGL,       L"Normal buffer object"
         ) )
      return false;

   if( !testPointer( vModelViewProjection,  L"Model View Projection Matrix" ) )
      return false;

   return true;
}




void rRenderVertexNormal_3_3::setDataFromShader( rShader *_s ) {
   vInputVertexLocation_OGL  = _s->getLocation( rShader::VERTEX_INPUT );
   vInputNormalsLocation_OGL = _s->getLocation( rShader::NORMALS_INPUT );

   vUniformMVP_OGL           = _s->getLocation( rShader::M_V_P_MATRIX );

   _s->getProgram( vShader_OGL );
}

void rRenderVertexNormal_3_3::setDataFromObject( rObjectBase *_obj ) {
   vVertexBufferObj_OGL = vIndexBufferObj_OGL = 0;
   _obj->getVBO( vVertexBufferObj_OGL );
   _obj->getIBO( vIndexBufferObj_OGL );
   _obj->getNBO( vNormalBufferObj_OGL );
   _obj->getMatrix( &vModelViewProjection, rObjectBase::MODEL_VIEW_PROJECTION );

   int lTemp;

   _obj->getHints( rObjectBase::NUM_INDEXES, lTemp );

   vDataSize_uI = ( GLuint ) lTemp;
}




}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
