/*!
 * \file lLoaderBase.hpp
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

#ifndef R_LOADER_BASE_HPP
#define R_LOADER_BASE_HPP

#include "defines.hpp"

#include <type_traits>
#include <memory>
#include "uParserHelper.hpp"

namespace e_engine {

namespace internal {

template <class T, class I>
class lLoaderBase : public uParserHelper {
   static_assert( std::is_floating_point<T>::value, "T must be a floating point type" );
   static_assert( std::is_unsigned<I>::value, "I must be an unsigned type" );

 public:
   virtual ~lLoaderBase() {}
   lLoaderBase() {}
   lLoaderBase( std::string _file ) : uParserHelper( _file ) {}

   virtual void unLoad() = 0;
};
}
}


#endif // R_LOADER_BASE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
