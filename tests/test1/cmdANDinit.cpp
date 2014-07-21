#include "cmdANDinit.hpp"
#include "config.hpp"
#include <boost/regex.hpp>

cmdANDinit::cmdANDinit( int argc, char *argv[] ) {
   argv0        = argv[0];
   vCanUseColor = COLOR == 1 ? true : false;
   dataRoot     = INSTALL_PREFIX + string( "/share/engineTests/test1/data/" );

   for ( auto i = 1; i < argc; ++i ) {
      args.push_back( argv[i] );
   }

   GlobConf.win.width           = 800;
   GlobConf.win.height          = 600;
   GlobConf.win.fullscreen      = false;
   GlobConf.win.windowName      = "Engine Test";
   GlobConf.win.iconName        = "ICON is missing";
   GlobConf.win.xlibWindowName  = "My icon";
   //GlobConf.win.winType         = e_engine::TOOLBAR;
   GlobConf.useAutoOpenGLVersion();
   GlobConf.config.appName      = "E Engine";


   GlobConf.win.restoreOldScreenRes = true;

   GlobConf.versions.glMajorVersion = 4;
   GlobConf.versions.glMinorVersion = 4;


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

}



void cmdANDinit::usage() {
   iLOG "Usage: " ADD argv0 ADD " [OPTIONS] MODE [BENCHMARK OPTIONS]"  END
   iLOG "" END
   iLOG "OPTIONS:" END
   dLOG "    -h | --help    : show this help message"                  END
   dLOG "    --log=<path>   : set a custom log file path to <path>"    END
   dLOG "    -w | --wait    : wait until log entry is printed"         END
   dLOG "    --data=<path>  : set a custom root path for the data dir" END
   dLOG "    --glMajor=<v>  : the OpenGL Major version (default: "
   ADD  GlobConf.versions.glMajorVersion ADD ")"  END
   dLOG "    --glMinor=<v>  : the OpenGL Major version (default: "
   ADD  GlobConf.versions.glMinorVersion ADD ")"  END
   if ( vCanUseColor ) {
      dLOG "    -n | --nocolor : disable colored output"                  END
   }
}


bool cmdANDinit::parseArgsAndInit() {

   for ( auto const & arg : args ) {
      if ( arg == "-h" || arg == "--help" ) {
         postInit();
         usage();
         return false;
      }

      if ( arg == "-w" || arg == "--wait" ) {
         iLOG "Wait is enabled" END
         GlobConf.log.waitUntilLogEntryPrinted = true;
         continue;
      }

      if ( ( arg == "-n" || arg == "--nocolor" ) && vCanUseColor ) {
         iLOG "Color is disabled" END
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
      
      eLOG "Unkonwn option '" ADD arg ADD "'" END
   }

   return true;
}


void cmdANDinit::postInit() {
   LOG.devInit();
   LOG.startLogLoop();
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
