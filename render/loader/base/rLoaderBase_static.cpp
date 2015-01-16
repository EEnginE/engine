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

#include "defines.hpp"
#include "rLoaderBase_static.hpp"

namespace e_engine {
namespace internal {

rLoaderBase_static::~rLoaderBase_static() {}

/*!
 * \brief loads the 3D content frome the OBJ file
 * \returns 1 on success
 * \returns 2 if there was a parsing error
 * \returns 3 if the File file doesn't exists
 * \returns 4 if the File file is not a regular file
 * \returns 5 if the File file is not readable
 * \returns 6 if already loaded
 */
int rLoaderBase_static::load() {
   if ( vIsDataLoaded_B )
      return 6;

   int lRet = vFile();

   if ( lRet != 1 )
      return lRet;

   return load_IMPL();
}

/*!
 * \brief Gets wether or not the file is loaded and parsed
 * \returns The state of the file being loaded and parsed
 */
bool rLoaderBase_static::getIsLoaded() const { return vIsDataLoaded_B; }

/*!
 * \brief Gets the path of the file to parse
 * \returns The path of the file to parse
 */
std::string rLoaderBase_static::getFilePath() const { return vFilePath_str; }


/*!
 * \brief Sets the file to load
 * \param[in] _file The file to load
 *
 * \note This will NOT load the file! You have to manually load it with load()
 */
void rLoaderBase_static::setFile( std::string _file ) {
   vFilePath_str = _file;
   vFile.setFilePath( _file );
}
}
}
