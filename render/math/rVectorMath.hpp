/*!
 * \file rVectorMath.hpp
 * \brief \b Classes: \a rVectorMath
 */

#ifndef R_VECTOR_MATH_HPP
#define R_VECTOR_MATH_HPP

#include "rMatrix.hpp"
#include <math.h>
#include "engine_render_Export.hpp"


namespace e_engine {

template<class T, int N> using rVecN = rMatrix<T, N, 1>;
template<class T>        using rVec2 = rMatrix<T, 2, 1>;
template<class T>        using rVec3 = rMatrix<T, 3, 1>;
template<class T>        using rVec4 = rMatrix<T, 4, 1>;

/*!
 * | Element | Value |
 * | :-----: | :---: |
 * |    0    |   x   |
 * |    1    |   y   |
 * |    2    |   z   |
 * |    3    |   w   |
 */

typedef rMatrix<float, 2, 1> rVec2f;
typedef rMatrix<float, 3, 1> rVec3f;
typedef rMatrix<float, 4, 1> rVec4f;

typedef rMatrix<double, 2, 1> rVec2d;
typedef rMatrix<double, 3, 1> rVec3d;
typedef rMatrix<double, 4, 1> rVec4d;

template<int N>
using rVecNf = rMatrix<float, N, 1>;

template<int N>
using rVecNd = rMatrix<double, N, 1>;

class rVectorMath {
   public:
      template<class T, int N> static T    dotProduct( const rVecN<T, N> &_vec1, const rVecN<T, N> &_vec2 );
      template<class T>        static T    dotProduct( const rVec2<T>    &_vec1, const rVec2<T>    &_vec2 );
      template<class T>        static T    dotProduct( const rVec3<T>    &_vec1, const rVec3<T>    &_vec2 );
      template<class T>        static T    dotProduct( const rVec4<T>    &_vec1, const rVec4<T>    &_vec2 );

      template<class T>        static void quaternionMultiplication( const rVec4<T> &_q1, const rVec4<T> &_q2, rVec4<T> &_out );

      template<class T, int N> static T    length( const rVecN<T, N> &_vec );
      template<class T>        static T    length( const rVec2<T>    &_vec );
      template<class T>        static T    length( const rVec3<T>    &_vec );
      template<class T>        static T    length( const rVec4<T>    &_vec );

      template<class T, int N> static void normalize( rVecN<T, N> &_vec );
      template<class T>        static void normalize( rVec2<T>    &_vec );
      template<class T>        static void normalize( rVec3<T>    &_vec );
      template<class T>        static void normalize( rVec4<T>    &_vec );

      template<class T, int N>
      static rVecN<T, N>  normalizeReturn( rVecN<T, N> const &_vec ) {
         rVecN<T, N> lResult = _vec;
         normalize( lResult );
         return lResult;
      }

      template<class T>
      static rVec3<T>     crossProduct( const rVec3<T> &_vec1, const rVec3<T> &_vec2 );
};


// =========================================================================================================================
// ==============================================================================================================================================
// =========            =====================================================================================================================================
// =======   Dot Product  ========================================================================================================================================
// =========            =====================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template<class T, int N>
T rVectorMath::dotProduct( const rVecN<T, N> &_vec1, const rVecN<T, N> &_vec2 ) {
   T lProduct = 0;

   for( int i = 0; i < N; ++i )
      lProduct += _vec1[i] * _vec2[i];

   return lProduct;
}

template<class T>
T rVectorMath::dotProduct( const rVec2<T> &_vec1, const rVec2<T> &_vec2 )  {
   return _vec1.x * _vec2.x + _vec1.y * _vec2.y;
}

template<class T>
T rVectorMath::dotProduct( const rVec3<T> &_vec1, const rVec3<T> &_vec2 ) {
   return _vec1.x * _vec2.x + _vec1.y * _vec2.y + _vec1.z * _vec2.z;
}

template<class T>
T rVectorMath::dotProduct( const rVec4<T> &_vec1, const rVec4<T> &_vec2 ) {
   return _vec1.x * _vec2.x + _vec1.y * _vec2.y + _vec1.z * _vec2.z + _vec1.w * _vec2.w;
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========          =======================================================================================================================================
// =======   Normalize  ==========================================================================================================================================
// =========          =======================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template<class T, int N>
void rVectorMath::normalize( rVecN<T, N> &_vec ) {
   T lTemp = 0;
   for( unsigned int i = 0; i < N; ++i )
      lTemp += _vec[i] * _vec[i];

   T lLength = sqrt( lTemp );

   for( unsigned int i = 0; i < N; ++i )
      _vec[i] /= lLength;
}

template<class T>
void rVectorMath::normalize( rVec2<T> &_vec ) {
   T lLength = sqrt( _vec.x * _vec.x + _vec.y * _vec.y );

   _vec.x /= lLength;
   _vec.y /= lLength;
}

template<class T>
void rVectorMath::normalize( rVec3<T> &_vec ) {
   T lLength = sqrt( _vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z );

   _vec.x /= lLength;
   _vec.y /= lLength;
   _vec.z /= lLength;
}

template<class T>
void rVectorMath::normalize( rVec4<T> &_vec ) {
   T lLength = sqrt( _vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z + _vec.w * _vec.w );

   _vec.x /= lLength;
   _vec.y /= lLength;
   _vec.z /= lLength;
   _vec.w /= lLength;
}

// =========================================================================================================================
// ==============================================================================================================================================
// =========       ==========================================================================================================================================
// =======   Length  =============================================================================================================================================
// =========       ==========================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


template<class T, int N>
T rVectorMath::length( const rVecN<T, N> &_vec ) {
   T lTemp = 0;
   for( unsigned int i = 0; i < N; ++i )
      lTemp += _vec[i] * _vec[i];

   return sqrt( lTemp );
}

template<class T>
T rVectorMath::length( const rVec2<T> &_vec )  {
   return sqrt( _vec.x * _vec.x + _vec.y * _vec.y );
}

template<class T>
T rVectorMath::length( const rVec3<T> &_vec ) {
   return sqrt( _vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z );
}

template<class T>
T rVectorMath::length( const rVec4<T> &_vec ) {
   return sqrt( _vec.x * _vec.x + _vec.y * _vec.y + _vec.z * _vec.z + _vec.w * _vec.w );
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========              ===================================================================================================================================
// =======   Cross Product  ======================================================================================================================================
// =========              ===================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


template<class T>
rVec3<T> rVectorMath::crossProduct( const rVec3<T> &_vec1, const rVec3<T> &_vec2 ) {
   return rVec3<T>
         (
               ( _vec1.y * _vec2.z ) - ( _vec1.z * _vec2.y ),
               ( _vec1.z * _vec2.x ) - ( _vec1.x * _vec2.z ),
               ( _vec1.x * _vec2.y ) - ( _vec1.y * _vec2.x )
         );
}

/*
(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2)
(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
*/

template<class T>
void rVectorMath::quaternionMultiplication( const rVec4<T> &_q1, const rVec4<T> &_q2, rVec4<T> &_out ) {
   _out.x = ( _q1.w * _q2.x ) + ( _q1.x * _q2.w ) + ( _q1.y * _q2.z ) - ( _q1.z * _q2.y );
   _out.y = ( _q1.w * _q2.y ) - ( _q1.x * _q2.z ) + ( _q1.y * _q2.w ) + ( _q1.z * _q2.x );
   _out.z = ( _q1.w * _q2.z ) + ( _q1.x * _q2.y ) - ( _q1.y * _q2.x ) + ( _q1.z * _q2.w );
   _out.w = ( _q1.w * _q2.w ) - ( _q1.x * _q2.x ) - ( _q1.y * _q2.y ) - ( _q1.z * _q2.z );
}


}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
