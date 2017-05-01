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

#ifndef BENCHCLASS_H
#define BENCHCLASS_H

#include "cmdANDinit.hpp"
#include <functional>

class BenchBaseVirtual {
 public:
  virtual double funcToCallVirtual(int _a, double _b)       = 0;
  virtual double funcToCallInlineVirtual(int _a, double _b) = 0;

  virtual ~BenchBaseVirtual();
};


class BenchClass : public BenchBaseVirtual {
 private:
  // --- function Bench ---
  unsigned int vLoopsToDo;

  e_engine::uSignal<double, int, double> vTheSignal;
  e_engine::uSlot<double, BenchClass, int, double> vTheSlot;

  e_engine::uSignal<double, int, double> vTheSignalInline;
  e_engine::uSlot<double, BenchClass, int, double> vTheSlotInline;

  double (BenchClass::*vFunctionPointer)(int, double);
  double (BenchClass::*vFunctionPointerInline)(int, double);

  double (*vCFunctionPointer)(int, double);
  double (*vCFunctionPointerInline)(int, double);

  std::function<double(int, double)> vStdFunc;
  std::function<double(int, double)> vStdFuncInline;

  double funcToCall(int _a, double _b);
  inline double funcToCallInline(int _a, double _b) {
    for (auto i = 0; i < 100; ++i) ++_a;
    return _b * _a;
  }

  virtual double funcToCallVirtual(int _a, double _b);
  virtual double funcToCallInlineVirtual(int _a, double _b) {
    for (auto i = 0; i < 100; ++i) ++_a;
    return _b * _a;
  }

  std::mutex   bMutex;
  unsigned int vLoopsToDoMutex;

  double funcNormal(int a, double b);
  double funcMutex(int a, double b);
  double funcLockGuard(int a, double b);

  unsigned int vLoopsToDoCast;

  void doFunction();
  void doMutex();

 public:
  BenchClass() = delete;
  BenchClass(cmdANDinit *_cmd);
};

double cFuncToCall(int _a, double _b);
inline double cFuncToCallInline(int _a, double _b) {
  for (auto i = 0; i < 100; ++i) ++_a;
  return _b * _a;
}

#endif // BENCHCLASS_H

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
