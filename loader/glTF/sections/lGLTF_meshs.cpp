/*!
 * \file lGLTF_meshs.cpp
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

bool lGLTF::sectionMeshsPrimitives( size_t _id ) {
   if ( !expect( '[' ) )
      return false;

   ELEMENTS lSection;
   std::string lName;

   while ( vIter != vEnd ) {
      if ( !expect( '{' ) )
         return false;

      vMeshs[_id].primitives.emplace_back();
      auto &lTemp = vMeshs[_id].primitives.back();

      while ( vIter != vEnd ) {
         lName.clear();
         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            case INDICES: READ_ITEM( vAccessors, vAccessorMap, lName, lTemp.indices );
            case MATERIAL: READ_ITEM( vMaterials, vMaterialMap, lName, lTemp.material );
            case MODE: READ_MAP_EL_ETC( lTemp.mode );
            case ATTRIBUTES:
               if ( !expect( '{' ) )
                  return false;

               while ( vIter != vEnd ) {
                  lName.clear();
                  lTemp.attributes.emplace_back();
                  auto &lTemp2 = lTemp.attributes.back();

                  if ( !getMapElement( lTemp2.type ) )
                     return false;

                  if ( !getString( lName ) )
                     return false;

                  lTemp2.accessor = getItem( vAccessors, vAccessorMap, lName );

                  END_GLTF_OBJECT
               }
               break;
            case EXTENSIONS:
            case EXTRAS:
               if ( !skipSection() )
                  return false;

               break;
            default: return wrongKeyWordError();
         }

         END_GLTF_OBJECT
      }

      END_GLTF_ARRAY
   }

   return true;
}

bool lGLTF::sectionMeshs() {
   BEGIN_GLTF_SECTION( lSection, lName, lID );

   while ( vIter != vEnd ) {
      BEGIN_GLTF_SECTION_MAIN_LOOP( lName, vMeshs, vMeshsMap, lID );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            case NAME: READ_STRING( vMeshs[lID].name );
            case PRIMITIVES: SUB_SECTION( sectionMeshsPrimitives, lID );
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
      vMeshs[lID].print( this );
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}
