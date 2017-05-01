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

#ifndef CMDANDINIT_HPP
#define CMDANDINIT_HPP

#include <engine.hpp>
#include <string>
#include <vector>

#include "testStarter.hpp"

class cmdANDinit {
 private:
  std::vector<std::string> args;
  std::string              argv0;
  std::string              dataRoot;

  std::vector<std::string> outputFiles;

  bool vCanUseColor;

  cmdANDinit() {}

  void usage();

 public:
  cmdANDinit(int argc, char *argv[], testStarter &_starter, bool &_errors);

  std::string getDataRoot() const { return dataRoot; }

  bool parseArgsAndInit(testStarter &_starter);
  void generate(e_engine::uJSON_data &_data);
};

#endif // CMDANDINIT_HPP

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
