#include "config.h"
#include "handler.hpp"
#include <engine.hpp>

using namespace std;
using namespace e_engine;

#define KDEVELOP 1
#define COLOR    1
#define DO_RSA   0
#define DO_SHA   1

void hexPrint ( std::vector<unsigned char> const &_v ) {
   for ( unsigned char const & c : _v )
      printf ( "%02X ", c );
   printf ( "\n\n" );
   fflush ( stdout );
}

// #undef  UNIX
// #define UNIX 0

int main ( int argc, char **argv ) {
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

   const int ValChange = 50;

   r = 0;
   g = 0;
   b = 0;

   R = ( ( float ) myRand ( 10, ValChange ) / 1000 );
   G = ( ( float ) myRand ( 10, ValChange ) / 1000 );
   B = ( ( float ) myRand ( 10, ValChange ) / 1000 );

   rr = myRand ( 0, 1 ) ? true : false;
   gg = myRand ( 0, 1 ) ? true : false;
   bb = myRand ( 0, 1 ) ? true : false;


   iLOG "User Name:     " ADD SYSTEM.getUserName()          END
   iLOG "User Login:    " ADD SYSTEM.getUserLogin()         END
   iLOG "Home:          " ADD SYSTEM.getUserHomeDirectory() END
   iLOG "Main config:   " ADD SYSTEM.getMainConfigDirPath() END
   iLOG "Log File Path: " ADD SYSTEM.getLogFilePath()       END


#if ! KDEVELOP
#if UNIX
   eInit start;
   MyHandler handeler;

   if ( start.init() == 1 ) {

         start.setRenderFunc ( render );
         start.addWindowCloseSlot ( handeler.getSWindowClose() );
         start.addResizeSlot ( handeler.getSResize() );
         start.addKeySlot ( handeler.getSKey() );

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
               displays[1].setPositionAbsolute ( 0, 0 );
               displays[0].setPositionRelative ( eDisplays::RIGHT_OFF, displays[1] );
               iLOG start.setDisplaySizes ( displays[0] ) END
               iLOG start.setDisplaySizes ( displays[1] ) END
               start.setPrimary ( displays[1] );
               start.applyNewRandRSettings();
               start.setPrimary ( displays[1] );
            }

         string temp;
         temp += ( string ) INSTALL_PREFIX + "/share/engineTests/test1/colors_p";

         if ( argc == 2 ) {
               temp = ( string ) argv[1] + "/colors_p" ;
            }

         eLinker prog ( temp );
         try {
               prog.link();
            }
         catch ( eError &e ) {
               e.what();
            }
         start.startMainLoop();
         start.closeWindow();
      }
#else // UNIX
   WinData.log.logFILE.logFileName =  SYSTEM.getLogFilePath();
   WinData.log.logFILE.logFileName += "\\Log";

   LOG.devInit();
   LOG.startLogLoop();
#endif
#else
   WinData.log.logFILE.logFileName =  SYSTEM.getLogFilePath();
   
#if UNIX
   WinData.log.logFILE.logFileName += "/Log";
#elif WINDOWS
   WinData.log.logFILE.logFileName += "\\Log";
#endif

   LOG.devInit();
   LOG.startLogLoop();
#endif

#if DO_RSA == 1
   RSA myRsa1;
   RSA myRsa2;

   myRsa1.generateNewKeyPair ( 4096 );
   myRsa2.generateNewKeyPair ( 4096 );

   std::vector<unsigned char> signature;

   std::string file1 = "FileToEncrypt";
   std::string file2 = "FileToEncrypt_encrypted";
   std::string file3 = "FileToEncrypt_decrypted";

   iLOG "Encrypting " ADD file1 ADD "... please wait" END
   int ret = RSA::encrypt ( myRsa1, myRsa2, file1, file2, signature );

   if ( ret != 1 )
      eLOG "Encryption Error: " ADD ret END;

   myRsa1.exportKey ( "key_1", 3 );
   myRsa2.exportKey ( "key_2", 3 );

   myRsa1.reset();
   myRsa2.reset();

   myRsa1.importKey ( "key_1" );
   myRsa2.importKey ( "key_2" );

   iLOG "Decrypting " ADD file2 ADD "... please wait" END
   ret = RSA::decrypt ( myRsa1, myRsa2, file2, file3, signature );
   if ( ret != 1 )
      eLOG "Decryption Error: " ADD ret END;
#endif

#if DO_SHA == 1
   SHA_2 mySHA ( SHA2_384 );
   mySHA.selftest();
#endif

   iLOG "Credits: " ADD 'B', 'G', "Daniel ( Mense ) Mensinger" END

   LOG.stopLogLoop();
   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

