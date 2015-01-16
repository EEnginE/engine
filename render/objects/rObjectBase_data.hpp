/*!
 * \file rObjectBase_data.hpp
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

#ifndef R_OBJECT_BASE_DATA_HPP
#define R_OBJECT_BASE_DATA_HPP

#include "defines.hpp"

#include "rLoaderBase.hpp"
#include "rObjectBase.hpp"

namespace e_engine {

template <class T, class D>
class rObjectBase_data {
 protected:
   D vData; //!< The stored data.

 public:
   template <class I>
   rObjectBase_data( internal::_3D_Data<T, I> const &_data )
       : vData( _data ) {}

   rObjectBase_data() = delete;
};
}

#endif
