/*!
 * \file uParserJSON.hpp
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

#ifndef U_PARSER_JSON_HPP
#define U_PARSER_JSON_HPP

#include "uParserJSON_data.hpp"

namespace e_engine {

class uParserJSON {
 private:
   std::string vFilePath_str;
   bool vIsParsed;

   std::string vWriteIndent_str;

   uJSON_data vData;

   std::string::const_iterator vIter;
   std::string::const_iterator vEnd;

   unsigned int vCurrentLine = 1;

   void
   writeValue( uJSON_data const &_data, std::string &_worker, std::string _level, bool _array );
   void prepareString( std::string const &_in, std::string &_out );

   bool continueWhitespace();

   bool parseObject( e_engine::uJSON_data &lCurrentObject );
   bool parseArray( e_engine::uJSON_data &_currentObject );
   bool parseValue( e_engine::uJSON_data &_currentObject, const std::string &_name );

 public:
   virtual ~uParserJSON() {}
   uParserJSON() : vIsParsed( false ), vWriteIndent_str( "  " ) {}
   uParserJSON( std::string _file )
       : vFilePath_str( _file ), vIsParsed( false ), vWriteIndent_str( "  " ) {}

   void setFile( std::string _file );
   int parse();
   void clear();

   int write( uJSON_data const &_data, bool _overwriteIfNeeded = false );

   uJSON_data getData() { return vData; }
   uJSON_data *getDataP() { return &vData; }

   void setWriteIndent( std::string _in );

   bool getIsParsed() const;
   std::string getFilePath() const;
};
}

#endif // U_PARSER_JSON_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
