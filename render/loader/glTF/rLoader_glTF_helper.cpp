/*!
 * \file rLoader_glTF_helper.hpp
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

bool rLoader_glTF::getMapElement( ELEMENTS &_el, bool _isSection ) {
   vMapElStr.clear();

   if ( !getString( vMapElStr ) )
      return false;

   auto lIter = vMap.find( vMapElStr );

   if ( lIter == vMap.end() ) {
      eLOG( "Unkown key word '", vMapElStr, "' line: ", vCurrentLine, " [", vFilePath_str, "]" );
      return false;
   }

   _el = lIter->second;

   if ( _isSection )
      return expect( ':' );

   return true;
}

bool rLoader_glTF::getMapElementETC( ELEMENTS &_el ) {
   vMapElStr.clear();

   if ( !continueWhitespace() )
      return false;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '\n': ++vCurrentLine; FALLTHROUGH
         case '\t':
         case ',':
         case ' ':
         case '}': goto VERY_UGLY_GOTO;
      }
      vMapElStr += *vIter;
      ++vIter;
   }

VERY_UGLY_GOTO:

   auto lIter = vMap.find( vMapElStr );

   if ( lIter == vMap.end() ) {
      eLOG( "Unkown key '", vMapElStr, "' line: ", vCurrentLine, " [", vFilePath_str, "]" );
      return false;
   }

   _el = lIter->second;
   return true;
}

bool rLoader_glTF::skipSection() {
   if ( !continueWhitespace() )
      return false;

   int lCounter = 1;
   auto lStart = vCurrentLine;

   switch ( *vIter ) {
      case '{':
         ++vIter;
         while ( vIter != vEnd && lCounter > 0 ) {
            switch ( *vIter ) {
               case '{': ++lCounter; break;
               case '}': --lCounter; break;
               case '\n': ++vCurrentLine; break;
            }

            ++vIter;
         }
         break;
      case '[':
         ++vIter;
         while ( vIter != vEnd && lCounter > 0 ) {
            switch ( *vIter ) {
               case '[': ++lCounter; break;
               case ']': --lCounter; break;
               case '\n': ++vCurrentLine; break;
            }

            ++vIter;
         }
         break;
      default:
         ++vIter;
         while ( vIter != vEnd ) {
            switch ( *vIter ) {
               case ',':
               case '}': return skipSectionWarning( lStart );
               case '\n': ++vCurrentLine; break;
            }

            ++vIter;
         }
         break;
   }

   if ( lCounter != 0 )
      return eofError();
   else
      return skipSectionWarning( lStart );
}



bool rLoader_glTF::skipSectionWarning( unsigned int _s ) {
   wLOG( "Skipped '", vMapElStr, "' ( ", _s, " - ", vCurrentLine, " ) [", vFilePath_str, "]" );
   return true;
}

bool rLoader_glTF::wrongKeyWordError() {
   eLOG( "Invalid key word '",
         vMapElStr,
         "' in this context! (",
         vCurrentLine,
         ") [",
         vFilePath_str,
         "]" );
   return false;
}

bool rLoader_glTF::selfTestFailed( std::string _type, std::string _name ) {
   eLOG( "Self test for ", _type, " '", _name, "' failed [", vFilePath_str, "]" );
   return false;
}
}
