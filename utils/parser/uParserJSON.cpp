/*!
 * \file uParserJSON.cpp
 * \brief \b Classes: \a uParserJSON
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

#include "uParserJSON.hpp"

#include "uLog.hpp"
#include "uFileIO.hpp"


namespace {
typedef std::vector<::e_engine::uJSON_data> VALUES;
}

namespace e_engine {

uParserJSON::~uParserJSON() {}

/*!
 * \brief Clears the memory
 */
void uParserJSON::clear() {
   vIsParsed = false;
   vData.value_str.clear();
   vData.id.clear();
   vData.value_obj.clear();
   vData.value_obj.clear();
   vData.type   = __JSON_NOT_SET__;
   vCurrentLine = 0;
}

bool uParserJSON::parseValue( e_engine::uJSON_data &_currentObject, const std::string &_name ) {
   switch ( *vIter ) {

      // String
      case '"':
         _currentObject.value_obj.emplace_back( _name, JSON_STRING );
         if ( !getString( _currentObject.value_obj.back().value_str ) )
            return false;

         break;

      // true
      case 't':
         ++vIter;
         if ( !expect( "rue" ) )
            return false;

         _currentObject.value_obj.emplace_back( _name, JSON_BOOL );
         _currentObject.value_obj.back().value_bool = true;
         break;

      // false
      case 'f':
         ++vIter;
         if ( !expect( "alse" ) )
            return false;

         _currentObject.value_obj.emplace_back( _name, JSON_BOOL );
         _currentObject.value_obj.back().value_bool = false;
         break;

      // Nil
      case 'n':
         ++vIter;
         if ( !expect( "il", true, true ) )
            if ( !expect( "ull" ) )
               return false;

         _currentObject.value_obj.emplace_back( _name, JSON_NULL );
         break;

      case '{':
         ++vIter;
         _currentObject.value_obj.emplace_back( _name, JSON_OBJECT );
         if ( !parseObject( _currentObject.value_obj.back() ) )
            return false;

         break;
      case '[':
         ++vIter;
         _currentObject.value_obj.emplace_back( _name, JSON_ARRAY );
         if ( !parseArray( _currentObject.value_obj.back() ) )
            return false;

         break;

      // Number
      default: {
         _currentObject.value_obj.emplace_back( _name, JSON_NUMBER );
         auto lTempIter = vIter;
         if ( getNum( _currentObject.value_obj.back().value_int, true ) ) {
            _currentObject.value_obj.back().type = JSON_INT;
            _currentObject.value_obj.back().value_num =
                  static_cast<double>( _currentObject.value_obj.back().value_int );
            break;
         }

         vIter = lTempIter;
         if ( !getNum( _currentObject.value_obj.back().value_num ) )
            return false;

         break;
      }
   }
   return true;
}

bool uParserJSON::parseArray( uJSON_data &_currentObject ) {
   if ( !continueWhitespace() )
      return false;

   // Empty array
   if ( *vIter == ']' ) {
      vIter++;
      return true;
   }

   if ( !parseValue( _currentObject, "" ) )
      return false;

   while ( expect( ',', true, true ) )
      if ( !parseValue( _currentObject, "" ) )
         return false;

   return expect( ']' );
}


bool uParserJSON::parseObject( uJSON_data &lCurrentObject ) {
   if ( !continueWhitespace() )
      return false;

   // Empty object
   if ( *vIter == '}' ) {
      vIter++;
      return true;
   }

   std::string lName;

   while ( true ) {
      lName.clear();

      if ( !getString( lName ) )
         return false;

      if ( !expect( ':' ) )
         return false;

      parseValue( lCurrentObject, lName );

      if ( expect( ',', true, true ) )
         continue;

      if ( expect( '}', false ) )
         break;

      return unexpectedCharError();
   }

   return true;
}



bool uParserJSON::load_IMPL() {
   if ( !expect( '{' ) )
      return false;

   vData.type = JSON_OBJECT;
   if ( !parseObject( vData ) )
      return false;

   while ( vIter != vEnd ) {
      switch ( *vIter ) {
         case '\n': vCurrentLine++; FALLTHROUGH;
         case '\t':
         case ' ': ++vIter; break;
         default: return unexpectedCharError();
      }
   }

   return true;
}

void uParserJSON::setWriteIndent( std::string _in ) { vWriteIndent_str = _in; }

std::string uParserJSON::toString( uJSON_data const &_data ) {
   if ( _data.type != JSON_OBJECT )
      return "";

   std::string lOutput = "{\n";
   for ( unsigned int i = 0; i < _data.value_obj.size(); ++i ) {
      writeValue( _data.value_obj[i], lOutput, vWriteIndent_str, false );
      if ( i != ( _data.value_obj.size() - 1 ) )
         lOutput += ",";

      lOutput += "\n";
   }
   lOutput += "}\n";

   return lOutput;
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
   if ( _data.type != JSON_OBJECT )
      return -2;

   std::string lOutput = "{\n";
   for ( unsigned int i = 0; i < _data.value_obj.size(); ++i ) {
      writeValue( _data.value_obj[i], lOutput, vWriteIndent_str, false );
      if ( i != ( _data.value_obj.size() - 1 ) )
         lOutput += ",";

      lOutput += "\n";
   }
   lOutput += "}\n";

   uFileIO lFile( vFilePath_str );
   return lFile.write( lOutput, _overwriteIfNeeded );
}

void uParserJSON::prepareString( const std::string &_in, std::string &_out ) {
   for ( char ch : _in ) {
      if ( ch == '"' || ch == '\\' )
         _out.append( 1, '\\' );

      _out.append( 1, ch );
   }
}


void uParserJSON::writeValue( const uJSON_data &_data,
                              std::string &_worker,
                              std::string _level,
                              bool _array ) {
   unsigned int i;
   std::string lTemp_str;

   if ( !_array )
      _worker += _level + "\"" + _data.id + "\"" + ": ";
   else
      _worker += _level;

   switch ( _data.type ) {
      case JSON_STRING:
         prepareString( _data.value_str, lTemp_str );
         _worker += "\"" + lTemp_str + "\"";
         break;
      case JSON_NUMBER: _worker += std::to_string( _data.value_num ); break;
      case JSON_INT: _worker += std::to_string( _data.value_int ); break;
      case JSON_BOOL: _worker += ( _data.value_bool ? "true" : "false" ); break;
      case JSON_NULL: _worker += "null"; break;
      case JSON_ARRAY:
         _worker += "[\n";
         for ( i = 0; i < _data.value_obj.size(); ++i ) {
            writeValue( _data.value_obj[i], _worker, _level + vWriteIndent_str, true );
            if ( i != ( _data.value_obj.size() - 1 ) )
               _worker += ",";

            _worker += "\n";
         }
         _worker += _level + "]";
         break;
      case JSON_OBJECT:
         _worker += "{\n";
         for ( i = 0; i < _data.value_obj.size(); ++i ) {
            writeValue( _data.value_obj[i], _worker, _level + vWriteIndent_str, false );
            if ( i != ( _data.value_obj.size() - 1 ) )
               _worker += ",";

            _worker += "\n";
         }
         _worker += _level + "}";
         break;
      case __JSON_FAIL__:
      case __JSON_NOT_SET__:
         wLOG( "Incomplete JSON data structure. Skipp unknown parts.  ( File: '",
               vFilePath_str,
               "' )" );
         break;
   }
}
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
