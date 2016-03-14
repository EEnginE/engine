/*!
 * \file uParserHelper.hpp
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

#ifndef U_PARSER_HELPER_HPP
#define U_PARSER_HELPER_HPP

#include "defines.hpp"

#include <string>

namespace e_engine {
namespace internal {

class UTILS_API uParserHelper {
 protected:
   std::string vFilePath_str;
   bool vIsParsed = false;

   std::string::const_iterator vIter;
   std::string::const_iterator vEnd;

   unsigned int vCurrentLine = 1;

   bool continueWhitespace( bool _quiet = false );
   bool expect( char _c, bool _continueWhitespace = true, bool _quiet = false );
   bool expect( std::string _str, bool _continueWhitespace = true, bool _quiet = false );
   bool getString( std::string &_str, bool _continueWhitespace = true, bool _quiet = false );
   bool getNum( double &_num, bool _quiet = false );
   bool getNum( float &_num, bool _quiet = false );
   bool getNum( int &_num, bool _quiet = false );
   bool getNum( unsigned int &_num, bool _quiet = false );
   bool getNum( unsigned short &_num, bool _quiet = false );

   bool eofError();
   bool unexpectedCharError();

   virtual bool load_IMPL() = 0;

 public:
   virtual ~uParserHelper();
   uParserHelper() {}
   uParserHelper( std::string _file ) : vFilePath_str( _file ) {}

   int parse();
   int parseString( std::string _data );

   int operator()() { return parse(); }

   bool getIsParsed() const;
   std::string getFilePath() const;
   void setFile( std::string _file );
};
}
}

#endif
