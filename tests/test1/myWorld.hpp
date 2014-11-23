/*!
 * \file myWorld.hpp
 * \brief Class myWorld
 */

#include <engine.hpp>
#include "cmdANDinit.hpp"
#include "myScene.hpp"

#ifndef HANDLER_HPP
#define HANDLER_HPP

using namespace std;
using namespace e_engine;
using namespace OS_NAMESPACE;

class myWorld final : public rWorld {
      typedef uSlot<void, myWorld, iEventInfo> _SLOT_;
   private:
      GLfloat vAlpha;

      std::vector<iDisplays> vDisp_RandR;

      GLfloat vNearZ;
      GLfloat vFarZ;

      myScene vScene;

      iInit  *vInitPointer;

      _SLOT_ slotWindowClose;
      _SLOT_ slotResize;
      _SLOT_ slotKey;
   public:
      myWorld( cmdANDinit &_cmd, iInit *_init ) :
         rWorld( _init ),
         vScene( _init, _cmd ),
         vInitPointer( _init ),
         vNearZ( _cmd.getNearZ() ),
         vFarZ( _cmd.getFarZ() ),
         slotWindowClose( &myWorld::windowClose, this ),
         slotResize( &myWorld::resize, this ),
         slotKey( &myWorld::key, this ) {

         _init->addWindowCloseSlot( &slotWindowClose );
         _init->addResizeSlot( &slotResize );
         _init->addKeySlot( &slotKey );

         vAlpha        = 1;
      }

      ~myWorld();
      myWorld() = delete;


      void windowClose( iEventInfo info ) {
         iLOG( "User closed window" );
         info.iInitPointer->closeWindow();
      }
      void key( iEventInfo info );
      void resize( iEventInfo info ) {
         iLOG( "Window resized: W = ", info.eResize.width, ";  H = ", info.eResize.height );
         updateViewPort( 0, 0, GlobConf.win.width, GlobConf.win.height );
         vScene.calculateProjectionPerspective( GlobConf.win.width, GlobConf.win.height, vNearZ, vFarZ, 35.0 );
      }

      int initGL();

      virtual void renderFrame() {
         vScene.renderScene();
      }
};


#endif // HANDLER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
