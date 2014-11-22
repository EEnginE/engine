/*!
 * \file rMatrixObjectBase.hpp
 * \brief \b Classes: \a rMatrixObjectBase
 */

#ifndef R_MATRIX_OBJECT_BASE_HPP
#define R_MATRIX_OBJECT_BASE_HPP

#include "rMatrixMath.hpp"

namespace e_engine {

/*!
 * \brief Class for managing Camera space matrix
 *
 *
 */
template<class T>
class rMatrixObjectBase {
   private:
      rMat4<T>  vScaleMatrix_MAT;
      rMat4<T>  vRotationMatrix_MAT;
      rMat4<T>  vTranslationMatrix_MAT;

      rMat4<T> *vCameraSpaceMatrix_MAT;

      rMat4<T>  vObjectSpaceMatrix_MAT;
      rMat4<T>  vFinalMatrix_MAT;

   public:
      rMatrixObjectBase();

      inline void setPosition( const rVec3<T> &_pos );
      inline void getPosition( rVec3<T> &_pos );
      inline rVec3<T> getPosition() {rVec3<T> lPos; getPosition( lPos ); return lPos;}
      inline void addPositionDelta( const rVec3<T> &_pos );

      inline void setRotation( const rVec3<T> &_axis, T _angle );

      inline void setScale( T _scale );
      inline void setScale( const rVec3<T> &_scale );
      inline void getScale( rVec3<T> &_scale );
      inline void addScaleDelta( const rVec3<T> &_scale );


      inline rMat4<T> *getScaleMatrix()       { return &vScaleMatrix_MAT; }
      inline rMat4<T> *getRotationMatrix()    { return &vRotationMatrix_MAT; }
      inline rMat4<T> *getTranslationMatrix() { return &vTranslationMatrix_MAT; }
      inline rMat4<T> *getObjectSpaceMatrix() { return &vObjectSpaceMatrix_MAT; }
      inline rMat4<T> *getFinalMatrix()       { return &vFinalMatrix_MAT; }

      inline void setCmaraSpaceMatrix( rMat4<T> *_mat ) { vCameraSpaceMatrix_MAT = _mat; }

      inline void updateFinalMatrix();
};

template<class T>
rMatrixObjectBase<T>::rMatrixObjectBase() {
   vScaleMatrix_MAT.toIdentityMatrix();
   vRotationMatrix_MAT.toIdentityMatrix();
   vTranslationMatrix_MAT.toIdentityMatrix();

   vFinalMatrix_MAT.toIdentityMatrix();

   vCameraSpaceMatrix_MAT = nullptr;


   vObjectSpaceMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if( vCameraSpaceMatrix_MAT )
      vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vObjectSpaceMatrix_MAT;
}

template<class T>
void rMatrixObjectBase<T>::setScale( T _scale ) {
   vScaleMatrix_MAT.setMat(
      _scale, 0     , 0     , 0,
      0     , _scale, 0     , 0,
      0     , 0     , _scale, 0,
      0     , 0     , 0     , 1
   );

   vObjectSpaceMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if( vCameraSpaceMatrix_MAT )
      vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vObjectSpaceMatrix_MAT;
}

template<class T>
void rMatrixObjectBase<T>::setScale( const rVec3< T > &_scale ) {
   vScaleMatrix_MAT.setMat(
      _scale.x, 0       , 0       , 0,
      0       , _scale.y, 0       , 0,
      0       , 0       , _scale.z, 0,
      0       , 0       , 0       , 1
   );

   vObjectSpaceMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if( vCameraSpaceMatrix_MAT )
      vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vObjectSpaceMatrix_MAT;
}

template<class T>
void rMatrixObjectBase<T>::getScale( rVec3< T > &_scale ) {
   _scale.x = vScaleMatrix_MAT.template get<0, 0>();
   _scale.y = vScaleMatrix_MAT.template get<1, 1>();
   _scale.z = vScaleMatrix_MAT.template get<2, 2>();
}

template<class T>
void rMatrixObjectBase<T>::addScaleDelta( const rVec3< T > &_scale ) {
   vScaleMatrix_MAT.setMat(
      vScaleMatrix_MAT( 0, 0 ) + _scale.x, 0                                   , 0                                  , 0,
      0                                  , vScaleMatrix_MAT( 1, 1 ) +  _scale.y, 0                                  , 0,
      0                                  , 0                                   , vScaleMatrix_MAT( 2, 2 ) + _scale.z, 0,
      0                                  , 0                                   , 0                                  , 1
   );

   vObjectSpaceMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if( vCameraSpaceMatrix_MAT )
      vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vObjectSpaceMatrix_MAT;
}

template<class T>
void rMatrixObjectBase<T>::setRotation( const rVec3< T > &_axis, T _angle ) {
   rMatrixMath::rotate( _axis, _angle, vRotationMatrix_MAT );

}


template<class T>
void rMatrixObjectBase<T>::setPosition( const rVec3< T > &_pos ) {
   vTranslationMatrix_MAT.setMat
      (
         1, 0, 0, _pos.x,
         0, 1, 0, _pos.y,
         0, 0, 1, _pos.z,
         0, 0, 0, 1
      );

   vObjectSpaceMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if( vCameraSpaceMatrix_MAT )
      vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vObjectSpaceMatrix_MAT;
}

template<class T>
void rMatrixObjectBase<T>::getPosition( rVec3< T > &_pos ) {
   _pos.x = vTranslationMatrix_MAT.template get<3, 0>();
   _pos.y = vTranslationMatrix_MAT.template get<3, 1>();
   _pos.z = vTranslationMatrix_MAT.template get<3, 2>();
}

template<class T>
void rMatrixObjectBase<T>::addPositionDelta( const rVec3< T > &_pos ) {
   vTranslationMatrix_MAT.setMat
      (
         1, 0, 0, vTranslationMatrix_MAT.template get<3, 0>() + _pos.x,
         0, 1, 0, vTranslationMatrix_MAT.template get<3, 1>() + _pos.y,
         0, 0, 1, vTranslationMatrix_MAT.template get<3, 2>() + _pos.z,
         0, 0, 0, 1
      );

   vObjectSpaceMatrix_MAT = vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;

   if( vCameraSpaceMatrix_MAT )
      vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vObjectSpaceMatrix_MAT;
}


template<class T>
void rMatrixObjectBase<T>::updateFinalMatrix() {
   if( vCameraSpaceMatrix_MAT )
      vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vObjectSpaceMatrix_MAT;
}



}

#endif // R_MATRIX_OBJECT_BASE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
