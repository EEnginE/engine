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

#pragma once

#include "uParserHelper.hpp"
#include "uParserJSON_data.hpp"

namespace e_engine {

class UTILS_API uParserJSON final : public internal::uParserHelper {
 private:
   std::string vWriteIndent_str;

   uJSON_data vData;

   void writeValue( uJSON_data const &_data, std::string &_worker, std::string _level, bool _array );
   void prepareString( std::string const &_in, std::string &_out );

   bool parseObject( e_engine::uJSON_data &lCurrentObject );
   bool parseArray( e_engine::uJSON_data &_currentObject );
   bool parseValue( e_engine::uJSON_data &_currentObject, const std::string &_name );

   bool load_IMPL();

 public:
   virtual ~uParserJSON();
   uParserJSON() : vWriteIndent_str( "   " ) {}
   uParserJSON( std::string _file ) : uParserHelper( _file ), vWriteIndent_str( "   " ) {}

   void clear();

   int write( uJSON_data const &_data, bool _overwriteIfNeeded = false );

   std::string toString( uJSON_data const &_data );

   uJSON_data  getData() { return vData; }
   uJSON_data *getDataP() { return &vData; }

   void setWriteIndent( std::string _in );
};
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
