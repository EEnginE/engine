/*!
 * \file lGLTF_materials.cpp
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

bool lGLTF::sectionNodes() {
   BEGIN_GLTF_SECTION( lSection, lName, lID );

   while ( vIter != vEnd ) {
      BEGIN_GLTF_SECTION_MAIN_LOOP( lName, vNodes, vNodesMap, lID );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            // clang-format off
            case NAME:        READ_STRING( vNodes[lID].name );
            case CAMERA:      READ_ITEM( vCameras, vCamerasMap, lName, vNodes[lID].camera );
            case SKIN:        READ_ITEM( vSkins, vSkinsMap, lName, vNodes[lID].skin );
            case JOINTNAME:   READ_STRING( vNodes[lID].jointName );
            case MATRIX:      READ_ARRAY( vNodes[lID].matrix );
            case ROTATION:    READ_ARRAY( vNodes[lID].rotation );
            case SCALE:       READ_ARRAY( vNodes[lID].scale );
            case TRANSLATION: READ_ARRAY( vNodes[lID].translation );
            // clang-format on
            case CHILDREN:
               if ( !expect( '[' ) )
                  return false;

               while ( vIter != vEnd ) {
                  if ( !getString( lName, true, true ) ) {
                     if ( !expect( ']' ) )
                        return false;

                     break;
                  }

                  vNodes[lID].children.push_back( getItem( vNodes, vNodesMap, lName ) );
                  END_GLTF_ARRAY
               }

               break;
            case SKELETONS:
               if ( !expect( '[' ) )
                  return false;

               while ( vIter != vEnd ) {
                  if ( !getString( lName, true, true ) ) {
                     if ( !expect( ']' ) )
                        return false;

                     break;
                  }

                  vNodes[lID].skeletons.push_back( getItem( vNodes, vNodesMap, lName ) );
                  END_GLTF_ARRAY
               }

               break;
            case MESHES:
               if ( !expect( '[' ) )
                  return false;

               while ( vIter != vEnd ) {
                  if ( !getString( lName, true, true ) ) {
                     if ( !expect( ']' ) )
                        return false;

                     break;
                  }

                  vNodes[lID].meshes.push_back( getItem( vMeshs, vMeshsMap, lName ) );
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
      vNodes[lID].print();
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}
