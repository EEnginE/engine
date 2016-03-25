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

#include "config.hpp"
#include "myWorld.hpp"
#include "cmdANDinit.hpp"
#include <time.h>
#include <engine.hpp>
#include <typeinfo>

#if WINDOWS
#include <windows.h>
#endif

using namespace std;
using namespace e_engine;

int main( int argc, char *argv[] ) {
   LOG.nameThread( L"MAIN" );
   cmdANDinit cmd( argc, argv );

   if ( !cmd.parseArgsAndInit() ) {
      B_SLEEP( seconds, 1 );
      return 1;
   }

   iLOG( "User Name:     ", SYSTEM.getUserName() );
   iLOG( "User Login:    ", SYSTEM.getUserLogin() );
   iLOG( "Home:          ", SYSTEM.getUserHomeDirectory() );
   iLOG( "Main config:   ", SYSTEM.getMainConfigDirPath() );
   iLOG( "Log File Path: ", SYSTEM.getLogFilePath() );

   iInit start;

   std::vector<std::string> layers = {
      "VK_LAYER_LUNARG_device_limits",
      "VK_LAYER_LUNARG_draw_state",
      "VK_LAYER_LUNARG_image",
      "VK_LAYER_LUNARG_mem_tracker",
      "VK_LAYER_LUNARG_object_tracker",
      "VK_LAYER_LUNARG_param_checker",
      "VK_LAYER_LUNARG_swapchain",
      "VK_LAYER_GOOGLE_threading",
      "VK_LAYER_GOOGLE_unique_objects",
//      "VK_LAYER_VALVE_steam_overlay_32",
      "VK_LAYER_VALVE_steam_overlay_64",
      "VK_LAYER_LUNARG_standard_validation"
   };

   //start.enableVulkanDebug();

   if ( start.init(layers) == 0 ) {
      rRoot lRoot(&start);
      lRoot.init();

      myWorld handler( cmd, &start );
      start.enableDefaultGrabControl();

      lRoot.getCommandPoolFlags( VK_QUEUE_GRAPHICS_BIT );

      if ( handler.initGL() == 0 )
         start.startMainLoop();
   }

   start.shutdown();

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
