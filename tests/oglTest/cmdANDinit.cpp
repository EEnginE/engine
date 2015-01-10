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
#include <regex>

using namespace std;
using namespace e_engine;

cmdANDinit::cmdANDinit( int argc, char *argv[], testStarter &_starter, bool &_errors ) {
   argv0 = argv[0];
   vCanUseColor = COLOR == 1 ? true : false;
#if WINDOWS
   dataRoot = INSTALL_PREFIX + string( "\\share\\engineTests\\oglTest\\data\\" );
#else
   dataRoot = INSTALL_PREFIX + string( "/share/engineTests/oglTest/data/" );
#endif

   for ( auto i = 1; i < argc; ++i ) {
      args.push_back( argv[i] );
   }

   GlobConf.win.width = 800;
   GlobConf.win.height = 600;
   GlobConf.win.fullscreen = false;
   GlobConf.win.windowName = "OpenGL_TEST";
   GlobConf.win.iconName = "ICON is missing";
   GlobConf.win.xlibWindowName = "My icon";
   // GlobConf.win.winType         = e_engine::TOOLBAR;
   GlobConf.useAutoOpenGLVersion();
   GlobConf.config.appName = "OpenGL_TEST";


   GlobConf.win.restoreOldScreenRes = true;

   GlobConf.versions.glMajorVersion = 4;
   GlobConf.versions.glMinorVersion = 6;


   if ( vCanUseColor ) {
      GlobConf.log.logOUT.colors = FULL;
      GlobConf.log.logERR.colors = FULL;
   } else {
      GlobConf.log.logOUT.colors = DISABLED;
      GlobConf.log.logERR.colors = DISABLED;

      GlobConf.log.width = 175;
   }
   GlobConf.log.logOUT.Time = LEFT_REDUCED;
   GlobConf.log.logOUT.File = RIGHT_FULL;
   GlobConf.log.logERR.Time = LEFT_REDUCED;
   GlobConf.log.logERR.File = RIGHT_FULL;
   GlobConf.log.logFILE.File = RIGHT_FULL;

   GlobConf.log.logFILE.logFileName = SYSTEM.getLogFilePath();
#if UNIX
   GlobConf.log.logFILE.logFileName += "/Log";
#elif WINDOWS
   GlobConf.log.logFILE.logFileName += "\\Log";
#endif

   GlobConf.log.waitUntilLogEntryPrinted = false;

   _errors = !parseArgsAndInit( _starter );
}



void cmdANDinit::usage() {
   iLOG( "Usage: ", argv0, " [OPTIONS] OUTPUT_FILES" );
   iLOG( "" );
   iLOG( "OPTIONS:" );
   dLOG( "    -h | --help      : show this help message" );
   dLOG( "    --log=<path>     : set a custom log file path to <path>" );
   dLOG( "    -w | --wait      : wait until log entry is printed" );
   dLOG( "    --data=<path>    : set a custom root path for the data dir" );
   dLOG( "    --glMajor=<v>    : the OpenGL Major version (default: ",
         GlobConf.versions.glMajorVersion,
         ")" );
   dLOG( "    --glMinor=<v>    : the OpenGL Major version (default: ",
         GlobConf.versions.glMinorVersion,
         ")" );
   if ( vCanUseColor ) {
      dLOG( "    -n | --nocolor   : disable colored output" );
   }
   dLOG( "    -l | --list      : lists all available tests" );
   dLOG( "    --without-<test> : disables test <test>" );
   dLOG( "    --with-<test>    : enables test <test>" );
   dLOG( "    --all-off        : disables all test" );
   dLOG( "    --all-on         : enables all test" );
}


bool cmdANDinit::parseArgsAndInit( testStarter &_starter ) {
   for ( auto const &arg : args ) {
      if ( arg == "-h" || arg == "--help" ) {
         usage();
         return false;
      }

      if ( arg == "-w" || arg == "--wait" ) {
         iLOG( "Wait is enabled" );
         GlobConf.log.waitUntilLogEntryPrinted = true;
         continue;
      }

      if ( ( arg == "-n" || arg == "--nocolor" ) && vCanUseColor ) {
         iLOG( "Color is disabled" );
         GlobConf.log.logOUT.colors = DISABLED;
         GlobConf.log.logERR.colors = DISABLED;
         continue;
      }

      std::regex lLogRegex( "^\\-\\-log=[a-zA-Z_0-9 \\/\\.\\-\\+\\*]+$" );
      if ( std::regex_match( arg, lLogRegex ) ) {
         std::regex lLogRegexRep( "^\\-\\-log=" );
         const char *lRep = "";
         string logPath = std::regex_replace( arg, lLogRegexRep, lRep );
         GlobConf.log.logFILE.logFileName = logPath;
         continue;
      }

      std::regex lDataRegex( "^\\-\\-data=[a-zA-Z_0-9 \\/\\.\\-\\+\\*]+$" );
      if ( std::regex_match( arg, lDataRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-data=" );
         const char *lRep = "";
         dataRoot = std::regex_replace( arg, lDataRegexRep, lRep );
         continue;
      }

      std::regex lMajorRegex( "^\\-\\-glMajor=[0-9]+$" );
      if ( std::regex_match( arg, lMajorRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-glMajor=" );
         const char *lRep = "";
         string version = std::regex_replace( arg, lDataRegexRep, lRep );
         GlobConf.versions.glMajorVersion = atoi( version.c_str() );
         continue;
      }

      std::regex lMinorRegex( "^\\-\\-glMinor=[0-9]+$" );
      if ( std::regex_match( arg, lMinorRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-glMinor=" );
         const char *lRep = "";
         string version = std::regex_replace( arg, lDataRegexRep, lRep );
         GlobConf.versions.glMinorVersion = atoi( version.c_str() );
         continue;
      }


      if ( arg == "--list" || arg == "-l" ) {
         _starter.list();
         return false;
      }


      std::regex lWithoutRegex( "^\\-\\-without\\-[a-zA-Z_0-9 \\/\\.\\-\\+\\*]+$" );
      if ( std::regex_match( arg, lWithoutRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-without\\-" );
         const char *lRep = "";
         _starter.disable( std::regex_replace( arg, lDataRegexRep, lRep ) );
         continue;
      }

      std::regex lWithRegex( "^\\-\\-with\\-[a-zA-Z_0-9 \\/\\.\\-\\+\\*]+$" );
      if ( std::regex_match( arg, lWithRegex ) ) {
         std::regex lDataRegexRep( "^\\-\\-with\\-" );
         const char *lRep = "";
         _starter.enable( std::regex_replace( arg, lDataRegexRep, lRep ) );
         continue;
      }

      if ( arg == "--all-off" ) {
         _starter.allTestsOff();
         continue;
      }

      if ( arg == "--all-on" ) {
         _starter.allTestsOn();
         continue;
      }

      outputFiles.push_back( arg );
   }

   return true;
}


void cmdANDinit::generate( uJSON_data &_data ) {
   if ( outputFiles.empty() ) {
      outputFiles.emplace_back( "./oglTest.json" );
   }
   for ( auto &f : outputFiles ) {
      uParserJSON generator( f );
      if ( generator.write( _data, true ) == 1 ) {
         iLOG( "Successfully written '", f, "'" );
      } else { eLOG( "Failed to write '", f, "'" ); }
   }
}



// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
