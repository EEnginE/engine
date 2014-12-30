/*!
 * \file rLoaderBase.hpp
 */

#ifndef R_LOADER_BASE_HPP
#define R_LOADER_BASE_HPP

#include "defines.hpp"

#include <GL/glew.h>
#include <vector>
#include <list>
#include <string>
#include <type_traits>
#include "uLog.hpp"

namespace e_engine {

namespace internal {

template <class T, class I>
struct _3D_Data_RAW {
   std::vector<T> vVertexData;
   std::vector<T> vUVData;
   std::vector<T> vNormalesData;

   std::vector<I> vIndexVertexData;
   std::vector<I> vIndexUVData;
   std::vector<I> vIndexNormalData;

   void clear() {
      vVertexData.clear();
      vVertexData.resize( 0 );
      vUVData.clear();
      vUVData.resize( 0 );
      vNormalesData.clear();
      vNormalesData.resize( 0 );
      vIndexVertexData.clear();
      vIndexVertexData.resize( 0 );
      vIndexUVData.clear();
      vIndexUVData.resize( 0 );
      vIndexNormalData.clear();
      vIndexNormalData.resize( 0 );
   }
};

template <class T, class I>
struct _3D_Data {
   std::vector<T> vVertexData;
   std::vector<T> vUVData;
   std::vector<T> vNormalesData;

   std::vector<I> vIndex;

   void clear() {
      vVertexData.clear();
      vVertexData.resize( 0 );
      vUVData.clear();
      vUVData.resize( 0 );
      vNormalesData.clear();
      vNormalesData.resize( 0 );
      vIndex.clear();
      vIndex.resize( 0 );
   }
};

typedef _3D_Data_RAW<GLfloat, GLuint> _3D_Data_RAWF;
typedef _3D_Data_RAW<GLdouble, GLuint> _3D_Data_RAWD;

template <class T, class I>
class rLoaderBase {
   static_assert( std::is_floating_point<T>::value, "T must be a floating point type" );
   static_assert( std::is_unsigned<I>::value, "I must be an unsigned type" );

 private:
   struct __indexFinder__ {
      I i1;
      I i2;

      I index;

      __indexFinder__( I _i1, I _i2, I _index ) : i1( _i1 ), i2( _i2 ), index( _index ) {}
   };

   template <I VERT, I S2>
   void reindex2( std::vector<T> *_vertIn,
                  std::vector<T> *_vertOut,
                  std::vector<T> *_2ndIn,
                  std::vector<T> *_2ndOut,
                  std::vector<I> *_indexVert,
                  std::vector<I> *_index2nd,
                  std::vector<I> *_indexOut );

   template <I VERT, I S2, I S3>
   void reindex3( std::vector<T> *_vertIn,
                  std::vector<T> *_vertOut,
                  std::vector<T> *_2ndIn,
                  std::vector<T> *_2ndOut,
                  std::vector<T> *_3rdIn,
                  std::vector<T> *_3rdOut,
                  std::vector<I> *_indexVert,
                  std::vector<I> *_index2nd,
                  std::vector<I> *_index3rd,
                  std::vector<I> *_indexOut );

 protected:
   _3D_Data_RAW<T, I> vDataRaw;
   _3D_Data<T, I> vData;

   bool vIsDataLoaded_B;
   std::string vFilePath_str;

 public:
   virtual ~rLoaderBase() {}

   _3D_Data<T, I> *getData();

   void setFile( std::string _file );

   bool getIsLoaded() const;
   std::string getFilePath() const;

   void reindex();

   virtual int load() = 0;
   void unLoad();
};


/*!
 * \brief Clears the memory
 * \returns Nothing
 */
template <class T, class I>
void rLoaderBase<T, I>::unLoad() {
   vIsDataLoaded_B = false;
   vDataRaw.clear();
   vData.clear();
}

/*!
 * \brief Gets wether or not the file is loaded and parsed
 * \returns The state of the file being loaded and parsed
 */
template <class T, class I>
bool rLoaderBase<T, I>::getIsLoaded() const {
   return vIsDataLoaded_B;
}

/*!
 * \brief Gets the path of the file to parse
 * \returns The path of the file to parse
 */
template <class T, class I>
std::string rLoaderBase<T, I>::getFilePath() const {
   return vFilePath_str;
}


/*!
 * \brief Sets the file to load
 * \param[in] _file The file to load
 *
 * \note This will NOT load the file! You have to manually load it with load()
 *
 * \returns Nothing
 */
template <class T, class I>
void rLoaderBase<T, I>::setFile( std::string _file ) {
   vFilePath_str = _file;
}

/*!
 * \brief Gets the data pointer
 * \returns The data pointer
 */
template <class T, class I>
_3D_Data<T, I> *rLoaderBase<T, I>::getData() {
   return &vData;
}


template <class T, class I>
void rLoaderBase<T, I>::reindex() {
   // Nothing to reindex
   if ( vDataRaw.vUVData.empty() && vDataRaw.vNormalesData.empty() ) {
      vData.vVertexData = std::move( vDataRaw.vVertexData );
      vData.vIndex = std::move( vDataRaw.vIndexVertexData );
      vDataRaw.clear();
      return;
   } else if ( vDataRaw.vUVData.empty() && !vDataRaw.vNormalesData.empty() ) {
      reindex2<3, 3>( &vDataRaw.vVertexData,
                      &vData.vVertexData,
                      &vDataRaw.vNormalesData,
                      &vData.vNormalesData,
                      &vDataRaw.vIndexVertexData,
                      &vDataRaw.vIndexNormalData,
                      &vData.vIndex );
      vDataRaw.clear();
      return;
   } else if ( !vDataRaw.vUVData.empty() && vDataRaw.vNormalesData.empty() ) {
      reindex2<3, 2>( &vDataRaw.vVertexData,
                      &vData.vVertexData,
                      &vDataRaw.vUVData,
                      &vData.vUVData,
                      &vDataRaw.vIndexVertexData,
                      &vDataRaw.vIndexUVData,
                      &vData.vIndex );
      vDataRaw.clear();
      return;
   } else if ( !vDataRaw.vUVData.empty() && !vDataRaw.vNormalesData.empty() ) {
      reindex3<3, 3, 2>( &vDataRaw.vVertexData,
                         &vData.vVertexData,
                         &vDataRaw.vNormalesData,
                         &vData.vNormalesData,
                         &vDataRaw.vUVData,
                         &vData.vUVData,
                         &vDataRaw.vIndexVertexData,
                         &vDataRaw.vIndexNormalData,
                         &vDataRaw.vIndexUVData,
                         &vData.vIndex );
      vDataRaw.clear();
      return;
   }

   vData.vVertexData = std::move( vDataRaw.vVertexData );
   vData.vNormalesData = std::move( vDataRaw.vNormalesData );
   vData.vIndex = std::move( vDataRaw.vIndexVertexData );
   vDataRaw.clear();
}

template <class T, class I>
template <I VERT, I S2>
void rLoaderBase<T, I>::reindex2( std::vector<T> *_vertIn,
                                  std::vector<T> *_vertOut,
                                  std::vector<T> *_2ndIn,
                                  std::vector<T> *_2ndOut,
                                  std::vector<I> *_indexVert,
                                  std::vector<I> *_index2nd,
                                  std::vector<I> *_indexOut ) {

   std::vector<std::list<__indexFinder__>> lHelper;
   lHelper.resize( _vertIn->size() );

   I lCurrentIndex = 0, lIV, lI2;
   bool lFoundIndex;

   if ( _index2nd->size() != _indexVert->size() ) {
      eLOG( "Invalid object -- not the same number of indexes '", vFilePath_str, "'" );
      return;
   }

   for ( size_t i = 0; i < _indexVert->size(); ++i ) {
      lFoundIndex = false;
      lIV = ( *_indexVert )[i];
      lI2 = ( *_index2nd )[i];

      // Check if it is save to access
      if ( lIV * VERT >= _vertIn->size() || lI2 * S2 >= _2ndIn->size() ) {
         eLOG( "Index out of range error! Invalid object! '", vFilePath_str, "'" );
         continue;
      }

      for ( auto const &i : lHelper[lIV] ) {
         if ( i.i1 == lI2 ) {
            _indexOut->emplace_back( i.index );
            lFoundIndex = true;
            break;
         }
      }

      if ( lFoundIndex )
         continue;

      for ( size_t j = 0; j < VERT; ++j )
         _vertOut->emplace_back( ( *_vertIn )[lIV * VERT + j] );

      for ( size_t j = 0; j < S2; ++j )
         _2ndOut->emplace_back( ( *_2ndIn )[lI2 * S2 + j] );

      _indexOut->emplace_back( lCurrentIndex );
      lHelper[lIV].emplace_back( lI2, 0, lCurrentIndex );
      ++lCurrentIndex;
   }
}


template <class T, class I>
template <I VERT, I S2, I S3>
void rLoaderBase<T, I>::reindex3( std::vector<T> *_vertIn,
                                  std::vector<T> *_vertOut,
                                  std::vector<T> *_2ndIn,
                                  std::vector<T> *_2ndOut,
                                  std::vector<T> *_3rdIn,
                                  std::vector<T> *_3rdOut,
                                  std::vector<I> *_indexVert,
                                  std::vector<I> *_index2nd,
                                  std::vector<I> *_index3rd,
                                  std::vector<I> *_indexOut ) {

   std::vector<std::list<__indexFinder__>> lHelper;
   lHelper.resize( _vertIn->size() );

   I lCurrentIndex = 0, lIV, lI2, lI3;
   bool lFoundIndex;

   if ( _index2nd->size() != _indexVert->size() || _index3rd->size() != _indexVert->size() ) {
      eLOG( "Invalid object -- not the same number of indexes '", vFilePath_str, "'" );
      return;
   }

   for ( size_t i = 0; i < _indexVert->size(); ++i ) {
      lFoundIndex = false;
      lIV = ( *_indexVert )[i];
      lI2 = ( *_index2nd )[i];
      lI3 = ( *_index3rd )[i];

      // Check if it is save to access
      if ( lIV * VERT >= _vertIn->size() || lI2 * S2 >= _2ndIn->size() ||
           lI3 * S3 >= _3rdIn->size() ) {
         eLOG( "Index out of range error! Invalid object! '", vFilePath_str, "'" );
         continue;
      }

      for ( auto const &i : lHelper[lIV] ) {
         if ( i.i1 == lI2 && i.i2 == lI3 ) {
            _indexOut->emplace_back( i.index );
            lFoundIndex = true;
            break;
         }
      }

      if ( lFoundIndex )
         continue;

      for ( size_t j = 0; j < VERT; ++j )
         _vertOut->emplace_back( ( *_vertIn )[lIV * VERT + j] );

      for ( size_t j = 0; j < S2; ++j )
         _2ndOut->emplace_back( ( *_2ndIn )[lI2 * S2 + j] );

      for ( size_t j = 0; j < S3; ++j )
         _3rdOut->emplace_back( ( *_3rdIn )[lI3 * S3 + j] );

      _indexOut->emplace_back( lCurrentIndex );
      lHelper[lIV].emplace_back( lI2, lI3, lCurrentIndex );
      ++lCurrentIndex;
   }
}
}
}


#endif // R_LOADER_BASE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
