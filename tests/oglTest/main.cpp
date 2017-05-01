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

#include "cmdANDinit.hpp"
#include "testStarter.hpp"

using namespace e_engine;

int main(int argc, char *argv[]) {
  LOG.nameThread(L"MAIN");
  testStarter tests;
  bool        errors;

  cmdANDinit parseCMD(argc, argv, tests, errors);

  if (errors) {
    LOG.devInit();
    LOG.startLogLoop();
    LOG.stopLogLoop();
    return 0;
  }

  iInit init;

  uJSON_data data;

  if (init.init() == 1) {
    data("oglTest", "init", "canCreateWindow", S_BOOL(true));
  } else {
    eLOG("Failed to create a window");
    data("oglTest", "init", "canCreateWindow", S_BOOL(false));
  }

  data("oglTest",
       "init",
       "version",
       "major",
       S_NUM(1),
       "oglTest",
       "init",
       "version",
       "minor",
       S_NUM(0)); //! \todo replace with vulkan versions

  tests.run(data, parseCMD.getDataRoot());

  parseCMD.generate(data);

  iLOG("");
  iLOG(
      "Tipp: You can use the output of 'oglTestBindings.sh' to 'parse' all data this program "
      "produced into GlobConf");
  iLOG("");

  B_SLEEP(milliseconds, 100); /// \todo Fix this segfault hack

  init.shutdown();

  return 0;
}


/*
 * Begin recommended Bindings
 *
 * Syntax: '//#!BIND ' <location in json file> , G_<TYPE>( <GlobConf value>, <default> )
 */


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
