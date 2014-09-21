#ifndef RMATRIX_H
#define RMATRIX_H

#include "uLog.hpp"

namespace e_engine {

template <class TYPE, int ROWS, int COLLUMNS>
class rMatrix {
   private:
      TYPE *vMatrix;
      bool vDelete;

      template<class... ARGS>
      inline void setHelper( const uint16_t _pos, const TYPE &_arg, const ARGS &... _args );
      inline void setHelper( const uint16_t _pos, const TYPE &_arg );
   public:
      rMatrix();
      rMatrix( TYPE _f );
      rMatrix( TYPE *_f );
      rMatrix( const rMatrix<TYPE, ROWS, COLLUMNS> &_newMatrix );

      template<class... ARGS>
      rMatrix( const ARGS &... _args );

      ~rMatrix();

      TYPE       &get( uint16_t _position )                     {return vMatrix[_position];}
      TYPE const &get( uint16_t _position ) const               {return vMatrix[_position];}
      TYPE       &get( uint16_t _x, uint16_t _y )               {return vMatrix[( _x * ROWS ) + _y];}
      TYPE const &get( uint16_t _x, uint16_t _y ) const         {return vMatrix[( _x * ROWS ) + _y];}
      TYPE       *get()                                         {return vMatrix;}
      void        set( uint16_t _position, TYPE _newVal )       {vMatrix[_position] = _newVal;}

      void        set( uint16_t _x, uint16_t _y, TYPE _newVal ) {vMatrix[( _x * ROWS ) + _y] = _newVal;}
      void        set( TYPE *_matrix );

      template<class... ARGS>
      bool        set2( const ARGS &... _args );

      int getRowSize()     {return ROWS;}
      int getCollumnSize() {return COLLUMNS;}

      void toIdentityMatrix();
      void fill( TYPE _f );


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

      TYPE &operator[]( uint16_t _x );
      TYPE &operator()( uint16_t _x, uint16_t _y );

      TYPE const &operator[]( uint16_t _x )              const;
      TYPE const &operator()( uint16_t _x, uint16_t _y ) const;

};


// =========================================================================================================================
// ==============================================================================================================================================
// =========             ====================================================================================================================================
// =======   Constructors  =======================================================================================================================================
// =========             ====================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix()  {
   vMatrix = new TYPE[ROWS * COLLUMNS];
   vDelete = true;
   fill( 0 );
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix( TYPE _f )  {
   vMatrix = new TYPE[ROWS * COLLUMNS];
   vDelete = true;
   fill( _f );
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix( TYPE *_f )  {
   vMatrix = new TYPE[ROWS * COLLUMNS];
   vDelete = true;

   if( _f == nullptr )
      return;

   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vMatrix[i] = _f[i];
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix( const rMatrix<TYPE, ROWS, COLLUMNS> &_newMatrix ) {
   vMatrix = new TYPE[ROWS * COLLUMNS];
   vDelete = true;

   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vMatrix[i] = _newMatrix.vMatrix[i];
}

template <class TYPE, int ROWS, int COLLUMNS>
template <class... ARGS>
rMatrix<TYPE, ROWS, COLLUMNS>::rMatrix( const ARGS &... _args ) {
   vMatrix = new TYPE[ROWS * COLLUMNS];
   vDelete = true;

   if( sizeof...( _args ) != ( ROWS * COLLUMNS ) ) {
      eLOG( "Can not set a ", ROWS, "x", COLLUMNS, " matrix with ", sizeof...( _args ), " Elements!" );
      fill( 0 );
   }
   setHelper( 0, _args... );
}



template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS>::~rMatrix() {
   if( vDelete ) delete[] vMatrix;
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========          =======================================================================================================================================
// =======   Operators  ==========================================================================================================================================
// =========          =======================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template <class TYPE, int ROWS, int COLLUMNS>
TYPE &rMatrix<TYPE, ROWS, COLLUMNS>::operator[]( uint16_t _x ) {
   return vMatrix[_x];
}

template <class TYPE, int ROWS, int COLLUMNS>
TYPE &rMatrix<TYPE, ROWS, COLLUMNS>::operator()( uint16_t _x, uint16_t _y ) {
   return vMatrix[( _x * ROWS ) + _y];
}

template <class TYPE, int ROWS, int COLLUMNS>
const TYPE &rMatrix<TYPE, ROWS, COLLUMNS>::operator[]( uint16_t _x ) const {
   return vMatrix[_x];
}

template <class TYPE, int ROWS, int COLLUMNS>
const TYPE &rMatrix<TYPE, ROWS, COLLUMNS>::operator()( uint16_t _x, uint16_t _y ) const {
   return vMatrix[( _x * ROWS ) + _y];
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> &rMatrix<TYPE, ROWS, COLLUMNS>::operator=( rMatrix<TYPE, ROWS, COLLUMNS> _newMatrix ) {
   _newMatrix.vDelete = false;
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vMatrix[i] = _newMatrix.get( i );
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
   for( uint16_t i = 0; i < (ROWS * COLLUMNS); ++i )
      vMatrix[i] *= _rScalar;
   
   return *this;
}




template <class TYPE, int ROWS, int COLLUMNS, int COLLUMNS_NEW>
rMatrix<TYPE, ROWS, COLLUMNS_NEW> operator*( rMatrix<TYPE, ROWS, COLLUMNS> _lMatrix, rMatrix<TYPE, COLLUMNS, COLLUMNS_NEW> &_rMatrix ) {
   rMatrix<TYPE, ROWS, COLLUMNS_NEW> _target;
   _target.vDelete  = false;
   _lMatrix.vDelete = false;
   _lMatrix.multiply( _rMatrix, &_target );
   return _target;
}


template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> operator*( TYPE _lScalar, const rMatrix<TYPE, ROWS, COLLUMNS> &_rMatrix ) {
   rMatrix<TYPE, ROWS, COLLUMNS> lTarget = _rMatrix;
   lTarget.vDelete  = false;
   lTarget *= _lScalar;
   return lTarget;
}

template <class TYPE, int ROWS, int COLLUMNS>
rMatrix<TYPE, ROWS, COLLUMNS> operator*( rMatrix<TYPE, ROWS, COLLUMNS> _lMatrix, const TYPE &_rScalar ) {
   _lMatrix.vDelete  = false;
   _lMatrix *= _rScalar;
   return _lMatrix;
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

template<class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::toIdentityMatrix() {
   if( ROWS != COLLUMNS ) {
      fill( 0 );
      return;
   }
   vMatrix[0] = 1;
   for( int i = 1; i < ROWS * ROWS; ++i )
      if( ( i % ( ROWS + 1 ) ) == 0 )
         vMatrix[i] = 1;
      else
         vMatrix[i] = 0;
}

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::fill( TYPE _f ) {
   for( int i = 1; i < ROWS * ROWS; ++i )
      vMatrix[i] = _f;
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
   for( int i = 0; i < COLLUMNS_NEW; ++i )  //Second Matrix
      for( int j = 0; j < ROWS; ++j ) {  // First Matrix
         for( int k = 0; k < COLLUMNS; ++k )  //Both Matrices, this calculates the sum given to the target Matrix
            currentSum += get( j, k ) * _matrix.get( k, i );
         _targetMatrix->set( currentIndex, currentSum );
         ++currentIndex;
         currentSum = 0;
      }
}

//HARDCODED 2x2
template<class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::multiply( const rMatrix<TYPE, 2, 2> &_matrix, rMatrix<TYPE, 2, 2> *_targetMatrix ) {
   _targetMatrix->vMatrix[0] = ( ( vMatrix[0] * _matrix.vMatrix[0] ) + ( vMatrix[4] * _matrix.vMatrix[1] ) );
   _targetMatrix->vMatrix[1] = ( ( vMatrix[1] * _matrix.vMatrix[0] ) + ( vMatrix[5] * _matrix.vMatrix[1] ) );
   _targetMatrix->vMatrix[2] = ( ( vMatrix[0] * _matrix.vMatrix[2] ) + ( vMatrix[4] * _matrix.vMatrix[3] ) );
   _targetMatrix->vMatrix[3] = ( ( vMatrix[1] * _matrix.vMatrix[2] ) + ( vMatrix[5] * _matrix.vMatrix[3] ) );
}


//HARDCODED 3x3
template<class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::multiply( const rMatrix<TYPE, 3, 3> &_matrix, rMatrix<TYPE, 3, 3> *_targetMatrix ) {
   _targetMatrix->vMatrix[0] = ( ( vMatrix[0] * _matrix.vMatrix[0] ) + ( vMatrix[4] * _matrix.vMatrix[1] ) + ( vMatrix[8]  * _matrix.vMatrix[2] ) )  ;
   _targetMatrix->vMatrix[1] = ( ( vMatrix[1] * _matrix.vMatrix[0] ) + ( vMatrix[5] * _matrix.vMatrix[1] ) + ( vMatrix[9]  * _matrix.vMatrix[2] ) )  ;
   _targetMatrix->vMatrix[2] = ( ( vMatrix[2] * _matrix.vMatrix[0] ) + ( vMatrix[6] * _matrix.vMatrix[1] ) + ( vMatrix[10] * _matrix.vMatrix[2] ) ) ;
   _targetMatrix->vMatrix[3] = ( ( vMatrix[0] * _matrix.vMatrix[3] ) + ( vMatrix[7] * _matrix.vMatrix[4] ) + ( vMatrix[11] * _matrix.vMatrix[5] ) ) ;
   _targetMatrix->vMatrix[4] = ( ( vMatrix[1] * _matrix.vMatrix[3] ) + ( vMatrix[4] * _matrix.vMatrix[4] ) + ( vMatrix[8]  * _matrix.vMatrix[5] ) )  ;
   _targetMatrix->vMatrix[5] = ( ( vMatrix[2] * _matrix.vMatrix[3] ) + ( vMatrix[5] * _matrix.vMatrix[4] ) + ( vMatrix[9]  * _matrix.vMatrix[5] ) )  ;
   _targetMatrix->vMatrix[6] = ( ( vMatrix[0] * _matrix.vMatrix[6] ) + ( vMatrix[6] * _matrix.vMatrix[7] ) + ( vMatrix[10] * _matrix.vMatrix[8] ) ) ;
   _targetMatrix->vMatrix[7] = ( ( vMatrix[1] * _matrix.vMatrix[6] ) + ( vMatrix[7] * _matrix.vMatrix[7] ) + ( vMatrix[11] * _matrix.vMatrix[8] ) ) ;
   _targetMatrix->vMatrix[8] = ( ( vMatrix[2] * _matrix.vMatrix[6] ) + ( vMatrix[4] * _matrix.vMatrix[7] ) + ( vMatrix[8]  * _matrix.vMatrix[8] ) )  ;
}


//HARDCODED 4x4
template<class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::multiply( const rMatrix<TYPE, 4, 4> &_matrix, rMatrix<TYPE, 4, 4> *_targetMatrix ) {
   _targetMatrix->vMatrix[0]  = ( ( vMatrix[0] * _matrix.vMatrix[0] )  + ( vMatrix[4] * _matrix.vMatrix[1] )  + ( vMatrix[8]  * _matrix.vMatrix[2] )  + ( vMatrix[12] * _matrix.vMatrix[3] ) )  ;
   _targetMatrix->vMatrix[1]  = ( ( vMatrix[1] * _matrix.vMatrix[0] )  + ( vMatrix[5] * _matrix.vMatrix[1] )  + ( vMatrix[9]  * _matrix.vMatrix[2] )  + ( vMatrix[13] * _matrix.vMatrix[3] ) )  ;
   _targetMatrix->vMatrix[2]  = ( ( vMatrix[2] * _matrix.vMatrix[0] )  + ( vMatrix[6] * _matrix.vMatrix[1] )  + ( vMatrix[10] * _matrix.vMatrix[2] )  + ( vMatrix[14] * _matrix.vMatrix[3] ) )  ;
   _targetMatrix->vMatrix[3]  = ( ( vMatrix[3] * _matrix.vMatrix[0] )  + ( vMatrix[7] * _matrix.vMatrix[1] )  + ( vMatrix[11] * _matrix.vMatrix[2] )  + ( vMatrix[15] * _matrix.vMatrix[3] ) )  ;
   _targetMatrix->vMatrix[4]  = ( ( vMatrix[0] * _matrix.vMatrix[4] )  + ( vMatrix[4] * _matrix.vMatrix[5] )  + ( vMatrix[8]  * _matrix.vMatrix[6] )  + ( vMatrix[12] * _matrix.vMatrix[7] ) )  ;
   _targetMatrix->vMatrix[5]  = ( ( vMatrix[1] * _matrix.vMatrix[4] )  + ( vMatrix[5] * _matrix.vMatrix[5] )  + ( vMatrix[9]  * _matrix.vMatrix[6] )  + ( vMatrix[13] * _matrix.vMatrix[7] ) )  ;
   _targetMatrix->vMatrix[6]  = ( ( vMatrix[2] * _matrix.vMatrix[4] )  + ( vMatrix[6] * _matrix.vMatrix[5] )  + ( vMatrix[10] * _matrix.vMatrix[6] )  + ( vMatrix[14] * _matrix.vMatrix[7] ) )  ;
   _targetMatrix->vMatrix[7]  = ( ( vMatrix[3] * _matrix.vMatrix[4] )  + ( vMatrix[7] * _matrix.vMatrix[5] )  + ( vMatrix[11] * _matrix.vMatrix[6] )  + ( vMatrix[15] * _matrix.vMatrix[7] ) )  ;
   _targetMatrix->vMatrix[8]  = ( ( vMatrix[0] * _matrix.vMatrix[8] )  + ( vMatrix[4] * _matrix.vMatrix[9] )  + ( vMatrix[8]  * _matrix.vMatrix[10] ) + ( vMatrix[12] * _matrix.vMatrix[11] ) ) ;
   _targetMatrix->vMatrix[9]  = ( ( vMatrix[1] * _matrix.vMatrix[8] )  + ( vMatrix[5] * _matrix.vMatrix[9] )  + ( vMatrix[9]  * _matrix.vMatrix[10] ) + ( vMatrix[13] * _matrix.vMatrix[11] ) ) ;
   _targetMatrix->vMatrix[10] = ( ( vMatrix[2] * _matrix.vMatrix[8] )  + ( vMatrix[6] * _matrix.vMatrix[9] )  + ( vMatrix[10] * _matrix.vMatrix[10] ) + ( vMatrix[14] * _matrix.vMatrix[11] ) ) ;
   _targetMatrix->vMatrix[11] = ( ( vMatrix[3] * _matrix.vMatrix[8] )  + ( vMatrix[7] * _matrix.vMatrix[9] )  + ( vMatrix[11] * _matrix.vMatrix[10] ) + ( vMatrix[15] * _matrix.vMatrix[11] ) ) ;
   _targetMatrix->vMatrix[12] = ( ( vMatrix[0] * _matrix.vMatrix[12] ) + ( vMatrix[4] * _matrix.vMatrix[13] ) + ( vMatrix[8]  * _matrix.vMatrix[14] ) + ( vMatrix[12] * _matrix.vMatrix[15] ) ) ;
   _targetMatrix->vMatrix[13] = ( ( vMatrix[1] * _matrix.vMatrix[12] ) + ( vMatrix[5] * _matrix.vMatrix[13] ) + ( vMatrix[9]  * _matrix.vMatrix[14] ) + ( vMatrix[13] * _matrix.vMatrix[15] ) ) ;
   _targetMatrix->vMatrix[14] = ( ( vMatrix[2] * _matrix.vMatrix[12] ) + ( vMatrix[6] * _matrix.vMatrix[13] ) + ( vMatrix[10] * _matrix.vMatrix[14] ) + ( vMatrix[14] * _matrix.vMatrix[15] ) ) ;
   _targetMatrix->vMatrix[15] = ( ( vMatrix[3] * _matrix.vMatrix[12] ) + ( vMatrix[7] * _matrix.vMatrix[13] ) + ( vMatrix[11] * _matrix.vMatrix[14] ) + ( vMatrix[15] * _matrix.vMatrix[15] ) ) ;
}

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::add( const rMatrix<TYPE, ROWS, COLLUMNS> &_matrix, rMatrix<TYPE, ROWS, COLLUMNS> *_targetMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vMatrix[i] + _matrix.get( i ) ) );
}

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::subtract( const rMatrix<TYPE, ROWS, COLLUMNS> &_matrix, rMatrix<TYPE, ROWS, COLLUMNS> *_targetMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vMatrix[i] - _matrix.get( i ) ) );
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========            =====================================================================================================================================
// =======   Get and set  ========================================================================================================================================
// =========            =====================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

template <class TYPE, int ROWS, int COLLUMNS>
template<class... ARGS>
bool rMatrix<TYPE, ROWS, COLLUMNS>::set2( const ARGS &... _args ) {
   if( sizeof...( _args ) != ( ROWS * COLLUMNS ) ) {
      eLOG( "Can not set a ", ROWS, "x", COLLUMNS, " matrix with ", sizeof...( _args ), " Elements!" );
      return false;
   }
   setHelper( 0, _args... );
   return true;
}

template <class TYPE, int ROWS, int COLLUMNS>
void rMatrix<TYPE, ROWS, COLLUMNS>::set( TYPE *_matrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      vMatrix[i] = _matrix[i];
}

template <class TYPE, int ROWS, int COLLUMNS>
template<class... ARGS>
inline void rMatrix<TYPE, ROWS, COLLUMNS>::setHelper( const uint16_t _pos, const TYPE &_arg, const ARGS &... _args ) {
   vMatrix[_pos] = _arg;
   setHelper( _pos + 1, _args... );
}

template <class TYPE, int ROWS, int COLLUMNS>
inline void rMatrix<TYPE, ROWS, COLLUMNS>::setHelper( const uint16_t _pos, const TYPE &_arg ) {
   vMatrix[_pos] = _arg;
}



}

#endif // RMATRIX_H
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

