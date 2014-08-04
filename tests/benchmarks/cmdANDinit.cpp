#include "cmdANDinit.hpp"
#include <boost/regex.hpp>

cmdANDinit::cmdANDinit( int argc, char *argv[], bool _color ) {
   argv0        = argv[0];
   vCanUseColor = _color;

   for ( auto i = 1; i < argc; ++i ) {
      args.push_back( argv[i] );
   }

   GlobConf.config.appName      = "Engine.Benchmarks";

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

   vDoFunction    = false;
   vFunctionLoops = 100000;
   
   vDoMutex       = false;
   vMutexLoops    = 10000000;
}



void cmdANDinit::usage() {
   iLOG "Usage: " ADD argv0 ADD " [OPTIONS] MODE [BENCHMARK OPTIONS]"  END
   iLOG "" END
   iLOG "OPTIONS:" END
   dLOG "    -h | --help    : show this help message"                  END
   dLOG "    --log=<path>   : set a custom log file path to <path>"    END
   dLOG "    -w | --wait    : wait until log entry is printed"         END
   if ( vCanUseColor ) {
      dLOG "    -n | --nocolor : disable colored output"                  END
   }
   iLOG ""                                                             END
   iLOG "MODES:"
   POINT "all            : do all benchmarks"
   POINT "func           : do the functions benchmark"
   POINT "mutex          : do the mutex benchmark"
   END
   iLOG "" END
   iLOG "BENCHMARK OPTIONS:" END
   dLOG "    --funcLoops=<loops>  : ammount of loops to do in function benchmark (default: " ADD vFunctionLoops ADD ")" END
   dLOG "    --mutexLoops=<loops> : ammount of loops to do in mutex benchmark (default: " ADD vMutexLoops ADD ")"
   NEWLINE
   NEWLINE
   END
   wLOG "You MUST define one ore more modes"
   NEWLINE
   NEWLINE
   END
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


      if ( arg == "all" ) {
         vDoFunction = true;
         vDoMutex    = true;
         continue;
      }

      if ( arg == "func" ) {
         vDoFunction = true;
         continue;
      }
      
      if( arg == "mutex" ) {
         vDoMutex = true;
         continue;
      }


      boost::regex lFuncRegex( "^\\-\\-funcLoops=[0-9 ]*$" );
      if ( boost::regex_match( arg, lFuncRegex ) ) {
         boost::regex lFuncRegexRep( "^\\-\\-funcLoops=" );
         const char *lRep = "";
         string funcString = boost::regex_replace( arg, lFuncRegexRep, lRep );
         vFunctionLoops = atoi( funcString.c_str() );
         continue;
      }
      
      boost::regex lMutexRegex( "^\\-\\-mutexLoops=[0-9 ]*$" );
      if ( boost::regex_match( arg, lFuncRegex ) ) {
         boost::regex lFuncRegexRep( "^\\-\\-mutexLoops=" );
         const char *lRep = "";
         string funcString = boost::regex_replace( arg, lFuncRegexRep, lRep );
         vMutexLoops = atoi( funcString.c_str() );
         continue;
      }

      eLOG "Unkonwn option '" ADD arg ADD "'" END
   }

   if ( vDoFunction == false && vDoMutex == false ) {
      postInit();
      usage();
      return false;
   }

   postInit();
   return true;
}


void cmdANDinit::postInit() {
   LOG.devInit();
   LOG.startLogLoop();
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
