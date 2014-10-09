#include "cmdANDinit.hpp"
#include <boost/regex.hpp>

cmdANDinit::cmdANDinit( int argc, char *argv[], bool _color ) {
   argv0        = argv[0];
   vCanUseColor = _color;

   for( auto i = 1; i < argc; ++i ) {
      args.push_back( argv[i] );
   }

   GlobConf.config.appName      = "Engine.Benchmarks";

   if( vCanUseColor ) {
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

   vDoCast        = false;
   vCastLoops     = 5000000;
}



void cmdANDinit::usage() {
   iLOG( "Usage: ", argv0, " [OPTIONS] MODE [BENCHMARK OPTIONS]" );
   iLOG( "" );
   iLOG( "OPTIONS:" );
   dLOG( "    -h | --help    : show this help message" );
   dLOG( "    --log=<path>   : set a custom log file path to <path>" );
   dLOG( "    -w | --wait    : wait until log entry is printed" );
   if( vCanUseColor ) {
      dLOG( "    -n | --nocolor : disable colored output" );
   }
   iLOG( "" );
   iLOG(
         "MODES:"
         "\nall            : do all benchmarks"
         "\nfunc           : do the functions benchmark"
         "\nmutex          : do the mutex benchmark"
         "\ncast           : do the number string cast benchmark"
   );
   iLOG( "" );
   iLOG( "BENCHMARK OPTIONS:" );
   dLOG( "    --funcLoops=<loops>  : ammount of loops to do in function benchmark (default: ", vFunctionLoops, ")" );
   dLOG( "    --mutexLoops=<loops> : ammount of loops to do in mutex benchmark    (default: ", vMutexLoops,    ")" );
   dLOG( "    --castLoops=<loops>  : ammount of loops to do in cast benchmark     (default: ", vCastLoops,     ")\n" );
   wLOG( "You MUST define one ore more modes\n\n" );
}


bool cmdANDinit::parseArgsAndInit() {

   for( auto const & arg : args ) {
      if( arg == "-h" || arg == "--help" ) {
         postInit();
         usage();
         return false;
      }

      if( arg == "-w" || arg == "--wait" ) {
         iLOG( "Wait is enabled" );
         GlobConf.log.waitUntilLogEntryPrinted = true;
         continue;
      }

      if( ( arg == "-n" || arg == "--nocolor" ) && vCanUseColor ) {
         iLOG( "Color is disabled" );
         GlobConf.log.logOUT.colors   = DISABLED;
         GlobConf.log.logERR.colors   = DISABLED;
         continue;
      }

      boost::regex lLogRegex( "^\\-\\-log=[\\/a-zA-Z0-9 \\._\\-\\+\\*]+$" );
      if( boost::regex_match( arg, lLogRegex ) ) {
         boost::regex lLogRegexRep( "^\\-\\-log=" );
         const char *lRep = "";
         string logPath = boost::regex_replace( arg, lLogRegexRep, lRep );
         GlobConf.log.logFILE.logFileName = logPath;
         continue;
      }


      if( arg == "all" ) {
         vDoFunction = true;
         vDoMutex    = true;
         vDoCast     = true;
         continue;
      }

      if( arg == "func" ) {
         vDoFunction = true;
         continue;
      }

      if( arg == "mutex" ) {
         vDoMutex = true;
         continue;
      }

      if( arg == "cast" ) {
         vDoCast = true;
         continue;
      }


      boost::regex lFuncRegex( "^\\-\\-funcLoops=[0-9 ]*$" );
      if( boost::regex_match( arg, lFuncRegex ) ) {
         boost::regex lFuncRegexRep( "^\\-\\-funcLoops=" );
         const char *lRep = "";
         string funcString = boost::regex_replace( arg, lFuncRegexRep, lRep );
         vFunctionLoops = atoi( funcString.c_str() );
         continue;
      }

      boost::regex lMutexRegex( "^\\-\\-mutexLoops=[0-9 ]*$" );
      if( boost::regex_match( arg, lFuncRegex ) ) {
         boost::regex lFuncRegexRep( "^\\-\\-mutexLoops=" );
         const char *lRep = "";
         string funcString = boost::regex_replace( arg, lFuncRegexRep, lRep );
         vMutexLoops = atoi( funcString.c_str() );
         continue;
      }

      boost::regex lCastRegex( "^\\-\\-castLoops=[0-9 ]*$" );
      if( boost::regex_match( arg, lFuncRegex ) ) {
         boost::regex lFuncRegexRep( "^\\-\\-castLoops=" );
         const char *lRep = "";
         string funcString = boost::regex_replace( arg, lFuncRegexRep, lRep );
         vCastLoops = atoi( funcString.c_str() );
         continue;
      }

      eLOG( "Unkonwn option '", arg, "'" );
   }

   if( vDoFunction == false && vDoMutex == false && vDoCast == false ) {
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

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
