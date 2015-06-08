/*!
 * \file rLoader_glTF_meshs.hpp
 * \brief \b Classes: \a rLoader_glTF
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

#include "rLoader_glTF.hpp"

namespace e_engine {

bool rLoader_glTF::sectionMeshsPrimitives( size_t _id ) {
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
            case INDICES:
               if ( !getString( lName ) )
                  return false;

               lTemp.indices = getItem( vAccessors, vAccessorMap, lName );
               break;

            // Main Mesh section
            case MATERIAL:
               if ( !getString( lName ) )
                  return false;

               lTemp.material = getItem( vMaterials, vMaterialMap, lName );
               break;
            case PRIMITIVE:
               if ( !getMapElementETC( lTemp.primitive ) )
                  return false;

               break;
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

                  if ( expect( ',', true, true ) )
                     continue;

                  if ( expect( '}', false ) )
                     break;

                  return unexpectedCharError();
               }
               break;
            default: return wrongKeyWordError();
         }

         if ( expect( ',', true, true ) )
            continue;

         if ( expect( '}', false ) )
            break;

         return unexpectedCharError();
      }

      if ( expect( ',', true, true ) )
         continue;

      if ( expect( ']', false ) )
         break;

      return unexpectedCharError();
   }

   return true;
}

bool rLoader_glTF::sectionMeshs() {
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

      lID = getItem( vMeshs, vMeshMap, lName );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            case NAME:
               if ( !getString( vMeshs[lID].userDefName ) )
                  return false;

               break;

            // Main Mesh section
            case PRIMITIVES:
               if ( !sectionMeshsPrimitives( lID ) )
                  return false;

               break;
            default: return wrongKeyWordError();
         }

         if ( expect( ',', true, true ) )
            continue;

         if ( expect( '}', false ) )
            break;

         return unexpectedCharError();
      }

#if D_LOG_GLTF
      vMeshs[lID].print( this );
#endif

      if ( expect( ',', true, true ) )
         continue;

      if ( expect( '}', false ) )
         break;

      return unexpectedCharError();
   }

   return true;
}
}
