#include "cmdANDinit.hpp"
#include "config.hpp"
#include <boost/regex.hpp>

cmdANDinit::cmdANDinit( int argc, char *argv[], testStarter &_starter, bool &_errors ) {
   argv0        = argv[0];
   vCanUseColor = COLOR == 1 ? true : false;
#if WINDOWS
   dataRoot     = INSTALL_PREFIX + string( "\\share\\engineTests\\oglTest\\data\\" );
#else
   dataRoot     = INSTALL_PREFIX + string( "/share/engineTests/oglTest/data/" );
#endif

   for ( auto i = 1; i < argc; ++i ) {
      args.push_back( argv[i] );
   }

   GlobConf.win.width           = 800;
   GlobConf.win.height          = 600;
   GlobConf.win.fullscreen      = false;
   GlobConf.win.windowName      = "OpenGL_TEST";
   GlobConf.win.iconName        = "ICON is missing";
   GlobConf.win.xlibWindowName  = "My icon";
   //GlobConf.win.winType         = e_engine::TOOLBAR;
   GlobConf.useAutoOpenGLVersion();
   GlobConf.config.appName      = "OpenGL_TEST";


   GlobConf.win.restoreOldScreenRes = true;

   GlobConf.versions.glMajorVersion = 4;
   GlobConf.versions.glMinorVersion = 6;


   if ( vCanUseColor ) {
      GlobConf.log.logOUT.colors   = FULL;
      GlobConf.log.logERR.colors   = FULL;
   } else {
      GlobConf.log.logOUT.colors   = DISABLED;
      GlobConf.log.logERR.colors   = DISABLED;

      GlobConf.log.width           = 175;
   }
   GlobConf.log.logOUT.Time     = LEFT_REDUCED;
   GlobConf.log.logOUT.File     = RIGHT_FULL;
   GlobConf.log.logERR.Time     = LEFT_REDUCED;
   GlobConf.log.logERR.File     = RIGHT_FULL;
   GlobConf.log.logFILE.File    = RIGHT_FULL;

   GlobConf.log.logFILE.logFileName =  SYSTEM.getLogFilePath();
#if UNIX
   GlobConf.log.logFILE.logFileName += "/Log";
#elif WINDOWS
   GlobConf.log.logFILE.logFileName += "\\Log";
#endif

   GlobConf.log.waitUntilLogEntryPrinted = false;
   
   _errors = !parseArgsAndInit( _starter );

}



void cmdANDinit::usage() {
   iLOG( "Usage: ", argv0, " [OPTIONS] OUTPUT_FILES"  );
   iLOG( "" );
   iLOG( "OPTIONS:" );
   dLOG( "    -h | --help      : show this help message"                  );
   dLOG( "    --log=<path>     : set a custom log file path to <path>"    );
   dLOG( "    -w | --wait      : wait until log entry is printed"         );
   dLOG( "    --data=<path>    : set a custom root path for the data dir" );
   dLOG( "    --glMajor=<v>    : the OpenGL Major version (default: "
  ,  GlobConf.versions.glMajorVersion, ")"  );
   dLOG( "    --glMinor=<v>    : the OpenGL Major version (default: "
  ,  GlobConf.versions.glMinorVersion, ")"  );
   if ( vCanUseColor ) {
      dLOG( "    -n | --nocolor   : disable colored output"                  );
   }
   dLOG( "    -l | --list      : lists all available tests" );
   dLOG( "    --without-<test> : disables test <test>"      );
   dLOG( "    --with-<test>    : enables test <test>"       );
   dLOG( "    --all-off        : disables all test"         );
   dLOG( "    --all-on         : enables all test"          );
}


bool cmdANDinit::parseArgsAndInit( testStarter &_starter ) {
   for ( auto const & arg : args ) {
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
         GlobConf.log.logOUT.colors   = DISABLED;
         GlobConf.log.logERR.colors   = DISABLED;
         continue;
      }

      boost::regex lLogRegex( "^\\-\\-log=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if ( boost::regex_match( arg, lLogRegex ) ) {
         boost::regex lLogRegexRep( "^\\-\\-log=" );
         const char *lRep = "";
         string logPath = boost::regex_replace( arg, lLogRegexRep, lRep );
         GlobConf.log.logFILE.logFileName = logPath;
         continue;
      }
      
      boost::regex lDataRegex( "^\\-\\-data=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if ( boost::regex_match( arg, lDataRegex ) ) {
         boost::regex lDataRegexRep( "^\\-\\-data=" );
         const char *lRep = "";
         dataRoot = boost::regex_replace( arg, lDataRegexRep, lRep );
         continue;
      }
      
      boost::regex lMajorRegex( "^\\-\\-glMajor=[0-9]+$" );
      if ( boost::regex_match( arg, lMajorRegex ) ) {
         boost::regex lDataRegexRep( "^\\-\\-glMajor=" );
         const char *lRep = "";
         string version = boost::regex_replace( arg, lDataRegexRep, lRep );
         GlobConf.versions.glMajorVersion = atoi( version.c_str() );
         continue;
      }
      
      boost::regex lMinorRegex( "^\\-\\-glMinor=[0-9]+$" );
      if ( boost::regex_match( arg, lMinorRegex ) ) {
         boost::regex lDataRegexRep( "^\\-\\-glMinor=" );
         const char *lRep = "";
         string version = boost::regex_replace( arg, lDataRegexRep, lRep );
         GlobConf.versions.glMinorVersion = atoi( version.c_str() );
         continue;
      }
      
      
      if ( arg == "--list" || arg == "-l" ) {
         _starter.list();
         return false;
      }
      
      
      boost::regex lWithoutRegex( "^\\-\\-without\\-[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if ( boost::regex_match( arg, lWithoutRegex ) ) {
         boost::regex lDataRegexRep( "^\\-\\-without\\-" );
         const char *lRep = "";
         _starter.disable( boost::regex_replace( arg, lDataRegexRep, lRep ) );
         continue;
      }
      
      boost::regex lWithRegex( "^\\-\\-with\\-[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if ( boost::regex_match( arg, lWithRegex ) ) {
         boost::regex lDataRegexRep( "^\\-\\-with\\-" );
         const char *lRep = "";
         _starter.enable( boost::regex_replace( arg, lDataRegexRep, lRep ) );
         continue;
      }
      
      if ( arg == "--all-off" ) {
         _starter.allTestsOff();
         continue;
      }
      
      if ( arg == "--all-on"  ) {
         _starter.allTestsOn();
         continue;
      }
      
      outputFiles.push_back( arg );
   }

   return true;
}


void cmdANDinit::generate( uJSON_data &_data ) {
   if( outputFiles.empty() ) {
      outputFiles.emplace_back( "./oglTest.json" );
   }
   for( auto &f : outputFiles ) {
      uParserJSON generator( f );
      if( generator.write( _data, true ) == 1 ) {
         iLOG( "Successfully written '", f, "'" );
      } else {
         eLOG( "Failed to write '", f, "'" );
      }
   }
}



// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
