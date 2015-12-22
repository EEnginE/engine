/*!
 * \file lGLTF_helper.hpp
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

bool lGLTF::getMapElement( ELEMENTS &_el, bool _isSection ) {
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

bool lGLTF::getMapElementETC( ELEMENTS &_el ) {
   vMapElStr.clear();

   if ( !continueWhitespace() )
      return false;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '\n': FALLTHROUGH
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

bool lGLTF::getBoolean( bool &_value ) {
   std::string lTempSTR = "";

   if ( !continueWhitespace() )
      return false;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case 't':
         case 'r':
         case 'u':
         case 'e':
         case 'f':
         case 'a':
         case 'l':
         case 's': lTempSTR += *vIter; break;
         case '\n': vCurrentLine++;
         default:
            if ( lTempSTR == "true" ) {
               _value = true;
               return true;
            } else if ( lTempSTR == "false" ) {
               _value = false;
               return true;
            }
            eLOG( "'",
                  lTempSTR,
                  "' Is not a boolean at line ",
                  vCurrentLine,
                  " [",
                  vFilePath_str,
                  "]" );
            return false;
            break;
      }

      vIter++;
   }

   return false;
}

bool lGLTF::skipSection() {
   if ( !continueWhitespace() )
      return false;

   int lCounter = 1;
   auto lStart  = vCurrentLine;

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



bool lGLTF::skipSectionWarning( unsigned int _s ) {
   wLOG( "Skipped '", vMapElStr, "' ( ", _s, " - ", vCurrentLine, " ) [", vFilePath_str, "]" );
   return true;
}

bool lGLTF::wrongKeyWordError() {
   eLOG( "Invalid key word '",
         vMapElStr,
         "' in this context! (",
         vCurrentLine,
         ") [",
         vFilePath_str,
         "]" );
   return false;
}

bool lGLTF::selfTestFailed( std::string _type, std::string _name ) {
   eLOG( "Self test for ", _type, " '", _name, "' failed [", vFilePath_str, "]" );
   return false;
}
}
