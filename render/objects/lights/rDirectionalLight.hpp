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

#include "rObjectBase.hpp"
#include "rMatrixObjectBase.hpp"

namespace e_engine {

template <class T>
class rDirectionalLight : public rObjectBase {
 private:
   rVec3<T> vAmbientColor;
   rVec3<T> vLightColor;
   rVec3<T> vLightDirection;

   void setFlags();


   virtual int clearOGLData__() { return -1; }
   virtual int setOGLData__() { return -1; }

 public:
   rDirectionalLight( std::string _name ) : rObjectBase( _name ) {
      vLightColor.fill( 0 );
      setFlags();
   }

   rDirectionalLight( std::string _name, rVec3<T> _direction ) : rObjectBase( _name ) {
      vLightDirection = _direction;
      vLightDirection.normalize();
      setFlags();
   }

   rDirectionalLight( std::string _name, rVec3<T> _direction, rVec3<T> _color, rVec3<T> _ambient )
       : rObjectBase( _name ) {
      vLightDirection = _direction;
      vLightColor     = _color;
      vAmbientColor   = _ambient;

      vLightDirection.normalize();
      setFlags();
   }

   void setColor( rVec3<T> _color, rVec3<T> _ambient ) {
      vLightColor   = _color;
      vAmbientColor = _ambient;
   }

   void setDirection( rVec3<T> _direction ) {
      vLightDirection = _direction;
      vLightDirection.normalize();
   }
   rVec3<T> *getColor() { return &vLightColor; }

   virtual uint32_t getVector( rVec3<T> **_vec, VECTOR_TYPES _type );
};


template <class T>
uint32_t rDirectionalLight<T>::getVector( rVec3<T> **_vec, VECTOR_TYPES _type ) {
   *_vec = nullptr;

   switch ( _type ) {
      case AMBIENT_COLOR: *_vec = &vAmbientColor; return ALL_OK;
      case LIGHT_COLOR: *_vec   = &vLightColor; return ALL_OK;
      case DIRECTION: *_vec     = &vLightDirection; return ALL_OK;
      case POSITION:
      case POSITION_MODEL_VIEW:
      case ATTENUATION: return UNSUPPORTED_TYPE;
   }

   return UNSUPPORTED_TYPE;
}

template <class T>
void rDirectionalLight<T>::setFlags() {
   vObjectHints[FLAGS]         = DIRECTIONAL_LIGHT | LIGHT_SOURCE;
   vObjectHints[IS_DATA_READY] = true;
   vObjectHints[MATRICES]      = 0;
}
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
