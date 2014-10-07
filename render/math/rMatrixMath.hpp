/*!
 * \file rMatrixMath.hpp
 * \brief \b Classes: \a rMatrixMath
 */

#ifndef R_MATRIX_MATH_HPP
#define R_MATRIX_MATH_HPP

#include "rMatrix.hpp"
#include "rVectorMath.hpp"
#include "defines.hpp"
#include "engine_render_Export.hpp"



namespace e_engine {

template<class T> using rMat2 = rMatrix<T, 2, 2>;
template<class T> using rMat3 = rMatrix<T, 3, 3>;
template<class T> using rMat4 = rMatrix<T, 4, 4>;

typedef rMatrix<float, 2, 2> rMat2f;
typedef rMatrix<float, 3, 3> rMat3f;
typedef rMatrix<float, 4, 4> rMat4f;

typedef rMatrix<double, 2, 2> rMat2d;
typedef rMatrix<double, 3, 3> rMat3d;
typedef rMatrix<double, 4, 4> rMat4d;

template<int N>
using rMatNf = rMatrix<float, N, N>;

template<int N>
using rMatNd = rMatrix<double, N, N>;

class rMatrixMath {
   public:
      template<class T> static void scale( T _n, rMat4<T> &_out );
      template<class T> static void scale( const rVec3<T> &_n, rMat4<T> &_out );

      template<class T> static void translate( const rVec3<T> &_n, rMat4<T> &_out );

      template<class T> static void rotate( const rVec3<T> &_axis, T _angle , rMat4<T> &_out );

      template<class T> static void perspective( T _width, T _height, T _nearZ, T _farZ, T _fofy, rMat4<T> &_out );
      template<class T> static void perspective( T _aspectRatio, T _nearZ, T _farZ, T _fofy, rMat4< T > &_out );

      template<class T>
      static void camera( const rVec3< T > &_position, const rVec3< T > &_lookAt, const rVec3< T > &_upVector, rMat4< T > &_out );
};

template<class T>
void rMatrixMath::scale( T _n, rMat4<T> &_out ) {
   _out.set2
   (
         _n, 0 , 0,  0,
         0,  _n, 0,  0,
         0,  0,  _n, 0,
         0,  0,  0,  1
   );
}

template<class T>
void rMatrixMath::scale( const rVec3<T> &_n, rMat4<T> &_out ) {
   _out.set2
   (
         _n.x, 0 ,   0,    0,
         0,    _n.y, 0,    0,
         0,    0,    _n.z, 0,
         0,    0,    0,    1
   );
}

template<class T>
void rMatrixMath::translate( const rVec3<T> &_n, rMat4<T> &_out ) {
   _out.set2
   (
         1, 0, 0,  _n.x,
         0, 1, 0,  _n.y,
         0, 0, 1,  _n.z,
         0, 0, 0, 1
   );
}

template<class T>
void rMatrixMath::rotate( const rVec3<T> &_axis, T _angle, rMat4<T> &_out ) {
   rVec3<T> lAxis = rVectorMath::normalizeReturn( _axis );
   T lAngleToUse = DEG_TO_RAD( _angle ) / 2;
   T lSin = sin( lAngleToUse );

   rVec4<T> lFinal( 1, 0, 0, 0 );
   rVec4<T> lTemp(
         cos( lAngleToUse ),
         _axis.x * lSin,
         _axis.y * lSin,
         _axis.z * lSin
   );

   rVectorMath::quaternionMultiplication( lTemp, lFinal, lFinal );

   _out.template get<0, 0>() = 1 - 2 * lFinal.y * lFinal.y - 2 * lFinal.z * lFinal.z;
   _out.template get<1, 0>() =     2 * lFinal.x * lFinal.y - 2 * lFinal.w * lFinal.z;
   _out.template get<2, 0>() =     2 * lFinal.x * lFinal.z + 2 * lFinal.w * lFinal.y;
   _out.template get<3, 0>() = 0;

   _out.template get<0, 1>() =     2 * lFinal.x * lFinal.y + 2 * lFinal.w * lFinal.z;
   _out.template get<1, 1>() = 1 - 2 * lFinal.x * lFinal.x - 2 * lFinal.z * lFinal.z;
   _out.template get<2, 1>() =     2 * lFinal.y * lFinal.z + 2 * lFinal.w * lFinal.x;
   _out.template get<3, 1>() = 0;

   _out.template get<0, 2>() =     2 * lFinal.x * lFinal.z - 2 * lFinal.w * lFinal.y;
   _out.template get<1, 2>() =     2 * lFinal.y * lFinal.z - 2 * lFinal.w * lFinal.x;
   _out.template get<2, 2>() = 1 - 2 * lFinal.x * lFinal.x - 2 * lFinal.y * lFinal.y;
   _out.template get<3, 2>() = 0;

   _out.template get<0, 3>() = 0;
   _out.template get<1, 3>() = 0;
   _out.template get<2, 3>() = 0;
   _out.template get<3, 3>() = 1;

#if E_DEBUG_LOGGING

   dLOG(
         "[MATRIX - rotate] IN:\n"
         "   _axis:  (", _axis.x, "|", _axis.y, "|", _axis.z, ")\n"
         "   _angle: ", _angle
   );
   _out.print( "[MATRIX - rotate] OUT", 'D' );

#endif

}

template<class T>
void rMatrixMath::perspective( T _aspectRatio, T _nearZ, T _farZ, T _fofy, rMat4< T > &_out ) {
   GLfloat f = ( 1.0f / tan( DEG_TO_RAD( _fofy / 2 ) ) );

   _out.fill( 0 );
   _out.template get<0, 0>() = f / _aspectRatio;
   _out.template get<1, 1>() = f;
   _out.template get<2, 2>() = ( _farZ + _nearZ ) / ( _nearZ - _farZ );
   _out.template get<3, 2>() = ( 2 * _farZ * _nearZ ) / ( _nearZ - _farZ );
   _out.template get<2, 3>() = -1;

#if E_DEBUG_LOGGING

   dLOG(
         "[MATRIX - perspective] IN:\n"
         "   _aspectRatio: ", _aspectRatio, "\n"
         "   _nearZ:       ", _nearZ,       "\n"
         "   _farZ:        ", _farZ,        "\n"
         "   _fofy:        ", _fofy
   );
   _out.print( "[MATRIX - perspective] OUT", 'D' );

#endif
}

template<class T>
void rMatrixMath::perspective( T _width, T _height, T _nearZ, T _farZ, T _fofy, rMat4< T > &_out ) {
   perspective( _width / _height, _nearZ, _farZ, _fofy, _out );
}

template<class T>
void rMatrixMath::camera( const rVec3< T > &_position, const rVec3< T > &_lookAt, const rVec3< T > &_upVector, rMat4< T > &_out ) {
   rVec3<T> f = rVectorMath::normalizeReturn( _lookAt - _position );
   rVec3<T> u = rVectorMath::normalizeReturn( _upVector );
   rVec3<T> s = rVectorMath::normalizeReturn( rVectorMath::crossProduct( f, u ) );
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

#if E_DEBUG_LOGGING

   dLOG(
         "[MATRIX - camera] IN:\n"
         "   _position: (", _position.x, "|", _position.y, "|", _position.z, ")\n"
         "   _lookAt:   (", _lookAt.x,   "|", _lookAt.y,   "|", _lookAt.z,   ")\n"
         "   _upVector: (", _upVector.x, "|", _upVector.y, "|", _upVector.z, ")"
   );
   _out.print( "[MATRIX - camera] OUT", 'D' );

#endif
}







}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

