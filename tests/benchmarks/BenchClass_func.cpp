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

#include "BenchClass.hpp"
#include <engine.hpp>

// Do it here so that the compiler can't do its inline optimization
// for inline we have another function
double BenchClass::funcToCall(int _a, double _b) {
  for (auto i = 0; i < 100; ++i) ++_a;
  return _b * _a;
}

double BenchClass::funcToCallVirtual(int _a, double _b) {
  for (auto i = 0; i < 100; ++i) ++_a;
  return _b * _a;
}

double cFuncToCall(int _a, double _b) {
  for (auto i = 0; i < 100; ++i) ++_a;
  return _b * _a;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
