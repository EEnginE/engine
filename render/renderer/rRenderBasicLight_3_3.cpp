/*!
 * \file rRenderBasicLight_3_3.cpp
 */

#include "rRenderBasicLight_3_3.hpp"

namespace e_engine {


void rRenderBasicLight_3_3::render() {
   glUseProgram( vShader_OGL );

   glUniformMatrix4fv( vUniformMVP_OGL,        1, false, vModelViewProjection->getMatrix() );
   glUniformMatrix4fv( vUniformModelView_OGL,  1, false, vModelView->getMatrix() );
   glUniformMatrix3fv( vUniformNormal_OGL,     1, false, vNormal->getMatrix() );

   glUniform3fv( vUniformAmbient_OGL,  1, vAmbientLight.color->getMatrix() );
   glUniform3fv( vUniformLight_OGL,    1, vLightSource.color->getMatrix() );
   glUniform3fv( vUniformLightPos_OGL, 1, vLightSource.position->getMatrix() );

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


bool rRenderBasicLight_3_3::testShader( rShader *_shader ) {
   if( !_shader->getIsLinked() )
      return false;

   return require(
         _shader,
         rShader::VERTEX_INPUT,
         rShader::NORMALS_INPUT,
         rShader::MODEL_VIEW_MATRIX,
         rShader::NORMAL_MATRIX,
         rShader::M_V_P_MATRIX,
         rShader::AMBIENT_COLOR,
         rShader::LIGHT_COLOR,
         rShader::LIGHT_POSITION
         );
}

bool rRenderBasicLight_3_3::testObject( rObjectBase *_obj ) {
   int lVert, lFlags, lMatrices, lnVBO, lnIBO, lnNBO, lLightModel;

   _obj->getHints(
         rObjectBase::NUM_INDEXES, lVert,
         rObjectBase::FLAGS,       lFlags,
         rObjectBase::MATRICES,    lMatrices,
         rObjectBase::NUM_VBO,     lnVBO,
         rObjectBase::NUM_IBO,     lnIBO,
         rObjectBase::NUM_NBO,     lnNBO,
         rObjectBase::LIGHT_MODEL, lLightModel
   );

   if( !( lFlags & MESH_OBJECT ) )
      return false;

   if( lVert < 3 )
      return false;

   if( !(
         lMatrices & MODEL_VIEW_PROJECTION_MATRIX_FLAG &&
         lMatrices & MODEL_VIEW_MATRIX_FLAG            &&
         lMatrices & NORMAL_MATRIX_FLAG
         ) )
      return false;

   if( lnVBO != 1 )
      return false;

   if( lnIBO != 1 )
      return false;

   if( lnNBO != 1 )
      return false;

   if( lLightModel != rObjectBase::SIMPLE_ADS_LIGHT )
      return false;

   return true;
}

bool rRenderBasicLight_3_3::canRender() {
   if( !testUnifrom(
         vInputVertexLocation_OGL,      "Input Vertex",
         vInputNormalsLocation_OGL,     "Input Normals",
         vUniformModelView_OGL,         "Model View Matrix",
         vUniformMVP_OGL,               "Model View Projection Matrix",
         vUniformNormal_OGL,            "Normal Matrix",
         vUniformAmbient_OGL,           "Ambient collor",
         vUniformLight_OGL,             "Light collor",
         vUniformLightPos_OGL,          "Light position",
         vShader_OGL,                   "The shader",
         vVertexBufferObj_OGL,          "Vertex buffer object",
         vIndexBufferObj_OGL,           "Index buffer object",
         vNormalBufferObj_OGL,          "Normal buffer object"
         ) )
      return false;

   if( !testPointer(
         vModelView,            "Model View Matrix",
         vModelViewProjection,  "Model View Projection Matrix",
         vNormal,               "Normal Matrix",
         vAmbientLight.color,   "Ambient collor",
         vLightSource.color,    "Light collor",
         vLightSource.position, "Light position"
         ) )
      return false;

   return true;
}




void rRenderBasicLight_3_3::setDataFromShader( rShader *_s ) {
   vInputVertexLocation_OGL  = _s->getLocation( rShader::VERTEX_INPUT );
   vInputNormalsLocation_OGL = _s->getLocation( rShader::NORMALS_INPUT );

   vUniformModelView_OGL     = _s->getLocation( rShader::MODEL_VIEW_MATRIX );
   vUniformNormal_OGL        = _s->getLocation( rShader::NORMAL_MATRIX );
   vUniformMVP_OGL           = _s->getLocation( rShader::M_V_P_MATRIX );

   vUniformAmbient_OGL       = _s->getLocation( rShader::AMBIENT_COLOR );
   vUniformLight_OGL         = _s->getLocation( rShader::LIGHT_COLOR );
   vUniformLightPos_OGL      = _s->getLocation( rShader::LIGHT_POSITION );

   _s->getProgram( vShader_OGL );
}

void rRenderBasicLight_3_3::setDataFromObject( rObjectBase *_obj ) {
   vVertexBufferObj_OGL = vIndexBufferObj_OGL = 0;
   _obj->getVBO( vVertexBufferObj_OGL );
   _obj->getIBO( vIndexBufferObj_OGL );
   _obj->getNBO( vNormalBufferObj_OGL );
   _obj->getMatrix( &vModelViewProjection, rObjectBase::MODEL_VIEW_PROJECTION );
   _obj->getMatrix( &vModelView,           rObjectBase::MODEL_VIEW_MATRIX );
   _obj->getMatrix( &vNormal,              rObjectBase::NORMAL_MATRIX );

   int lTemp;

   _obj->getHints( rObjectBase::NUM_INDEXES, lTemp );

   vDataSize_uI = ( GLuint ) lTemp;
}

void rRenderBasicLight_3_3::setDataFromAdditionalObjects( rObjectBase *_obj ) {
   int lLightType;

   _obj->getHints( rObjectBase::FLAGS, lLightType );

   if( lLightType & AMBIENT_LIGHT ) {
      vAmbientLight.setAmbient( _obj );
      return;
   }

   if( lLightType & LIGHT_SOURCE ) {
      vLightSource.setLight( _obj );
      return;
   }

   wLOG( "Unsupported light type: ", lLightType );
}



}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
