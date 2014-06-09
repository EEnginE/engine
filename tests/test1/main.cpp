
#include "config.hpp"
#include "handler.hpp"
#include <engine.hpp>

#if WINDOWS
#include <windows.h>
#endif

using namespace std;
using namespace e_engine;

#define KDEVELOP 0
#define COLOR    0
#define DO_SHA   0

void hexPrint( std::vector<unsigned char> const &_v ) {
   for ( unsigned char const & c : _v )
      printf( "%02X ", c );
   printf( "\n\n" );
   fflush( stdout );
}

// #undef  UNIX
// #define UNIX 0

void ftemp( eInit *_init ) {
   B_SLEEP( seconds, 5 );
   _init->setDecoration( C_REMOVE );
   _init->fullScreen( C_ADD );
   _init->restartIfNeeded();
}

int main( int argc, char **argv ) {
   B_SLEEP( milliseconds, 1 ); // Why does this crash without this?
   WinData.win.width           = 800;
   WinData.win.height          = 600;
   WinData.win.fullscreen      = false;
   WinData.win.windowName      = "Engine Test";
   WinData.win.iconName        = "ICON is missing";
   WinData.win.xlibWindowName  = "My icon";
   //WinData.win.winType         = e_engine::TOOLBAR;
   WinData.useAutoOpenGLVersion();
   WinData.config.appName      = "E Engine";

#if ! KDEVELOP || COLOR
   WinData.log.logOUT.colors   = FULL;
   WinData.log.logERR.colors   = FULL;
#else
   WinData.log.logOUT.colors   = DISABLED;
   WinData.log.logERR.colors   = DISABLED;

   WinData.log.width           = 175;
#endif
   WinData.log.logOUT.Time     = LEFT_REDUCED;
   WinData.log.logOUT.File     = RIGHT_FULL;
   WinData.log.logERR.Time     = LEFT_REDUCED;
   WinData.log.logERR.File     = RIGHT_FULL;
   WinData.log.logFILE.File    = RIGHT_FULL;
   WinData.win.restoreOldScreenRes = true;
   
   WinData.versions.glMajorVersion = 4;
   WinData.versions.glMinorVersion = 6;
   
//    WinData.log.waitUntilLogEntryPrinted = true;
//    WinData.log.logFILE.logFileName = "./log";

   RandISAAC myRand;

   const int ValChange = 50;

   r = 0;
   g = 0;
   b = 0;

   R = ( ( float ) myRand( 10, ValChange ) / 5000 );
   G = ( ( float ) myRand( 10, ValChange ) / 5000 );
   B = ( ( float ) myRand( 10, ValChange ) / 5000 );

   rr = myRand( 0, 1 ) ? true : false;
   gg = myRand( 0, 1 ) ? true : false;
   bb = myRand( 0, 1 ) ? true : false;


   iLOG "User Name:     " ADD SYSTEM.getUserName()          END
   iLOG "User Login:    " ADD SYSTEM.getUserLogin()         END
   iLOG "Home:          " ADD SYSTEM.getUserHomeDirectory() END
   iLOG "Main config:   " ADD SYSTEM.getMainConfigDirPath() END
   iLOG "Log File Path: " ADD SYSTEM.getLogFilePath()       END
   

#if ! KDEVELOP
   eInit start;
   MyHandler handler;

   if ( start.init() == 1 ) {
      start.setRenderFunc( render );
      start.addWindowCloseSlot( handler.getSWindowClose() );
      start.addResizeSlot( handler.getSResize() );
      start.addKeySlot( handler.getSKey() );

      iLOG "Test" END

      vector<eDisplays> displays = start.getDisplayResolutions();

      iLOG "Displays: " ADD displays.size() END

      for ( GLuint i = 0; i < displays.size(); ++i ) {
         iLOG "Display " ADD i ADD ": " ADD displays[i].getName() END
      }

      if ( displays.size() == 2 ) {
//          displays[0].disable();
//          displays[1].disable();

//          iLOG start.setDisplaySizes( displays[0] ) END
//          iLOG start.setDisplaySizes( displays[1] ) END

//          start.applyNewRandRSettings();

//          B_SLEEP( seconds, 1 );

//          displays.clear();
//          displays = start.getDisplayResolutions();

         displays[0].enable();
         displays[1].enable();
         displays[0].autoSelectBest();
         displays[1].autoSelectBest();
//          iLOG start.setDisplaySizes( displays[0] ) END
//          iLOG start.setDisplaySizes( displays[1] ) END
//          start.applyNewRandRSettings();

//          displays.clear();
//          displays = start.getDisplayResolutions();

         displays[0].setNoClones();
         displays[1].setNoClones();
         displays[1].setPositionAbsolute( 0, 0 );
         displays[0].setPositionRelative( eDisplays::RIGHT_OFF, displays[1] );
         iLOG start.setDisplaySizes( displays[0] ) END
         iLOG start.setDisplaySizes( displays[1] ) END
         start.setPrimary( displays[1] );
         start.applyNewRandRSettings();
         start.setPrimary( displays[1] );
      }

      string temp;
      temp += ( string ) INSTALL_PREFIX + "/share/engineTests/test1/data/shaders/colors_p";

      if ( argc == 2 ) {
         temp = ( string ) argv[1] + "/colors_p" ;
      }
      
      eLinker prog( temp );
      GLuint dummy;
      prog.link( dummy );

      start.startMainLoop();
      B_SLEEP( seconds, 1 );
      start.closeWindow();
   }

#endif


#if DO_SHA == 1
   SHA_2 mySHA( SHA2_384 );
   mySHA.selftest();
#endif

//    iLOG "Credits: "
//    POINT "Daniel ( Mense ) Mensinger"
//    POINT "Dennis Schunder"
//    POINT "Silas Bartel"
//    END

//    B_SLEEP( seconds, 1 );

   start.shutdown();

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

