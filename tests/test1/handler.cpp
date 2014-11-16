/*!
 * \file handler.cpp
 * \brief Class MyHandler
 */

#include "handler.hpp"
#include "config.hpp"

#include <GL/glew.h>

#define USE_OLD_KEY_BINDINGS 0


MyHandler::~MyHandler() {}


void MyHandler::mouse( iEventInfo info ) {
   if ( info.iMouse.button <= E_MOUSE_6 ) { // We dont want move events and etc.
      switch ( info.iMouse.button ) {
         case E_MOUSE_LEFT:
            vObject1.setRotation( rVec3f( 0, 1, 1 ), ++vCurrentRot );
            break;
         case E_MOUSE_RIGHT:
            vObject1.setRotation( rVec3f( 0, 1, 1 ), --vCurrentRot );
            break;
         default:
            iLOG( "Button ", info.iMouse.state == E_PRESSED ? "pressed:  '" : "released: '", ( uint16_t )info.iMouse.button, "'" );
            break;
      }
   }
}

void MyHandler::key( iEventInfo info ) {
   if ( vDisp_RandR.empty() )
      vDisp_RandR = info.iInitPointer->getDisplayResolutions();

   char lHex_CSTR[6];

   rVec3f lTemp; // For Camara control

   if ( info.eKey.state == E_PRESSED ) {
      switch ( info.eKey.key ) {
            // Handle fullscreen
         case E_KEY_F8:  info.iInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_VERT, MAXIMIZED_HORZ ); break;
         case E_KEY_F9:  info.iInitPointer->fullScreenMultiMonitor(); break;
         case E_KEY_F11: info.iInitPointer->fullScreen( e_engine::C_TOGGLE ); info.iInitPointer->restartIfNeeded( true ); break;
         case E_KEY_F10: if ( vDisp_RandR.size() > 0 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[0] ); break;
         case E_KEY_F12: if ( vDisp_RandR.size() > 1 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[1] ); break;

            // Mouse control
         case L'g': info.iInitPointer->grabMouse(); break;
         case L'G': info.iInitPointer->freeMouse(); break;
         case L'c': info.iInitPointer->hideMouseCursor(); break;
         case L'C': info.iInitPointer->showMouseCursor(); break;

            // Object control
//          case E_KEY_UP:        vObject1.addPositionDelta( rVec3f( 0, 0, -0.1 ) );  vObject1.updateFinalMatrix(); break;
//          case E_KEY_DOWN:      vObject1.addPositionDelta( rVec3f( 0, 0, 0.1 ) );   vObject1.updateFinalMatrix(); break;
//          case E_KEY_LEFT:      vObject1.addPositionDelta( rVec3f( -0.01, 0, 0 ) ); vObject1.updateFinalMatrix(); break;
//          case E_KEY_RIGHT:     vObject1.addPositionDelta( rVec3f( 0.01, 0, 0 ) );  vObject1.updateFinalMatrix(); break;
//          case E_KEY_PAGE_UP:   vObject1.addPositionDelta( rVec3f( 0, 0.01, 0 ) );  vObject1.updateFinalMatrix(); break;
//          case E_KEY_PAGE_DOWN: vObject1.addPositionDelta( rVec3f( 0, -0.01, 0 ) ); vObject1.updateFinalMatrix(); break;

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
         case E_KEY_ESCAPE: info.iInitPointer->closeWindow(); break;
         
         case L'w':
         case L'a':
         case L's':
         case L'd':
         case L'q':
         case L'e': break;

         default:
#ifdef _MSC_VER
            _snprintf( lHex_CSTR, 5, "%04X", info.eKey.key );
#else
            snprintf( lHex_CSTR, 5, "%04X", info.eKey.key );
#endif
            iLOG( "Key ", info.eKey.state == E_PRESSED ? "pressed:  '" : "released: '", info.eKey.key, "' - ", "0x", lHex_CSTR );
      }
   }
}


int MyHandler::initGL() {
   int lReturn = vObject1.loadData( this );
   // vInitPointer->grabMouse();
   // vInitPointer->fullScreen( C_ADD );
   vInitPointer->moveMouse( GlobConf.win.width / 2, GlobConf.win.height / 2 );
   vInitPointer->hideMouseCursor();
   vObject1.setPosition( rVec3f( 0, 0, -5 ) );
   calculateProjectionPerspective( GlobConf.win.width, GlobConf.win.height, 0.1, 100.0, 35.0 );
   vCameraDirection = vObject1.getPosition();
   vCameraDirection.normalize();
   setCamera( vCameraPos, vCameraPos + vCameraDirection, vCameraUp );
   updateCameraSpaceMatrix();
   vObject1.updateUniformsAlways( true );
   vObject1.updateFinalMatrix( true );
   return lReturn;
}





// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
