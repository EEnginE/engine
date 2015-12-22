/*!
 * \file lGLTF_bufferViews.hpp
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

bool lGLTF::sectionBufferViews() {
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

      lID = getItem( vBufferViews, vBufferViewMap, lName );

      while ( vIter != vEnd ) {
         lName.clear();

         if ( !getMapElement( lSection ) )
            return false;

         switch ( lSection ) {
            // clang-format off
            case NAME:       READ_STRING( vBufferViews[lID].name );
            case BUFFER:     READ_ITEM( vBuffers, vBufferMap, lName, vBufferViews[lID].buffer );
            case BYTEOFFSET: READ_NUM( vBufferViews[lID].byteOffset );
            case BYTELENGTH: READ_NUM( vBufferViews[lID].byteLength );
            case TARGET:     READ_MAP_EL_ETC( vBufferViews[lID].target );
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
      vBufferViews[lID].print( this );
#endif

      END_GLTF_OBJECT
   }

   return true;
}
}
