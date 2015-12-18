/*!
 * \file rLoaderBase.hpp
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
#include "rLoaderStructs.hpp"

namespace e_engine {

namespace internal {

typedef _3D_Data_RAW<GLfloat, GLushort> _3D_Data_RAWF;
typedef _3D_Data_RAW<GLdouble, GLushort> _3D_Data_RAWD;

typedef _3D_Data<GLfloat, GLushort> _3D_DataF;
typedef _3D_Data<GLdouble, GLushort> _3D_DataD;

template <class T, class I>
class rLoaderBase : public uParserHelper {
   static_assert( std::is_floating_point<T>::value, "T must be a floating point type" );
   static_assert( std::is_unsigned<I>::value, "I must be an unsigned type" );

   using S_TYP = _3D_Engine<T, I>;
   using DATA  = std::shared_ptr<S_TYP>;

 protected:
   DATA vData;

 public:
   virtual ~rLoaderBase() {}
   rLoaderBase() {}
   rLoaderBase( std::string _file ) : uParserHelper( _file ), vData( std::make_shared<S_TYP>() ) {}

   template <class C, class... ARGS>
   bool generateObjects( std::vector<C> &_output, ARGS &&... _args );

   void unLoad();
};

template <class T, class I>
template <class C, class... ARGS>
bool rLoaderBase<T, I>::generateObjects( std::vector<C> &_output, ARGS &&... _args ) {
   if ( !vData )
      return false;

   _output.emplace_back( std::forward<ARGS>( _args )..., vData );
   return true;
}


/*!
 * \brief Clears the memory
 */
template <class T, class I>
void rLoaderBase<T, I>::unLoad() {
   vIsParsed = false;
   if ( vData )
      vData.clear();
}
}
}


#endif // R_LOADER_BASE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
