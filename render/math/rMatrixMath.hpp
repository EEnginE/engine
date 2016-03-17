/*!
 * \file rMatrixMath.hpp
 * \brief \b Classes: \a rMatrixMath
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

#include "rMatrix.hpp"
#include "rVectorMath.hpp"

namespace e_engine {

template <class T>
using rMat2 = rMatrix<T, 2, 2>;
template <class T>
using rMat3 = rMatrix<T, 3, 3>;
template <class T>
using rMat4 = rMatrix<T, 4, 4>;

typedef rMatrix<float, 2, 2> rMat2f;
typedef rMatrix<float, 3, 3> rMat3f;
typedef rMatrix<float, 4, 4> rMat4f;

typedef rMatrix<double, 2, 2> rMat2d;
typedef rMatrix<double, 3, 3> rMat3d;
typedef rMatrix<double, 4, 4> rMat4d;

template <int N>
using rMatNf = rMatrix<float, N, N>;

template <int N>
using rMatNd = rMatrix<double, N, N>;


template <class T, int N>
using rMat = rMatrix<T, N, N>;

class rMatrixMath {
 public:
   template <class T>
   static void scale( T _n, rMat4<T> &_out );
   template <class T>
   static void scale( const rVec3<T> &_n, rMat4<T> &_out );

   template <class T>
   static void translate( const rVec3<T> &_n, rMat4<T> &_out );

   template <class T>
   static void rotate( const rVec3<T> &_axis, T _angle, rMat4<T> &_out );

   template <class T>
   static void perspective( T _aspectRatio, T _nearZ, T _farZ, T _fofy, rMat4<T> &_out );

   template <class T>
   static void getNormalMatrix( rMat4<T> const &_in, rMat3<T> &_out );
   template <class T>
   static void getNormalMatrix( rMat3<T> const &_in, rMat3<T> &_out );

   template <class T>
   static void camera( const rVec3<T> &_position,
                       const rVec3<T> &_lookAt,
                       const rVec3<T> &_upVector,
                       rMat4<T> &_out );
};

template <class T>
void rMatrixMath::scale( T _n, rMat4<T> &_out ) {
   _out.setMat( _n, 0, 0, 0, 0, _n, 0, 0, 0, 0, _n, 0, 0, 0, 0, 1 );
}

template <class T>
void rMatrixMath::scale( const rVec3<T> &_n, rMat4<T> &_out ) {
   _out.setMat( _n.x, 0, 0, 0, 0, _n.y, 0, 0, 0, 0, _n.z, 0, 0, 0, 0, 1 );
}

template <class T>
void rMatrixMath::translate( const rVec3<T> &_n, rMat4<T> &_out ) {
   _out.setMat( 1, 0, 0, _n.x, 0, 1, 0, _n.y, 0, 0, 1, _n.z, 0, 0, 0, 1 );
}

template <class T>
void rMatrixMath::rotate( const rVec3<T> &_axis, T _angle, rMat4<T> &_out ) {
   rVec3<T> lAxis = _axis;
   lAxis.normalize();
   T lAngleToUse = static_cast<T>( DEG_TO_RAD( _angle ) / 2 );
   T lSin        = static_cast<T>( sin( lAngleToUse ) );


   rVec4<T> lTemp;
   lTemp.x = _axis.x * lSin;
   lTemp.y = _axis.y * lSin;
   lTemp.z = _axis.z * lSin;
   lTemp.w = static_cast<T>( cos( lAngleToUse ) );

   lTemp.normalize();

   T x2 = lTemp.x * lTemp.x;
   T y2 = lTemp.y * lTemp.y;
   T z2 = lTemp.z * lTemp.z;
   T xy = lTemp.x * lTemp.y;
   T xz = lTemp.x * lTemp.z;
   T yz = lTemp.y * lTemp.z;
   T wx = lTemp.w * lTemp.x;
   T wy = lTemp.w * lTemp.y;
   T wz = lTemp.w * lTemp.z;

   _out.template get<0, 0>() = 1 - 2 * y2 - 2 * z2;
   _out.template get<1, 0>() = 2 * xy - 2 * wz;
   _out.template get<2, 0>() = 2 * xz + 2 * wy;
   _out.template get<3, 0>() = 0;

   _out.template get<0, 1>() = 2 * xy + 2 * wz;
   _out.template get<1, 1>() = 1 - 2 * x2 - 2 * z2;
   _out.template get<2, 1>() = 2 * yz + 2 * wx;
   _out.template get<3, 1>() = 0;

   _out.template get<0, 2>() = 2 * xz - 2 * wy;
   _out.template get<1, 2>() = 2 * yz - 2 * wx;
   _out.template get<2, 2>() = 1 - 2 * x2 - 2 * y2;
   _out.template get<3, 2>() = 0;

   _out.template get<0, 3>() = 0;
   _out.template get<1, 3>() = 0;
   _out.template get<2, 3>() = 0;
   _out.template get<3, 3>() = 1;
}

template <class T>
void rMatrixMath::perspective( T _aspectRatio, T _nearZ, T _farZ, T _fofy, rMat4<T> &_out ) {
   T f = static_cast<T>( 1.0 / tan( static_cast<double>( DEG_TO_RAD( _fofy / 2 ) ) ) );

   _out.fill( 0 );
   _out.template get<0, 0>() = f / _aspectRatio;
   _out.template get<1, 1>() = f;
   _out.template get<2, 2>() = ( _farZ + _nearZ ) / ( _nearZ - _farZ );
   _out.template get<3, 2>() = ( 2 * _farZ * _nearZ ) / ( _nearZ - _farZ );
   _out.template get<2, 3>() = -1;
}

template <class T>
void rMatrixMath::camera( const rVec3<T> &_position,
                          const rVec3<T> &_lookAt,
                          const rVec3<T> &_upVector,
                          rMat4<T> &_out ) {
   rVec3<T> f = _lookAt - _position;
   f.normalize();

   rVec3<T> u = _upVector;
   u.normalize();

   rVec3<T> s = rVectorMath::crossProduct( f, u );
   s.normalize();

   u = rVectorMath::crossProduct( s, f );

   _out.template get<0, 0>() = s.x;
   _out.template get<1, 0>() = s.y;
   _out.template get<2, 0>() = s.z;
   _out.template get<0, 1>() = u.x;
   _out.template get<1, 1>() = u.y;
   _out.template get<2, 1>() = u.z;
   _out.template get<0, 2>() = -f.x;
   _out.template get<1, 2>() = -f.y;
   _out.template get<2, 2>() = -f.z;
   _out.template get<3, 0>() = -rVectorMath::dotProduct( s, _position );
   _out.template get<3, 1>() = -rVectorMath::dotProduct( u, _position );
   _out.template get<3, 2>() = rVectorMath::dotProduct( f, _position );
}


template <class T>
void rMatrixMath::getNormalMatrix( rMat4<T> const &_in, rMat3<T> &_out ) {
   rMat3<T> lTemp;
   _in.downscale( &lTemp );
   getNormalMatrix( lTemp, _out );
}

template <class T>
void rMatrixMath::getNormalMatrix( rMat3<T> const &_in, rMat3<T> &_out ) {
   T lDeterminante =
         +_in.template get<0, 0>() * ( _in.template get<1, 1>() * _in.template get<2, 2>() -
                                       _in.template get<1, 2>() * _in.template get<2, 1>() ) -
         _in.template get<0, 1>() * ( _in.template get<1, 0>() * _in.template get<2, 2>() -
                                      _in.template get<1, 2>() * _in.template get<2, 0>() ) +
         _in.template get<0, 2>() * ( _in.template get<1, 0>() * _in.template get<2, 1>() -
                                      _in.template get<1, 1>() * _in.template get<2, 0>() );

   _out.template get<0, 0>() = +( _in.template get<1, 1>() * _in.template get<2, 2>() -
                                  _in.template get<2, 1>() * _in.template get<1, 2>() );
   _out.template get<0, 1>() = -( _in.template get<1, 0>() * _in.template get<2, 2>() -
                                  _in.template get<2, 0>() * _in.template get<1, 2>() );
   _out.template get<0, 2>() = +( _in.template get<1, 0>() * _in.template get<2, 1>() -
                                  _in.template get<2, 0>() * _in.template get<1, 1>() );
   _out.template get<1, 0>() = -( _in.template get<0, 1>() * _in.template get<2, 2>() -
                                  _in.template get<2, 1>() * _in.template get<0, 2>() );
   _out.template get<1, 1>() = +( _in.template get<0, 0>() * _in.template get<2, 2>() -
                                  _in.template get<2, 0>() * _in.template get<0, 2>() );
   _out.template get<1, 2>() = -( _in.template get<0, 0>() * _in.template get<2, 1>() -
                                  _in.template get<2, 0>() * _in.template get<0, 1>() );
   _out.template get<2, 0>() = +( _in.template get<0, 1>() * _in.template get<1, 2>() -
                                  _in.template get<1, 1>() * _in.template get<0, 2>() );
   _out.template get<2, 1>() = -( _in.template get<0, 0>() * _in.template get<1, 2>() -
                                  _in.template get<1, 0>() * _in.template get<0, 2>() );
   _out.template get<2, 2>() = +( _in.template get<0, 0>() * _in.template get<1, 1>() -
                                  _in.template get<1, 0>() * _in.template get<0, 1>() );
   _out /= lDeterminante;
}
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
