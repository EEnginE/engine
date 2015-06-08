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
#include "rRenderBase.hpp"


namespace e_engine {



int rSimpleMesh::clearOGLData__() {
   glDeleteBuffers( 1, &vVertexBufferObject );
   glDeleteBuffers( 1, &vIndexBufferObject );

   vObjectHints[DATA_BUFFER] = -1;
   vObjectHints[INDEX_BUFFER] = -1;
   vObjectHints[IS_DATA_READY] = 0;
   vObjectHints[LIGHT_MODEL] = NO_LIGHTS;

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
 * \returns 1 if everything went fine
 * \returns 0 if data is empty
 */
int rSimpleMesh::setOGLData__() {
   void *lData = &vData->vData.at( 0 );
   void *lIndex = &vData->vIndex.at( 0 );

   GLsizeiptr lDataS = static_cast<GLsizeiptr>( vData->vData.size() * sizeof( float ) );
   GLsizeiptr lIndexS = static_cast<GLsizeiptr>( vData->vIndex.size() * sizeof( unsigned short ) );

   if ( lIndexS == 0 || lDataS == 0 || !lData || !lIndex ) {
      eLOG( "Data is empty!" );
      return 0;
   }

   glGenBuffers( 1, &vVertexBufferObject );
   glGenBuffers( 1, &vIndexBufferObject );

   glBindBuffer( GL_ARRAY_BUFFER, vVertexBufferObject );
   glBufferData( GL_ARRAY_BUFFER, lDataS, lData, GL_STATIC_DRAW );

   glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, vIndexBufferObject );
   glBufferData( GL_ELEMENT_ARRAY_BUFFER, lIndexS, lIndex, GL_STATIC_DRAW );

   vObjectHints[DATA_BUFFER] = vVertexBufferObject;
   vObjectHints[INDEX_BUFFER] = vIndexBufferObject;
   vObjectHints[IS_DATA_READY] = 1;
   vObjectHints[LIGHT_MODEL] = SIMPLE_ADS_LIGHT;

   return 1;
}

uint32_t rSimpleMesh::getMatrix( rMat4f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   switch ( _type ) {
      case SCALE: *_mat = getScaleMatrix(); return 0;
      case ROTATION: *_mat = getRotationMatrix(); return 0;
      case TRANSLATION: *_mat = getTranslationMatrix(); return 0;
      case CAMERA_MATRIX: *_mat = getViewProjectionMatrix(); return 0;
      case MODEL_MATRIX: *_mat = getModelMatrix(); return 0;
      case VIEW_MATRIX: *_mat = getViewMatrix(); return 0;
      case PROJECTION_MATRIX: *_mat = getProjectionMatrix(); return 0;
      case MODEL_VIEW_MATRIX: *_mat = getModelViewMatrix(); return 0;
      case MODEL_VIEW_PROJECTION: *_mat = getModelViewProjectionMatrix(); return 0;
      case NORMAL_MATRIX: break;
   }

   return INDEX_OUT_OF_RANGE;
}

uint32_t rSimpleMesh::getMatrix( rMat3f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   switch ( _type ) {
      case NORMAL_MATRIX: *_mat = getNormalMatrix(); return 0;
      default: return INDEX_OUT_OF_RANGE;
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
