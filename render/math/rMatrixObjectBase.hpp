/*!
 * \file rMatrixObjectBase.hpp
 * \brief \b Classes: \a rMatrixObjectBase
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
#include "rMatrixSceneBase.hpp"
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtx/transform.hpp>
#include <mutex>

namespace e_engine {

/*!
 * \brief Class for managing Camera space matrix
 *
 *
 */
template <class T, glm::precision P = glm::precision::highp>
class rMatrixObjectBase {
 private:
  glm::tmat4x4<T, P> vScaleMatrix_MAT;
  glm::tmat4x4<T, P> vRotationMatrix_MAT;
  glm::tmat4x4<T, P> vTranslationMatrix_MAT;

  glm::tmat4x4<T, P> *vViewProjectionMatrix_MAT;
  glm::tmat4x4<T, P> *vViewMatrix_MAT;
  glm::tmat4x4<T, P> *vProjectionMatrix_MAT;

  glm::tmat4x4<T, P> vModelMatrix_MAT;
  glm::tmat4x4<T, P> vModelViewMatrix_MAT;
  glm::tmat4x4<T, P> vModelViewProjectionMatrix_MAT;

  glm::tmat3x3<T, P> vNormalMatrix;

  glm::tvec3<T, P> vPosition;
  glm::tvec3<T, P> vPositionModelView;
  glm::tvec3<T, P> vScale;

  rMatrixObjectBase();

 protected:
  std::recursive_mutex vMatrixAccess;

 public:
  rMatrixObjectBase(rMatrixSceneBase<T> *_scene);

  inline void setPosition(const glm::tvec3<T, P> &_pos);
  inline void getPosition(glm::tvec3<T, P> &_pos);
  inline glm::tvec3<T, P> *getPosition() { return &vPosition; }
  inline void addPositionDelta(const glm::tvec3<T, P> &_pos);

  inline glm::tvec3<T, P> *getPositionModelView() { return &vPositionModelView; }

  inline void setRotation(const glm::tvec3<T, P> &_axis, T _angle);

  inline void setScale(T _scale);
  inline void setScale(const glm::tvec3<T, P> &_scale);
  inline glm::tvec3<T, P> *getScale() { return &vScale; }
  inline void addScaleDelta(const glm::tvec3<T, P> &_scale);


  inline glm::tmat4x4<T, P> *getScaleMatrix() { return &vScaleMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getRotationMatrix() { return &vRotationMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getTranslationMatrix() { return &vTranslationMatrix_MAT; }

  inline glm::tmat4x4<T, P> *getModelMatrix() { return &vModelMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getModelViewMatrix() { return &vModelViewMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getViewMatrix() { return vViewMatrix_MAT; }

  inline glm::tmat4x4<T, P> *getProjectionMatrix() { return vProjectionMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getViewProjectionMatrix() { return vViewProjectionMatrix_MAT; }
  inline glm::tmat4x4<T, P> *getModelViewProjectionMatrix() { return &vModelViewProjectionMatrix_MAT; }

  inline glm::tmat3x3<T, P> *getNormalMatrix() { return &vNormalMatrix; }

  inline void updateFinalMatrix();
};

template <class T, glm::precision P>
rMatrixObjectBase<T, P>::rMatrixObjectBase(rMatrixSceneBase<T> *_scene) {
  vScaleMatrix_MAT       = glm::tmat4x4<T, P>();
  vRotationMatrix_MAT    = glm::tmat4x4<T, P>();
  vTranslationMatrix_MAT = glm::tmat4x4<T, P>();
  vModelViewMatrix_MAT   = glm::tmat4x4<T, P>();

  vViewProjectionMatrix_MAT = _scene->getViewProjectionMatrix();
  vViewMatrix_MAT           = _scene->getViewMatrix();
  vProjectionMatrix_MAT     = _scene->getProjectionMatrix();

  vModelMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

  if (vViewProjectionMatrix_MAT)
    vModelViewProjectionMatrix_MAT = *vViewProjectionMatrix_MAT * vModelMatrix_MAT;
  else
    vModelViewProjectionMatrix_MAT = glm::tmat4x4<T, P>();
}

template <class T, glm::precision P>
void rMatrixObjectBase<T, P>::setScale(T _scale) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vScale           = glm::tvec3<T, P>(_scale, _scale, _scale);
  vScaleMatrix_MAT = glm::scale(vScale);
  updateFinalMatrix();
}

template <class T, glm::precision P>
void rMatrixObjectBase<T, P>::setScale(const glm::tvec3<T, P> &_scale) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vScale           = _scale;
  vScaleMatrix_MAT = glm::scale(vScale);
  updateFinalMatrix();
}


template <class T, glm::precision P>
void rMatrixObjectBase<T, P>::addScaleDelta(const glm::tvec3<T, P> &_scale) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vScale += _scale;
  vScaleMatrix_MAT = glm::scale(vScale);
  updateFinalMatrix();
}

template <class T, glm::precision P>
void rMatrixObjectBase<T, P>::setRotation(const glm::tvec3<T, P> &_axis, T _angle) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vRotationMatrix_MAT = glm::rotate(_angle, _axis);
  updateFinalMatrix();
}


template <class T, glm::precision P>
void rMatrixObjectBase<T, P>::setPosition(const glm::tvec3<T, P> &_pos) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vPosition              = _pos;
  vTranslationMatrix_MAT = glm::translate(vPosition);
  updateFinalMatrix();
}


template <class T, glm::precision P>
void rMatrixObjectBase<T, P>::addPositionDelta(const glm::tvec3<T, P> &_pos) {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vPosition += _pos;
  vTranslationMatrix_MAT = glm::translate(vPosition);
  updateFinalMatrix();
}


template <class T, glm::precision P>
void rMatrixObjectBase<T, P>::updateFinalMatrix() {
  std::lock_guard<std::recursive_mutex> lLock(vMatrixAccess);
  vModelMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

  if (vViewProjectionMatrix_MAT)
    vModelViewProjectionMatrix_MAT = *vViewProjectionMatrix_MAT * vModelMatrix_MAT;
  if (vViewMatrix_MAT)
    vModelViewMatrix_MAT = *vViewMatrix_MAT * vModelMatrix_MAT;

  vPositionModelView = vModelViewMatrix_MAT * glm::tvec4<T, P>(0, 0, 0, 1);
  vNormalMatrix      = glm::inverseTranspose(glm::tmat3x3<T, P>(vModelViewMatrix_MAT));
}
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
