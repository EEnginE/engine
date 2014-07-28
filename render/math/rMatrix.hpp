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
         if( vDelete ) delete[] vMatrix;
      }

      float &operator[]( int x ) {
         return vMatrix[x];
      }

      float &operator()( int x, int y ) {
         return get( x, y );
      }

      float       &get( int _position )                {return vMatrix[_position];}
      float       &get( int _x, int _y )               {return vMatrix[( _x * ROWS ) + _y];}
      float       *get()                               {return vMatrix;}
      float const &get( int _position ) const          {return vMatrix[_position];}
      void         set( int _position, float _newVal ) {vMatrix[_position] = _newVal;}
      void         set( int _x, int _y, float _newVal ) {vMatrix[( _x * ROWS ) + _y] = _newVal;}
      void         set( float *_matrix )               {for( int i = 0; i < ( ROWS * COLLUMNS ); ++i ) vMatrix[i] = _matrix[i];}

      int getRowSize() {return ROWS;}
      int getCollumnSize() {return COLLUMNS;}

      template <int COLLUMNS_NEW>
      void multiply( rMatrix<COLLUMNS, COLLUMNS_NEW> *_matrix, rMatrix<ROWS, COLLUMNS_NEW> *_targetMatrix );

      void add( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix );

      void subtract( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix );


      /*!
       * The template parameters are changed due to a conflict with the already existing templates.
       */
      template <int R, int C, int C_N >
      friend rMatrix<R, C_N> operator*( rMatrix<R, C> _lMatrix, rMatrix<C, C_N> &_rMatrix );


      rMatrix<ROWS, COLLUMNS> &operator=( rMatrix<ROWS, COLLUMNS> _newMatrix ) {
         _newMatrix.vDelete = false;
         for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
            vMatrix[i] = _newMatrix.get( i );
         return *this;
      }

      rMatrix<ROWS, COLLUMNS> operator+( rMatrix<ROWS, COLLUMNS> _rMatrix ) {
         rMatrix<ROWS, COLLUMNS> lTarget;
         lTarget.vDelete  = false;
         _rMatrix.vDelete = false;
         add( &_rMatrix, &lTarget );
         return lTarget;
      }

      rMatrix<ROWS, COLLUMNS> operator-( rMatrix<ROWS, COLLUMNS> _rMatrix ) {
         rMatrix<ROWS, COLLUMNS> lTarget;
         lTarget.vDelete  = false;
         _rMatrix.vDelete = false;
         subtract( &_rMatrix, &lTarget );
         return lTarget;
      }






};


template <int ROWS, int COLLUMNS, int COLLUMNS_NEW>
rMatrix<ROWS, COLLUMNS_NEW> operator*( rMatrix<ROWS, COLLUMNS> _lMatrix, rMatrix<COLLUMNS, COLLUMNS_NEW> &_rMatrix ) {
   rMatrix<ROWS, COLLUMNS_NEW> lTarget;
   lTarget.vDelete  = false;
   _lMatrix.vDelete = false;
   _lMatrix.multiply( &_rMatrix, &lTarget );
   return lTarget;
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
   for( int i = 0; i < COLLUMNS_NEW; ++i )  //Second Matrix
      for( int j = 0; j < ROWS; ++j ) {  // First Matrix
         for( int k = 0; k < COLLUMNS; ++k )  //Both Matrices, this calculates the sum given to the target Matrix
            currentSum += get( j, k ) * _matrix->get( k, i );
         _targetMatrix->set( currentIndex, currentSum );
         ++currentIndex;
         currentSum = 0;
      }
}

template <int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::add( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vMatrix[i] + _matrix->get( i ) ) );
}

template <int ROWS, int COLLUMNS>
void rMatrix<ROWS, COLLUMNS>::subtract( rMatrix<ROWS, COLLUMNS> *_matrix, rMatrix<ROWS, COLLUMNS> *_targetMatrix ) {
   for( int i = 0; i < ( ROWS * COLLUMNS ); ++i )
      _targetMatrix->set( i, ( vMatrix[i] - _matrix->get( i ) ) );
}



}

#endif // RMATRIX_H
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
