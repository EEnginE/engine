/*!
 * \file rSimpleMesh.cpp
 * \brief \b Classes: \a rSimpleMesh
 */

#include "rSimpleMesh.hpp"


namespace e_engine {


rSimpleMesh::~rSimpleMesh() {
   clearOGLData();
}



int rSimpleMesh::clearOGLData__() {
   glDeleteBuffers( 1, &vVertexBufferObject );
   glDeleteBuffers( 1, &vIndexBufferObject );

   vObjectHints[IS_DATA_READY] = 0;

   return 1;
}

/*!
 * \brief Loads the content of the object
 *
 * This function loads the content of the object and prepares it for
 * rendering.
 *
 * \warning This function needs an \b ACTIVE OpenGL context for THIS THREAD
 *
 * \returns 1  if everything went fine
 */
int rSimpleMesh::setOGLData__() {
   glGenBuffers( 1, &vVertexBufferObject );
   glGenBuffers( 1, &vIndexBufferObject );

   glBindBuffer( GL_ARRAY_BUFFER,         vVertexBufferObject );
   glBufferData( GL_ARRAY_BUFFER,         sizeof( GLfloat ) * vLoaderData->getRawVertexData()->size(), &vLoaderData->getRawVertexData()->at( 0 ), GL_STATIC_DRAW );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObject );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( GLuint )  * vLoaderData->getRawIndexVertexData()->size(), &vLoaderData->getRawIndexVertexData()->at( 0 ), GL_STATIC_DRAW );

   vObjectHints[IS_DATA_READY] = 1;

   return 1;
}


uint32_t rSimpleMesh::getVBO( uint32_t &_n ) {
   uint32_t lRet = 0;

   if ( !vIsLoaded_B )
      lRet |= DATA_NOT_LOADED;

   if ( _n != 0 )
      lRet |= INDEX_OUT_OF_RANGE;

   if ( lRet == 0 )
      _n = vVertexBufferObject;

   return lRet;
}

uint32_t rSimpleMesh::getIBO( uint32_t &_n ) {
   uint32_t lRet = 0;

   if ( !vIsLoaded_B )
      lRet |= DATA_NOT_LOADED;

   if ( _n != 0 )
      lRet |= INDEX_OUT_OF_RANGE;

   if ( lRet == 0 )
      _n = vIndexBufferObject;

   return lRet;
}

uint32_t rSimpleMesh::getMatrix( rMat4f **_mat, internal::rObjectBase::MATRIX_TYPES _type ) {
   switch ( _type ) {
      case SCALE:        *_mat = getScaleMatrix();       return 0;
      case ROTATION:     *_mat = getRotationMatrix();    return 0;
      case TRANSLATION:  *_mat = getTranslationMatrix(); return 0;
      case CAMERA_SPACE: *_mat = getCameraSpaceMatrix(); return 0;
      case OBJECT_SPACE: *_mat = getObjectSpaceMatrix(); return 0;
      case FINAL:        *_mat = getFinalMatrix();       return 0;
      default: return INDEX_OUT_OF_RANGE;
   }
}

void rSimpleMesh::setFlags() {
   vObjectHints[FLAGS]    = MESH_OBJECT;
   vObjectHints[MATRICES] =
      SCALE_MATRIX_FLAG       |
      ROTATION_MATRIX_FLAG    |
      TRANSLATION_MATRIX_FLAG |
      CAMERA_MATRIX_FLAG      |
      OBJECT_MATRIX_FLAG      |
      FINAL_MATRIX_FLAG;
   vObjectHints[NUM_VBO]  = 1;
   vObjectHints[NUM_IBO]  = 1;
   vObjectHints[IS_DATA_READY] = 0;
}





}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
