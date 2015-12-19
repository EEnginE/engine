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

#include "lGLTF.hpp"

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
            case NAME:
               if ( !getString( vBufferViews[lID].name ) )
                  return false;

               break;
            case BUFFER:
               if ( !getString( lName ) )
                  return false;

               vBufferViews[lID].buffer = getItem( vBuffers, vBufferMap, lName );
               break;
            case BYTEOFFSET:
               if ( !getNum( vBufferViews[lID].byteOffset ) )
                  return false;

               break;
            case BYTELENGTH:
               if ( !getNum( vBufferViews[lID].byteLength ) )
                  return false;

               break;
            case TARGET:
               if ( !getMapElementETC( vBufferViews[lID].target ) )
                  return false;

               break;
            case EXTENSIONS:
            case EXTRAS:
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

#if D_LOG_GLTF
      vBufferViews[lID].print( this );
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
