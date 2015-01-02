/*!
 * \file uFileIO.hpp
 * \brief \b Classes: \a uFileIO
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

#ifndef U_FILE_IO_HPP
#define U_FILE_IO_HPP

#include <string>
#include <vector>

namespace e_engine {

class uFileIO {
 public:
   typedef std::string::const_iterator C_ITERATOR;
   typedef std::string::iterator ITERATOR;

   typedef std::string TYPE;

 private:
   std::string vFilePath_str;
   TYPE vData;
   bool vFileRead_B;

 public:
   uFileIO() : vFileRead_B( false ) {}
   uFileIO( std::string _file ) : vFilePath_str( _file ), vFileRead_B( false ) {}
   void setFilePath( std::string _file );
   std::string getFilePath();

   C_ITERATOR begin() const { return vData.begin(); }
   ITERATOR begin() { return vData.begin(); }

   C_ITERATOR end() const { return vData.end(); }
   ITERATOR end() { return vData.end(); }

   bool isFileRead() { return vFileRead_B; }

   int read( bool _autoReload = true );
   int write( TYPE const &_data, bool _overWrite = false );
   void clear();

   int operator()( bool _autoReload = true ) { return read( _autoReload ); }

   virtual ~uFileIO() {}
};
}

#endif // U_FILE_IO_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
