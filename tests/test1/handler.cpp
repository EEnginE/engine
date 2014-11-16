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
   if( info.iMouse.button <= E_MOUSE_6 )  // We dont want move events and etc.
      switch( info.iMouse.button ) {
         case E_MOUSE_LEFT:
            vObject1.setRotation( rVec3f( 0, 1, 1 ), ++vCurrentRot );
            break;
         case E_MOUSE_RIGHT:
            vObject1.setRotation( rVec3f( 0, 1, 1 ), --vCurrentRot );
            break;
         default:
            iLOG( "Button ", info.iMouse.state == E_PRESSED ? "pressed:  '" : "released: '", (uint16_t)info.iMouse.button, "'" );
            break;
      }

#if 0
   if( info.iMouse.posX != (int)( GlobConf.win.width / 2 ) || info.iMouse.posY != (int)( GlobConf.win.height / 2 ) ) {
      dLOG(
      "  DELATA X: ", info.iMouse.posX - signed( GlobConf.win.width  / 2 ),
      "  DELATA Y: ", info.iMouse.posY - signed( GlobConf.win.height / 2 )
      );
      vInitPointer->moveMouse( GlobConf.win.width / 2, GlobConf.win.height / 2 );
   }
#endif
}

void MyHandler::key( iEventInfo info ) {
   if( vDisp_RandR.empty() )
      vDisp_RandR = info.iInitPointer->getDisplayResolutions();

   char lHex_CSTR[6];

#if USE_OLD_KEY_BINDINGS
   vector<iDisplays> displays;
   snprintf( lHex_CSTR, 5, "%04X", info.eKey.key );
   iLOG( "Key ", info.eKey.state == E_PRESSED ? "pressed:  '" : "released: '", info.eKey.key, "' - ", "0x", lHex_CSTR );
#endif

   rVec3f lTemp; // For Camara control

   if( info.eKey.state == E_PRESSED ) {
      switch( info.eKey.key ) {
#if USE_OLD_KEY_BINDINGS
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
         case L't':              vAlpha = ( ( vAlpha - 0.1 ) < 0 ) ? 0 : vAlpha -= 0.01; updateClearColor( 0, 0, 0, vAlpha ); break;
         case L'T':              vAlpha = ( ( vAlpha + 0.1 ) > 1 ) ? 1 : vAlpha += 0.01; updateClearColor( 0, 0, 0, vAlpha ); break;
         case L'o':              info.iInitPointer->changeWindowConfig( 800, 600, 10, 10 ); break;
         case L'F':
         case L'f':              info.iInitPointer->fullScreen( e_engine::C_TOGGLE ); info.iInitPointer->restartIfNeeded( true ); break;
         case L's':              info.iInitPointer->fullScreenMultiMonitor(); break;
         case L'a':              if( vDisp_RandR.size() > 0 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[0] ); break;
         case L'd':              if( vDisp_RandR.size() > 1 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[1] ); break;
         case L'r':              info.iInitPointer->restart( true ); break;
         case E_KEY_UP:          vObject1.move( 0, 0, 0.1 );  vObject1.createResultMatrix(); break;
         case E_KEY_DOWN:        vObject1.move( 0, 0, -0.1 ); vObject1.createResultMatrix(); break;
         case E_KEY_LEFT:        vObject1.move( -0.01, 0, 0 ); vObject1.createResultMatrix(); break;
         case E_KEY_RIGHT:       vObject1.move( 0.01, 0, 0 );  vObject1.createResultMatrix(); break;
         case E_KEY_PAGE_UP:     vObject1.move( 0, 0.01, 0 );  vObject1.createResultMatrix(); break;
         case E_KEY_PAGE_DOWN:   vObject1.move( 0, -0.01, 0 ); vObject1.createResultMatrix(); break;
         case L'1':
            calculatePerspective( 0.1, 100.0, 35.0 );
            vObject1.createResultMatrix();
            break;
         case L'p':
            iLOG( "Pausing" );
            info.iInitPointer->pauseMainLoop( true );
            B_SLEEP( seconds, 5 );
            iLOG( "Unpausing" );
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
         case L'g': info.iInitPointer->grabMouse(); break;
         case L'G': info.iInitPointer->freeMouse(); break;
         case L'w': info.iInitPointer->moveMouse( GlobConf.win.width / 2, GlobConf.win.height / 2 ); break;
         case L'c': info.iInitPointer->hideMouseCursor(); break;
         case L'C': info.iInitPointer->showMouseCursor(); break;
         case L'q':
         case L'Q':
         case E_KEY_ESCAPE:      info.iInitPointer->closeWindow(); break;
         case L'u':
            if( info.iInitPointer->getKeyState( L'n' ) == E_PRESSED ) {
               iLOG( "JAAAAA" );;
            }
            break;
#else
            // Handle fullscreen
         case E_KEY_F8:  info.iInitPointer->setAttribute( C_TOGGLE, MAXIMIZED_VERT, MAXIMIZED_HORZ ); break;
         case E_KEY_F9:  info.iInitPointer->fullScreenMultiMonitor(); break;
         case E_KEY_F11: info.iInitPointer->fullScreen( e_engine::C_TOGGLE ); info.iInitPointer->restartIfNeeded( true ); break;
         case E_KEY_F10: if( vDisp_RandR.size() > 0 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[0] ); break;
         case E_KEY_F12: if( vDisp_RandR.size() > 1 ) info.iInitPointer->setFullScreenMonitor( vDisp_RandR[1] ); break;

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

            // Camera control

         case L'W':
         case L'w': 
            vCameraPos += 0.25f * vCameraLook;
            setCamera( vCameraPos, vCameraLook, rVec3f(0 , 1, 0) );
            break;

         case L'A':
         case L'a':
            vXVector = rVectorMath::crossProduct( vCameraLook, vCameraUp );
            vXVector.normalize();
            vCameraPos -= 0.25f * vXVector;
            setCamera( vCameraPos, vCameraLook, vCameraUp );
            break;

         case L'S':
         case L's':
            vCameraPos -= 0.25f * vCameraLook;
            setCamera( vCameraPos, vCameraLook, rVec3f(0 , 1, 0) );
            break;

         case L'D':
         case L'd':
            vXVector = rVectorMath::crossProduct( vCameraLook, vCameraUp );
            vXVector.normalize();
            vCameraPos += 0.25f * vXVector;
            setCamera( vCameraPos, vCameraLook, vCameraUp );
            break;

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
         case L'q':
         case L'Q':
         case E_KEY_ESCAPE: info.iInitPointer->closeWindow(); break;

         default:
			#ifdef _MSC_VER
			 _snprintf(lHex_CSTR, 5, "%04X", info.eKey.key);
			#else
             snprintf( lHex_CSTR, 5, "%04X", info.eKey.key );
			#endif
            iLOG( "Key ", info.eKey.state == E_PRESSED ? "pressed:  '" : "released: '", info.eKey.key, "' - ", "0x", lHex_CSTR );
#endif
      }
   }
}


int MyHandler::initGL() {
   int lReturn = vObject1.loadData( this );
   // vInitPointer->grabMouse();
   // vInitPointer->fullScreen( C_ADD );
   vInitPointer->moveMouse( GlobConf.win.width / 2, GlobConf.win.height / 2 );
   vObject1.setPosition( rVec3f( 0, 0, -5 ) );
   calculateProjectionPerspective( GlobConf.win.width, GlobConf.win.height, 0.1, 100.0, 35.0 );
   vCameraLook = vObject1.getPosition();
   vCameraLook.normalize();
   setCamera( vCameraPos, vCameraLook, vCameraUp );
   updateCameraSpaceMatrix();
   vObject1.updateUniformsAlways( true );
   vObject1.updateFinalMatrix( true );
   return lReturn;
}





// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
