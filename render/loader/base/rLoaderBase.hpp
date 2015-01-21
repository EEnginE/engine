/*!
 * \file rLoaderBase.hpp
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

#ifndef R_LOADER_BASE_HPP
#define R_LOADER_BASE_HPP

#include "defines.hpp"

#include <GL/glew.h>
#include <vector>
#include <list>
#include <string>
#include <type_traits>
#include "uLog.hpp"
#include "uParserHelper.hpp"

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
};

template <class T, class I>
struct _3D_Data {
   std::vector<T> vVertexData;
   std::vector<T> vUVData;
   std::vector<T> vNormalesData;

   std::vector<I> vIndex;

   std::string vName;
};

typedef _3D_Data_RAW<GLfloat, GLuint> _3D_Data_RAWF;
typedef _3D_Data_RAW<GLdouble, GLuint> _3D_Data_RAWD;

typedef _3D_Data<GLfloat, GLuint> _3D_DataF;
typedef _3D_Data<GLdouble, GLuint> _3D_DataD;

template <class T, class I>
class rLoaderBase : public uParserHelper {
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
   std::vector<_3D_Data<T, I>> vData;

   void reindex( _3D_Data_RAW<T, I> &_data );

 public:
   virtual ~rLoaderBase() {}
   rLoaderBase() {}
   rLoaderBase( std::string _file ) : uParserHelper( _file ) {}

   template <class C, class... ARGS>
   void generateObjects( std::vector<C> &_output, ARGS &&... _args );

   std::vector<_3D_Data<T, I>> *getData();

   void unLoad();
};

template <class T, class I>
template <class C, class... ARGS>
void rLoaderBase<T, I>::generateObjects( std::vector<C> &_output, ARGS &&... _args ) {
   for ( auto const &d : vData ) {
      _output.emplace_back( std::forward<ARGS>( _args )..., d.vName, d );
   }
}


/*!
 * \brief Clears the memory
 */
template <class T, class I>
void rLoaderBase<T, I>::unLoad() {
   vIsParsed = false;
   vData.clear();
}


/*!
 * \brief Gets the data pointer
 * \returns The data pointer
 */
template <class T, class I>
std::vector<_3D_Data<T, I>> *rLoaderBase<T, I>::getData() {
   return &vData;
}


template <class T, class I>
void rLoaderBase<T, I>::reindex( _3D_Data_RAW<T, I> &_data ) {
   // Nothing to reindex
   if ( _data.vVertexData.empty() && _data.vNormalesData.empty() && _data.vUVData.empty() )
      return;

   vData.emplace_back();

   if ( _data.vUVData.empty() && _data.vNormalesData.empty() ) {
      vData.back().vVertexData = std::move( _data.vVertexData );
      vData.back().vIndex = std::move( _data.vIndexVertexData );
      return;
   } else if ( _data.vUVData.empty() && !_data.vNormalesData.empty() ) {
      reindex2<3, 3>( &_data.vVertexData,
                      &vData.back().vVertexData,
                      &_data.vNormalesData,
                      &vData.back().vNormalesData,
                      &_data.vIndexVertexData,
                      &_data.vIndexNormalData,
                      &vData.back().vIndex );
      return;
   } else if ( !_data.vUVData.empty() && _data.vNormalesData.empty() ) {
      reindex2<3, 2>( &_data.vVertexData,
                      &vData.back().vVertexData,
                      &_data.vUVData,
                      &vData.back().vUVData,
                      &_data.vIndexVertexData,
                      &_data.vIndexUVData,
                      &vData.back().vIndex );
      return;
   } else if ( !_data.vUVData.empty() && !_data.vNormalesData.empty() ) {
      reindex3<3, 3, 2>( &_data.vVertexData,
                         &vData.back().vVertexData,
                         &_data.vNormalesData,
                         &vData.back().vNormalesData,
                         &_data.vUVData,
                         &vData.back().vUVData,
                         &_data.vIndexVertexData,
                         &_data.vIndexNormalData,
                         &_data.vIndexUVData,
                         &vData.back().vIndex );
      return;
   }

   vData.back().vVertexData = std::move( _data.vVertexData );
   vData.back().vNormalesData = std::move( _data.vNormalesData );
   vData.back().vIndex = std::move( _data.vIndexVertexData );
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

      for ( auto const &j : lHelper[lIV] ) {
         if ( j.i1 == lI2 ) {
            _indexOut->emplace_back( j.index );
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

      for ( auto const &j : lHelper[lIV] ) {
         if ( j.i1 == lI2 && j.i2 == lI3 ) {
            _indexOut->emplace_back( j.index );
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
