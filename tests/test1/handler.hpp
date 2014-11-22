/*!
 * \file handler.hpp
 * \brief Class MyHandler
 */

#include <engine.hpp>
#include "cmdANDinit.hpp"
#include "myScene.hpp"

#ifndef HANDLER_HPP
#define HANDLER_HPP

using namespace std;
using namespace e_engine;
using namespace OS_NAMESPACE;

class MyHandler final : public rWorld {
      typedef uSlot<void, MyHandler, iEventInfo> _SLOT_;
   private:
      GLfloat vAlpha;

      std::vector<iDisplays> vDisp_RandR;

      GLfloat vCurrentRot;

      myScene vScene;

      iInit  *vInitPointer;

      MyHandler();
   public:
      _SLOT_ slotWindowClose;
      _SLOT_ slotResize;
      _SLOT_ slotKey;
      MyHandler( cmdANDinit &_cmd, iInit *_init ) :
         rWorld( _init ),
         vScene( _init, _cmd ),
         vInitPointer( _init ) {
         slotWindowClose.setFunc( &MyHandler::windowClose, this );
         slotResize.setFunc( &MyHandler::resize, this );
         slotKey.setFunc( &MyHandler::key, this );

         vAlpha        = 1;
         vCurrentRot   = 0;
      }
      ~MyHandler();
      void windowClose( iEventInfo info ) {
         iLOG( "User closed window" );
         info.iInitPointer->closeWindow();
      }
      void key( iEventInfo info );
      void resize( iEventInfo info ) {
         iLOG( "Window resized: W = ", info.eResize.width, ";  H = ", info.eResize.height );
         updateViewPort( 0, 0, GlobConf.win.width, GlobConf.win.height );
         vScene.calculateProjectionPerspective( GlobConf.win.width, GlobConf.win.height, 0.1, 100.0, 35.0 );
      }

      int initGL();

      virtual void renderFrame() {
         vScene.renderScene();
      }


      _SLOT_ *getSWindowClose() {return &slotWindowClose;}
      _SLOT_ *getSResize()      {return &slotResize;}
      _SLOT_ *getSKey()         {return &slotKey;}
};


#endif // HANDLER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
