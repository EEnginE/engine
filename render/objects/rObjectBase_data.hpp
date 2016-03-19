/*!
 * \file rObjectBase_data.hpp
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

#pragma once

#include "defines.hpp"

#include "lLoaderBase.hpp"
#include "rObjectBase.hpp"
#include "lLoaderStructs.hpp"
#include <memory>

#ifdef _OBJ
#error "_OBJ is already defined"
#endif

#ifdef _CALC
#error "_CALC is already defined"
#endif

#define _OBJ( x ) rObjectBase::x
#define _CALC( x ) _OBJ( x ) - _OBJ( __LAST__ )

namespace e_engine {

template <class T, class I>
class rObjectBase_data {
 public:
   using DATA = std::shared_ptr<internal::_3D_Engine<T, I>>;

 protected:
   DATA vData; //!< The stored data.

 public:
   rObjectBase_data(
         DATA _data,
         std::array<int64_t, rObjectBase::__LAST__> &_hints,
         std::vector<std::array<int64_t, rObjectBase::__LAST_OBJECT__ - rObjectBase::__LAST__>>
               &_objHints )
       : vData( _data ) {

      _hints[_OBJ( NUM_MESHS )] = 0;
      _hints[_OBJ( NUM_LINES )] = 0;
      _hints[_OBJ( NUM_POINTS )] = 0;

      for ( auto const &i : vData->vObjects ) {
         _objHints.emplace_back();

         for ( auto &j : _objHints.back() )
            j = -1;

         _objHints.back().at( _CALC( INDEX_STRIDE ) ) = i.vStride;
         _objHints.back().at( _CALC( INDEX_OFFSET ) ) = i.vOffset;
         _objHints.back().at( _CALC( NUM_INDEXES ) ) = static_cast<int64_t>( i.vNumVertex );
         _objHints.back().at( _CALC( MESH_TYPE ) ) = i.vType;

         switch ( i.vType ) {
            case MESH_3D: _hints[_OBJ( NUM_MESHS )]++; break;
            case LINES_3D: _hints[_OBJ( NUM_LINES )]++; break;
            case POINTS_3D: _hints[_OBJ( NUM_POINTS )]++; break;
         }

         using DATA_T = internal::_3D_Engine<T, I>;

         for ( auto const &j : i.vAccessor ) {
            switch ( j.vType ) {
               case DATA_T::POSITION:
                  _objHints.back().at( _CALC( VERTEX_OFFSET ) ) = j.vOffset;
                  _objHints.back().at( _CALC( VERTEX_STRIDE ) ) = j.vStride;
                  break;
               case DATA_T::NORMAL:
                  _objHints.back().at( _CALC( NORMAL_OFFSET ) ) = j.vOffset;
                  _objHints.back().at( _CALC( NORMAL_STRIDE ) ) = j.vStride;
                  break;
               case DATA_T::UV:
                  _objHints.back().at( _CALC( UV_OFFSET ) ) = j.vOffset;
                  _objHints.back().at( _CALC( UV_STRIDE ) ) = j.vStride;
                  break;
            }
         }
      }
   }
   rObjectBase_data() = delete;
};
}

#undef _OBJ
#undef _CALC
