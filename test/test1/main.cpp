#include "engine.hpp"
#include "config.h"

using namespace std;
using namespace e_engine;

#define KDEVELOP 0
#define COLOR    0
#define DO_RSA   0
#define DO_SHA   0

GLfloat alpha = 1;

inline void color( float r, float g, float b, eInit *i ) {
   glClearColor( r, g, b, alpha );
   glClear( GL_COLOR_BUFFER_BIT );
   i->swapBuffers();
   usleep( 100 );
   //E_CLOCK.sleepUsec( 25000 );
}

namespace mensinda {
int count = 0;
}

void hexPrint( std::vector<unsigned char> const &_v ) {
   for ( unsigned char const & c : _v )
      printf( "%02X ", c );
   printf( "\n\n" );
   fflush( stdout );
}

GLfloat r, g, b, R, G, B;
bool    rr, gg, bb;

void render( eWinInfo info ) {

   color( r, g, b, info.eInitPointer );

   if ( rr ) {
      if ( r + R > 1 ) {
         r -= R;
         rr = false;
      } else
         r += R;
   } else {
      if ( r - R < 0 ) {
         r += R;
         rr = true;
      } else
         r -= R;
   }

   if ( gg ) {
      if ( g + G > 1 ) {
         g -= G;
         gg = false;
      } else
         g += G;
   } else {
      if ( g - G < 0 ) {
         g += G;
         gg = true;
      } else
         g -= G;
   }

   if ( bb ) {
      if ( b + B > 1 ) {
         b -= B;
         bb = false;
      } else
         b += B;
   } else {
      if ( b - B < 0 ) {
         b += B;
         bb = true;
      } else
         b -= B;
   }

   mensinda::count++;

   if ( mensinda::count >= 5000 ) {
      info.eInitPointer->quitMainLoop();
   }
}


class MyHandler {
      typedef eSlot<void, MyHandler, eWinInfo> _SLOT_;
   private:
      vector<eRandRDisplay> vDisp_RandR;

   public:
      _SLOT_ slotWindowClose;
      _SLOT_ slotResize;
      _SLOT_ slotKey;
      _SLOT_ slotMouse;
      MyHandler() {
         slotWindowClose.setFunc( &MyHandler::windowClose, this );
         slotResize.setFunc( &MyHandler::resize, this );
         slotKey.setFunc( &MyHandler::key, this );
         slotMouse.setFunc( &MyHandler::mouse, this );
      }
      void windowClose( eWinInfo info ) {
         iLOG "User closed window" END
         info.eInitPointer->closeWindow();
      }
      void key( eWinInfo info );
      void mouse( eWinInfo info ) {}
      void resize( eWinInfo info ) {
         iLOG "Window resized" END
      }

      _SLOT_ *getSWindowClose() {return &slotWindowClose;}
      _SLOT_ *getSResize()      {return &slotResize;}
      _SLOT_ *getSKey()         {return &slotKey;}
      _SLOT_ *getSMouse()       {return &slotMouse;}
};

void MyHandler::key( eWinInfo info ) {
   if ( vDisp_RandR.empty() )
      vDisp_RandR = info.eInitPointer->getDisplayResolutions();

   if ( info.eKey.state == E_KEY_PRESSED ) {
      switch ( info.eKey.key ) {
         case E_KEY_F1:          info.eInitPointer->setAttribute( C_TOGGLE, MODAL ); break;
         case E_KEY_F2:          info.eInitPointer->setAttribute( C_TOGGLE, STICKY ); break;
         case E_KEY_F3:          info.eInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_VERT ); break;
         case E_KEY_F4:          info.eInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_HORZ ); break;
         case E_KEY_F5:          info.eInitPointer->setAttribute( C_TOGGLE, SHADED ); break;
         case E_KEY_F6:          info.eInitPointer->setAttribute( C_TOGGLE, SKIP_TASKBAR ); break;
         case E_KEY_F7:          info.eInitPointer->setAttribute( C_TOGGLE, SKIP_PAGER ); break;
         case E_KEY_F8:          info.eInitPointer->setAttribute( C_TOGGLE, HIDDEN ); break;
         case E_KEY_F9:          info.eInitPointer->setAttribute( C_TOGGLE, ABOVE ); break;
         case E_KEY_F10:         info.eInitPointer->setAttribute( C_TOGGLE, BELOW ); break;
         case E_KEY_F11:         info.eInitPointer->setAttribute( C_TOGGLE, FOCUSED ); break;
         case E_KEY_F12:
            B_SLEEP( seconds( 5 ) );
            info.eInitPointer->setAttribute( C_TOGGLE, DEMANDS_ATTENTION ); break;
         case L'm':              info.eInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_HORZ, MAXIMIZED_VERT ); break;
         case E_KEY_KP_SUBTRACT: info.eInitPointer->setDecoration( e_engine::C_REMOVE ); break;
         case E_KEY_KP_ADD:      info.eInitPointer->setDecoration( e_engine::C_ADD ); break;
         case E_KEY_KP_MULTIPLY: info.eInitPointer->setDecoration( e_engine::C_TOGGLE ); break;
         case L't':              if ( ( alpha - 0.1 ) < 0 ) alpha = 0; else alpha -= 0.01; break;
         case L'T':              if ( ( alpha + 0.1 ) > 1 ) alpha = 1; else alpha += 0.01; break;
         case L'o':              info.eInitPointer->changeWindowConfig( 800, 600, 10, 10 ); break;
         case L'F':
         case L'f':              info.eInitPointer->fullScreen( e_engine::C_TOGGLE ); break;
         case L's':              info.eInitPointer->fullScreenMultiMonitor(); break;
         case L'a':              if ( vDisp_RandR.size() > 0 ) info.eInitPointer->setFullScreenMonitor( vDisp_RandR[0] ); break;
         case L'd':              if ( vDisp_RandR.size() > 1 ) info.eInitPointer->setFullScreenMonitor( vDisp_RandR[1] ); break;
         case L'q':
         case L'Q':
         case E_KEY_ESCAPE:      info.eInitPointer->closeWindow(); break;
         case L'u':
            if ( info.eInitPointer->E_KEYS.getKeyState( E_KEY_BACKSPACE ) == E_KEY_PRESSED ) {
               iLOG "JAAAAA" END;
            }
            break;
      }
   }
}


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

#if KDEVELOP == 0 || COLOR == 1
   WinData.log.stdout.colors   = FULL;
   WinData.log.stderr.colors   = FULL;
#else
   WinData.log.stdout.colors   = DISABLED;
   WinData.log.stderr.colors   = DISABLED;

   WinData.log.width           = 175;
#endif
   WinData.log.stdout.Time     = LEFT_FULL;
   WinData.log.stdout.File     = RIGHT_FULL;
   WinData.log.stderr.Time     = LEFT_FULL;
   WinData.log.stderr.File     = RIGHT_FULL;
   WinData.log.stdlog.File     = RIGHT_FULL;
   WinData.win.restoreOldScreenRes = false;

   RandISAAC myRand;

   const int ValChange = 50;

   r = 0;
   g = 0;
   b = 0;

   R = ( ( float )myRand( 10, ValChange ) / 1000 );
   G = ( ( float )myRand( 10, ValChange ) / 1000 );
   B = ( ( float )myRand( 10, ValChange ) / 1000 );

   rr = myRand( 0, 1 ) ? true : false;
   gg = myRand( 0, 1 ) ? true : false;
   bb = myRand( 0, 1 ) ? true : false;

#if KDEVELOP == 0
   eInit start;
   MyHandler handeler;

   if ( start.init() == 1 ) {

      start.setRenderFunc( render );
      start.addWindowCloseSlot( handeler.getSWindowClose() );
      start.addResizeSlot( handeler.getSResize() );
      start.addKeySlot( handeler.getSKey() );

      vector<eRandRDisplay> displays = start.getDisplayResolutions();

      iLOG "Displays: " ADD displays.size() END

      for ( GLuint i = 0; i < displays.size(); ++i ) {
         iLOG "Display " ADD i ADD ": " ADD displays[i].getName() END
      }

      if ( displays.size() == 2 ) {
         displays[0].enable();
         displays[1].enable();
         displays[0].setNoClones();
         displays[1].setNoClones();
         displays[1].setPositionAbsolute( 0, 0 );
         displays[0].setPositionRelative( eRandRDisplay::RIGHT_OFF, displays[1] );
         iLOG start.setDisplaySizes( displays[0] ) END
         iLOG start.setDisplaySizes( displays[1] ) END
         start.setPrimary( displays[1] );
         start.applyNewRandRSettings();
      }

      string temp;
      temp += ( string )INSTALL_PREFIX + "/share/engineTests/test1/colors_p";

      if ( argc == 2 ) {
         temp = ( string )argv[1] + "/colors_p" ;
      }

      eLinker prog( temp );
      try {
         prog.link();
      } catch ( eError &e ) {
         e.what();
      }
      start.startMainLoop();
      start.closeWindow();
   }
#else
   LOG.devInit();
   LOG.startLogLoop();
#endif

#if DO_RSA == 1
   RSA myRsa1;
   RSA myRsa2;

   myRsa1.generateNewKeyPair( 4096 );
   myRsa2.generateNewKeyPair( 4096 );

   std::vector<unsigned char> signature;

   std::string file1 = "FileToEncrypt";
   std::string file2 = "FileToEncrypt_encrypted";
   std::string file3 = "FileToEncrypt_decrypted";

   iLOG "Encrypting " ADD file1 ADD "... please wait" END
   int ret = RSA::encrypt( myRsa1, myRsa2, file1, file2, signature );

   if ( ret != 1 )
      eLOG "Encryption Error: " ADD ret END;

   myRsa1.exportKey( "key_1", 3 );
   myRsa2.exportKey( "key_2", 3 );

   myRsa1.reset();
   myRsa2.reset();

   myRsa1.importKey( "key_1" );
   myRsa2.importKey( "key_2" );

   iLOG "Decrypting " ADD file2 ADD "... please wait" END
   ret = RSA::decrypt( myRsa1, myRsa2, file2, file3, signature );
   if ( ret != 1 )
      eLOG "Decryption Error: " ADD ret END;
#endif

#if DO_SHA == 1
      SHA_2 mySHA( SHA2_384 );
   mySHA.selftest();

   iLOG "Credits: " ADD 'B', 'G', "Daniel ( Mense ) Mensinger" END
#endif 

   LOG.stopLogLoop();
   return EXIT_SUCCESS;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
