/*!
 * \file myWorld.cpp
 * \brief Class myWorld
 */

#include "myWorld.hpp"
#include "config.hpp"

#include <GL/glew.h>

#define USE_OLD_KEY_BINDINGS 0


myWorld::~myWorld() {}


void myWorld::key( iEventInfo const &info ) {
   if ( vDisp_RandR.empty() )
      vDisp_RandR = info.iInitPointer->getDisplayResolutions();

   if ( info.eKey.state == E_PRESSED ) {
      switch ( info.eKey.key ) {
         // Handle fullscreen
         case E_KEY_F8:
            info.iInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_VERT, MAXIMIZED_HORZ );
            break;
         case E_KEY_F9:
            info.iInitPointer->fullScreenMultiMonitor();
            break;
         case E_KEY_F11:
            info.iInitPointer->fullScreen( e_engine::C_TOGGLE );
            info.iInitPointer->restartIfNeeded( true );
            break;
         case E_KEY_F10:
            if ( vDisp_RandR.size() > 0 )
               info.iInitPointer->setFullScreenMonitor( vDisp_RandR[0] );
            break;
         case E_KEY_F12:
            if ( vDisp_RandR.size() > 1 )
               info.iInitPointer->setFullScreenMonitor( vDisp_RandR[1] );
            break;

         // Mouse control
         case L'g':
            info.iInitPointer->grabMouse();
            break;
         case L'G':
            info.iInitPointer->freeMouse();
            break;
         case L'c':
            info.iInitPointer->hideMouseCursor();
            break;
         case L'C':
            info.iInitPointer->showMouseCursor();
            break;

         // Object control
         //          case E_KEY_UP:        vObject1.addPositionDelta( rVec3f( 0, 0, -0.1 ) );
         //          vObject1.updateFinalMatrix(); break;
         //          case E_KEY_DOWN:      vObject1.addPositionDelta( rVec3f( 0, 0, 0.1 ) );
         //          vObject1.updateFinalMatrix(); break;
         //          case E_KEY_LEFT:      vObject1.addPositionDelta( rVec3f( -0.01, 0, 0 ) );
         //          vObject1.updateFinalMatrix(); break;
         //          case E_KEY_RIGHT:     vObject1.addPositionDelta( rVec3f( 0.01, 0, 0 ) );
         //          vObject1.updateFinalMatrix(); break;
         //          case E_KEY_PAGE_UP:   vObject1.addPositionDelta( rVec3f( 0, 0.01, 0 ) );
         //          vObject1.updateFinalMatrix(); break;
         //          case E_KEY_PAGE_DOWN: vObject1.addPositionDelta( rVec3f( 0, -0.01, 0 ) );
         //          vObject1.updateFinalMatrix(); break;

         // Pause - restart
         case L'p':
         case L'P':
            iLOG( "Pausing" );
            info.iInitPointer->pauseMainLoop( true );
            B_SLEEP( seconds, 5 );
            iLOG( "Unpausing" );
            info.iInitPointer->continueMainLoop();
            break;
         case L'r':
         case L'R':
            info.iInitPointer->restart( true );
            break;

         // Window Border
         case L'b':
         case L'B':
            info.iInitPointer->setDecoration( e_engine::C_TOGGLE );
            info.iInitPointer->restartIfNeeded( true );
            break;

         // Quit
         case L'Q':
         case E_KEY_ESCAPE:
            info.iInitPointer->quitMainLoop();
            break;
      }
   }
}


int myWorld::initGL() {
   // vInitPointer->fullScreen( C_ADD );
   int lReturn = vScene.init();

   vInitPointer->moveMouse( GlobConf.win.width / 2, GlobConf.win.height / 2 );
   vInitPointer->hideMouseCursor();
   return lReturn;
}





// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
