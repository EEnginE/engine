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

#define ENABLE_GLTF_MACRO_HELPERS

#include "lGLTF.hpp"
#include "lGLTF_macroHelpers.hpp"

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

bool lGLTF::getValue( value &_val ) {
   if ( !getString( _val.id ) )
      return false;

   if ( !expect( ':', true ) )
      return false;

   bool lBoolTemp;

   auto lIterSave    = vIter;
   auto lCurrentLine = vCurrentLine;
   char c;

   switch ( *vIter ) {
      case '"':
         _val.type = value::STR;
         _val.valStr.emplace_back();
         if ( !getString( _val.valStr.back() ) )
            return false;

         break;
      case '[':
         vIter++;
         if ( !continueWhitespace() )
            return false;

         c = *vIter;

         vIter        = lIterSave;
         vCurrentLine = lCurrentLine;

         switch ( c ) {
            case '"':
               _val.type = value::A_STR;
               getArray( _val.valStr );
               break;
            case 't':
            case 'f':
               _val.type = value::A_BOOL;
               getArray( _val.valBool );
               break;
            default:
               _val.type = value::A_NUM;
               getArray( _val.valNum );
               break;
         }

         break;
      case 't':
      case 'f':
         _val.type = value::BOOL;
         if ( !getBoolean( lBoolTemp ) )
            return false;

         _val.valBool.emplace_back( lBoolTemp );

         break;
      default:
         _val.type = value::NUM;
         _val.valNum.emplace_back();
         if ( !getNum( _val.valNum.back() ) )
            return false;
   }

   return true;
}

bool lGLTF::getArray( std::vector<float> &_array ) {
   if ( !expect( '[' ) )
      return false;

   _array.clear();
   float lTemp = 0;

   while ( vIter != vEnd ) {
      if ( !getNum( lTemp ) )
         return false;

      _array.push_back( lTemp );

      END_GLTF_ARRAY
   }

   return true;
}

bool lGLTF::getArray( std::vector<bool> &_array ) {
   if ( !expect( '[' ) )
      return false;

   _array.clear();
   bool lTemp = false;

   while ( vIter != vEnd ) {
      if ( !getBoolean( lTemp ) )
         return false;

      _array.push_back( lTemp );

      END_GLTF_ARRAY
   }

   return true;
}

bool lGLTF::getArray( std::vector<std::string> &_array ) {
   if ( !expect( '[' ) )
      return false;

   _array.clear();
   std::string lTemp;

   while ( vIter != vEnd ) {
      if ( !getString( lTemp ) )
         return false;

      _array.push_back( lTemp );

      END_GLTF_ARRAY
   }

   return true;
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
