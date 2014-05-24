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
      typedef eSlot<void, MyHandler, eWinInfo> _SLOT_;
   private:
      vector<eDisplays> vDisp_RandR;

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


void render( eWinInfo info );


#endif // HANDLER_HPP