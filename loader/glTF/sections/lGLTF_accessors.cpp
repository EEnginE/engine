/*!
 * \file lGLTF_accessors.cpp
 * \brief \b Classes: \a lGLTF
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

#define ENABLE_GLTF_MACRO_HELPERS

#include "lGLTF.hpp"
#include "lGLTF_macroHelpers.hpp"

namespace e_engine {

bool lGLTF::sectionAccessors() {
   if ( !expect( '{' ) )
      return false;

   ELEMENTS lSection;
   std::string lName;
   size_t lID;

   while ( vIter != vEnd ) {
      lName.clear();
      if ( !getString( lName ) )
         return false;

      if ( !expect( ':' ) )
         return false;

      if ( !expect( '{' ) )
         return false;

      lID = getItem( vAccessors, vAccessorMap, lName );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            // clang-format off
            case NAME:          READ_STRING( vAccessors[lID].name );
            case BUFFERVIEW:    READ_ITEM( vBufferViews, vBufferViewMap, lName, vAccessors[lID].bufferView );
            case BYTEOFFSET:    READ_NUM( vAccessors[lID].byteOffset );
            case BYTESTRIDE:    READ_NUM( vAccessors[lID].byteStride );
            case COMPONENTTYPE: READ_MAP_EL_ETC( vAccessors[lID].componentType );
            case COUNT:         READ_NUM( vAccessors[lID].count );
            case TYPE:          READ_MAP_EL( vAccessors[lID].type, false );
            case MAX:           READ_ARRAY( vAccessors[lID].max );
            case MIN:           READ_ARRAY( vAccessors[lID].min );
            // clang-format on
            case EXTENSIONS:
            case EXTRAS:
               if ( !skipSection() )
                  return false;

               break;
            default: return wrongKeyWordError();
         }

         END_GLTF_OBJECT
      }

#if D_LOG_GLTF
      vAccessors[lID].print( this );
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}
