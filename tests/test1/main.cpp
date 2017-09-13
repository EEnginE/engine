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
#include "config.hpp"
#include "myWorld.hpp"
#include <engine.hpp>
#include <time.h>
#include <typeinfo>

#if WINDOWS
#include <windows.h>
#endif

#define USE_LAYERS 1

using namespace std;
using namespace e_engine;

int main(int argc, char *argv[]) {
  LOG.nameThread(L"MAIN");
  cmdANDinit cmd(argc, argv);

  if (!cmd.parseArgsAndInit()) {
    B_SLEEP(seconds, 1);
    return 1;
  }

  iLOG("User Name:     ", SYSTEM.getUserName());
  iLOG("User Login:    ", SYSTEM.getUserLogin());
  iLOG("Home:          ", SYSTEM.getUserHomeDirectory());
  iLOG("Main config:   ", SYSTEM.getMainConfigDirPath());
  iLOG("Log File Path: ", SYSTEM.getLogFilePath());

  iInit start;

  std::vector<std::string> layers = {"VK_LAYER_LUNARG_parameter_validation",
                                     "VK_LAYER_LUNARG_object_tracker",
                                     "VK_LAYER_LUNARG_core_validation",
                                     "VK_LAYER_GOOGLE_threading",
                                     "VK_LAYER_GOOGLE_unique_objects"};

#if USE_LAYERS
  start.enableVulkanDebug();
#else
  layers.clear();
#endif

  if (start.init(layers) == 0) {
    myWorld handler(cmd, &start);
    start.enableDefaultGrabControl();

    if (handler.initGL() == 0)
      handler.waitForEnd();
  }

  start.destroy();

  return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
