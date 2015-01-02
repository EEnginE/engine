/*!
 * \file rSimpleMesh.cpp
 * \brief \b Classes: \a rSimpleMesh
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

#include "rSimpleMesh.hpp"


namespace e_engine {


rSimpleMesh::~rSimpleMesh() { clearOGLData(); }



int rSimpleMesh::clearOGLData__() {
   glDeleteBuffers( 1, &vVertexBufferObject );
   glDeleteBuffers( 1, &vIndexBufferObject );

   if ( vHasNormals ) {
      glDeleteBuffers( 1, &vNormalBufferObject );
      vHasNormals = false;
   }

   vObjectHints[IS_DATA_READY] = 0;
   vObjectHints[LIGHT_MODEL] = NO_LIGHTS;
   vObjectHints[NUM_VBO] = 0;
   vObjectHints[NUM_IBO] = 0;
   vObjectHints[NUM_NBO] = 0;

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

   auto *lData = vLoaderData->getData();

   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObject );
   glBufferData( GL_ARRAY_BUFFER,
                 sizeof( GLfloat ) * lData->vVertexData.size(),
                 &lData->vVertexData.at( 0 ),
                 GL_STATIC_DRAW );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObject );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER,
                 sizeof( GLuint ) * lData->vIndex.size(),
                 &lData->vIndex.at( 0 ),
                 GL_STATIC_DRAW );

   if ( lData->vNormalesData.size() > 0 ) {
      glGenBuffers( 1, &vNormalBufferObject );

      glBindBuffer( GL_ARRAY_BUFFER, vNormalBufferObject );
      glBufferData( GL_ARRAY_BUFFER,
                    sizeof( GLfloat ) * lData->vNormalesData.size(),
                    &lData->vNormalesData.at( 0 ),
                    GL_STATIC_DRAW );

      vHasNormals = true;
      vObjectHints[LIGHT_MODEL] = SIMPLE_ADS_LIGHT;
      vObjectHints[NUM_NBO] = 1;
   }

   vObjectHints[IS_DATA_READY] = 1;

   vObjectHints[NUM_VBO] = 1;
   vObjectHints[NUM_IBO] = 1;

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

uint32_t rSimpleMesh::getNBO( uint32_t &_n ) {
   uint32_t lRet = 0;

   if ( !vIsLoaded_B || !vHasNormals )
      lRet |= DATA_NOT_LOADED;

   if ( _n != 0 )
      lRet |= INDEX_OUT_OF_RANGE;

   if ( lRet == 0 )
      _n = vNormalBufferObject;

   return lRet;
}

uint32_t rSimpleMesh::getMatrix( rMat4f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   switch ( _type ) {
      case SCALE:
         *_mat = getScaleMatrix();
         return 0;
      case ROTATION:
         *_mat = getRotationMatrix();
         return 0;
      case TRANSLATION:
         *_mat = getTranslationMatrix();
         return 0;
      case CAMERA_MATRIX:
         *_mat = getViewProjectionMatrix();
         return 0;
      case MODEL_MATRIX:
         *_mat = getModelMatrix();
         return 0;
      case VIEW_MATRIX:
         *_mat = getViewMatrix();
         return 0;
      case PROJECTION_MATRIX:
         *_mat = getProjectionMatrix();
         return 0;
      case MODEL_VIEW_MATRIX:
         *_mat = getModelViewMatrix();
         return 0;
      case MODEL_VIEW_PROJECTION:
         *_mat = getModelViewProjectionMatrix();
         return 0;
      default:
         return INDEX_OUT_OF_RANGE;
   }
}

uint32_t rSimpleMesh::getMatrix( rMat3f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   switch ( _type ) {
      case NORMAL_MATRIX:
         *_mat = getNormalMatrix();
         return 0;
      default:
         return INDEX_OUT_OF_RANGE;
   }
}

void rSimpleMesh::setFlags() {
   vObjectHints[FLAGS] = MESH_OBJECT;
   vObjectHints[MATRICES] = SCALE_MATRIX_FLAG | ROTATION_MATRIX_FLAG | TRANSLATION_MATRIX_FLAG |
                            CAMERA_MATRIX_FLAG | MODEL_MATRIX_FLAG | VIEW_MATRIX_FLAG |
                            PROJECTION_MATRIX_FLAG | MODEL_VIEW_MATRIX_FLAG | NORMAL_MATRIX_FLAG |
                            MODEL_VIEW_PROJECTION_MATRIX_FLAG;

   vObjectHints[IS_DATA_READY] = 0;
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
