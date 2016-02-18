/*!
 * \file lGLTF_programs.cpp
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

bool lGLTF::sectionPrograms() {
   BEGIN_GLTF_SECTION( lSection, lName, lID );

   while ( vIter != vEnd ) {
      BEGIN_GLTF_SECTION_MAIN_LOOP( lName, vPrograms, vProgramsMap, lID );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            // clang-format off
            case NAME:           READ_STRING( vPrograms[lID].name );
            case ATTRIBUTES:     READ_ARRAY( vPrograms[lID].attributes );
            case FRAGMENTSHADER: READ_ITEM( vShaders, vShadersMap, lName, vPrograms[lID].fragmentShader );
            case VERTEXSHADER:   READ_ITEM( vShaders, vShadersMap, lName, vPrograms[lID].vertexShader );
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
      vPrograms[lID].print();
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}
