#ifndef RMATRIX_H
#define RMATRIX_H

#include "uLog.hpp"
#include <type_traits>

#define TOLERANCE 0.001

namespace e_engine {

namespace internal {

template<class T, int R, int S>
struct rMatrixData {
   T vDataMat[R *S];
};

template<class T>
struct rMatrixData<T, 2, 1> {
   union {
      struct {
         T x, y;
      };

      T vDataMat[2];
   };

   void normalize() {
      T lLength2 = x * x + y * y;

      if( lLength2 > ( T )( 1.0 + TOLERANCE ) && lLength2 < ( T )( 1.0 - TOLERANCE ) )
         return; // Nothing to do here

      T lLength = sqrt( lLength2 );

      x /= lLength;
      y /= lLength;
   }

   T length() {
      return sqrt( x * x + y * y );
   }
};

template<class T>
struct rMatrixData<T, 3, 1> {
   union {
      struct {
         T x, y, z;
      };

      T vDataMat[3];
   };

   void normalize() {
      T lLength2 = x * x + y * y + z * z;

      if( lLength2 > ( T )( 1.0 + TOLERANCE ) && lLength2 < ( T )( 1.0 - TOLERANCE ) )
         return; // Nothing to do here

      T lLength = sqrt( lLength2 );

      x /= lLength;
      y /= lLength;
      z /= lLength;
   }

   T length() {
      return sqrt( x * x + y * y + z * z );
   }
};

template<class T>
struct rMatrixData<T, 4, 1> {
   union {
      struct {
         T x, y, z, w;
      };

      T vDataMat[4];
   };

   void normalize() {
      T lLength2 = x * x + y * y + z * z + w * w;

      if( lLength2 > ( T )( 1.0 + TOLERANCE ) && lLength2 < ( T )( 1.0 - TOLERANCE ) )
         return; // Nothing to do here

      T lLength = sqrt( lLength2 );

      x /= lLength;
      y /= lLength;
      z /= lLength;
      w /= lLength;
   }

   T length() {
      return sqrt( x * x + y * y + z * z + w * w );
   }
};

}

template <class TYPE, int ROWS, int COLLUMNS>
class rMatrix : public internal::rMatrixData<TYPE, ROWS, COLLUMNS> {
      static_assert( ( ROWS *COLLUMNS ) >= 2 , "Matrix size (ROWS*COLLUMNS) must be at least 2" );

   private:
      template<uint32_t POS, class... ARGS>  inline void setHelper( TYPE && _arg, ARGS && ... _args );
      template<uint32_t POS>                 inline void setHelper( TYPE && _arg );

      template<uint32_t POS, class... ARGS>  inline void setHelper( const TYPE &_arg, ARGS && ... _args );
      template<uint32_t POS>                 inline void setHelper( const TYPE &_arg );


      inline void TYPE2String( uint32_t && _pos, std::string &_str );

   public:

      // Tell the compiler that we are using templates and can access this:
      using internal::rMatrixData<TYPE, ROWS, COLLUMNS>::vDataMat;

      rMatrix()             {}
      rMatrix( TYPE &_f )   {fill( std::forward<TYPE>( _f ) );}
      rMatrix( TYPE && _f ) {fill( _f );}
      rMatrix( TYPE *_f );
      rMatrix( const rMatrix<TYPE, ROWS, COLLUMNS> &_newMatrix );

      template<class... ARGS>
      rMatrix( TYPE && _a1, ARGS && ... _args );

      ~rMatrix() {}

      TYPE       &get( uint32_t _position )                     {return vDataMat[_position];}
      TYPE const &get( uint32_t _position ) const               {return vDataMat[_position];}
      TYPE       &get( uint32_t _x, uint32_t _y )               {return vDataMat[( _x * ROWS ) + _y];}
      TYPE const &get( uint32_t _x, uint32_t _y ) const         {return vDataMat[( _x * ROWS ) + _y];}

      template<uint32_t I>             TYPE       &get()       {static_assert( I     < ROWS * COLLUMNS  , "Out of range" ); return vDataMat[I];}
      template<uint32_t I>             TYPE const &get() const {static_assert( I     < ROWS * COLLUMNS  , "Out of range" ); return vDataMat[I];}
      template<uint32_t X, uint32_t Y> TYPE       &get()       {static_assert( X < COLLUMNS && Y < ROWS , "Out of range" ); return vDataMat[( X * ROWS ) + Y];}
      template<uint32_t X, uint32_t Y> TYPE const &get() const {static_assert( X < COLLUMNS && Y < ROWS , "Out of range" ); return vDataMat[( X * ROWS ) + Y];}

      TYPE       *getMatrix()                                   {return vDataMat;}
      void        set( uint32_t _position, TYPE _newVal )       {vDataMat[_position] = _newVal;}

      void        set( uint32_t _x, uint32_t _y, TYPE _newVal ) {vDataMat[( _x * ROWS ) + _y] = _newVal;}
      void        set( TYPE *_matrix );

      template<class... ARGS>
      void        setMat( ARGS && ... _args );

      int getRowSize()     {return ROWS;}
      int getCollumnSize() {return COLLUMNS;}
      int getSize()        {return ROWS * COLLUMNS;}

      // DTTSEIW = DUMMY_TEMPLATE_THAT_STD_ENABLE_IF_WORKS
      template<class DTTSEIW = void>
      typename std::enable_if<ROWS == COLLUMNS, DTTSEIW>::type
      toIdentityMatrix();

      void fill( TYPE &_f )  {fill( std::forward<TYPE>( _f ) );}
      void fill( TYPE && _f );

      template<int R, int C>
      void downscale( rMatrix<TYPE, R, C> *_new ) const;

      template<int R, int C>
      void upscale( rMatrix<TYPE, R, C> *_new ) const;

      template <int COLLUMNS_NEW>
      void multiply( const rMatrix<TYPE, COLLUMNS, COLLUMNS_NEW> &_matrix, rMatrix<TYPE, ROWS, COLLUMNS_NEW> *_targetMatrix );

      //Hardcoded multiply methods
      void multiply( const rMatrix<TYPE, 2, 2> &_matrix, rMatrix<TYPE, 2, 2> *_targetMatrix );
      void multiply( const rMatrix<TYPE, 3, 3> &_matrix, rMatrix<TYPE, 3, 3> *_targetMatrix );
      void multiply( const rMatrix<TYPE, 4, 4> &_matrix, rMatrix<TYPE, 4, 4> *_targetMatrix );

      void add( const rMatrix<TYPE, ROWS, COLLUMNS> &_matrix, rMatrix< TYPE, ROWS, COLLUMNS > *_targetMatrix );
      void subtract( const rMatrix<TYPE, ROWS, COLLUMNS> &_matrix, rMatrix< TYPE, ROWS, COLLUMNS > *_targetMatrix );

      // Operators

      /*!
       * The template parameters are changed due to a conflict with the already existing templates.
       */
      template <class T, int R, int C, int C_N >
      friend rMatrix<T, R, C_N> operator*( rMatrix<T, R, C> _lMatrix, rMatrix<T, C, C_N> &_rMatrix );

      template <class T, int R, int C >
      friend rMatrix<T, R, C> operator*( T _lScalar, const rMatrix<T, R, C> &_rMatrix );

      template <class T, int R, int C >
      friend rMatrix<T, R, C> operator*( rMatrix<T, R, C> _lMatrix, const T &_rScalar );

      template <class T, int R, int C >
      friend rMatrix<T, R, C> operator+( rMatrix<T, R, C> _lMatrix, const rMatrix<T, R, C> &_rMatrix );

      template <class T, int R, int C>
      friend rMatrix<T, R, C> operator-( rMatrix<T, R, C> _lMatrix, const rMatrix<T, R, C> &_rMatrix );

      rMatrix<TYPE, ROWS, COLLUMNS> &operator=( rMatrix<TYPE, ROWS, COLLUMNS> _newMatrix );

      rMatrix<TYPE, ROWS, COLLUMNS> &operator+=( const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix );
      rMatrix<TYPE, ROWS, COLLUMNS> &operator-=( const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix );
      rMatrix<TYPE, ROWS, COLLUMNS> &operator*=( const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix );
      rMatrix<TYPE, ROWS, COLLUMNS> &operator*=( const TYPE                          &_rScalar );
      rMatrix<TYPE, ROWS, COLLUMNS> &operator/=( const TYPE                          &_rScalar );

      TYPE &operator[]( uint32_t _x )                          {return get( _x );}
      TYPE &operator()( uint32_t _x, uint32_t _y )             {return get( _x, _y );}

      TYPE const &operator[]( uint32_t _x )              const {return get( _x );}
      TYPE const &operator()( uint32_t _x, uint32_t _y ) const {return get( _x, _y );}

      void print( std::string _name = "Matrix", char _type = 'D' );
};


// =========================================================================================================================
// ==============================================================================================================================================
// =========             ====================================================================================================================================
// =======   Constructors  =======================================================================================================================================
// =========             ====================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================



template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix( TYPE *_f )  {
   if( _f == nullptr )
      return;

   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vDataMat[i] = _f[i];
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix( const rMatrix<TYPE, ROWS, COLLUMNS> &_newMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vDataMat[i] = _newMatrix.vDataMat[i];
}

template <class TYPE, int ROWS, int COLLUMNS>
template <class... ARGS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix( TYPE && _a1, ARGS && ... _args ) {
   static_assert( sizeof...( _args ) == ( ROWS * COLLUMNS - 1 ), "Wrong Number of arguments for this size of matrix / vector" );
   setHelper<0>( std::forward<TYPE>( _a1 ), std::forward<ARGS>( _args )... );
}



// =========================================================================================================================
// ==============================================================================================================================================
// =========          =======================================================================================================================================
// =======   Operators  ==========================================================================================================================================
// =========          =======================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> &rMatrix<TYPE, ROWS, COLLUMNS>::operator=( rMatrix<TYPE, ROWS, COLLUMNS> _newMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vDataMat[i] = _newMatrix.get( i );
   return *this;
}



template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> &rMatrix<TYPE, ROWS, COLLUMNS>::operator+=( const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix ) {
   add( _rMatrix, this );
   return *this;
}


template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> &rMatrix<TYPE, ROWS, COLLUMNS>::operator-=( const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix ) {
   subtract( _rMatrix, this );
   return *this;
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> &rMatrix<TYPE, ROWS, COLLUMNS>::operator*=( const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix ) {
   multiply( _rMatrix, this );
   return *this;
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> &rMatrix<TYPE, ROWS, COLLUMNS>::operator*=( const TYPE &_rScalar ) {
   for( uint32_t i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vDataMat[i] *= _rScalar;

   return *this;
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> &rMatrix<TYPE, ROWS, COLLUMNS>::operator/=( const TYPE &_rScalar ) {
   for( uint32_t i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vDataMat[i] /= _rScalar;

   return *this;
}




template <class TYPE, int ROWS, int COLLUMNS, int COLLUMNS_NEW>
rMatrix<TYPE, ROWS, COLLUMNS_NEW> operator*( rMatrix<TYPE, ROWS, COLLUMNS> _lMatrix, rMatrix<TYPE, COLLUMNS, COLLUMNS_NEW> &_rMatrix ) {
   rMatrix<TYPE, ROWS, COLLUMNS_NEW> _target;
   _lMatrix.multiply( _rMatrix, &_target );
   return _target;
}


template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> operator*( TYPE _lScalar, const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix ) {
   rMatrix<TYPE, ROWS, COLLUMNS> lTarget = _rMatrix;
   lTarget *= _lScalar;
   return lTarget;
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> operator*( rMatrix<TYPE, ROWS, COLLUMNS> _lMatrix, const TYPE &_rScalar ) {
   return _lMatrix *= _rScalar;
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> operator+( rMatrix<TYPE, ROWS, COLLUMNS> _lMatrix, const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix ) {
   return _lMatrix += _rMatrix;
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> operator-( rMatrix<TYPE, ROWS, COLLUMNS> _lMatrix, const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix ) {
   return _lMatrix -= _rMatrix;
}



// =========================================================================================================================
// ==============================================================================================================================================
// =========                  ===============================================================================================================================
// =======   Predefined matrix  ==================================================================================================================================
// =========                  ===============================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief Sets the Matrix to a identity matrix
 *
 * \note Only Works when ROWS == COLLUMNS else disabled with std::enable_if
 */
// DTTSEIW = DUMMY_TEMPLATE_THAT_STD_ENABLE_IF_WORKS
template<class TYPE, int ROWS, int COLLUMNS>
template<class DTTSEIW>
typename std::enable_if<ROWS == COLLUMNS, DTTSEIW>::type
rMatrix<TYPE, ROWS, COLLUMNS>::toIdentityMatrix() {
   vDataMat[0] = 1;
   for( int i = 1; i < ROWS * ROWS; ++i )
      if( ( i % ( ROWS + 1 ) ) == 0 )
         vDataMat[i] = 1;
      else
         vDataMat[i] = 0;
}

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::fill( TYPE && _f ) {
   for( int i = 1; i < ROWS * COLLUMNS; ++i )
      vDataMat[i] = _f;
}

template <class TYPE, int ROWS, int COLLUMNS>
template <int R, int C>
void rMatrix<TYPE, ROWS, COLLUMNS>::downscale( rMatrix<TYPE, R, C> *_new ) const {
   static_assert( ( ROWS * COLLUMNS ) >= 2 , "Matrix size (R*C) must be at least 2" );
   static_assert( R <= ROWS && C <= COLLUMNS, "The matrix to downscale must be smaller" );

   for( int x = 0; x < C; ++x )
      for( int y = 0; y < R; ++y )
         _new->set( x, y, get( x, y ) );
}

template <class TYPE, int ROWS, int COLLUMNS>
template <int R, int C>
void rMatrix<TYPE, ROWS, COLLUMNS>::upscale( rMatrix<TYPE, R, C> *_new ) const {
   static_assert( ( ROWS * COLLUMNS ) >= 2 , "Matrix size (R*C) must be at least 2" );
   static_assert( R >= ROWS && C >= COLLUMNS, "The matrix to upscale must be larger" );

   for( int x = 0; x < COLLUMNS; ++x )
      for( int y = 0; y < ROWS; ++y )
         _new->set( x, y, get( x, y ) );
}

// =========================================================================================================================
// ==============================================================================================================================================
// =========                =================================================================================================================================
// =======   Math Operations  ====================================================================================================================================
// =========                =================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*! The target matrix will
      *   have the row-count of the initial Matrix
      *   and the collumn-count of the second Matrix.
      *
      *   The second matrix needs to have a row-count
      *   equal to the collumn-count of the initial matrix.
      */

template <class TYPE, int ROWS, int COLLUMNS>
template <int COLLUMNS_NEW>
void rMatrix<TYPE, ROWS, COLLUMNS>::multiply( const rMatrix<TYPE, COLLUMNS, COLLUMNS_NEW> &_matrix, rMatrix<TYPE, ROWS, COLLUMNS_NEW> *_targetMatrix ) {
   int currentIndex = 0;
   TYPE currentSum = 0;
   for( int i = 0; i < COLLUMNS_NEW; ++i ) { //Second Matrix
      for( int j = 0; j < ROWS; ++j ) {  // First Matrix
         for( int k = 0; k < COLLUMNS; ++k )  //Both Matrices, this calculates the sum given to the target Matrix
            currentSum += get( k, j ) * _matrix.get( i, k );
         _targetMatrix->set( i, j, currentSum );
         ++currentIndex;
         currentSum = 0;
      }
   }
}

//HARDCODED 2x2
template<class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::multiply( const rMatrix<TYPE, 2, 2> &_matrix, rMatrix<TYPE, 2, 2> *_targetMatrix ) {
   _targetMatrix->vDataMat[0] = ( ( vDataMat[0] * _matrix.vDataMat[0] ) + ( vDataMat[2] * _matrix.vDataMat[1] ) );
   _targetMatrix->vDataMat[1] = ( ( vDataMat[1] * _matrix.vDataMat[0] ) + ( vDataMat[3] * _matrix.vDataMat[1] ) );
   _targetMatrix->vDataMat[2] = ( ( vDataMat[0] * _matrix.vDataMat[2] ) + ( vDataMat[2] * _matrix.vDataMat[3] ) );
   _targetMatrix->vDataMat[3] = ( ( vDataMat[1] * _matrix.vDataMat[2] ) + ( vDataMat[3] * _matrix.vDataMat[3] ) );
}


//HARDCODED 3x3
template<class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::multiply( const rMatrix<TYPE, 3, 3> &_matrix, rMatrix<TYPE, 3, 3> *_targetMatrix ) {
   _targetMatrix->vDataMat[0] = ( ( vDataMat[0] * _matrix.vDataMat[0] ) + ( vDataMat[3] * _matrix.vDataMat[1] ) + ( vDataMat[6] * _matrix.vDataMat[2] ) ) ;
   _targetMatrix->vDataMat[1] = ( ( vDataMat[1] * _matrix.vDataMat[0] ) + ( vDataMat[4] * _matrix.vDataMat[1] ) + ( vDataMat[7] * _matrix.vDataMat[2] ) ) ;
   _targetMatrix->vDataMat[2] = ( ( vDataMat[2] * _matrix.vDataMat[0] ) + ( vDataMat[5] * _matrix.vDataMat[1] ) + ( vDataMat[8] * _matrix.vDataMat[2] ) ) ;
   _targetMatrix->vDataMat[3] = ( ( vDataMat[0] * _matrix.vDataMat[3] ) + ( vDataMat[3] * _matrix.vDataMat[4] ) + ( vDataMat[6] * _matrix.vDataMat[5] ) ) ;
   _targetMatrix->vDataMat[4] = ( ( vDataMat[1] * _matrix.vDataMat[3] ) + ( vDataMat[4] * _matrix.vDataMat[4] ) + ( vDataMat[7] * _matrix.vDataMat[5] ) ) ;
   _targetMatrix->vDataMat[5] = ( ( vDataMat[2] * _matrix.vDataMat[3] ) + ( vDataMat[5] * _matrix.vDataMat[4] ) + ( vDataMat[8] * _matrix.vDataMat[5] ) ) ;
   _targetMatrix->vDataMat[6] = ( ( vDataMat[0] * _matrix.vDataMat[6] ) + ( vDataMat[3] * _matrix.vDataMat[7] ) + ( vDataMat[6] * _matrix.vDataMat[8] ) ) ;
   _targetMatrix->vDataMat[7] = ( ( vDataMat[1] * _matrix.vDataMat[6] ) + ( vDataMat[4] * _matrix.vDataMat[7] ) + ( vDataMat[7] * _matrix.vDataMat[8] ) ) ;
   _targetMatrix->vDataMat[8] = ( ( vDataMat[2] * _matrix.vDataMat[6] ) + ( vDataMat[5] * _matrix.vDataMat[7] ) + ( vDataMat[8] * _matrix.vDataMat[8] ) ) ;
}


//HARDCODED 4x4
template<class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::multiply( const rMatrix<TYPE, 4, 4> &_matrix, rMatrix<TYPE, 4, 4> *_targetMatrix ) {
   _targetMatrix->vDataMat[0]  = ( ( vDataMat[0] * _matrix.vDataMat[0] )  + ( vDataMat[4] * _matrix.vDataMat[1] )  + ( vDataMat[8]  * _matrix.vDataMat[2] )  + ( vDataMat[12] * _matrix.vDataMat[3] ) )  ;
   _targetMatrix->vDataMat[1]  = ( ( vDataMat[1] * _matrix.vDataMat[0] )  + ( vDataMat[5] * _matrix.vDataMat[1] )  + ( vDataMat[9]  * _matrix.vDataMat[2] )  + ( vDataMat[13] * _matrix.vDataMat[3] ) )  ;
   _targetMatrix->vDataMat[2]  = ( ( vDataMat[2] * _matrix.vDataMat[0] )  + ( vDataMat[6] * _matrix.vDataMat[1] )  + ( vDataMat[10] * _matrix.vDataMat[2] )  + ( vDataMat[14] * _matrix.vDataMat[3] ) )  ;
   _targetMatrix->vDataMat[3]  = ( ( vDataMat[3] * _matrix.vDataMat[0] )  + ( vDataMat[7] * _matrix.vDataMat[1] )  + ( vDataMat[11] * _matrix.vDataMat[2] )  + ( vDataMat[15] * _matrix.vDataMat[3] ) )  ;
   _targetMatrix->vDataMat[4]  = ( ( vDataMat[0] * _matrix.vDataMat[4] )  + ( vDataMat[4] * _matrix.vDataMat[5] )  + ( vDataMat[8]  * _matrix.vDataMat[6] )  + ( vDataMat[12] * _matrix.vDataMat[7] ) )  ;
   _targetMatrix->vDataMat[5]  = ( ( vDataMat[1] * _matrix.vDataMat[4] )  + ( vDataMat[5] * _matrix.vDataMat[5] )  + ( vDataMat[9]  * _matrix.vDataMat[6] )  + ( vDataMat[13] * _matrix.vDataMat[7] ) )  ;
   _targetMatrix->vDataMat[6]  = ( ( vDataMat[2] * _matrix.vDataMat[4] )  + ( vDataMat[6] * _matrix.vDataMat[5] )  + ( vDataMat[10] * _matrix.vDataMat[6] )  + ( vDataMat[14] * _matrix.vDataMat[7] ) )  ;
   _targetMatrix->vDataMat[7]  = ( ( vDataMat[3] * _matrix.vDataMat[4] )  + ( vDataMat[7] * _matrix.vDataMat[5] )  + ( vDataMat[11] * _matrix.vDataMat[6] )  + ( vDataMat[15] * _matrix.vDataMat[7] ) )  ;
   _targetMatrix->vDataMat[8]  = ( ( vDataMat[0] * _matrix.vDataMat[8] )  + ( vDataMat[4] * _matrix.vDataMat[9] )  + ( vDataMat[8]  * _matrix.vDataMat[10] ) + ( vDataMat[12] * _matrix.vDataMat[11] ) ) ;
   _targetMatrix->vDataMat[9]  = ( ( vDataMat[1] * _matrix.vDataMat[8] )  + ( vDataMat[5] * _matrix.vDataMat[9] )  + ( vDataMat[9]  * _matrix.vDataMat[10] ) + ( vDataMat[13] * _matrix.vDataMat[11] ) ) ;
   _targetMatrix->vDataMat[10] = ( ( vDataMat[2] * _matrix.vDataMat[8] )  + ( vDataMat[6] * _matrix.vDataMat[9] )  + ( vDataMat[10] * _matrix.vDataMat[10] ) + ( vDataMat[14] * _matrix.vDataMat[11] ) ) ;
   _targetMatrix->vDataMat[11] = ( ( vDataMat[3] * _matrix.vDataMat[8] )  + ( vDataMat[7] * _matrix.vDataMat[9] )  + ( vDataMat[11] * _matrix.vDataMat[10] ) + ( vDataMat[15] * _matrix.vDataMat[11] ) ) ;
   _targetMatrix->vDataMat[12] = ( ( vDataMat[0] * _matrix.vDataMat[12] ) + ( vDataMat[4] * _matrix.vDataMat[13] ) + ( vDataMat[8]  * _matrix.vDataMat[14] ) + ( vDataMat[12] * _matrix.vDataMat[15] ) ) ;
   _targetMatrix->vDataMat[13] = ( ( vDataMat[1] * _matrix.vDataMat[12] ) + ( vDataMat[5] * _matrix.vDataMat[13] ) + ( vDataMat[9]  * _matrix.vDataMat[14] ) + ( vDataMat[13] * _matrix.vDataMat[15] ) ) ;
   _targetMatrix->vDataMat[14] = ( ( vDataMat[2] * _matrix.vDataMat[12] ) + ( vDataMat[6] * _matrix.vDataMat[13] ) + ( vDataMat[10] * _matrix.vDataMat[14] ) + ( vDataMat[14] * _matrix.vDataMat[15] ) ) ;
   _targetMatrix->vDataMat[15] = ( ( vDataMat[3] * _matrix.vDataMat[12] ) + ( vDataMat[7] * _matrix.vDataMat[13] ) + ( vDataMat[11] * _matrix.vDataMat[14] ) + ( vDataMat[15] * _matrix.vDataMat[15] ) ) ;
}

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::add( const rMatrix<TYPE, ROWS, COLLUMNS> &_matrix, rMatrix<TYPE, ROWS, COLLUMNS> *_targetMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vDataMat[i] + _matrix.get( i ) ) );
}

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::subtract( const rMatrix<TYPE, ROWS, COLLUMNS> &_matrix, rMatrix<TYPE, ROWS, COLLUMNS> *_targetMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vDataMat[i] - _matrix.get( i ) ) );
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========            =====================================================================================================================================
// =======   Get and set  ========================================================================================================================================
// =========            =====================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::set( TYPE *_matrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vDataMat[i] = _matrix[i];
}

template <class TYPE, int ROWS, int COLLUMNS>
template<class... ARGS>
void rMatrix<TYPE, ROWS, COLLUMNS>::setMat( ARGS && ... _args ) {
   static_assert( sizeof...( _args ) == ( ROWS * COLLUMNS ), "Wrong Number of arguments to set the size of this size of matrix / vector [set2]" );
   setHelper<0>( std::forward<ARGS>( _args )... );
}



template <class TYPE, int ROWS, int COLLUMNS>
template<uint32_t POS, class... ARGS>
inline void rMatrix<TYPE, ROWS, COLLUMNS>::setHelper( TYPE && _arg, ARGS && ... _args ) {
   vDataMat[( ( POS % ROWS ) * COLLUMNS ) + ( POS / ROWS )] = _arg;
   setHelper < POS + 1 > ( std::forward<ARGS>( _args )... );
}

template <class TYPE, int ROWS, int COLLUMNS>
template<uint32_t POS>
inline void rMatrix<TYPE, ROWS, COLLUMNS>::setHelper( TYPE && _arg ) {
   vDataMat[( ( POS % ROWS ) * COLLUMNS ) + ( POS / ROWS )] = _arg;
}


template <class TYPE, int ROWS, int COLLUMNS>
template<uint32_t POS, class... ARGS>
inline void rMatrix<TYPE, ROWS, COLLUMNS>::setHelper( const TYPE &_arg, ARGS && ... _args ) {
   vDataMat[( ( POS % ROWS ) * COLLUMNS ) + ( POS / ROWS )] = _arg;
   setHelper < POS + 1 > ( std::forward<ARGS>( _args )... );
}

template <class TYPE, int ROWS, int COLLUMNS>
template<uint32_t POS>
inline void rMatrix<TYPE, ROWS, COLLUMNS>::setHelper( const TYPE &_arg ) {
   vDataMat[( ( POS % ROWS ) * COLLUMNS ) + ( POS / ROWS )] = _arg;
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========          =======================================================================================================================================
// =======   Printing   ==========================================================================================================================================
// =========          =======================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::TYPE2String( uint32_t && _pos, std::string &_str ) {
   static std::string lTempStr;
   lTempStr = std::to_string( vDataMat[_pos] );

   if( lTempStr.size() < 10 ) {
      lTempStr.insert( lTempStr.begin(), 10 - lTempStr.size(), ' ' );
   } else {
      lTempStr.resize( 10 );
   }

   _str += lTempStr + " ";
}


template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::print( std::string _name, char _type ) {
   LOG( _type, false, __FILE__, __LINE__, LOG_FUNCTION_NAME, _name, ": " );

   std::string lRowStr;

   for( size_t row = 0; row < ROWS; ++row ) {
      lRowStr.clear();
      for( size_t collumn = 0; collumn < COLLUMNS; ++collumn ) {
         TYPE2String( ( collumn * ROWS ) + row, lRowStr );
      }
      LOG( _type, true, __FILE__, __LINE__, LOG_FUNCTION_NAME, "( ", lRowStr, " )" );
   }

   LOG( _type, true, __FILE__, __LINE__, LOG_FUNCTION_NAME, "" );
}

}

#endif // RMATRIX_H
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;



