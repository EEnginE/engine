/*!
 * \file lGLTF_structs.cpp
 * \brief \b Classes: \a lGLTF_structs
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

#include "lGLTF_structs.hpp"
#include "uLog.hpp"

namespace e_engine {
namespace glTF {

lGLTF_structs::~lGLTF_structs() {}

#if D_LOG_GLTF

std::string lGLTF_structs::getStringFromElement( ELEMENTS _el ) {
   for ( auto i : vMap ) {
      if ( i.second == _el )
         return i.first;
   }

   return "<UNKOWN>";
}

void lGLTF_structs::value::print() const {
   std::string lTypeSTR;
   switch ( type ) {
      // clang-format off
      case NUM:    lTypeSTR = "number";         break;
      case BOOL:   lTypeSTR = "bool";           break;
      case STR:    lTypeSTR = "string";         break;
      case A_NUM:  lTypeSTR = "[array] number"; break;
      case A_BOOL: lTypeSTR = "[array] bool";   break;
      case A_STR:  lTypeSTR = "[array] string"; break;
         // clang-format on
   }

   dLOG( "     - '", id, "': type: ", lTypeSTR );

   switch ( type ) {
      case NUM:
      case A_NUM:
         for ( auto const &i : valNum ) {
            dLOG( "       - ", i );
         }
         break;

      case BOOL:
      case A_BOOL:
         for ( auto i : valBool ) {
            dLOG( "       - ", i );
         }
         break;

      case STR:
      case A_STR:
         for ( auto const &i : valStr ) {
            dLOG( "       - ", i );
         }
         break;
   }
}

// clang-format off
void lGLTF_structs::accessor::print( lGLTF_structs *_parent ) const {
   dLOG( "Accessor '", id, "' (", name, "):",
         "\n   - bufferView:    ", bufferView,
         "\n   - byteOffset:    ", byteOffset,
         "\n   - byteStride:    ", byteStride,
         "\n   - componentType: ", _parent->getStringFromElement( componentType ),
         "\n   - count:         ", count,
         "\n   - type:          ", _parent->getStringFromElement( type ),
         "\n   - max:"  );

   for( auto lTemp : max ) {
      dLOG( "     - ", lTemp );
   }

   dLOG( "   - min:" );
   for( auto lTemp : min ) {
      dLOG( "     - ", lTemp );
   }
}

void lGLTF_structs::asset::print() const {
   dLOG( "Asset:"
         "\n   - copyright:          ", copyright,
         "\n   - generator:          ", generator,
         "\n   - premultipliedAlpha: ", premultipliedAlpha,
         "\n   - profile.api:        ", profile.api,
         "\n   - profile.version:    ", profile.version,
         "\n   - version:            ", version );
}

void lGLTF_structs::bufferView::print( lGLTF_structs *_parent ) const {
   dLOG( "Buffer View '", id, "' (", name, "):",
         "\n   - buffer:     ", buffer,
         "\n   - byteOffset: ", byteOffset,
         "\n   - byteLength: ", byteLength,
         "\n   - target:     ", _parent->getStringFromElement( target ) );
}

void lGLTF_structs::buffer::print( lGLTF_structs *_parent ) const {
   dLOG( "Buffer '", id, "' (", name, "):",
         "\n   - uri:        ", uri,
         "\n   - byteLength: ", byteLength,
         "\n   - type:       ", _parent->getStringFromElement( type ) );
}

void lGLTF_structs::image::print() const {
   dLOG( "Image '", id, "' (", name, "):",
         "\n   - uri: ", uri );
}

void lGLTF_structs::material::print() const {
   dLOG( "Material '", id, "' (", name, "):",
         "\n   - technique: ", technique,
         "\n   - values:" );

   for( auto const &v : values ) {
      v.print();
   }
}

void lGLTF_structs::mesh::print( lGLTF_structs *_parent ) const {
   dLOG( "Mesh '", id, "' (", name, "):", "\n   - primitives: ", primitives.size() );

   for ( auto const &i : primitives ) {
      dLOG( "   - PRIMITIVE:",
            "\n     - indices:    ", i.indices,
            "\n     - material:   ", i.material,
            "\n     - mode:       ", _parent->getStringFromElement( i.mode ),
            "\n     - attributes: ", i.attributes.size() );

      for ( auto const &a : i.attributes ) {
         dLOG( "       - ", _parent->getStringFromElement( a.type ), ": ", a.accessor );
      }
   }
}

void lGLTF_structs::node::print() const {
   dLOG( "Node '", id, "' (", name, "):",
         "\n   - camera:    ", camera,
         "\n   - skin:      ", skin,
         "\n   - jointName: ", jointName );

   dLOG( "   - children:"  );
   for( auto const& i : children )
      dLOG( "     - ", i );

   dLOG( "   - skeletons:"  );
   for( auto const& i : skeletons )
      dLOG( "     - ", i );

   dLOG( "   - matrix:"  );
   for( auto const& i : matrix )
      dLOG( "     - ", i );

   dLOG( "   - meshes:"  );
   for( auto const& i : meshes )
      dLOG( "     - ", i );

   dLOG( "   - rotation:"  );
   for( auto const& i : rotation )
      dLOG( "     - ", i );

   dLOG( "   - scale:"  );
   for( auto const& i : scale )
      dLOG( "     - ", i );

   dLOG( "   - translation:"  );
   for( auto const& i : translation )
      dLOG( "     - ", i );
}

// clang-format on

#endif

bool lGLTF_structs::accessor::test() const {
   return ( bufferView != static_cast<size_t>( -1 ) ) && ( byteOffset >= 0 ) &&
          ( byteStride >= 0 && byteStride <= 255 ) &&
          ( componentType >= TP_BYTE && componentType <= TP_FLOAT ) && ( count >= 1 ) &&
          ( type >= SCALAR || type <= MAT4 );
}

bool lGLTF_structs::asset::test() const { return !version.empty(); }

bool lGLTF_structs::bufferView::test() const {
   return ( buffer != static_cast<size_t>( -1 ) ) && ( byteOffset >= 0 ) && ( byteLength >= 0 ) &&
          ( target >= TG_ARRAY_BUFFER && target <= TG_ELEMENT_ARRAY_BUFFER );
}

bool lGLTF_structs::buffer::test() const {
   return ( !uri.empty() ) && ( byteLength >= 0 ) && ( type >= ARRAYBUFFER && type <= TEXT );
}

bool lGLTF_structs::image::test() const { return ( !uri.empty() ); }

bool lGLTF_structs::material::test() const { return true; }

bool lGLTF_structs::mesh::test() const {
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

bool lGLTF_structs::node::test() const { return true; }
}
}
