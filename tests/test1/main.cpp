
#include "config.hpp"
#include "handler.hpp"
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
   cmdANDinit cmd( argc, argv );

   if ( ! cmd.parseArgsAndInit() ) {
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
      MyHandler handler( cmd, &start );
      start.addFocusSlot( start.getAdvancedGrabControlSlot() );

      if ( handler.initGL() == 0 )
         start.startMainLoop();

      start.closeWindow();
   }

//    iLOG( "Credits: "
//    POINT "Daniel ( Mense ) Mensinger"
//    POINT "Dennis Schunder"
//    POINT "Silas Bartel"
//    );

//    B_SLEEP( seconds, 1 );

//    B_SLEEP( seconds, 1 );

   start.shutdown();

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;



