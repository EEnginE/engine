/*!
 * \file uRandomISAAC.hpp
 * \brief \b Classes: \a uRandomISAAC
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

#include "defines.hpp"
#include <stdint.h>

namespace e_engine {

class uRandomISAAC {
 private:
  uint32_t mm[256];
  uint32_t aa;
  uint32_t bb;
  uint32_t cc;

  uint8_t step;

  void mixUp(uint32_t _seed[256]);

 public:
  uRandomISAAC() { init(0); }
  uRandomISAAC(uint32_t _seed) { init(_seed); }

  void init(uint32_t _seed = 0);

  uint32_t        get();
  inline uint32_t get(uint32_t _min, uint32_t _max) {
    return (_max <= _min) ? _min : (get() % ((_max + 1) - _min) + _min);
  }

  inline uint32_t operator()() { return get(); }
  inline uint32_t operator()(uint32_t _min, uint32_t _max) { return get(_min, _max); }
};
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
