/*!
 * \file rLoaderStructs.hpp
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

#ifndef R_LOADER_STRUCTS_HPP
#define R_LOADER_STRUCTS_HPP

#include "defines.hpp"
#include <vector>
#include <list>
#include <string>
#include <GL/glew.h>
#include "uLog.hpp"

namespace e_engine {
namespace internal {

/*!
 * \brief Data structure for raw, seperately indexed data
 */
template <class T, class I>
struct _3D_Data_RAW final {
   std::vector<T> vVertexData;
   std::vector<T> vUVData;
   std::vector<T> vNormalesData;

   std::vector<I> vIndexVertexData;
   std::vector<I> vIndexUVData;
   std::vector<I> vIndexNormalData;
};

/*!
 * \brief Data structure for raw, correctly indexed data
 */
template <class T, class I>
struct _3D_Data final {
   std::vector<T> vVertexData;
   std::vector<T> vUVData;
   std::vector<T> vNormalesData;

   std::vector<I> vIndex;

   unsigned int vDimensions = 3;

   std::string vName;
};

/*!
 * \brief Main data structure for OpenGL data. Already formated for buffer usage
 */
template <class T, class I>
struct _3D_Engine final {
   enum TYPE { POSITION, NORMAL, UV };

   struct __obj final {
      struct __accessor final {
         TYPE vType;
         unsigned char num = 3;
         unsigned int vOffset = 0;
         unsigned int vStride; //!< DATA stride

         __accessor() {}
      };

      std::vector<__accessor> vAccessor;
      unsigned int vOffset; //!< INDEX offset
      unsigned int vStride = 0;
      size_t vNumVertex;
      MESH_TYPES vType;

      __obj() {}
   };

   std::vector<__obj> vObjects; //!< Index offsets. INDEX ONLY

   std::vector<T> vData;  //!< This will be 1:1 a OGL buffer
   std::vector<I> vIndex; //!< This will be 1:1 a OGL buffer

   std::string vName;

   _3D_Engine() {}
};

typedef _3D_Data_RAW<GLfloat, GLushort> _3D_Data_RAWF;
typedef _3D_Data_RAW<GLdouble, GLushort> _3D_Data_RAWD;

typedef _3D_Data<GLfloat, GLushort> _3D_DataF;
typedef _3D_Data<GLdouble, GLushort> _3D_DataD;


//     _____                           _
//    /  __ \                         | |
//    | /  \/ ___  _ ____   _____ _ __| |_
//    | |    / _ \| '_ \ \ / / _ \ '__| __|
//    | \__/\ (_) | | | \ V /  __/ |  | |_
//     \____/\___/|_| |_|\_/ \___|_|   \__|
//

template <class T, class I, int D>
struct convert {
   static bool toEngine3D( _3D_Data<T, I> &_in, std::shared_ptr<_3D_Engine<T, I>> _out ) {
      if ( _in.vNormalesData.empty() && _in.vUVData.empty() )
         return internal::convert<T, I, D>::toEngine3D_1( _in, _out );

      if ( !_in.vNormalesData.empty() && _in.vUVData.empty() )
         return internal::convert<T, I, D>::toEngine3D_2( _in, _out );

      if ( !_in.vNormalesData.empty() && !_in.vUVData.empty() )
         return internal::convert<T, I, D>::toEngine3D_3( _in, _out );

      return false;
   }

   static bool toEngine3D_1( _3D_Data<T, I> &_in, std::shared_ptr<_3D_Engine<T, I>> _out ) {
      auto lSize = _in.vVertexData.size();

      if ( lSize % D != 0 )
         return false;

      _out->vData.resize( lSize );
      _out->vName = std::move( _in.vName );
      _out->vIndex = std::move( _in.vIndex );

      _out->vObjects.emplace_back();
      _out->vObjects[0].vAccessor.emplace_back();

      _out->vObjects[0].vAccessor[0].vType = _3D_Engine<T, I>::POSITION;
      _out->vObjects[0].vAccessor[0].num = D;
      _out->vObjects[0].vAccessor[0].vOffset = 0;
      _out->vObjects[0].vAccessor[0].vStride = sizeof( T ) * D;

      _out->vObjects[0].vNumVertex = _out->vIndex.size();
      _out->vObjects[0].vType = MESH_3D;
      _out->vObjects[0].vStride = 0;

      for ( size_t i = 0; i < lSize; ++i ) {
         _out->vData[i] = _in.vVertexData[i];
      }

      std::hash<std::vector<float>> lHash_f;
      wLOG( "DATA: ", lHash_f( _out->vData ) );

      return true;
   }

   static bool toEngine3D_2( _3D_Data<T, I> &_in, std::shared_ptr<_3D_Engine<T, I>> _out ) {
      auto lSize = _in.vVertexData.size();

      if ( lSize != _in.vNormalesData.size() )
         return false;

      if ( lSize % D != 0 )
         return false;

      _out->vData.resize( lSize * 2 );
      _out->vName = std::move( _in.vName );
      _out->vIndex = std::move( _in.vIndex );

      _out->vObjects.emplace_back();
      _out->vObjects[0].vAccessor.emplace_back();
      _out->vObjects[0].vAccessor.emplace_back();

      // Vertex
      _out->vObjects[0].vAccessor[0].vType = _3D_Engine<T, I>::POSITION;
      _out->vObjects[0].vAccessor[0].num = D;
      _out->vObjects[0].vAccessor[0].vOffset = 0;
      _out->vObjects[0].vAccessor[0].vStride = sizeof( T ) * D * 2;

      // Normal
      _out->vObjects[0].vAccessor[1].vType = _3D_Engine<T, I>::NORMAL;
      _out->vObjects[0].vAccessor[1].num = D;
      _out->vObjects[0].vAccessor[1].vOffset = D * sizeof( T );
      _out->vObjects[0].vAccessor[1].vStride = sizeof( T ) * D * 2;


      _out->vObjects[0].vNumVertex = _out->vIndex.size();
      _out->vObjects[0].vType = MESH_3D;
      _out->vObjects[0].vOffset = 0;
      _out->vObjects[0].vStride = 0;

      for ( size_t i = 0; i < ( lSize / D ); ++i ) {
         for ( unsigned int j = 0; j < D; ++j ) {
            _out->vData[i * 2 * D + j] = _in.vVertexData[i * D + j];
            _out->vData[i * 2 * D + j + D] = _in.vNormalesData[i * D + j];
         }
      }

      return true;
   }

   static bool toEngine3D_3( _3D_Data<T, I> &_in, std::shared_ptr<_3D_Engine<T, I>> _out ) {
      auto lSize = _in.vVertexData.size();

      if ( lSize != _in.vNormalesData.size() )
         return false;

      if ( lSize % D != 0 )
         return false;

      if ( _in.vUVData.size() % 2 != 0 )
         return false;

      if ( lSize / D != _in.vUVData.size() / 2 )
         return false;

      _out->vData.resize( lSize * 2 + _in.vUVData.size() );
      _out->vName = std::move( _in.vName );
      _out->vIndex = std::move( _in.vIndex );

      _out->vObjects.emplace_back();
      _out->vObjects[0].vAccessor.emplace_back();
      _out->vObjects[0].vAccessor.emplace_back();
      _out->vObjects[0].vAccessor.emplace_back();

      // Vertex
      _out->vObjects[0].vAccessor[0].vType = _3D_Engine<T, I>::POSITION;
      _out->vObjects[0].vAccessor[0].num = D;
      _out->vObjects[0].vAccessor[0].vOffset = 0;
      _out->vObjects[0].vAccessor[0].vStride = sizeof( T ) * D * 2 + sizeof( T ) * 2;

      // Mormal
      _out->vObjects[0].vAccessor[1].vType = _3D_Engine<T, I>::NORMAL;
      _out->vObjects[0].vAccessor[1].num = D;
      _out->vObjects[0].vAccessor[1].vOffset = D * sizeof( T );
      _out->vObjects[0].vAccessor[1].vStride = sizeof( T ) * D * 2 + sizeof( T ) * 2;

      // UV
      _out->vObjects[0].vAccessor[2].vType = _3D_Engine<T, I>::UV;
      _out->vObjects[0].vAccessor[2].num = 2;
      _out->vObjects[0].vAccessor[2].vOffset = D * sizeof( T ) * 2;
      _out->vObjects[0].vAccessor[2].vStride = sizeof( T ) * D * 2 + sizeof( T ) * 2;

      _out->vObjects[0].vNumVertex = _out->vIndex.size();
      _out->vObjects[0].vType = MESH_3D;
      _out->vObjects[0].vOffset = 0;
      _out->vObjects[0].vStride = 0;

      for ( size_t i = 0; i < ( lSize / D ); ++i ) {
         for ( unsigned int j = 0; j < D; ++j ) {
            _out->vData[i + j] = _in.vVertexData[i * D + j];
            _out->vData[i + j + D] = _in.vNormalesData[i * D + j];
         }

         for ( unsigned int j = 0; j < 2; ++j ) {
            _out->vData[i + j + D + D] = _in.vNormalesData[i * 2 + j];
         }
      }

      return true;
   }
};


//    ______     _           _
//    | ___ \   (_)         | |
//    | |_/ /___ _ _ __   __| | _____  __
//    |    // _ \ | '_ \ / _` |/ _ \ \/ /
//    | |\ \  __/ | | | | (_| |  __/>  <
//    \_| \_\___|_|_| |_|\__,_|\___/_/\_\
//


namespace converter {

template <class T, class I, I VERT, I S2>
void reindex2( std::vector<T> *_vertIn,
               std::vector<T> *_vertOut,
               std::vector<T> *_2ndIn,
               std::vector<T> *_2ndOut,
               std::vector<I> *_indexVert,
               std::vector<I> *_index2nd,
               std::vector<I> *_indexOut,
               std::string &_path );

template <class T, class I, I VERT, I S2, I S3>
void reindex3( std::vector<T> *_vertIn,
               std::vector<T> *_vertOut,
               std::vector<T> *_2ndIn,
               std::vector<T> *_2ndOut,
               std::vector<T> *_3rdIn,
               std::vector<T> *_3rdOut,
               std::vector<I> *_indexVert,
               std::vector<I> *_index2nd,
               std::vector<I> *_index3rd,
               std::vector<I> *_indexOut,
               std::string &_path );

template <class T, class I>
void reindex( _3D_Data_RAW<T, I> &_data, _3D_Data<T, I> &_out, std::string &_path );

template <class T, class I>
void reindex( _3D_Data_RAW<T, I> &_data, _3D_Data<T, I> &_out, std::string &_path ) {
   // Nothing to reindex
   if ( _data.vVertexData.empty() && _data.vNormalesData.empty() && _data.vUVData.empty() )
      return;

   if ( _data.vUVData.empty() && _data.vNormalesData.empty() ) {
      _out.vVertexData = std::move( _data.vVertexData );
      _out.vIndex = std::move( _data.vIndexVertexData );
      return;
   } else if ( _data.vUVData.empty() && !_data.vNormalesData.empty() ) {
      reindex2<T, I, 3, 3>( &_data.vVertexData,
                            &_out.vVertexData,
                            &_data.vNormalesData,
                            &_out.vNormalesData,
                            &_data.vIndexVertexData,
                            &_data.vIndexNormalData,
                            &_out.vIndex,
                            _path );
      return;
   } else if ( !_data.vUVData.empty() && _data.vNormalesData.empty() ) {
      reindex2<T, I, 3, 2>( &_data.vVertexData,
                            &_out.vVertexData,
                            &_data.vUVData,
                            &_out.vUVData,
                            &_data.vIndexVertexData,
                            &_data.vIndexUVData,
                            &_out.vIndex,
                            _path );
      return;
   } else if ( !_data.vUVData.empty() && !_data.vNormalesData.empty() ) {
      reindex3<T, I, 3, 3, 2>( &_data.vVertexData,
                               &_out.vVertexData,
                               &_data.vNormalesData,
                               &_out.vNormalesData,
                               &_data.vUVData,
                               &_out.vUVData,
                               &_data.vIndexVertexData,
                               &_data.vIndexNormalData,
                               &_data.vIndexUVData,
                               &_out.vIndex,
                               _path );
      return;
   }

   _out.vVertexData = std::move( _data.vVertexData );
   _out.vNormalesData = std::move( _data.vNormalesData );
   _out.vIndex = std::move( _data.vIndexVertexData );
}

template <class I>
struct __indexFinder__ {
   I i1;
   I i2;

   I index;

   __indexFinder__( I _i1, I _i2, I _index ) : i1( _i1 ), i2( _i2 ), index( _index ) {}
};

template <class T, class I, I VERT, I S2>
void reindex2( std::vector<T> *_vertIn,
               std::vector<T> *_vertOut,
               std::vector<T> *_2ndIn,
               std::vector<T> *_2ndOut,
               std::vector<I> *_indexVert,
               std::vector<I> *_index2nd,
               std::vector<I> *_indexOut,
               std::string &_path ) {

   std::vector<std::list<__indexFinder__<I>>> lHelper;
   lHelper.resize( _vertIn->size() );

   I lCurrentIndex = 0, lIV, lI2;
   bool lFoundIndex;

   if ( _index2nd->size() != _indexVert->size() ) {
      eLOG( "Invalid object -- not the same number of indexes '", _path, "'" );
      return;
   }

   for ( size_t i = 0; i < _indexVert->size(); ++i ) {
      lFoundIndex = false;
      lIV = ( *_indexVert )[i];
      lI2 = ( *_index2nd )[i];

      // Check if it is save to access
      if ( lIV * VERT >= _vertIn->size() || lI2 * S2 >= _2ndIn->size() ) {
         eLOG( "Index out of range error! Invalid object! '", _path, "'" );
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


template <class T, class I, I VERT, I S2, I S3>
void reindex3( std::vector<T> *_vertIn,
               std::vector<T> *_vertOut,
               std::vector<T> *_2ndIn,
               std::vector<T> *_2ndOut,
               std::vector<T> *_3rdIn,
               std::vector<T> *_3rdOut,
               std::vector<I> *_indexVert,
               std::vector<I> *_index2nd,
               std::vector<I> *_index3rd,
               std::vector<I> *_indexOut,
               std::string &_path ) {

   std::vector<std::list<__indexFinder__<I>>> lHelper;
   lHelper.resize( _vertIn->size() );

   I lCurrentIndex = 0, lIV, lI2, lI3;
   bool lFoundIndex;

   if ( _index2nd->size() != _indexVert->size() || _index3rd->size() != _indexVert->size() ) {
      eLOG( "Invalid object -- not the same number of indexes '", _path, "'" );
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
         eLOG( "Index out of range error! Invalid object! '", _path, "'" );
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
}

#endif // R_LOADER_STRUCTS_HPP
