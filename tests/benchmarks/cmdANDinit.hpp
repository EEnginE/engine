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

#ifndef CMDANDINIT_H
#define CMDANDINIT_H

#include <engine.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace e_engine;

class cmdANDinit {
 private:
   vector<string> args;
   string argv0;

   bool vCanUseColor;

   bool vDoFunction;
   unsigned int vFunctionLoops;

   bool vDoMutex;
   unsigned int vMutexLoops;

   cmdANDinit() {}

   void postInit();
   void preInit();
   void usage();

 public:
   cmdANDinit( int argc, char *argv[], bool _color );

   bool parseArgsAndInit();

   void getFunctionInf( unsigned int &_loops, bool &_doIt ) {
      _loops = vFunctionLoops;
      _doIt = vDoFunction;
   }
   void getMutexInf( unsigned int &_loops, bool &_doIt ) {
      _loops = vMutexLoops;
      _doIt = vDoMutex;
   }
};

#endif // CMDANDINIT_H
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
