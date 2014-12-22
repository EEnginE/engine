/*!
 * \file uParserJSON.cpp
 * \brief \b Classes: \a uParserJSON
 */

#include "uParserJSON.hpp"

#include <boost/lexical_cast.hpp>

#include "uLog.hpp"
#include "uFileIO.hpp"


namespace {
typedef std::vector<::e_engine::uJSON_data>  VALUES;
}

namespace e_engine {

/*!
 * \brief Gets wether or not the file is loaded and parsed
 * \returns The state of the file being loaded and parsed
 */

bool uParserJSON::getIsParsed() const {
   return vIsParsed;
}

/*!
 * \brief Gets the path of the file to parse
 * \returns The path of the file to parse
 */
std::string uParserJSON::getFilePath() const {
   return vFilePath_str;
}


/*!
 * \brief Sets the file to parse
 * \param[in] _file The file to parse
 *
 * \note This will NOT parse the file! You have to manually parse it with parse()
 *
 * \returns Nothing
 */
void uParserJSON::setFile( std::string _file ) {
   vFilePath_str = _file;
}

/*!
 * \brief Clears the memory
 * \returns Nothing
 */
void uParserJSON::clear() {
   vIsParsed = false;
   vData.value_str.clear();
   vData.id.clear();
   vData.value_obj.clear();
   vData.value_obj.clear();
   vData.type = __JSON_NOT_SET__;
}


bool uParserJSON::continueWhitespace() {
   while( vIter != vEnd ) {
      switch( *vIter ) {
         case '\n': ++vCurrentLine;
         case '\t':
         case ' ':
            ++vIter;
            break;
         default:
            return true;
      }
   }

   // End of file
   return false;
}

bool uParserJSON::parseValue( e_engine::uJSON_data &_currentObject, const std::string &_name ) {
   bool lFound = false;

   switch( *vIter ) {

         // String
      case '"': {
         _currentObject.value_obj.emplace_back( _name, JSON_STRING );

         ++vIter;

         // Get String
         while( vIter != vEnd ) {
            switch( *vIter ) {
               case '\\':
                  ++vIter;

                  if( vIter == vEnd )
                     break;

                  switch( *vIter ) {
                     case '\\': _currentObject.value_obj.back().value_str += '\\'; ++vIter; break;
                     case '"':  _currentObject.value_obj.back().value_str += '"';  ++vIter; break;
                     default:
                        eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
                        return false;
                  }
               case '"':
                  lFound = true;
                  ++vIter;
                  break;

               case '\n': ++vCurrentLine;
               default:
                  _currentObject.value_obj.back().value_str += *vIter;
                  ++vIter;
            }

            if( lFound )
               break;
         }
         break;
      }

      // true
      case 't': {
         std::string lTemp = "rue";
         ++vIter;

         for( auto const & c : lTemp ) {
            if( vIter == vEnd ) {
               eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
               return false;
            }

            if( c != *vIter ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
               return false;
            }
            ++vIter;
         }

         _currentObject.value_obj.emplace_back( _name, JSON_BOOL );
         _currentObject.value_obj.back().value_bool = true;

         break;
      }

      // false
      case 'f': {
         std::string lTemp = "alse";
         ++vIter;

         for( auto const & c : lTemp ) {
            if( vIter == vEnd ) {
               eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
               return false;
            }

            if( c != *vIter ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
               return false;
            }
            ++vIter;
         }

         _currentObject.value_obj.emplace_back( _name, JSON_BOOL );
         _currentObject.value_obj.back().value_bool = false;

         break;
      }

      // Nil
      case 'n': {
         std::string lTemp = "il";
         ++vIter;

         for( auto const & c : lTemp ) {
            if( vIter == vEnd ) {
               eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
               return false;
            }

            if( c != *vIter ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
               return false;
            }
            ++vIter;
         }

         _currentObject.value_obj.emplace_back( _name, JSON_NULL );
         break;
      }
      case '{':
         ++vIter;
         _currentObject.value_obj.emplace_back( _name, JSON_OBJECT );
         if( !parseObject( _currentObject.value_obj.back() ) )
            return false;

         break;
      case '[':
         ++vIter;
         _currentObject.value_obj.emplace_back( _name, JSON_ARRAY );
         if( ! parseArray( _currentObject.value_obj.back() ) )
            return false;

         break;

         // Number
      default: {
         std::string lNum;
         if( *vIter == '-' ) {
            lNum += '-';
            ++vIter;
         }
         bool lHasDot = false;
         while( vIter != vEnd ) {
            switch( *vIter ) {
               case '.':
                  if( lHasDot ) {
                     eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
                     return false;
                  }
                  lHasDot = true;
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

               case '\n': ++vCurrentLine;
               case '\t':
               case ' ':
               case ',':
                  lFound = true;
                  break;

               default:
                  eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
                  return false;
            }

            if( lFound )
               break;
         }

         // Must have reached EOF
         if( !lFound ) {
            eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
            return false;
         }

         _currentObject.value_obj.emplace_back( _name, JSON_NUMBER );
         _currentObject.value_obj.back().value_num = std::stod( lNum );
      }
   }

   return true;
}

bool uParserJSON::parseArray( uJSON_data &_currentObject ) {
   if( !continueWhitespace() ) {
      eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
      return false;
   }

   if( *vIter == ']' )
      return true;

   if( !parseValue( _currentObject, "" ) )
      return false;

   while( *vIter == ',' ) {
      ++vIter;
      if( !continueWhitespace() ) {
         eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
         return false;
      }

      if( !parseValue( _currentObject, "" ) )
         return false;

      if( !continueWhitespace() ) {
         eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
         return false;
      }
   }

   if( !continueWhitespace() ) {
      eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
      return false;
   }

   if( *vIter != ']' ) {
      eLOG( "Failed parsing file '", vFilePath_str, "': expected ']'" );
      return false;
   }

   ++vIter;
   return true;
}


bool uParserJSON::parseObject( uJSON_data &lCurrentObject ) {
   if( !continueWhitespace() )
      return false;

   // Empty object
   if( *vIter == '}' )
      return true;

   while( true ) {
      if( !continueWhitespace() )
         return false;

      if( *vIter != '"' ) {
         eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
         return true;
      }

      std::string lName;
      bool lFound = false;

      ++vIter;

      // Get Name
      while( vIter != vEnd ) {
         switch( *vIter ) {
            case '\\':
               ++vIter;

               if( vIter == vEnd )
                  break;

               switch( *vIter ) {
                  case '\\': lName += '\\'; ++vIter; break;
                  case '"':  lName += '"';  ++vIter; break;
                  default:
                     eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
                     return false;
               }
            case '"':
               lFound = true;
               ++vIter;
               break;

            case '\n': ++vCurrentLine;
            default:
               lName += *vIter;
               ++vIter;
         }

         if( lFound )
            break;
      }

      continueWhitespace();
      if( *vIter != ':' ) {
         eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
         return false;
      }

      ++vIter;
      continueWhitespace();

      parseValue( lCurrentObject, lName );

      continueWhitespace();
      switch( *vIter ) {
         case '}': ++vIter; return true;
         case ',': ++vIter; break;
         default:
            eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
            return false;
      }
   }

   eLOG( "Failed parsing file '", vFilePath_str, "': unexpected end of file" );
   return false;
}


/*!
 * \brief loads the content of the JSON file
 * \returns 1 on success
 * \returns 2 if there was a parsing error
 * \returns 3 if the JSON file doesn't exists
 * \returns 4 if the JSON file is not a regular file
 * \returns 5 if the JSON file is not readable
 * \returns 6 if already parsed
 */
int uParserJSON::parse() {
   if( vIsParsed )
      return 6;

   uFileIO lFile( vFilePath_str );
   int lRet = lFile();
   if( lRet != 1 ) return lRet;

   vIter = lFile.begin();
   vEnd  = lFile.end();

   bool lHasPrimaryObject = false;

   while( vIter != vEnd ) {
      switch( *vIter ) {
         case ' ':  ++vIter;                 break;
         case '\n': ++vIter; ++vCurrentLine; break;
         case '\t': ++vIter;                 break;
         case '{':
            if( lHasPrimaryObject ) {
               eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine, "; Only one primary object is allowed" );
               return 2;
            }
            ++vIter;

            vData.type = JSON_OBJECT;
            if( !parseObject( vData ) )
               return 2;

            break;
         default:
            eLOG( "Failed parsing file '", vFilePath_str, "' at char '", *vIter, "' Line ", vCurrentLine );
            return 2;
      }
   }

   vIsParsed = true;

   return 1;
}

void uParserJSON::setWriteIndent( std::string _in ) {
   vWriteIndent_str = _in;
}


/*!
 * \brief Generates a JSON file from a uJSON_data structure
 *
 * \param[in] _data              the data to write
 * \param[in] _overwriteIfNeeded When true overwrites the file if it exists
 *
 * \returns 1  if everything went fine
 * \returns 2  if the file already exisits read (and _overWrite == false)
 * \returns 3  if the file exists and is not a regular file
 * \returns 4  if the file exists and is not removable
 * \returns 5  if the file is not writable
 * \returns -2 if _data has not the type JSON_OBJECT
 */
int uParserJSON::write( const uJSON_data &_data, bool _overwriteIfNeeded ) {
   if( _data.type != JSON_OBJECT )
      return -2;

   std::string lOutput = "{\n";
   for( unsigned int i = 0; i < _data.value_obj.size(); ++i ) {
      writeValue( _data.value_obj[i], lOutput, vWriteIndent_str, false );
      if( i != ( _data.value_obj.size() - 1 ) )
         lOutput += ",";

      lOutput += "\n";
   }
   lOutput += "}\n";

   uFileIO lFile( vFilePath_str );
   return lFile.write( lOutput, _overwriteIfNeeded );;
}

void uParserJSON::prepareString( const std::string &_in, std::string &_out ) {
   for( char ch : _in ) {
      if( ch == '"' || ch == '\\' )
         _out.append( 1, '\\' );

      _out.append( 1, ch );
   }
}


void uParserJSON::writeValue( const uJSON_data &_data, std::string &_worker, std::string _level, bool _array ) {
   unsigned int i;
   std::string  lTemp_str;

   if( !_array )
      _worker += _level + "\"" + _data.id + "\"" + ": ";
   else
      _worker += _level;

   switch( _data.type ) {
      case JSON_STRING:
         prepareString( _data.value_str, lTemp_str );
         _worker += "\"" + lTemp_str + "\"";
         break;
      case JSON_NUMBER:
         _worker += boost::lexical_cast<std::string>( _data.value_num );
         break;
      case JSON_BOOL:
         _worker += ( _data.value_bool ? "true" : "false" );
         break;
      case JSON_NULL:
         _worker += "null";
         break;
      case JSON_ARRAY:
         _worker += "[\n";
         for( i = 0; i < _data.value_obj.size(); ++i ) {
            writeValue( _data.value_obj[i], _worker, _level  + vWriteIndent_str, true );
            if( i != ( _data.value_obj.size() - 1 ) )
               _worker += ",";

            _worker += "\n";
         }
         _worker += _level + "]";
         break;
      case JSON_OBJECT:
         _worker += "{\n";
         for( i = 0; i < _data.value_obj.size(); ++i ) {
            writeValue( _data.value_obj[i], _worker, _level  + vWriteIndent_str, false );
            if( i != ( _data.value_obj.size() - 1 ) )
               _worker += ",";

            _worker += "\n";
         }
         _worker += _level + "}";
         break;
      default:
         wLOG( "Incomplete JSON data structure. Skipp unknown parts.  ( File: '", vFilePath_str, "' )" );
         break;
   }
}




}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;




