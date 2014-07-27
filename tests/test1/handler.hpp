/*!
 * \file handler.hpp
 * \brief Class MyHandler
 */

#include <engine.hpp>

#ifndef HANDLER_HPP
#define HANDLER_HPP

using namespace std;
using namespace e_engine;
using namespace OS_NAMESPACE;

class MyHandler : public rWorld {
      typedef uSlot<void, MyHandler, iEventInfo> _SLOT_;
   private:
      vector<iDisplays> vDisp_RandR;

      string  vDataRoot_str;

      rNormalObject vObject1;
      
      GLfloat vAlpha;

      MyHandler() : rWorld( nullptr ), vObject1( "OBJ_1", nullptr ) {}
   public:
      _SLOT_ slotWindowClose;
      _SLOT_ slotResize;
      _SLOT_ slotKey;
      _SLOT_ slotMouse;
      MyHandler( string _dataRoot, iInit *_init ) : rWorld( _init ), vObject1( "OBJ_1", _init ) {
         slotWindowClose.setFunc( &MyHandler::windowClose, this );
         slotResize.setFunc( &MyHandler::resize, this );
         slotKey.setFunc( &MyHandler::key, this );
         slotMouse.setFunc( &MyHandler::mouse, this );

         vDataRoot_str = _dataRoot;
         vAlpha        = 1;

         vObject1.addData( vDataRoot_str + "/mesh.obj" );
         vObject1.addShader( vDataRoot_str + "shaders/triangle1" );
      }
      ~MyHandler();
      void windowClose( iEventInfo info ) {
         iLOG "User closed window" END
         info.iInitPointer->closeWindow();
      }
      void key( iEventInfo info );
      void mouse( iEventInfo info );
      void resize( iEventInfo info ) {
         iLOG "Window resized" END
         updateViewPort( 0, 0, info.eResize.width, info.eResize.height );
      }

      int initGL() {return vObject1.loadData();}

      virtual void renderFrame() {vObject1.render();}


      _SLOT_ *getSWindowClose() {return &slotWindowClose;}
      _SLOT_ *getSResize()      {return &slotResize;}
      _SLOT_ *getSKey()         {return &slotKey;}
      _SLOT_ *getSMouse()       {return &slotMouse;}
};


#endif // HANDLER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
