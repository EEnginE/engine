#include "config.hpp"
#include "handler.hpp"
#include <engine.hpp>

#if WINDOWS
#include <windows/context.hpp>
#endif

using namespace std;
using namespace e_engine;
using namespace OS_NAMESPACE;

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

int main( int argc, char **argv ) {
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
   WinData.log.logOUT.Time     = LEFT_FULL;
   WinData.log.logOUT.File     = RIGHT_FULL;
   WinData.log.logERR.Time     = LEFT_FULL;
   WinData.log.logERR.File     = RIGHT_FULL;
   WinData.log.logFILE.File    = RIGHT_FULL;
   WinData.win.restoreOldScreenRes = false;

   RandISAAC myRand;


   r = 0;
   g = 0;
   b = 0;
   double a = 0;

   LOG.devInit();
   LOG.startLogLoop();

   iLOG "User Name:     " ADD SYSTEM.getUserName()          END
   iLOG "User Login:    " ADD SYSTEM.getUserLogin()         END
   iLOG "Home:          " ADD SYSTEM.getUserHomeDirectory() END
   iLOG "Main config:   " ADD SYSTEM.getMainConfigDirPath() END
   iLOG "Log File Path: " ADD SYSTEM.getLogFilePath()       END
   
   
   
   windows_win32::eContext lec;
   
   lec.createContext();
   lec.enableVSync();   
    while(1) {
       glClearColor(r,g,b,a);
       glClear(GL_COLOR_BUFFER_BIT);
       lec.swapBuffers();
    }


   iLOG "Credits: " ADD 'B', 'G', "Daniel ( GOTT ) Mensinger" END
   
   B_SLEEP( seconds, 1 );

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

