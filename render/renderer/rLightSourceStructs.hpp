/*!
 * \file rLightSourceStructs.hpp
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

#ifndef R_LIGHT_SOURCE_STUCTS_HPP
#define R_LIGHT_SOURCE_STUCTS_HPP

#include "rObjectBase.hpp"

namespace e_engine {

template <class T>
struct rRenderAmbientLight {
   rVec3<T> *ambient = nullptr;

   rRenderAmbientLight() {}
   rRenderAmbientLight( rObjectBase *_obj ) { setAmbient( _obj ); }

   void setAmbient( rObjectBase *_obj ) { _obj->getVector( &ambient, rObjectBase::AMBIENT_COLOR ); }
};

template <class T>
struct rRenderDirectionalLight : rRenderAmbientLight<T> {
   rVec3<T> *color = nullptr;
   rVec3<T> *direction = nullptr;

   using rRenderAmbientLight<T>::setAmbient;

   rRenderDirectionalLight() : rRenderAmbientLight<T>() {}
   rRenderDirectionalLight( rObjectBase *_obj ) { setLight( _obj ); }

   void setLight( rObjectBase *_obj ) {
      _obj->getVector( &direction, rObjectBase::DIRECTION );
      _obj->getVector( &color, rObjectBase::LIGHT_COLOR );

      setAmbient( _obj );
   }
};

template <class T>
struct rRenderPointLight : rRenderAmbientLight<T> {
   rVec3<T> *color = nullptr;
   rVec3<T> *position = nullptr;
   rVec3<T> *attenuation = nullptr;

   using rRenderAmbientLight<T>::setAmbient;

   rRenderPointLight() : rRenderAmbientLight<T>() {}
   rRenderPointLight( rObjectBase *_obj ) { setLight( _obj ); }

   void setLight( rObjectBase *_obj ) {
      _obj->getVector( &position, rObjectBase::POSITION_MODEL_VIEW );
      _obj->getVector( &color, rObjectBase::LIGHT_COLOR );
      _obj->getVector( &attenuation, rObjectBase::ATTENUATION );

      setAmbient( _obj );
   }
};
}

#endif // R_LIGHT_SOURCE_STUCTS_HPP
