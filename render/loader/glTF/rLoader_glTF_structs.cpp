/*!
 * \file rLoader_glTF_structs.cpp
 * \brief \b Classes: \a rLoader_glTF_structs
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

#include "rLoader_glTF_structs.hpp"
#include "uLog.hpp"

namespace e_engine {
namespace glTF {

rLoader_glTF_structs::~rLoader_glTF_structs() {}

#if D_LOG_GLTF

std::string rLoader_glTF_structs::getStringFromElement( ELEMENTS _el ) {
   for ( auto i : vMap ) {
      if ( i.second == _el )
         return i.first;
   }

   return "<UNKOWN>";
}
// clang-format off
void rLoader_glTF_structs::accessor::print( rLoader_glTF_structs *_parent ) const {
   dLOG( "Accessor '", id, "' (", name, "):",
         "\n - bufferView:    ", bufferView,
         "\n - byteOffset:    ", byteOffset,
         "\n - byteStride:    ", byteStride,
         "\n - componentType: ", _parent->getStringFromElement( componentType ),
         "\n - count:         ", count,
         "\n - type:          ", _parent->getStringFromElement( type ) );
}

void rLoader_glTF_structs::bufferView::print( rLoader_glTF_structs *_parent ) const {
   dLOG( "Buffer View '", id, "' (", name, "):",
         "\n - buffer:     ", buffer,
         "\n - byteOffset: ", byteOffset,
         "\n - byteLength: ", byteLength,
         "\n - target:     ", _parent->getStringFromElement( target ) );
}

void rLoader_glTF_structs::buffer::print( rLoader_glTF_structs *_parent ) const {
   dLOG( "Buffer '", id, "' (", name, "):",
         "\n - uri:        ", uri,
         "\n - byteLength: ", byteLength,
         "\n - type:       ", _parent->getStringFromElement( type ) );
}

void rLoader_glTF_structs::mesh::print( rLoader_glTF_structs *_parent ) const {
   dLOG( "Mesh '", id, "' (", name, "):", "\n - primitives: ", primitives.size() );

   for ( auto const &i : primitives ) {
      dLOG( " - PRIMITIVE:",
            "\n   - indices:    ", i.indices,
            "\n   - material:   ", i.material,
            "\n   - mode:       ", _parent->getStringFromElement( i.mode ),
            "\n   - attributes: ", i.attributes.size() );

      for ( auto const &a : i.attributes ) {
         dLOG( "     - ", _parent->getStringFromElement( a.type ), ": ", a.accessor );
      }
   }
}

// clang-format on

#endif

bool rLoader_glTF_structs::accessor::test() const {
   return ( bufferView != static_cast<size_t>( -1 ) ) && ( byteOffset >= 0 ) &&
          ( byteStride >= 0 && byteStride <= 255 ) &&
          ( componentType >= TP_BYTE && componentType <= TP_FLOAT ) && ( count >= 1 ) &&
          ( type >= SCALAR || type <= MAT4 );
}

bool rLoader_glTF_structs::bufferView::test() const {
   return ( buffer != static_cast<size_t>( -1 ) ) && ( byteOffset >= 0 ) && ( byteLength >= 0 ) &&
          ( target >= TG_ARRAY_BUFFER && target <= TG_ELEMENT_ARRAY_BUFFER );
}

bool rLoader_glTF_structs::buffer::test() const {
   return ( !uri.empty() ) && ( byteLength >= 0 ) && ( type >= ARRAYBUFFER && type <= TEXT );
}

bool rLoader_glTF_structs::mesh::test() const {
   if ( primitives.empty() )
      return false;

   bool lRet = true;

   for ( auto const &i : primitives ) {
      lRet = lRet && ( i.indices != static_cast<size_t>( -1 ) ) &&
             ( i.material != static_cast<size_t>( -1 ) ) && ( !i.attributes.empty() ) &&
             ( i.mode >= P_POINTS && i.mode <= P_TRIANGLE_FAN );

      for ( auto const &a : i.attributes ) {
         lRet = lRet && ( a.accessor != static_cast<size_t>( -1 ) ) &&
                ( a.type >= SM_POSITION && a.type <= SM_WEIGHT );
      }
   }

   return lRet;
}
}
}
