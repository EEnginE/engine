
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

   if ( start.init() == 1 ) {
      myWorld handler( cmd, &start );
      start.enableDefaultGrabControl();

      if ( handler.initGL() == 0 )
         start.startMainLoop();
   }

   start.shutdown();

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
