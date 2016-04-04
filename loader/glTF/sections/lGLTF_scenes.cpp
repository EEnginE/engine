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
#include "uLog.hpp"

namespace e_engine {

bool lGLTF::sectionScene() {
   if ( !getString( vScene ) )
      return false;

#if D_LOG_GLTF
   dLOG( "Scene: '", vScene, "'" );
#endif

   return true;
}

bool lGLTF::sectionScenes() {
   BEGIN_GLTF_SECTION( lSection, lName, lID );

   while ( vIter != vEnd ) {
      BEGIN_GLTF_SECTION_MAIN_LOOP( lName, vScenes, vScenesMap, lID );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            case NAME: READ_STRING( vScenes[lID].name );
            case NODES:
               if ( !expect( '[' ) )
                  return false;

               while ( vIter != vEnd ) {
                  if ( !getString( lName, true, true ) ) {
                     if ( !expect( ']' ) )
                        return false;

                     break;
                  }

                  vScenes[lID].nodes.push_back( getItem( vNodes, vNodesMap, lName ) );
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
      vScenes[lID].print();
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}
