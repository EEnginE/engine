/*!
 * \file rPointLight.hpp
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

#include "rLightRenderBase.hpp"
#include "rMatrixObjectBase.hpp"

namespace e_engine {

template <class T, glm::precision P = glm::precision::highp>
class rPointLight : public rLightRenderBase, public rMatrixObjectBase<T> {
  typedef rMatrixSceneBase<float> *SCENE;

 private:
  glm::tvec3<T, P> vAmbientColor;
  glm::tvec3<T, P> vLightColor;
  glm::tvec3<T, P> vAttenuation;

 public:
  using rMatrixObjectBase<T>::getPosition;

  rPointLight(SCENE _scene, std::string _name) : rLightRenderBase(_name), rMatrixObjectBase<T>(_scene) {
    vLightColor = glm::tvec3<T, P>(0, 0, 0);
    vIsLoaded_B = true;
  }

  rPointLight(SCENE _scene, std::string _name, glm::tvec3<T, P> _color, glm::tvec3<T, P> _ambient)
      : rLightRenderBase(_name), rMatrixObjectBase<T>(_scene) {
    vLightColor   = _color;
    vAmbientColor = _ambient;
    vIsLoaded_B   = true;
  }

  rPointLight(SCENE _scene, std::string _name, glm::tvec3<T, P> _color, glm::tvec3<T, P> _ambient, glm::tvec3<T, P> _att)
      : rLightRenderBase(_name), rMatrixObjectBase<T>(_scene) {
    vLightColor   = _color;
    vAmbientColor = _ambient;
    vAttenuation  = _att;
    vIsLoaded_B   = true;
  }

  void setColor(glm::tvec3<T, P> _color, glm::tvec3<T, P> _ambient) {
    vLightColor   = _color;
    vAmbientColor = _ambient;
  }
  void setAttenuation(glm::tvec3<T, P> _att) { vAttenuation = _att; }
  void setAttenuation(T _const, T _linear, T _exp) {
    vAttenuation.x = _const;
    vAttenuation.y = _linear;
    vAttenuation.z = _exp;
  }
  glm::tvec3<T, P> *getColor() { return &vLightColor; }
  glm::tvec3<T, P> *getAttenuation() { return &vAttenuation; }

  uint32_t getVector(glm::tvec3<T, P> **_vec, VECTOR_TYPES _type) override;

  bool checkIsCompatible(rPipeline *) override { return true; }
};


template <class T, glm::precision P>
uint32_t rPointLight<T, P>::getVector(glm::tvec3<T, P> **_vec, VECTOR_TYPES _type) {
  *_vec = nullptr;

  switch (_type) {
    case AMBIENT_COLOR: *_vec = &vAmbientColor; return ALL_OK;
    case LIGHT_COLOR: *_vec = &vLightColor; return ALL_OK;
    case POSITION_MODEL_VIEW: *_vec = this->getPositionModelView(); return ALL_OK;
    case POSITION: *_vec = this->getPosition(); return ALL_OK;
    case ATTENUATION: *_vec = &vAttenuation; return ALL_OK;
    case DIRECTION: return UNSUPPORTED_TYPE;
  }

  return UNSUPPORTED_TYPE;
}

typedef rPointLight<float> rPointLightF;
} // namespace e_engine


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
