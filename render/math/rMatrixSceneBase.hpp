/*!
 * \file rMatrixSceneBase.hpp
 * \brief \b Classes: \a rMatrixWorldBase
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

#pragma once

#include "defines.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <mutex>

namespace e_engine {

class rWorld;

/*!
 * \brief Class for managing Camera space matrix
 *
 *
 */
template <class T, glm::precision P = glm::precision::highp>
class rMatrixSceneBase {
 private:
  rWorld *vWoldPtr;

  glm::tmat4x4<T, P> vProjectionMatrix_MAT;
  glm::tmat4x4<T, P> vViewMatrix_MAT;
  glm::tmat4x4<T, P> vViewProjectionMatrix_MAT;

  std::recursive_mutex vMatrixAccess;

 public:
  rMatrixSceneBase() = delete;
  rMatrixSceneBase(rWorld *_init);

  inline void calculateProjectionPerspective(T _width, T _height, T _nearZ, T _farZ, T _fofy);
  inline void calculateProjectionPerspective(T _aspectRatio, T _nearZ, T _farZ, T _fofy);

  inline void setCamera(const glm::tvec3<T, P> &_position,
                        const glm::tvec3<T, P> &_lookAt,
                        const glm::tvec3<T, P> &_upVector);

  inline glm::tmat4x4<T, P> *getProjectionMatrix() { return &vProjectionMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getViewMatrix() { return &vViewMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getViewProjectionMatrix() { return &vViewProjectionMatrix_MAT; }

  rWorld *getWorldPtr() { return vWoldPtr; }
};


template <class T, glm::precision P>
rMatrixSceneBase<T, P>::rMatrixSceneBase(rWorld *_init) : vWoldPtr(_init) {
  glm::mat4 test;
  vProjectionMatrix_MAT     = glm::tmat4x4<T, P>(1);
  vViewMatrix_MAT           = glm::tmat4x4<T, P>(1);
  vViewProjectionMatrix_MAT = glm::tmat4x4<T, P>(1);
}

/*!
 * \brief calculates the projection matrix (perspective)
 *
 * \param[in] _aspectRatio The aspect ratio of the viewport to render
 * \param[in] _nearZ       The near clipping plane
 * \param[in] _farZ        The far clipping plane
 * \param[in] _fofy        The field of view angle
 */
template <class T, glm::precision P>
void rMatrixSceneBase<T, P>::calculateProjectionPerspective(T _aspectRatio, T _nearZ, T _farZ, T _fofy) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vProjectionMatrix_MAT     = glm::perspective(_fofy, _aspectRatio, _nearZ, _farZ);
  vViewProjectionMatrix_MAT = vProjectionMatrix_MAT * vViewMatrix_MAT;
}

/*!
 * \brief calculates the projection matrix (perspective)
 *
 * \param[in] _width       The width of the viewport to render
 * \param[in] _height      The height of the viewport to render
 * \param[in] _nearZ       The near clipping plane
 * \param[in] _farZ        The far clipping plane
 * \param[in] _fofy        The field of view angle
 */
template <class T, glm::precision P>
void rMatrixSceneBase<T, P>::calculateProjectionPerspective(T _width, T _height, T _nearZ, T _farZ, T _fofy) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vProjectionMatrix_MAT     = glm::perspective(_fofy, _width / _height, _nearZ, _farZ);
  vViewProjectionMatrix_MAT = vProjectionMatrix_MAT * vViewMatrix_MAT;
}

/*!
 * \brief calculates the camera matrix
 *
 * \param[in] _position The position of the camera
 * \param[in] _lookAt   The direction of the camera
 * \param[in] _upVector The up direction of the camera ( mostly rVec3( 0, 1, 0 ) )
 */
template <class T, glm::precision P>
void rMatrixSceneBase<T, P>::setCamera(const glm::tvec3<T, P> &_position,
                                       const glm::tvec3<T, P> &_lookAt,
                                       const glm::tvec3<T, P> &_upVector) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vViewMatrix_MAT           = glm::lookAt(_position, _lookAt, _upVector);
  vViewProjectionMatrix_MAT = vProjectionMatrix_MAT * vViewMatrix_MAT;
}
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
