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

#ifndef R_POINT_LIGHT_HPP
#define R_POINT_LIGHT_HPP

#include "defines.hpp"

#include "rObjectBase.hpp"
#include "rMatrixObjectBase.hpp"

namespace e_engine {

template <class T>
class rPointLight : public rObjectBase, public rMatrixObjectBase<T> {
   typedef rMatrixSceneBase<float> *SCENE;

 private:
   rVec3<T> vAmbientColor;
   rVec3<T> vLightColor;
   rVec3<T> vAttenuation;

   void setFlags();


   virtual int clearOGLData__() { return -1; };
   virtual int setOGLData__() { return -1; };

 public:
   using rMatrixObjectBase<T>::getPosition;

   rPointLight( SCENE _scene, std::string _name )
       : rObjectBase( _name, "", SET_DATA_MANUALLY ), rMatrixObjectBase<T>( _scene ) {
      vLightColor.fill( 0 );
      setFlags();
   }

   rPointLight( SCENE _scene, std::string _name, rVec3<T> _color, rVec3<T> _ambient )
       : rObjectBase( _name, "", SET_DATA_MANUALLY ), rMatrixObjectBase<T>( _scene ) {
      vLightColor = _color;
      vAmbientColor = _ambient;
      setFlags();
   }

   rPointLight( SCENE _scene, std::string _name, rVec3<T> _color, rVec3<T> _ambient, rVec3<T> _att )
       : rObjectBase( _name, "", SET_DATA_MANUALLY ), rMatrixObjectBase<T>( _scene ) {
      vLightColor = _color;
      vAmbientColor = _ambient;
      vAttenuation = _att;
      setFlags();
   }

   void setColor( rVec3<T> _color, rVec3<T> _ambient ) {
      vLightColor = _color;
      vAmbientColor = _ambient;
   }
   void setAttenuation( rVec3<T> _att ) { vAttenuation = _att; }
   void setAttenuation( T _const, T _linear, T _exp ) {
      vAttenuation.x = _const;
      vAttenuation.y = _linear;
      vAttenuation.z = _exp;
   }
   rVec3<T> *getColor() { return &vLightColor; }
   rVec3<T> *getAttenuation() { return &vAttenuation; }

   virtual uint32_t getVector( rVec3<T> **_vec, VECTOR_TYPES _type );
};


template <class T>
uint32_t rPointLight<T>::getVector( rVec3<T> **_vec, VECTOR_TYPES _type ) {
   *_vec = nullptr;

   switch ( _type ) {
      case AMBIENT_COLOR:
         *_vec = &vAmbientColor;
         return ALL_OK;
      case LIGHT_COLOR:
         *_vec = &vLightColor;
         return ALL_OK;
      case POSITION_MODEL_VIEW:
         *_vec = this->getPositionModelView();
         return ALL_OK;
      case POSITION:
         *_vec = this->getPosition();
         return ALL_OK;
      case ATTENUATION:
         *_vec = &vAttenuation;
         return ALL_OK;
      default:
         return UNSUPPORTED_TYPE;
   }
}

template <class T>
void rPointLight<T>::setFlags() {
   vObjectHints[FLAGS] = POINT_LIGHT | LIGHT_SOURCE;
   vObjectHints[IS_DATA_READY] = GL_TRUE;
   vObjectHints[MATRICES] = 0;
}
}

#endif // R_POINT_LIGHT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
