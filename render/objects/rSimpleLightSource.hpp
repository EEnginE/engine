/*!
 * \file rSimpleLightSource.hpp
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

#ifndef R_SIMPLE_LIGHT_SOURCE_HPP
#define R_SIMPLE_LIGHT_SOURCE_HPP

#include "defines.hpp"

#include "rObjectBase.hpp"
#include "rMatrixObjectBase.hpp"

namespace e_engine {

template <class T>
class rSimpleLightSource : public rObjectBase, public rMatrixObjectBase<T> {
 private:
   rVec3<T> vLightColor;

   void setFlags();


   virtual int clearOGLData__() { return -1; };
   virtual int setOGLData__() { return -1; };

 public:
   using rMatrixObjectBase<T>::getPosition;

   rSimpleLightSource( rMatrixSceneBase<float> *_scene, std::string _name )
       : rObjectBase( _name, "", SET_DATA_MANUALLY ), rMatrixObjectBase<T>( _scene ) {
      vLightColor.fill( 0 );
      setFlags();
   }

   rSimpleLightSource( rMatrixSceneBase<float> *_scene, std::string _name, rVec3<T> const &_collor )
       : rObjectBase( _name, "", SET_DATA_MANUALLY ), rMatrixObjectBase<T>( _scene ) {
      vLightColor = _collor;
      setFlags();
   }
   rSimpleLightSource( rMatrixSceneBase<float> *_scene,
                       std::string _name,
                       rVec3<T> const &&_collor )
       : rObjectBase( _name, "", SET_DATA_MANUALLY ), rMatrixObjectBase<T>( _scene ) {
      vLightColor = _collor;
      setFlags();
   }

   void setCollor( rVec3<T> const &_collor ) { vLightColor = _collor; }
   void setCollor( rVec3<T> const &&_collor ) { vLightColor = _collor; }
   rVec3<T> *getCollor() { return &vLightColor; }

   virtual uint32_t getVector( rVec3<T> **_vec, VECTOR_TYPES _type );
};


template <class T>
uint32_t rSimpleLightSource<T>::getVector( rVec3<T> **_vec, VECTOR_TYPES _type ) {
   *_vec = nullptr;

   switch ( _type ) {
      case LIGHT_COLOR:
         *_vec = &vLightColor;
         return ALL_OK;
      case POSITION_MODEL_VIEW:
         *_vec = this->getPositionModelView();
         return ALL_OK;
      case POSITION:
         *_vec = this->getPosition();
         return ALL_OK;
      default:
         return UNSUPPORTED_TYPE;
   }
}

template <class T>
void rSimpleLightSource<T>::setFlags() {
   vObjectHints[FLAGS] = LIGHT_SOURCE;
   vObjectHints[IS_DATA_READY] = GL_TRUE;
   vObjectHints[MATRICES] = 0;
}
}

#endif // R_SIMPLE_LIGHT_SOURCE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
