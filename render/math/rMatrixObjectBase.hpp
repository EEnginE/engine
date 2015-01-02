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

#ifndef R_MATRIX_OBJECT_BASE_HPP
#define R_MATRIX_OBJECT_BASE_HPP

#include "defines.hpp"

#include "rMatrixMath.hpp"
#include "rMatrixSceneBase.hpp"

namespace e_engine {

/*!
 * \brief Class for managing Camera space matrix
 *
 *
 */
template <class T>
class rMatrixObjectBase {
 private:
   rMat4<T> vScaleMatrix_MAT;
   rMat4<T> vRotationMatrix_MAT;
   rMat4<T> vTranslationMatrix_MAT;

   rMat4<T> *vViewProjectionMatrix_MAT;
   rMat4<T> *vViewMatrix_MAT;
   rMat4<T> *vProjectionMatrix_MAT;

   rMat4<T> vModelMatrix_MAT;
   rMat4<T> vModelViewMatrix_MAT;
   rMat4<T> vModelViewProjectionMatrix_MAT;

   rMat3<T> vNormalMatrix;

   rVec3<T> vPosition;
   rVec3<T> vPositionModelView;
   rVec3<T> vScale;

   rMatrixObjectBase();

 public:
   rMatrixObjectBase( rMatrixSceneBase<T> *_scene );

   inline void setPosition( const rVec3<T> &_pos );
   inline void getPosition( rVec3<T> &_pos );
   inline rVec3<T> *getPosition() { return &vPosition; }
   inline rVec3<T> *getPositionModelView() { return &vPositionModelView; }
   inline void addPositionDelta( const rVec3<T> &_pos );

   inline void setRotation( const rVec3<T> &_axis, T _angle );

   inline void setScale( T _scale );
   inline void setScale( const rVec3<T> &_scale );
   inline rVec3<T> *getScale() { return &vScale; }
   inline void addScaleDelta( const rVec3<T> &_scale );


   inline rMat4<T> *getScaleMatrix() { return &vScaleMatrix_MAT; }
   inline rMat4<T> *getRotationMatrix() { return &vRotationMatrix_MAT; }
   inline rMat4<T> *getTranslationMatrix() { return &vTranslationMatrix_MAT; }

   inline rMat4<T> *getModelMatrix() { return &vModelMatrix_MAT; }
   inline rMat4<T> *getModelViewMatrix() { return &vModelViewMatrix_MAT; }
   inline rMat4<T> *getViewMatrix() { return vViewMatrix_MAT; }

   inline rMat4<T> *getProjectionMatrix() { return vProjectionMatrix_MAT; }
   inline rMat4<T> *getViewProjectionMatrix() { return vViewProjectionMatrix_MAT; }
   inline rMat4<T> *getModelViewProjectionMatrix() { return &vModelViewProjectionMatrix_MAT; }

   inline rMat3<T> *getNormalMatrix() { return &vNormalMatrix; }

   inline void updateFinalMatrix();
};

template <class T>
rMatrixObjectBase<T>::rMatrixObjectBase( rMatrixSceneBase<T> *_scene ) {
   vScaleMatrix_MAT.toIdentityMatrix();
   vRotationMatrix_MAT.toIdentityMatrix();
   vTranslationMatrix_MAT.toIdentityMatrix();
   vModelViewMatrix_MAT.toIdentityMatrix();

   vViewProjectionMatrix_MAT = _scene->getViewProjectionMatrix();
   vViewMatrix_MAT = _scene->getViewMatrix();
   vProjectionMatrix_MAT = _scene->getProjectionMatrix();

   vModelMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if ( vViewProjectionMatrix_MAT )
      vModelViewProjectionMatrix_MAT = *vViewProjectionMatrix_MAT * vModelMatrix_MAT;
   else
      vModelViewProjectionMatrix_MAT.toIdentityMatrix();
}

template <class T>
void rMatrixObjectBase<T>::setScale( T _scale ) {
   vScale.fill( _scale );

   vScaleMatrix_MAT.setMat( _scale, 0, 0, 0, 0, _scale, 0, 0, 0, 0, _scale, 0, 0, 0, 0, 1 );

   updateFinalMatrix();
}

template <class T>
void rMatrixObjectBase<T>::setScale( const rVec3<T> &_scale ) {
   vScale = _scale;

   vScaleMatrix_MAT.setMat( _scale.x, 0, 0, 0, 0, _scale.y, 0, 0, 0, 0, _scale.z, 0, 0, 0, 0, 1 );

   updateFinalMatrix();
}


template <class T>
void rMatrixObjectBase<T>::addScaleDelta( const rVec3<T> &_scale ) {
   vScale += _scale;

   vScaleMatrix_MAT.setMat( vScale.x, 0, 0, 0, 0, vScale.y, 0, 0, 0, 0, vScale.z, 0, 0, 0, 0, 1 );

   updateFinalMatrix();
}

template <class T>
void rMatrixObjectBase<T>::setRotation( const rVec3<T> &_axis, T _angle ) {
   rMatrixMath::rotate( _axis, _angle, vRotationMatrix_MAT );

   updateFinalMatrix();
}


template <class T>
void rMatrixObjectBase<T>::setPosition( const rVec3<T> &_pos ) {
   vPosition = _pos;

   vTranslationMatrix_MAT.setMat( 1, 0, 0, _pos.x, 0, 1, 0, _pos.y, 0, 0, 1, _pos.z, 0, 0, 0, 1 );

   updateFinalMatrix();
}


template <class T>
void rMatrixObjectBase<T>::addPositionDelta( const rVec3<T> &_pos ) {
   vPosition += _pos;

   vTranslationMatrix_MAT.setMat(
         1, 0, 0, vPosition.x, 0, 1, 0, vPosition.y, 0, 0, 1, vPosition.z, 0, 0, 0, 1 );

   updateFinalMatrix();
}


template <class T>
void rMatrixObjectBase<T>::updateFinalMatrix() {
   vModelMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if ( vViewProjectionMatrix_MAT )
      vModelViewProjectionMatrix_MAT = *vViewProjectionMatrix_MAT * vModelMatrix_MAT;

   if ( vViewMatrix_MAT ) {
      vModelViewMatrix_MAT = *vViewMatrix_MAT * vModelMatrix_MAT;

      rVec4<T> lTemp( 0, 0, 0, 1 );
      lTemp = vModelViewMatrix_MAT * lTemp;
      lTemp.downscale( &vPositionModelView );
   }

   rMatrixMath::getNormalMatrix( vModelViewMatrix_MAT, vNormalMatrix );
}
}

#endif // R_MATRIX_OBJECT_BASE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
