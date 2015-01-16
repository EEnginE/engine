/*!
 * \file rLoaderBase_static.hpp
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

#ifndef R_LOADER_BASE_STATIC_HPP
#define R_LOADER_BASE_STATIC_HPP

#include "defines.hpp"
#include "uFileIO.hpp"

#include <string>

namespace e_engine {
namespace internal {

class rLoaderBase_static {
 protected:
   bool vIsDataLoaded_B;
   std::string vFilePath_str;

   uFileIO vFile;

   virtual int load_IMPL() = 0;

 public:
   virtual ~rLoaderBase_static();

   rLoaderBase_static() : vIsDataLoaded_B( false ) {}
   rLoaderBase_static( std::string _file )
       : vIsDataLoaded_B( false ), vFilePath_str( _file ), vFile( _file ) {}

   void setFile( std::string _file );

   bool getIsLoaded() const;
   std::string getFilePath() const;

   int load();
   int operator()() { return load(); }
};
}
}

#endif // R_LOADER_BASE_STATIC_HPP
