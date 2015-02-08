/*!
 * \file uParserHelper.cpp
 * \brief \b Classes: \a uParserHelper
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

#include "uParserHelper.hpp"
#include "uLog.hpp"
#include "uFileIO.hpp"

namespace e_engine {
namespace internal {

uParserHelper::~uParserHelper() {}

/*!
 * \brief Gets wether or not the file is loaded and parsed
 * \returns The state of the file being loaded and parsed
 */

bool uParserHelper::getIsParsed() const { return vIsParsed; }

/*!
 * \brief Gets the path of the file to parse
 * \returns The path of the file to parse
 */
std::string uParserHelper::getFilePath() const { return vFilePath_str; }


/*!
 * \brief Sets the file to parse
 * \param[in] _file The file to parse
 *
 * \note This will NOT parse the file! You have to manually parse it with parse()
 */
void uParserHelper::setFile( std::string _file ) { vFilePath_str = _file; }

/*!
 * \brief loads the content of the JSON file
 * \returns 1 on success
 * \returns 2 if there was a parsing error
 * \returns 3 if the file file doesn't exists
 * \returns 4 if the file file is not a regular file
 * \returns 5 if the file file is not readable
 * \returns 6 if already parsed
 */
int uParserHelper::parse() {
   if ( vIsParsed )
      return 6;

   uFileIO lFile( vFilePath_str );
   int lRet = lFile();
   if ( lRet != 1 )
      return lRet;

   vIter = lFile.begin();
   vEnd = lFile.end();

   if ( !load_IMPL() ) {
      eLOG( "Failed parsing '", vFilePath_str, "'" );
      return 2;
   }

   vIsParsed = true;

   return 1;
}



bool uParserHelper::continueWhitespace( bool _quiet ) {
   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '\n':
            ++vCurrentLine;
            FALLTHROUGH
         case '\t':
         case ' ':
            ++vIter;
            break;
         default:
            return true;
      }
   }

   // End of file
   if ( !_quiet )
      eofError();

   return false;
}

bool uParserHelper::expect( char _c, bool _continueWhitespace, bool _quiet ) {
   if ( !continueWhitespace() )
      return false;

   if ( *vIter != _c ) {
      if ( !_quiet ) {
         eLOG( "Expected '",
               _c,
               "' at line ",
               vCurrentLine,
               ", instead got a '",
               *vIter,
               "' [",
               vFilePath_str,
               "]" );
      }
      return false;
   }

   if ( _c == '\n' )
      vCurrentLine++;

   ++vIter;

   if ( _continueWhitespace )
      return continueWhitespace( _quiet );

   return true;
}

bool uParserHelper::expect( std::string _str, bool _continueWhitespace, bool _quiet ) {
   if ( !continueWhitespace() )
      return false;

   for ( char c : _str ) {
      if ( *vIter != c ) {
         if ( !_quiet ) {
            eLOG( "Expected '",
                  _str,
                  "' at line ",
                  vCurrentLine,
                  ", but got a '",
                  *vIter,
                  "' [",
                  vFilePath_str,
                  "]" );
         }
         return false;
      }

      if ( c == '\n' )
         vCurrentLine++;

      ++vIter;
   }

   if ( _continueWhitespace )
      return continueWhitespace( _quiet );

   return true;
}

bool uParserHelper::getString( std::string &_str, bool _continueWhitespace ) {
   if ( !expect( '"', false ) )
      return false;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '\\':
            ++vIter;

            if ( vIter == vEnd )
               return eofError();

            switch ( *vIter ) {
               case '\\':
                  _str += '\\';
                  ++vIter;
                  break;
               case '"':
                  _str += '"';
                  ++vIter;
                  break;
               default:
                  return unexpectedCharError();
            }
            break;
         case '"':
            ++vIter;

            if ( _continueWhitespace )
               return continueWhitespace();

            return true;

         case '\n':
            ++vCurrentLine;
            FALLTHROUGH
         default:
            _str += *vIter;
            ++vIter;
      }
   }

   return eofError();
}

bool uParserHelper::getNum( double &_num ) {
   if ( !continueWhitespace() )
      return false;

   // Static because clear wont change the capacity
   static std::string lNum;
   lNum.clear();

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '-':
         case '.':
         case 'e':
         case 'E':
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            lNum += *vIter;
            ++vIter;
            break;

         default:
            if ( lNum.empty() )
               return unexpectedCharError();

            _num = std::stod( lNum );
            return true;
      }
   }

   return eofError();
}

bool uParserHelper::getNum( float &_num ) {
   if ( !continueWhitespace() )
      return false;

   // Static because clear wont change the capacity
   static std::string lNum;
   lNum.clear();

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '-':
         case '.':
         case 'e':
         case 'E':
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            lNum += *vIter;
            ++vIter;
            break;

         default:
            if ( lNum.empty() )
               return unexpectedCharError();

            _num = static_cast<float>( std::stof( lNum ) );
            return true;
      }
   }

   return eofError();
}

bool uParserHelper::getNum( int &_num ) {
   if ( !continueWhitespace() )
      return false;

   // Static because clear wont change the capacity
   static std::string lNum;
   lNum.clear();

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '-':
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            lNum += *vIter;
            ++vIter;
            break;

         default:
            if ( lNum.empty() )
               return unexpectedCharError();

            _num = std::stoi( lNum );
            return true;
      }
   }

   return eofError();
}


bool uParserHelper::getNum( unsigned int &_num ) {
   if ( !continueWhitespace() )
      return false;

   // Static because clear wont change the capacity
   static std::string lNum;
   lNum.clear();

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            lNum += *vIter;
            ++vIter;
            break;

         default:
            if ( lNum.empty() )
               return unexpectedCharError();

            _num = static_cast<unsigned>( std::stod( lNum ) );
            return true;
      }
   }

   return eofError();
}

bool uParserHelper::getNum( unsigned short &_num ) {
   if ( !continueWhitespace() )
      return false;

   // Static because clear wont change the capacity
   static std::string lNum;
   lNum.clear();

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '0':
         case '1':
         case '2':
         case '3':
         case '4':
         case '5':
         case '6':
         case '7':
         case '8':
         case '9':
            lNum += *vIter;
            ++vIter;
            break;

         default:
            if ( lNum.empty() )
               return unexpectedCharError();

            _num = static_cast<unsigned short>( std::stod( lNum ) );
            return true;
      }
   }

   return eofError();
}




bool uParserHelper::eofError() {
   eLOG( "Unexpected end of file! Line: ", vCurrentLine, " [", vFilePath_str, "]" );
   return false;
}

bool uParserHelper::unexpectedCharError() {
   eLOG( "Unexpected char '", *vIter, "' at line ", vCurrentLine, " [", vFilePath_str, "]" );
   return false;
}
}
}
