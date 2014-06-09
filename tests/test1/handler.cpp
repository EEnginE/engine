/*!
 * \file handler.cpp
 * \brief Class MyHandler
 */

#include "handler.hpp"

GLfloat alpha = 1;
GLfloat r, g, b, R, G, B;
bool    rr, gg, bb;
int counter1 = 0;

void MyHandler::key( eWinInfo info ) {
   if ( vDisp_RandR.empty() )
      vDisp_RandR = info.eInitPointer->getDisplayResolutions();

   char lHex_CSTR[6];
   snprintf( lHex_CSTR, 5, "%04X", info.eKey.key );
   iLOG "Key " ADD info.eKey.state == E_KEY_PRESSED ? "pressed:  '" : "released: '" ADD info.eKey.key ADD "' - " ADD "0x" ADD lHex_CSTR END
   
   vector<eDisplays> displays;

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
            B_SLEEP( seconds, 5 );
            info.eInitPointer->setAttribute( C_TOGGLE, DEMANDS_ATTENTION ); break;
         case L'm':              info.eInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_HORZ, MAXIMIZED_VERT ); break;
         case E_KEY_KP_SUBTRACT: info.eInitPointer->setDecoration( e_engine::C_REMOVE ); info.eInitPointer->restartIfNeeded( true ); break;
         case E_KEY_KP_ADD:      info.eInitPointer->setDecoration( e_engine::C_ADD );    info.eInitPointer->restartIfNeeded( true ); break;
         case E_KEY_KP_MULTIPLY: info.eInitPointer->setDecoration( e_engine::C_TOGGLE ); info.eInitPointer->restartIfNeeded( true ); break;
         case L't':              if ( ( alpha - 0.1 ) < 0 ) alpha = 0; else alpha -= 0.01; break;
         case L'T':              if ( ( alpha + 0.1 ) > 1 ) alpha = 1; else alpha += 0.01; break;
         case L'o':              info.eInitPointer->changeWindowConfig( 800, 600, 10, 10 ); break;
         case L'F':
         case L'f':              info.eInitPointer->fullScreen( e_engine::C_TOGGLE ); info.eInitPointer->restartIfNeeded( true ); break;
         case L's':              info.eInitPointer->fullScreenMultiMonitor(); break;
         case L'a':              if ( vDisp_RandR.size() > 0 ) info.eInitPointer->setFullScreenMonitor( vDisp_RandR[0] ); break;
         case L'd':              if ( vDisp_RandR.size() > 1 ) info.eInitPointer->setFullScreenMonitor( vDisp_RandR[1] ); break;
         case L'r':              info.eInitPointer->restart( true ); break;
         case L'p':
            iLOG "Pausing" END
            info.eInitPointer->pauseMainLoop();
            B_SLEEP( seconds, 5 );
            iLOG "Unpausing" END
            info.eInitPointer->continueMainLoop();
            break;
         case L'B':
            displays = info.eInitPointer->getDisplayResolutions();
            displays[0].autoSelectBySize( 1024, 768, 60 );
            info.eInitPointer->setDisplaySizes( displays[0] );
            info.eInitPointer->applyNewRandRSettings();
            break;
         case L'N':
            displays = info.eInitPointer->getDisplayResolutions();
            displays[0].disable();
            info.eInitPointer->setDisplaySizes( displays[0] );
            info.eInitPointer->applyNewRandRSettings();
            break;
         case L'q':
         case L'g': info.eInitPointer->grabMouse(); break;
         case L'G': info.eInitPointer->freeMouse(); break;
         case L'Q':
         case E_KEY_ESCAPE:      info.eInitPointer->closeWindow(); break;
         case L'u':
            if ( info.eInitPointer->getKeyState( E_KEY_BACKSPACE ) == E_KEY_PRESSED ) {
               iLOG "JAAAAA" END;
            }
            break;
      }
   }
}


inline void color( float _r, float _g, float _b, eInit *i ) {
   glClearColor( _r, _g, _b, alpha );
   glClear( GL_COLOR_BUFFER_BIT );
   i->swapBuffers();
   //E_CLOCK.sleepUsec( 25000 );
}



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

   counter1++;

   if ( counter1 >= 5000000 ) {
      info.eInitPointer->quitMainLoop();
   }
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
