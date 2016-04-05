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

bool rObjectBase::setData( VkCommandBuffer _buf,
                           std::vector<uint32_t> const &_index,
                           std::vector<float> const &_pos,
                           std::vector<float> const &_norm,
                           std::vector<float> const &_uv ) {
   if ( vIsLoaded_B || vPartialLoaded_B ) {
      eLOG( "Data already loaded! Object ", vName_str );
      return false;
   }

   vLoadBuffers = setData_IMPL( _buf, _index, _pos, _norm, _uv );

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

bool rObjectBase::setupVertexData_PN( std::vector<float> const &_pos,
                                      std::vector<float> const &_norm,
                                      std::vector<float> &_out ) {
   if ( _pos.size() != _norm.size() || ( _pos.size() % 3 ) != 0 ) {
      eLOG( "Invalid data! Object ", vName_str );
      return false;
   }

   _out.resize( _pos.size() * 2 );
   for ( uint32_t i = 0, counter = 0; i < _pos.size(); i += 3, counter++ ) {
      _out[6 * counter + 0] = _pos[i + 0];
      _out[6 * counter + 1] = _pos[i + 1];
      _out[6 * counter + 2] = _pos[i + 2];
      _out[6 * counter + 3] = _norm[i + 0];
      _out[6 * counter + 4] = _norm[i + 1];
      _out[6 * counter + 5] = _norm[i + 2];
   }

   return true;
}

/*!
 * \brief returns the shader object
 * \returns the shader or nullptr on error
 */
rShaderBase *rObjectBase::getShader() {
   if( !vPipeline )
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
