/*!
 * \file rObjectBase.cpp
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

#include "rObjectBase.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"
#include "rPipeline.hpp"
#include "rShaderBase.hpp"
#include <regex>

namespace e_engine {

/*!
 * \brief Destructor
 * \warning NEVER CALL A VIRTUAL FUNCTION HERE!!
 */
rObjectBase::~rObjectBase() {}

bool rObjectBase::setPipeline( rPipeline *_pipe ) {
   if ( !checkIsCompatible( _pipe ) ) {
      eLOG( "Pipeline not compatible with object ", vName_str );
      return false;
   }

   vPipeline = _pipe;
   return true;
}

bool rObjectBase::setData( VkCommandBuffer _buf, aiMesh const *_mesh ) {
   if ( vIsLoaded_B || vPartialLoaded_B ) {
      eLOG( "Data already loaded! Object ", vName_str );
      return false;
   }

   if ( !_mesh ) {
      eLOG( "Invalid mesh object!" );
      return false;
   }

   uint32_t lIndexSize = 3;

   switch ( getMeshType() ) {
      case POINTS_3D:
         if ( _mesh->mPrimitiveTypes != aiPrimitiveType_POINT ) {
            eLOG( "Invalid primitive type ",
                  _mesh->mPrimitiveTypes,
                  " expected ",
                  aiPrimitiveType_POINT,
                  " (point)" );
            return false;
         }

         lIndexSize = 1;
         break;
      case LINES_3D:
         if ( _mesh->mPrimitiveTypes != aiPrimitiveType_LINE ) {
            eLOG( "Invalid primitive type ",
                  _mesh->mPrimitiveTypes,
                  " expected ",
                  aiPrimitiveType_LINE,
                  " (line)" );
            return false;
         }

         lIndexSize = 2;
         break;
      case MESH_3D:
         if ( _mesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE ) {
            eLOG( "Invalid primitive type ",
                  _mesh->mPrimitiveTypes,
                  " expected ",
                  aiPrimitiveType_TRIANGLE,
                  " (triangle)" );
            return false;
         }

         lIndexSize = 3;
         break;
      case POLYGON_3D:
         if ( _mesh->mPrimitiveTypes != aiPrimitiveType_POLYGON ) {
            eLOG( "Invalid primitive type ",
                  _mesh->mPrimitiveTypes,
                  " expected ",
                  aiPrimitiveType_POLYGON,
                  " (polygon)" );
            return false;
         }

         eLOG( "Polygon type currently not supported!" );
         return false;

         break;
      default: eLOG( "This object type does not support mesh data!" ); return false;
   }

   std::vector<uint32_t> lIndex;
   std::vector<float> lData;

   lIndex.resize( lIndexSize * _mesh->mNumFaces );
   for ( uint32_t i = 0; i < _mesh->mNumFaces; i++ )
      for ( uint32_t j = 0; j < lIndexSize; j++ )
         lIndex[i * lIndexSize + j] = _mesh->mFaces[i].mIndices[j];

   switch ( getDataLayout() ) {
      case POS_NORM: setupVertexData_PN( _mesh, lData ); break;
      default: eLOG( "Data layout ", uEnum2Str::toStr( getDataLayout() ) ); return false;
   }

   vLoadBuffers = setData_IMPL( _buf, lIndex, lData );

   vPartialLoaded_B = true;
   return true;
}

bool rObjectBase::finishData() {
   if ( vIsLoaded_B ) {
      eLOG( "Data already loaded! Object ", vName_str );
      return false;
   }

   if ( !vPartialLoaded_B ) {
      eLOG( "setData not called yet! Object ", vName_str );
      return false;
   }

   for ( auto const &i : vLoadBuffers ) {
      if ( !i->doneCopying() ) {
         eLOG( "Failed to init data for object ", vName_str );
         return false;
      }
   }

   vIsLoaded_B = true;
   return true;
}

bool rObjectBase::setupVertexData_PN( aiMesh const *_mesh, std::vector<float> &_out ) {
   if ( !_mesh->HasNormals() ) {
      eLOG( "Invalid data! Object ", vName_str );
      return false;
   }

   _out.resize( _mesh->mNumVertices * 3 * 2 );
   for ( uint32_t i = 0; i < _mesh->mNumVertices; i++ ) {
      _out[6 * i + 0] = _mesh->mVertices[i].x;
      _out[6 * i + 1] = _mesh->mVertices[i].y;
      _out[6 * i + 2] = _mesh->mVertices[i].z;
      _out[6 * i + 3] = _mesh->mNormals[i].x;
      _out[6 * i + 4] = _mesh->mNormals[i].y;
      _out[6 * i + 5] = _mesh->mNormals[i].z;
   }

   return true;
}

/*!
 * \brief returns the shader object
 * \returns the shader or nullptr on error
 */
rShaderBase *rObjectBase::getShader() {
   if ( !vPipeline )
      return nullptr;

   return vPipeline->getShader();
}

#if COMPILER_CLANG
#pragma clang diagnostic push // This warning is irrelevant here
#pragma clang diagnostic ignored "-Wunused-parameter"
#endif

#if COMPILER_GCC
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix( rMat4d **_mat, rObjectBase::MATRIX_TYPES _type ) {
   *_mat = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix( rMat4f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   *_mat = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix( rMat3d **_mat, rObjectBase::MATRIX_TYPES _type ) {
   *_mat = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix( rMat3f **_mat, rObjectBase::MATRIX_TYPES _type ) {
   *_mat = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}



/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector( rVec4d **_vec, rObjectBase::VECTOR_TYPES _type ) {
   *_vec = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector( rVec4f **_vec, rObjectBase::VECTOR_TYPES _type ) {
   *_vec = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}


/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector( rVec3d **_vec, rObjectBase::VECTOR_TYPES _type ) {
   *_vec = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector( rVec3f **_vec, rObjectBase::VECTOR_TYPES _type ) {
   *_vec = nullptr;
   return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

#if COMPILER_CLANG
#pragma clang diagnostic pop
#endif

#if COMPILER_GCC
#pragma GCC diagnostic pop
#endif



} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
