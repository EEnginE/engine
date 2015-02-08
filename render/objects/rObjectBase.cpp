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
#include "rLoader_3D_f_OBJ.hpp"
#include "iInit.hpp"
#include <regex>
#include <boost/filesystem.hpp>

namespace e_engine {

/*!
 * \brief Destructor
 * \warning NEVER CALL A VIRTUAL FUNCTION HERE!!
 */
rObjectBase::~rObjectBase() {}

/*!
 * \brief Clears the content of the object
 *
 * This function clears the content of the object and prepares it for
 * rendering.
 *
 * \warning This function needs an \b ACTIVE OpenGL context for THIS THREAD
 *
 * \returns 1  if everything went fine
 * \returns 100 if data is already loaded
 * \returns 101 if there is no OpenGL context current for this thead
 * \returns the result of clearOGLData__();
 */
int rObjectBase::clearOGLData() {
   if ( !vIsLoaded_B ) {
      wLOG( "Data is already cleared [OBJECT: '", vName_str, "']" );
      return 100;
   }

   if ( !iInit::isAContextCurrentForThisThread() ) {
      eLOG( "Can NOT FREE data because no OpenGL context is current for this thread!\nThis "
            "function may be called from the destructor!  [OBJECT: '",
            vName_str,
            "']" );
      return 101;
   }

   int lRet = clearOGLData__();

   if ( lRet < 0 ) {
      wLOG( "There where errors while clearing the OpenGL data, but data is successfully cleard "
            "[OBJECT: '",
            vName_str,
            "']" );
      vIsLoaded_B = false;
      return lRet;
   }

   if ( lRet > 1 ) {
      eLOG( "There where errors while clearing the OpenGL data, and the data was NOT cleard and "
            "can still be used or cleard later [OBJECT: '",
            vName_str,
            "']" );
      return lRet;
   }

   if ( lRet == 0 ) {
      eLOG( "OBJECT '", vName_str, "' IS COMPLETELY BROKEN!" );
      return lRet;
   }

   iLOG( "Cleared data for object '", vName_str, "'" );

   vIsLoaded_B = false;
   return lRet;
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
 * \returns 100 if there is no OpenGL context current for this thead
 * \returns 102 if data is not already loaded
 * \returns the result of setOGLData__();
 */
int rObjectBase::setOGLData() {
   if ( !iInit::isAContextCurrentForThisThread() ) {
      eLOG( "Cannot init data because no OpenGL context is current for this thread!" );
      return 100;
   }

   if ( vIsLoaded_B ) {
      eLOG( "Data already loaded [OBJECT: '", vName_str, "']" );
      return 102;
   }

   int lRet = setOGLData__();

   if ( lRet < 0 ) {
      wLOG( "There where errors while setting the OpenGL data, but data is successfully set "
            "[OBJECT: '",
            vName_str,
            "']" );
      vIsLoaded_B = true;
      return lRet;
   }

   if ( lRet > 1 ) {
      eLOG( "There where errors while setting the OpenGL data, and the data was NOT set and can "
            "still be used or set later [OBJECT: '",
            vName_str,
            "']" );
      return lRet;
   }

   if ( lRet == 0 ) {
      eLOG( "OBJECT '", vName_str, "' IS COMPLETELY BROKEN!" );
      return lRet;
   }

   iLOG( "Loaded data for object '", vName_str, "'" );

   vIsLoaded_B = true;

   return lRet;
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
