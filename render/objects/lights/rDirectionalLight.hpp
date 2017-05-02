/*!
 * \file rDirectionalLight.hpp
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

#include "rMatrixObjectBase.hpp"
#include "rObjectBase.hpp"

namespace e_engine {

template <class T, glm::precision P = glm::precision::highp>
class rDirectionalLight : public rObjectBase {
 private:
  glm::tvec3<T, P> vAmbientColor;
  glm::tvec3<T, P> vLightColor;
  glm::tvec3<T, P> vLightDirection;

  bool isMesh() override { return false; }

 public:
  rDirectionalLight(std::string _name) : rObjectBase(_name) { vLightColor.fill(0); }

  rDirectionalLight(std::string _name, glm::tvec3<T, P> _direction) : rObjectBase(_name) {
    vLightDirection = _direction;
    vIsLoaded_B     = true;
    vLightDirection.normalize();
  }

  rDirectionalLight(std::string _name, glm::tvec3<T, P> _direction, glm::tvec3<T, P> _color, glm::tvec3<T, P> _ambient)
      : rObjectBase(_name) {
    vLightDirection = _direction;
    vLightColor     = _color;
    vAmbientColor   = _ambient;
    vIsLoaded_B     = true;

    vLightDirection.normalize();
  }

  void setColor(glm::tvec3<T, P> _color, glm::tvec3<T, P> _ambient) {
    vLightColor   = _color;
    vAmbientColor = _ambient;
  }

  void setDirection(glm::tvec3<T, P> _direction) {
    vLightDirection = _direction;
    vLightDirection.normalize();
  }
  glm::tvec3<T, P> *getColor() { return &vLightColor; }

  uint32_t getVector(glm::tvec3<T, P> **_vec, VECTOR_TYPES _type) override;
  bool checkIsCompatible(rPipeline *) override { return true; }
};


template <class T, glm::precision P>
uint32_t rDirectionalLight<T, P>::getVector(glm::tvec3<T, P> **_vec, VECTOR_TYPES _type) {
  *_vec = nullptr;

  switch (_type) {
    case AMBIENT_COLOR: *_vec = &vAmbientColor; return ALL_OK;
    case LIGHT_COLOR: *_vec   = &vLightColor; return ALL_OK;
    case DIRECTION: *_vec     = &vLightDirection; return ALL_OK;
    case POSITION:
    case POSITION_MODEL_VIEW:
    case ATTENUATION: return UNSUPPORTED_TYPE;
  }

  return UNSUPPORTED_TYPE;
}

typedef rDirectionalLight<float> rDirectionalLightF;
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
