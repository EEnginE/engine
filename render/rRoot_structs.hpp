/*!
 * \file rRoot_structs.hpp
 */
/*
 * Copyright (C) 2016 EEnginE project
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
 *
 */

#pragma once

#include <thread>
#include <vulkan.h>

namespace e_engine {
namespace internal {
struct CommandPoolInfo {
   std::thread::id tID;
   uint32_t qfIndex;
   VkCommandPoolCreateFlags flags;

   friend inline bool operator==( const CommandPoolInfo &_l, const CommandPoolInfo &_r ) {
      return _l.tID == _r.tID && _l.qfIndex == _r.qfIndex && _l.flags == _r.flags;
   }
};
}
}

namespace std {
template <>
struct hash<e_engine::internal::CommandPoolInfo> {
   typedef e_engine::internal::CommandPoolInfo argument_type;
   typedef std::size_t result_type;
   result_type operator()( argument_type const &s ) const {
      result_type const h1( std::hash<uint32_t>()( s.qfIndex ) );
      result_type const h2( std::hash<std::thread::id>()( s.tID ) );
      result_type const h3( std::hash<int>()( static_cast<int>( s.flags ) ) );
      size_t temp = 0;
      temp ^= h1 + 0x9e3779b9 + ( temp << 6 ) + ( temp >> 2 );
      temp ^= h2 + 0x9e3779b9 + ( temp << 6 ) + ( temp >> 2 );
      temp ^= h3 + 0x9e3779b9 + ( temp << 6 ) + ( temp >> 2 );
      return temp;
   }
};
}
