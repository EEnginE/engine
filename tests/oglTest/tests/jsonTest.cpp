/*!
 * \file jsonTest.cpp
 * \brief \b Classes: \a jsonTest
 *
 * Class for testing:
 * Tests the JSON parser
 *
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

#include "jsonTest.hpp"

using namespace std;
using namespace e_engine;

const string jsonTest::desc = "Tests the JSON parser";

void jsonTest::runTest( uJSON_data &_data, string _dataRoot ) {
   uParserJSON lParser( ( _dataRoot + "testJSON.json" ) );

   _data( "utils", "jsonParser", "contentOK", S_BOOL( false ) );
   _data( "utils", "jsonParser", "works", S_BOOL( false ) );
   _data( "utils", "jsonParser", "parses", S_BOOL( false ) );

   if ( lParser.parse() != 1 ) {
      eLOG( "Parsing error" );
      return;
   }

   _data( "utils", "jsonParser", "parses", S_BOOL( true ) );

   bool b1 = true;
   string str1;
   double n[4];

   auto lData = lParser.getData();

   lData( "string", G_STR( str1, "" ) );
   lData( "obj1", "bool", G_BOOL( b1, true ) );
   lData( "obj1", "double", G_NUM( n[0], 0 ) );
   lData( "obj1", "array", 0u, G_NUM( n[1], 0 ) );
   lData( "obj1", "array", 1u, G_NUM( n[2], 0 ) );
   lData( "obj1", "array", 2u, G_NUM( n[3], 0 ) );

   if ( str1 != " I am \\ a \"string\"" ) {
      eLOG( "String parsing error" );
      return;
   }

   if ( b1 != false ) {
      eLOG( "bool parsing error" );
      return;
   }

   if ( n[0] != -12.23123 ) {
      eLOG( "Number parsing error" );
      return;
   }

   if ( n[1] != 1 ) {
      eLOG( "Number / array parsing error" );
      return;
   }

   if ( n[2] != 2 ) {
      eLOG( "Number / array parsing error" );
      return;
   }

   if ( n[3] != 42 ) {
      eLOG( "Number / array parsing error" );
      return;
   }

   _data( "utils", "jsonParser", "contentOK", S_BOOL( true ) );
   _data( "utils", "jsonParser", "works", S_BOOL( true ) );
}

/*
 * Begin recommended Bindings
 *
 * Syntax: '//#!BIND DATA(' <location in json file> , G_<TYPE>( <GlobConf value>, <default> );
 */
