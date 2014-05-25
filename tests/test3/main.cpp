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

class testT {
   private:
      boost::condition_variable lCond1;
      boost::mutex              lMut1;
      bool                      lDone;
      
//       eContext                  lEE;

   public:
      testT() : lDone(false) {}
      
      void testThreads() {
         iLOG "T: Thread started" END
//    B_SLEEP( seconds, 1 );
         boost::lock_guard<boost::mutex> lLock_BT( lMut1 );
         iLOG "T: Mutex locked" END
         B_SLEEP( seconds, 1 );
//          lEE.createContext();
         lDone = true;
         lCond1.notify_one();
         iLOG "T: SEND" END
      }

      void run() {
         iLOG "M: Done With Rendering" END
         boost::unique_lock<boost::mutex> lLock_BT( lMut1 );
         iLOG "M: Mutex Locked" END
         boost::thread lTest_BT = boost::thread( &testT::testThreads, this );
         iLOG "M: Thread started" END
         B_SLEEP( seconds, 1 );
         iLOG "M: Begin Wait" END

         while ( !lDone ) lCond1.wait( lLock_BT );
         
//          lEE.destroyContext();
      }
};

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

   WinData.log.logFILE.logFileName =  SYSTEM.getLogFilePath();

#if UNIX
   WinData.log.logFILE.logFileName += "/Log";
#elif WINDOWS
   WinData.log.logFILE.logFileName += "\\Log";
#endif

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
   for ( int i = 0; i < 120; ++i ) {
      glClearColor( r, g, b, a );
      glClear( GL_COLOR_BUFFER_BIT );
      lec.swapBuffers();

      MSG msg;

      if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {

         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
   }

   lec.destroyContext();

   testT ttt;
   
   ttt.run();




   iLOG "Credits: " ADD 'B', 'G', "Daniel ( Mense ) Mensinger" END

   B_SLEEP( seconds, 1 );

   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

