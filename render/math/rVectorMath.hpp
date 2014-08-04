/*!
 * \file rVectorMath.hpp
 * \brief \b Classes: \a rVectorMath
 */

#ifndef R_VECTOR_MATH_HPP
#define R_VECTOR_MATH_HPP

#include "rMatrix.hpp"
#include <math.h>

namespace e_engine {

template<class T> using rVec2       = rMatrix<T, 1, 2>;
template<class T> using rVec3       = rMatrix<T, 1, 3>;

/*!
 * | Element | Value |
 * | :-----: | :---: |
 * |    0    |   w   |
 * |    1    |   x   |
 * |    2    |   y   |
 * |    3    |   z   |
 */
template<class T> using rQuaternion = rMatrix<T, 1, 4>;

typedef rMatrix<float, 1, 2> rVec2f;
typedef rMatrix<float, 1, 3> rVec3f;
typedef rMatrix<float, 1, 4> rQuaternionf;

typedef rMatrix<double, 1, 2> rVec2d;
typedef rMatrix<double, 1, 3> rVec3d;
typedef rMatrix<double, 1, 4> rQuaterniond;

template<int N>
using rVecNf = rMatrix<float, 1, N>;

template<int N>
using rVecNd = rMatrix<double, 1, N>;

class rVectorMath {
   public:
      template<class T, int N> static T    dotProduct( const rMatrix<T, 1, N> &_vec1, const rMatrix<T, 1, N> &_vec2 );
      template<class T>        static T    dotProduct( const rMatrix<T, 1, 2> &_vec1, const rMatrix<T, 1, 2> &_vec2 );
      template<class T>        static T    dotProduct( const rMatrix<T, 1, 3> &_vec1, const rMatrix<T, 1, 3> &_vec2 );
      template<class T>        static T    dotProduct( const rMatrix<T, 1, 4> &_vec1, const rMatrix<T, 1, 4> &_vec2 );

      template<class T>        static void quaternionMultiplication( const rQuaternion<T> &_q1, const rQuaternion<T> &_q2, rQuaternion<T> &_out );
      
      template<class T, int N> static T    length( const rMatrix<T, 1, N> &_vec);
      template<class T>        static T    length( const rMatrix<T, 1, 2> &_vec);
      template<class T>        static T    length( const rMatrix<T, 1, 3> &_vec);
      template<class T>        static T    length( const rMatrix<T, 1, 4> &_vec);

      template<class T, int N> static void normalize( rMatrix<T, 1, N> &_vec );
      template<class T>        static void normalize( rMatrix<T, 1, 2> &_vec );
      template<class T>        static void normalize( rMatrix<T, 1, 3> &_vec );
      template<class T>        static void normalize( rMatrix<T, 1, 4> &_vec );

      template<class T, int N>
      static rMatrix<T, 1, N>  normalizeReturn( rMatrix<T, 1, N> const &_vec ) {
         rMatrix<T, 1, N> lResult = _vec;
         normalize( lResult );
         return lResult;
      }

      template<class T>
      static rMatrix<T, 1, 3>  crossProduct( const rMatrix<T, 1, 3> &_vec1, const rMatrix<T, 1, 3> &_vec2 );
};


// =========================================================================================================================
// ==============================================================================================================================================
// =========            =====================================================================================================================================
// =======   Dot Product  ========================================================================================================================================
// =========            =====================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template<class T, int N>
T rVectorMath::dotProduct( const rMatrix<T, 1 , N> &_vec1, const rMatrix<T, 1 , N> &_vec2 ) {
   T lProduct = 0;

   for( int i = 0; i < N; ++i )
      lProduct += _vec1[i] * _vec2[i];

   return lProduct;
}

template<class T>
T rVectorMath::dotProduct( const rMatrix<T, 1 , 2> &_vec1, const rMatrix<T, 1 , 2> &_vec2 )  {
   return _vec1[0] * _vec2[0] + _vec1[1] * _vec2[1];
}

template<class T>
T rVectorMath::dotProduct( const rMatrix<T, 1 , 3> &_vec1, const rMatrix<T, 1 , 3> &_vec2 ) {
   return _vec1[0] * _vec2[0] + _vec1[1] * _vec2[1] + _vec1[2] * _vec2[2];
}

template<class T>
T rVectorMath::dotProduct( const rMatrix<T, 1 , 4> &_vec1, const rMatrix<T, 1 , 4> &_vec2 ) {
   return _vec1[0] * _vec2[0] + _vec1[1] * _vec2[1] + _vec1[2] * _vec2[2] + _vec1[3] * _vec2[3];
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========          =======================================================================================================================================
// =======   Normalize  ==========================================================================================================================================
// =========          =======================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template<class T, int N>
void rVectorMath::normalize( rMatrix< T, 1 , N  > &_vec ) {
   T lTemp = 0;
   for( unsigned int i = 0; i < N; ++i )
      lTemp += _vec[i] * _vec[i];

   T lLength = sqrt( lTemp );

   for( unsigned int i = 0; i < N; ++i )
      _vec[i] /= lLength;
}

template<class T>
void rVectorMath::normalize( rMatrix< T, 1 , 2  > &_vec ) {
   T lLength = sqrt( _vec[0] * _vec[0] + _vec[1] * _vec[1] );

   _vec[0] /= lLength;
   _vec[1] /= lLength;
}

template<class T>
void rVectorMath::normalize( rMatrix< T, 1 , 3  > &_vec ) {
   T lLength = sqrt( _vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2] );

   _vec[0] /= lLength;
   _vec[1] /= lLength;
   _vec[2] /= lLength;
}

template<class T>
void rVectorMath::normalize( rMatrix< T, 1 , 4  > &_vec ) {
   T lLength = sqrt( _vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2] + _vec[3] * _vec[3] );

   _vec[0] /= lLength;
   _vec[1] /= lLength;
   _vec[2] /= lLength;
   _vec[3] /= lLength;
}

// =========================================================================================================================
// ==============================================================================================================================================
// =========       ==========================================================================================================================================
// =======   Length  =============================================================================================================================================
// =========       ==========================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


template<class T, int N>
T rVectorMath::length( const rMatrix<T, 1 , N> &_vec) {
   T lTemp = 0;
   for( unsigned int i = 0; i < N; ++i )
      lTemp += _vec[i] * _vec[i];

   return sqrt( lTemp );
}

template<class T>
T rVectorMath::length( const rMatrix<T, 1 , 2> &_vec)  {
   return sqrt( _vec[0] * _vec[0] + _vec[1] * _vec[1] );
}

template<class T>
T rVectorMath::length( const rMatrix<T, 1 , 3> &_vec) {
   return sqrt( _vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2] );
}

template<class T>
T rVectorMath::length( const rMatrix<T, 1 , 4> &_vec) {
   return sqrt( _vec[0] * _vec[0] + _vec[1] * _vec[1] + _vec[2] * _vec[2] + _vec[3] * _vec[3] );
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========              ===================================================================================================================================
// =======   Cross Product  ======================================================================================================================================
// =========              ===================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


template<class T>
rMatrix< T, 1, 3 > rVectorMath::crossProduct( const rMatrix< T, 1, 3 > &_vec1, const rMatrix< T, 1, 3 > &_vec2 ) {
   return rMatrix< T, 1, 3 >
         (
               ( _vec1[1] * _vec2[2] ) - ( _vec1[2] * _vec2[1] ),
               ( _vec1[2] * _vec2[0] ) - ( _vec1[0] * _vec2[2] ),
               ( _vec1[0] * _vec2[1] ) - ( _vec1[1] * _vec2[0] )
         );
}

/*
(Q1 * Q2).w = (w1w2 - x1x2 - y1y2 - z1z2)
(Q1 * Q2).x = (w1x2 + x1w2 + y1z2 - z1y2)
(Q1 * Q2).y = (w1y2 - x1z2 + y1w2 + z1x2)
(Q1 * Q2).z = (w1z2 + x1y2 - y1x2 + z1w2
*/

template<class T>
void rVectorMath::quaternionMultiplication( const rQuaternion<T> &_q1, const rQuaternion<T> &_q2, rQuaternion<T> &_out ) {
   _out.set2(
         ( ( _q1[0] * _q2[0] ) - ( _q1[1] * _q2[1] ) - ( _q1[2] * _q2[2] ) - ( _q1[3] * _q2[3] ) ),
         ( ( _q1[0] * _q2[1] ) + ( _q1[1] * _q2[0] ) + ( _q1[2] * _q2[3] ) - ( _q1[3] * _q2[2] ) ),
         ( ( _q1[0] * _q2[2] ) - ( _q1[1] * _q2[3] ) + ( _q1[2] * _q2[0] ) + ( _q1[3] * _q2[1] ) ),
         ( ( _q1[0] * _q2[3] ) + ( _q1[1] * _q2[2] ) - ( _q1[2] * _q2[1] ) + ( _q1[3] * _q2[0] ) )
   );
}


}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
