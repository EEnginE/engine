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

      GLuint vVertexArray;
      
      GLuint vVertexBufferObject;
      GLuint vIndexBufferObject;
      
      GLuint vShaderProgram;
      
      rLinker vProgram;
      string  vDataRoot_str;
            
      rLoader_3D_f_OBJ data1;
   public:
      _SLOT_ slotWindowClose;
      _SLOT_ slotResize;
      _SLOT_ slotKey;
      _SLOT_ slotMouse;
      MyHandler( string _dataRoot ) {
         slotWindowClose.setFunc( &MyHandler::windowClose, this );
         slotResize.setFunc( &MyHandler::resize, this );
         slotKey.setFunc( &MyHandler::key, this );
         slotMouse.setFunc( &MyHandler::mouse, this );
         
         vDataRoot_str = _dataRoot;
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
      }
      
      void initGL();
      void doRenderTriangle( iInit *_init );

      _SLOT_ *getSWindowClose() {return &slotWindowClose;}
      _SLOT_ *getSResize()      {return &slotResize;}
      _SLOT_ *getSKey()         {return &slotKey;}
      _SLOT_ *getSMouse()       {return &slotMouse;}
};

extern MyHandler *_HANDLER_;

void render( iInit * _init );
void renderTriangle( iInit * _init );


#endif // HANDLER_HPP