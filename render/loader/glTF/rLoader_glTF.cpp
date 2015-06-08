/*!
 * \file rLoader_glTF.hpp
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

bool rLoader_glTF::load_IMPL() {
   if ( !expect( '{' ) )
      return false;

   ELEMENTS lSection;

#if D_LOG_GLTF
   dLOG( "Begin parsing '", vFilePath_str, "'" );
#endif

   while ( vIter != vEnd ) {
      if ( !getMapElement( lSection ) )
         return false;

      switch ( lSection ) {
         case ACCESSORS:
            if ( !sectionAccessors() )
               return false;

            break;
         case BUFFERVIEWS:
            if ( !sectionBufferViews() )
               return false;

            break;
         case BUFFERS:
            if ( !sectionBuffers() )
               return false;

            break;
         case MESHES:
            if ( !sectionMeshs() )
               return false;

            break;
         case ANIMATIONS:
         case ASSET:
         case CAMERAS:
         case IMAGES:
         case LIGHTS:
         case MATERIALS:
         case NODES:
         case PROGRAMS:
         case SAMPLERS:
         case SCENE:
         case SCENES:
         case SHADERS:
         case SKINS:
         case TECHNIQUES:
         case TEXTURES:
            if ( !skipSection() )
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

   return interprete();
}
}
