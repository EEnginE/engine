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

#include "uRandomISAAC.hpp"
#include <chrono>
#if defined __liunx__
#include <sys/time.h>
#else
#include <ctime>
#endif // __liunx__

#define mix(a, b, c, d, e, f, g, h) \
  {                                 \
    a ^= b << 11;                   \
    d += a;                         \
    b += c;                         \
    b ^= c >> 2;                    \
    e += b;                         \
    c += d;                         \
    c ^= d << 8;                    \
    f += c;                         \
    d += e;                         \
    d ^= e >> 16;                   \
    g += d;                         \
    e += f;                         \
    e ^= f << 10;                   \
    h += e;                         \
    f += g;                         \
    f ^= g >> 4;                    \
    a += f;                         \
    g += h;                         \
    g ^= h << 8;                    \
    b += g;                         \
    h += a;                         \
    h ^= a >> 9;                    \
    c += h;                         \
    a += b;                         \
  }

namespace e_engine {

void uRandomISAAC::init(uint32_t _seed) {
  uint32_t lSeed_ulI[256];
  uint32_t lTime_ulI;

  step = 0;

  unsigned int x, y, z, w;

  if (_seed != 0) {
    lTime_ulI = _seed;
  } else {
#if defined __liunx__
    // In linux we can use time in usec and can set this as the first seed
    timeval lTV_tv;
    gettimeofday(&lTV_tv, NULL);
    // Now mix a little bit
    lTime_ulI = (lTV_tv.tv_sec * 1000) + (lTV_tv.tv_usec / 1000);
#else
    // Unfortunately we need to use this method on other systems
    lTime_ulI = static_cast<uint32_t>(std::time(nullptr) * 1000);
#endif // __liunx__
  }

  std::chrono::system_clock::time_point lStart_bcTP = std::chrono::system_clock::now();

  // Init xorshift seeds:
  x = (lTime_ulI % 2 == 0) ? (lTime_ulI * lTime_ulI) / 7 : lTime_ulI * 7 + lTime_ulI * (lTime_ulI % 3);
  y = (lTime_ulI % 3 == 0 || x % 2 == 1) ? ((lTime_ulI << 3) + (lTime_ulI * 6) * lTime_ulI) : lTime_ulI * x;
  z = (x < y || y % 3 == 0) ? lTime_ulI * (y << x) : (x * (y % 2)) + lTime_ulI * x;
  w = ((x * y % 2) == 0) ? lTime_ulI * z + z * (x >> z) : (y << z) * x;

  for (auto &elem : lSeed_ulI) {
    unsigned int t;
    t = x ^ (x << 11);
    x = y;
    y = z;
    z = w;
    w ^= (w >> 19) ^ t ^ (t >> 8);
    elem = w;
  }

  mixUp(lSeed_ulI);

  std::chrono::nanoseconds lNSec_bcNSEC = std::chrono::system_clock::now() - lStart_bcTP;

  for (unsigned int i = 0; i < lNSec_bcNSEC.count(); ++i) {
    // make some random numbers
    get();
  }

  for (auto &elem : lSeed_ulI)
    elem = get(); // this should generate the final complete random seeds

  mixUp(lSeed_ulI);
}

void uRandomISAAC::mixUp(uint32_t _seed[256]) {
  int      i;
  uint32_t a, b, c, d, e, f, g, h;
  aa = bb = cc = 0;
  a = b = c = d = e = f = g = h = 0x9e3779b9; // the golden ratio

  for (i = 0; i < 8; ++i) // scramble it
    mix(a, b, c, d, e, f, g, h);

  for (i = 0; i < 256; i += 8) { // fill in mm[] with messy stuff
    // use all the information in the seed
    a += _seed[i];
    b += _seed[i + 1];
    c += _seed[i + 2];
    d += _seed[i + 3];
    e += _seed[i + 4];
    f += _seed[i + 5];
    g += _seed[i + 6];
    h += _seed[i + 7];

    mix(a, b, c, d, e, f, g, h);

    mm[i]     = a;
    mm[i + 1] = b;
    mm[i + 2] = c;
    mm[i + 3] = d;
    mm[i + 4] = e;
    mm[i + 5] = f;
    mm[i + 6] = g;
    mm[i + 7] = h;
  }

  for (i = 0; i < 256; i += 8) { // do a second pass to make the seed affect all of mm
    a += mm[i];
    b += mm[i + 1];
    c += mm[i + 2];
    d += mm[i + 3];
    e += mm[i + 4];
    f += mm[i + 5];
    g += mm[i + 6];
    h += mm[i + 7];

    mix(a, b, c, d, e, f, g, h);

    mm[i]     = a;
    mm[i + 1] = b;
    mm[i + 2] = c;
    mm[i + 3] = d;
    mm[i + 4] = e;
    mm[i + 5] = f;
    mm[i + 6] = g;
    mm[i + 7] = h;
  }

  // count and discard the first 256 numbers
  for (unsigned int j = 0; j < 256; ++j)
    get();
}


uint32_t uRandomISAAC::get() {
  uint32_t x, y;

  if (!step) {
    cc += 1;  /* cc just gets incremented once per 256 results */
    bb += cc; /* then combined with bb */
  }

  x = mm[step];
  switch (step % 4) {
    case 0: aa = aa ^ (aa << 13); break;
    case 1: aa = aa ^ (aa >> 6); break;
    case 2: aa = aa ^ (aa << 2); break;
    case 3: aa = aa ^ (aa >> 16); break;
  }
  aa       = mm[(step + 128) % 256] + aa;
  mm[step] = y = mm[(x >> 2) % 256] + aa + bb;
  bb           = mm[(y >> 10) % 256] + x;

  /* Note that bits 2..9 are chosen from x but 10..17 are chosen
     from y.  The only important thing here is that 2..9 and 10..17
     don't overlap.  2..9 and 10..17 were then chosen for speed in
     the optimized version (rand.c) */
  /* See http://burtleburtle.net/bob/rand/isaac.html
     for further explanations and analysis. */
  ++step;
  return bb;
}
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
