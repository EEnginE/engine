/*!
 * \file handler.cpp
 * \brief Class MyHandler
 */

#include "handler.hpp"

GLfloat alpha = 1;
GLfloat r, g, b, R, G, B;
bool    rr, gg, bb;
int counter1 = 0;

void MyHandler::mouse( iEventInfo info ) {
   if ( info.iMouse.button <= E_MOUSE_6 ) // We dont want move events and etc.
      iLOG "Button " ADD info.iMouse.state == E_PRESSED ? "pressed:  '" : "released: '" ADD info.iMouse.button ADD "'" END

   }

void MyHandler::key( iEventInfo info ) {
   if ( vDisp_RandR.empty() )
      vDisp_RandR = info.iInitPointer->getDisplayResolutions();

   char lHex_CSTR[6];
   snprintf( lHex_CSTR, 5, "%04X", info.eKey.key );
   iLOG "Key " ADD info.eKey.state == E_PRESSED ? "pressed:  '" : "released: '" ADD info.eKey.key ADD "' - " ADD "0x" ADD lHex_CSTR END

   vector<iDisplays> displays;

   if ( info.eKey.state == E_PRESSED ) {
      switch ( info.eKey.key ) {
         case E_KEY_F1:          info.iInitPointer->setAttribute( C_TOGGLE, MODAL ); break;
         case E_KEY_F2:          info.iInitPointer->setAttribute( C_TOGGLE, STICKY ); break;
         case E_KEY_F3:          info.iInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_VERT ); break;
         case E_KEY_F4:          info.iInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_HORZ ); break;
         case E_KEY_F5:          info.iInitPointer->setAttribute( C_TOGGLE, SHADED ); break;
         case E_KEY_F6:          info.iInitPointer->setAttribute( C_TOGGLE, SKIP_TASKBAR ); break;
         case E_KEY_F7:          info.iInitPointer->setAttribute( C_TOGGLE, SKIP_PAGER ); break;
         case E_KEY_F8:          info.iInitPointer->setAttribute( C_TOGGLE, HIDDEN ); break;
         case E_KEY_F9:          info.iInitPointer->setAttribute( C_TOGGLE, ABOVE ); break;
         case E_KEY_F10:         info.iInitPointer->setAttribute( C_TOGGLE, BELOW ); break;
         case E_KEY_F11:         info.iInitPointer->setAttribute( C_TOGGLE, FOCUSED ); break;
         case E_KEY_F12:
            B_SLEEP( seconds, 5 );
            info.iInitPointer->setAttribute( C_TOGGLE, DEMANDS_ATTENTION ); break;
         case L'm':              info.iInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_HORZ, MAXIMIZED_VERT ); break;
         case E_KEY_KP_SUBTRACT: info.iInitPointer->setDecoration( e_engine::C_REMOVE ); info.iInitPointer->restartIfNeeded( true ); break;
         case E_KEY_KP_ADD:      info.iInitPointer->setDecoration( e_engine::C_ADD );    info.iInitPointer->restartIfNeeded( true ); break;
         case E_KEY_KP_MULTIPLY: info.iInitPointer->setDecoration( e_engine::C_TOGGLE ); info.iInitPointer->restartIfNeeded( true ); break;
         case L't':              if ( ( alpha - 0.1 ) < 0 ) alpha = 0; else alpha -= 0.01; break;
         case L'T':              if ( ( alpha + 0.1 ) > 1 ) alpha = 1; else alpha += 0.01; break;
         case L'o':              info.iInitPointer->changeWindowConfig( 800, 600, 10, 10 ); break;
         case L'F':
         case L'f':              info.iInitPointer->fullScreen( e_engine::C_TOGGLE ); info.iInitPointer->restartIfNeeded( true ); break;
         case L's':              info.iInitPointer->fullScreenMultiMonitor(); break;
         case L'a':              if ( vDisp_RandR.size() > 0 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[0] ); break;
         case L'd':              if ( vDisp_RandR.size() > 1 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[1] ); break;
         case L'r':              info.iInitPointer->restart( true ); break;
         case L'p':
            iLOG "Pausing" END
            info.iInitPointer->pauseMainLoop( true );
            B_SLEEP( seconds, 5 );
            iLOG "Unpausing" END
            info.iInitPointer->continueMainLoop();
            break;
         case L'B':
            displays = info.iInitPointer->getDisplayResolutions();
            displays[0].autoSelectBySize( 1024, 768, 60 );
            info.iInitPointer->setDisplaySizes( displays[0] );
            info.iInitPointer->applyNewRandRSettings();
            break;
         case L'N':
            displays = info.iInitPointer->getDisplayResolutions();
            displays[0].disable();
            info.iInitPointer->setDisplaySizes( displays[0] );
            info.iInitPointer->applyNewRandRSettings();
            break;
         case L'q':
         case L'g': info.iInitPointer->grabMouse(); break;
         case L'G': info.iInitPointer->freiMouse(); break;
         case L'w': info.iInitPointer->moviMouse( GlobConf.win.width / 2, GlobConf.win.height / 2 ); break;
         case L'c': info.iInitPointer->hidiMouseCursor(); break;
         case L'C': info.iInitPointer->showMouseCursor(); break;
         case L'Q':
         case E_KEY_ESCAPE:      info.iInitPointer->closeWindow(); break;
         case L'u':
            if ( info.iInitPointer->getKeyState( L'n' ) == E_PRESSED ) {
               iLOG "JAAAAA" END;
            }
            break;
      }
   }
}


inline void color( float _r, float _g, float _b, iInit *i ) {
   glClearColor( _r, _g, _b, alpha );
   glClear( GL_COLOR_BUFFER_BIT );
   i->swapBuffers();
   //E_CLOCK.sleepUsec( 25000 );
}



void render( iEventInfo info ) {

   color( r, g, b, info.iInitPointer );

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

   counter1++;

   if ( counter1 >= 5000000 ) {
      info.iInitPointer->quitMainLoop();
   }
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
