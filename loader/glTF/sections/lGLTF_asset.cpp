/*!
 * \file lGLTF_asset.cpp
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

bool lGLTF::sectionAsset() {
   if ( !expect( '{' ) )
      return false;

   if ( expect( '}', true, true ) )
      return true;

   ELEMENTS lSection;

   while ( vIter != vEnd ) {
      if ( !getMapElement( lSection ) )
         return false;

      switch ( lSection ) {
         case COPYRIGHT: READ_STRING( vAsset.copyright );
         case GENERATOR: READ_STRING( vAsset.generator );
         case PREMULTIPLIEDALPHA: READ_BOOL( vAsset.premultipliedAlpha );
         case PROFILE:
            if ( !expect( '{' ) )
               return false;

            while ( vIter != vEnd ) {
               if ( !getMapElement( lSection ) )
                  return false;

               switch ( lSection ) {
                  case API: READ_STRING( vAsset.profile.api );
                  case VERSION: READ_STRING( vAsset.profile.version );
                  case EXTENSIONS:
                  case EXTRAS:
                     if ( !skipSection() )
                        return false;

                     break;
                  default: return wrongKeyWordError();
               }

               END_GLTF_OBJECT
            }

            break;
         case VERSION: READ_STRING( vAsset.version );
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
   vAsset.print();
#endif

   return true;
}
}
