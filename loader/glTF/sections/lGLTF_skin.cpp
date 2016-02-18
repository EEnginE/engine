/*!
 * \file lGLTF_scenes.cpp
 * \brief \b Classes: \a lGLTF
 */
/*
 * Copyright (C) 2016 EEnginE project
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

bool lGLTF::sectionSkins() {
   BEGIN_GLTF_SECTION( lSection, lName, lID );

   while ( vIter != vEnd ) {
      BEGIN_GLTF_SECTION_MAIN_LOOP( lName, vSkins, vSkinsMap, lID );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            case NAME: READ_STRING( vSkins[lID].name );
            case BINDSHAPEMATRIX: READ_ARRAY( vSkins[lID].bindShapeMatrix );
            case INVERSEBINDMATRICES:
               READ_ITEM( vAccessors, vAccessorMap, lName, vSkins[lID].inverseBindMatrices );
            case JOINTNAMES:
               if ( !expect( '[' ) )
                  return false;

               while ( vIter != vEnd ) {
                  if ( !getString( lName, true, true ) ) {
                     if ( !expect( ']' ) )
                        return false;

                     break;
                  }

                  vSkins[lID].jointNames.push_back( getItem( vNodes, vNodesMap, lName ) );
                  END_GLTF_ARRAY
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

#if D_LOG_GLTF
      vSkins[lID].print();
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}
