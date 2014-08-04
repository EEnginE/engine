/*!
 * \file rMatrixMath.hpp
 * \brief \b Classes: \a rMatrixMath
 */

#ifndef R_MATRIX_MATH_HPP
#define R_MATRIX_MATH_HPP

#include "rMatrix.hpp"
#include "rVectorMath.hpp"

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
         _n[0], 0 ,    0,     0,
         0,     _n[1], 0,     0,
         0,     0,     _n[2], 0,
         0,     0,     0,     1
   );
}

template<class T>
void rMatrixMath::translate( const rVec3<T> &_n, rMat4<T> &_out ) {
   _out.set2
   (
         1,     0,     0, 0,
         0,     1,     0, 0,
         0,     0,     1, 0,
         _n[0], _n[1], _n[2], 1
   );
}

template<class T>
void rMatrixMath::rotate( const rVec3<T> &_axis, T _angle, rMat4<T> &_out ) {
   rVec3<T> lAxis = rVectorMath::normalizeReturn( _axis );
   T lAngleToUse = DEG_TO_RAD( _angle ) / 2;
   T lSin = sin( lAngleToUse );

   rQuaternion<T> lFinal( 1, 0, 0, 0 );
   rQuaternion<T> lTemp(
         cos( lAngleToUse ),
         _axis[0] * lSin,
         _axis[1] * lSin,
         _axis[2] * lSin
   );

   rVectorMath::quaternionMultiplication( lTemp, lFinal, lFinal );

   _out( 0, 0 ) = 1 - 2 * lFinal[2] * lFinal[2] - 2 * lFinal[3] * lFinal[3];
   _out( 1, 0 ) =     2 * lFinal[1] * lFinal[2] - 2 * lFinal[0] * lFinal[3];
   _out( 2, 0 ) =     2 * lFinal[1] * lFinal[3] + 2 * lFinal[0] * lFinal[2];
   _out( 3, 0 ) = 0;

   _out( 0, 1 ) =     2 * lFinal[1] * lFinal[2] + 2 * lFinal[0] * lFinal[3];
   _out( 1, 1 ) = 1 - 2 * lFinal[1] * lFinal[1] - 2 * lFinal[3] * lFinal[3];
   _out( 2, 1 ) =     2 * lFinal[2] * lFinal[3] + 2 * lFinal[0] * lFinal[1];
   _out( 3, 1 ) = 0;

   _out( 0, 2 ) =     2 * lFinal[1] * lFinal[3] - 2 * lFinal[0] * lFinal[2];
   _out( 1, 2 ) =     2 * lFinal[2] * lFinal[3] - 2 * lFinal[0] * lFinal[1];
   _out( 2, 2 ) = 1 - 2 * lFinal[1] * lFinal[1] - 2 * lFinal[2] * lFinal[2];
   _out( 3, 2 ) = 0;

   _out( 0, 3 ) = 0;
   _out( 1, 3 ) = 0;
   _out( 2, 3 ) = 0;
   _out( 3, 3 ) = 1;

}

template<class T>
void rMatrixMath::perspective( T _aspectRatio, T _nearZ, T _farZ, T _fofy, rMat4< T > &_out ) {
   GLfloat f = ( 1.0f / tan( DEG_TO_RAD( _fofy / 2 ) ) );

   _out.fill( 0 );
   _out( 0, 0 ) = f / _aspectRatio;
   _out( 1, 1 ) = f;
   _out( 2, 2 ) = ( _farZ + _nearZ ) / ( _nearZ - _farZ );
   _out( 3, 2 ) = ( 2 * _farZ * _nearZ ) / ( _nearZ - _farZ );
   _out( 2, 3 ) = -1;
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

   _out( 0, 0 ) = s[0];
   _out( 1, 0 ) = s[1];
   _out( 2, 0 ) = s[2];
   _out( 0, 1 ) = u[0];
   _out( 1, 1 ) = u[1];
   _out( 2, 1 ) = u[2];
   _out( 0, 2 ) = -f[0];
   _out( 1, 2 ) = -f[1];
   _out( 2, 2 ) = -f[2];
   _out( 3, 0 ) = -rVectorMath::dotProduct( s, _position );
   _out( 3, 1 ) = -rVectorMath::dotProduct( u, _position );
   _out( 3, 2 ) = rVectorMath::dotProduct( f, _position );

   for( unsigned int i = 0; i < 4; ++i ) {
      std::string lStr;
      for( unsigned int j = 0; j < 4; ++j ) {
         lStr += boost::lexical_cast<std::string>( _out( j, i ) ) + "  ";
      }
      eLOG lStr END
   }
   dLOG "" END
}







}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
