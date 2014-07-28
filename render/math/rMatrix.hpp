#ifndef RMATRIX_H
#define RMATRIX_H

#include "uLog.hpp"

namespace e_engine {

template <int ROWS, int COLLUMNS>
class rMatrix {
   private:
      float *vMatrix;
      bool vDelete;
   public:
      rMatrix() {
         vMatrix = new float[ROWS * COLLUMNS];
         vDelete = true;
      }

//       rMatrix( const rMatrix<ROWS, COLLUMNS> &_newMatrix) {
//    for(int i = 0; i < (ROWS*COLLUMNS); ++i)
//     vMatrix[i] = _newMatrix.get()[i];
//       }

      ~rMatrix() {
         if ( vDelete ) delete[] vMatrix;
      }

      float &operator[]( int x ) {
         return vMatrix[x];
      }

      float &operator()( int x, int y ) {
         return get( x, y );
      }

      float       &get( int _position )                 {return vMatrix[_position];}
      float       &get( int _x, int _y )                {return vMatrix[( _x * ROWS ) + _y];}
      float       *get()                                {return vMatrix;}
      float const &get( int _position ) const           {return vMatrix[_position];}
      void         set( int _position, float _newVal  ) {vMatrix[_position] = _newVal;}
      void         set( int _x, int _y, float _newVal ) {vMatrix[( _x * ROWS ) + _y] = _newVal;}
      void         set( float *_matrix )                {for ( int i = 0; i < ( ROWS * COLLUMNS ); ++i ) vMatrix[i] = _matrix[i];}

      int getRowSize() {return ROWS;}
      int getCollumnSize() {return COLLUMNS;}

      void toIdentityMatrix();
      void toZeroMatrix();


      template <int COLLUMNS_NEW>
      void multiply( rMatrix<COLLUMNS, COLLUMNS_NEW> *_matrix, rMatrix<ROWS, COLLUMNS_NEW> *_targetMatrix );

      //Hardcoded multiply methods
      void multiply( rMatrix<2, 2> *_matrix, rMatrix<2, 2> *_targetMatrix );
      void multiply( rMatrix<3, 3> *_matrix, rMatrix<3, 3> *_targetMatrix );
      void multiply( rMatrix<4, 4> *_matrix, rMatrix<4, 4> *_targetMatrix );

      void add( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix );

      void subtract( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix );


      /*!
       * The template parameters are changed due to a conflict with the already existing templates.
       */
      template <int R, int C, int C_N >
      friend rMatrix<R, C_N> operator*( rMatrix<R, C> _lMatrix, rMatrix<C, C_N> &_rMatrix );

      rMatrix<ROWS, COLLUMNS> &operator=( rMatrix<ROWS, COLLUMNS> _newMatrix ) {
         _newMatrix.vDelete = false;
         for ( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
            vMatrix[i] = _newMatrix.get( i );
         return *this;
      }

      rMatrix<ROWS, COLLUMNS> operator+( rMatrix<ROWS, COLLUMNS> _rMatrix ) {
         _rMatrix.vDelete = false;
         rMatrix<ROWS, COLLUMNS> _target;
         _target.vDelete = false;
         add( &_rMatrix, &_target );
         return _target;
      }

      rMatrix<ROWS, COLLUMNS> operator-( rMatrix<ROWS, COLLUMNS> _rMatrix ) {
         _rMatrix.vDelete = false;
         rMatrix<ROWS, COLLUMNS> _target;
         _target.vDelete = false;
         subtract( &_rMatrix, &_target );
         return _target;
      }






};



template <int ROWS, int COLLUMNS, int COLLUMNS_NEW>
rMatrix<ROWS, COLLUMNS_NEW> operator*( rMatrix<ROWS, COLLUMNS> _lMatrix, rMatrix<COLLUMNS, COLLUMNS_NEW> &_rMatrix ) {
   rMatrix<ROWS, COLLUMNS_NEW> _target;
   _target.vDelete  = false;
   _lMatrix.vDelete = false;
   _lMatrix.multiply( &_rMatrix, &_target );
   return _target;
}

template<int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::toIdentityMatrix() {
   if ( ROWS != COLLUMNS ) {
      toZeroMatrix();
      return;
   }
   vMatrix[0] = 1;
   for ( int i = 1; i < ROWS * ROWS; ++i )
      if ( ( i % ( ROWS + 1 ) ) == 0 )
         vMatrix[i] = 1;
      else
         vMatrix[i] = 0;
}

template<int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::toZeroMatrix() {
   for ( int i = 1; i < ROWS * ROWS; ++i )
      vMatrix[i] = 0;
}

/*! The target matrix will
      *   have the row-count of the initial Matrix
      *   and the collumn-count of the second Matrix.
      *
      *   The second matrix needs to have a row-count
      *   equal to the collumn-count of the initial matrix.
      */

template <int ROWS, int COLLUMNS>
template <int COLLUMNS_NEW>
void rMatrix<ROWS, COLLUMNS>::multiply( rMatrix<COLLUMNS, COLLUMNS_NEW> *_matrix, rMatrix<ROWS, COLLUMNS_NEW> *_targetMatrix ) {
   int currentIndex = 0;
   float currentSum = 0;
   for ( int i = 0; i < COLLUMNS_NEW; ++i ) //Second Matrix
      for ( int j = 0; j < ROWS; ++j ) { // First Matrix
         for ( int k = 0; k < COLLUMNS; ++k ) //Both Matrices, this calculates the sum given to the target Matrix
            currentSum += get( j, k ) * _matrix->get( k, i );
         _targetMatrix->set( currentIndex, currentSum );
         ++currentIndex;
         currentSum = 0;
      }
}

//HARDCODED 2x2
template<int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::multiply( rMatrix<2, 2> *_matrix, rMatrix<2, 2> *_targetMatrix ) {
   _targetMatrix->vMatrix[0] = ( ( vMatrix[0] * _matrix->vMatrix[0] ) + ( vMatrix[4] * _matrix->vMatrix[1] ) );
   _targetMatrix->vMatrix[1] = ( ( vMatrix[1] * _matrix->vMatrix[0] ) + ( vMatrix[5] * _matrix->vMatrix[1] ) );
   _targetMatrix->vMatrix[2] = ( ( vMatrix[0] * _matrix->vMatrix[2] ) + ( vMatrix[4] * _matrix->vMatrix[3] ) );
   _targetMatrix->vMatrix[3] = ( ( vMatrix[1] * _matrix->vMatrix[2] ) + ( vMatrix[5] * _matrix->vMatrix[3] ) );
}


//HARDCODED 3x3
template<int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::multiply( rMatrix<3, 3> *_matrix, rMatrix<3, 3> *_targetMatrix ) {
   _targetMatrix->vMatrix[0] = ( ( vMatrix[0] * _matrix->vMatrix[0] ) + ( vMatrix[4] * _matrix->vMatrix[1] ) + ( vMatrix[8] * _matrix->vMatrix[2] ) )  ;
   _targetMatrix->vMatrix[1] = ( ( vMatrix[1] * _matrix->vMatrix[0] ) + ( vMatrix[5] * _matrix->vMatrix[1] ) + ( vMatrix[9] * _matrix->vMatrix[2] ) )  ;
   _targetMatrix->vMatrix[2] = ( ( vMatrix[2] * _matrix->vMatrix[0] ) + ( vMatrix[6] * _matrix->vMatrix[1] ) + ( vMatrix[10] * _matrix->vMatrix[2] ) ) ;
   _targetMatrix->vMatrix[3] = ( ( vMatrix[0] * _matrix->vMatrix[3] ) + ( vMatrix[7] * _matrix->vMatrix[4] ) + ( vMatrix[11] * _matrix->vMatrix[5] ) ) ;
   _targetMatrix->vMatrix[4] = ( ( vMatrix[1] * _matrix->vMatrix[3] ) + ( vMatrix[4] * _matrix->vMatrix[4] ) + ( vMatrix[8] * _matrix->vMatrix[5] ) )  ;
   _targetMatrix->vMatrix[5] = ( ( vMatrix[2] * _matrix->vMatrix[3] ) + ( vMatrix[5] * _matrix->vMatrix[4] ) + ( vMatrix[9] * _matrix->vMatrix[5] ) )  ;
   _targetMatrix->vMatrix[6] = ( ( vMatrix[0] * _matrix->vMatrix[6] ) + ( vMatrix[6] * _matrix->vMatrix[7] ) + ( vMatrix[10] * _matrix->vMatrix[8] ) ) ;
   _targetMatrix->vMatrix[7] = ( ( vMatrix[1] * _matrix->vMatrix[6] ) + ( vMatrix[7] * _matrix->vMatrix[7] ) + ( vMatrix[11] * _matrix->vMatrix[8] ) ) ;
   _targetMatrix->vMatrix[8] = ( ( vMatrix[2] * _matrix->vMatrix[6] ) + ( vMatrix[4] * _matrix->vMatrix[7] ) + ( vMatrix[8] * _matrix->vMatrix[8] ) )  ;
}


//HARDCODED 4x4
template<int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::multiply( rMatrix<4, 4> *_matrix, rMatrix<4, 4> *_targetMatrix ) {
   _targetMatrix->vMatrix[0] = ( ( vMatrix[0] * _matrix->vMatrix[0] ) + ( vMatrix[4] * _matrix->vMatrix[1] ) + ( vMatrix[8] * _matrix->vMatrix[2] ) + ( vMatrix[12] * _matrix->vMatrix[3] ) );
   _targetMatrix->vMatrix[1] = ( ( vMatrix[1] * _matrix->vMatrix[0] ) + ( vMatrix[5] * _matrix->vMatrix[1] ) + ( vMatrix[9] * _matrix->vMatrix[2] ) + ( vMatrix[13] * _matrix->vMatrix[3] ) )  ;
   _targetMatrix->vMatrix[2] = ( ( vMatrix[2] * _matrix->vMatrix[0] ) + ( vMatrix[6] * _matrix->vMatrix[1] ) + ( vMatrix[10] * _matrix->vMatrix[2] ) + ( vMatrix[14] * _matrix->vMatrix[3] ) ) ;
   _targetMatrix->vMatrix[3] = ( ( vMatrix[3] * _matrix->vMatrix[0] ) + ( vMatrix[7] * _matrix->vMatrix[1] ) + ( vMatrix[11] * _matrix->vMatrix[2] ) + ( vMatrix[15] * _matrix->vMatrix[3] ) ) ;
   _targetMatrix->vMatrix[4] = ( ( vMatrix[0] * _matrix->vMatrix[4] ) + ( vMatrix[4] * _matrix->vMatrix[5] ) + ( vMatrix[8] * _matrix->vMatrix[6] ) + ( vMatrix[12] * _matrix->vMatrix[7] ) )  ;
   _targetMatrix->vMatrix[5] = ( ( vMatrix[1] * _matrix->vMatrix[4] ) + ( vMatrix[5] * _matrix->vMatrix[5] ) + ( vMatrix[9] * _matrix->vMatrix[6] ) + ( vMatrix[13] * _matrix->vMatrix[7] ) )  ;
   _targetMatrix->vMatrix[6] = ( ( vMatrix[2] * _matrix->vMatrix[4] ) + ( vMatrix[6] * _matrix->vMatrix[5] ) + ( vMatrix[10] * _matrix->vMatrix[6] ) + ( vMatrix[14] * _matrix->vMatrix[7] ) ) ;
   _targetMatrix->vMatrix[7] = ( ( vMatrix[3] * _matrix->vMatrix[4] ) + ( vMatrix[7] * _matrix->vMatrix[5] ) + ( vMatrix[11] * _matrix->vMatrix[6] ) + ( vMatrix[15] * _matrix->vMatrix[7] ) ) ;
   _targetMatrix->vMatrix[8] = ( ( vMatrix[0] * _matrix->vMatrix[8] ) + ( vMatrix[4] * _matrix->vMatrix[9] ) + ( vMatrix[8] * _matrix->vMatrix[10] ) + ( vMatrix[12] * _matrix->vMatrix[11] ) )  ;
   _targetMatrix->vMatrix[9] = ( ( vMatrix[1] * _matrix->vMatrix[8] ) + ( vMatrix[5] * _matrix->vMatrix[9] ) + ( vMatrix[9] * _matrix->vMatrix[10] ) + ( vMatrix[13] * _matrix->vMatrix[11] ) )  ;
   _targetMatrix->vMatrix[10] = ( ( vMatrix[2] * _matrix->vMatrix[8] ) + ( vMatrix[6] * _matrix->vMatrix[9] ) + ( vMatrix[10] * _matrix->vMatrix[10] ) + ( vMatrix[14] * _matrix->vMatrix[11] ) ) ;
   _targetMatrix->vMatrix[11] = ( ( vMatrix[3] * _matrix->vMatrix[8] ) + ( vMatrix[7] * _matrix->vMatrix[9] ) + ( vMatrix[11] * _matrix->vMatrix[10] ) + ( vMatrix[15] * _matrix->vMatrix[11] ) ) ;
   _targetMatrix->vMatrix[12] = ( ( vMatrix[0] * _matrix->vMatrix[12] ) + ( vMatrix[4] * _matrix->vMatrix[13] ) + ( vMatrix[8] * _matrix->vMatrix[14] ) + ( vMatrix[12] * _matrix->vMatrix[15] ) ) ;
   _targetMatrix->vMatrix[13] = ( ( vMatrix[1] * _matrix->vMatrix[12] ) + ( vMatrix[5] * _matrix->vMatrix[13] ) + ( vMatrix[9] * _matrix->vMatrix[14] ) + ( vMatrix[13] * _matrix->vMatrix[15] ) ) ;
   _targetMatrix->vMatrix[14] = ( ( vMatrix[2] * _matrix->vMatrix[12] ) + ( vMatrix[6] * _matrix->vMatrix[13] ) + ( vMatrix[10] * _matrix->vMatrix[14] ) + ( vMatrix[14] * _matrix->vMatrix[15] ) ) ;
   _targetMatrix->vMatrix[15] = ( ( vMatrix[3] * _matrix->vMatrix[12] ) + ( vMatrix[7] * _matrix->vMatrix[13] ) + ( vMatrix[11] * _matrix->vMatrix[14] ) + ( vMatrix[15] * _matrix->vMatrix[15] ) ) ;
}

template <int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::add( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix ) {
   for ( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vMatrix[i] + _matrix->get( i ) ) );
}

template <int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::subtract( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix ) {
   for ( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vMatrix[i] - _matrix->get( i ) ) );
}



}

#endif // RMATRIX_H
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
