/*!
 * \file lGLTF.hpp
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

bool lGLTF::load_IMPL() {
   if ( !expect( '{' ) )
      return false;

   ELEMENTS lSection;

   dLOG_glTF( "Begin parsing '", vFilePath_str, "'" );

   while ( vIter != vEnd ) {
      if ( !getMapElement( lSection ) )
         return false;

      switch ( lSection ) {
         // clang-format off
         case ACCESSORS:    SUB_SECTION( sectionAccessors );
         case ASSET:        SUB_SECTION( sectionAsset );
         case BUFFERVIEWS:  SUB_SECTION( sectionBufferViews );
         case BUFFERS:      SUB_SECTION( sectionBuffers );
         case IMAGES:       SUB_SECTION( sectionImages );
         case MATERIALS:    SUB_SECTION( sectionMaterials );
         case MESHES:       SUB_SECTION( sectionMeshs );
         case NODES:        SUB_SECTION( sectionNodes );
         case PROGRAMS:     SUB_SECTION( sectionPrograms );
         case SCENE:        SUB_SECTION( sectionScene );
         case SCENES:       SUB_SECTION( sectionScenes );
         case SHADERS:      SUB_SECTION( sectionShaders );
         case SKINS:        SUB_SECTION( sectionSkins );
         case TECHNIQUES:   SUB_SECTION( sectionTechniques )
         case EXTENSIONSUSED:
         case ANIMATIONS:
         case CAMERAS:
         case SAMPLERS:
         case TEXTURES:
            // clang-format on
            if ( !skipSection() )
               return false;

            break;
         default: return wrongKeyWordError();
      }

      END_GLTF_OBJECT
   }

   return interprete();
}
}
