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
#include <regex>

namespace e_engine {

/*!
 * \brief Destructor
 * \warning NEVER CALL A VIRTUAL FUNCTION HERE!!
 */
rObjectBase::~rObjectBase() {}


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
