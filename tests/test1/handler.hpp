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

extern GLfloat alpha;
extern GLfloat r, g, b, R, G, B;
extern bool    rr, gg, bb;
extern int counter1;

class MyHandler {
      typedef uSlot<void, MyHandler, iEventInfo> _SLOT_;
   private:
      vector<iDisplays> vDisp_RandR;

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
      void windowClose( iEventInfo info ) {
         iLOG "User closed window" END
         info.iInitPointer->closeWindow();
      }
      void key( iEventInfo info );
      void mouse( iEventInfo info );
      void resize( iEventInfo info ) {
         iLOG "Window resized" END
      }

      _SLOT_ *getSWindowClose() {return &slotWindowClose;}
      _SLOT_ *getSResize()      {return &slotResize;}
      _SLOT_ *getSKey()         {return &slotKey;}
      _SLOT_ *getSMouse()       {return &slotMouse;}
};


void render( iEventInfo info );


#endif // HANDLER_HPP