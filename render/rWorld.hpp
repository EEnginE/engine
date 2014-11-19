/*!
 * \file rWorld.hpp
 * \brief \b Classes: \a rWorld
 */

#ifndef R_WORLD_H
#define R_WORLD_H

#include "uSignalSlot.hpp"
#include "iInit.hpp"
#include "rMatrixWorldBase.hpp"
#include "engine_render_Export.hpp"

namespace e_engine {

class render_EXPORT rWorld : public rMatrixWorldBase<float> {
   private:
      uSlot<void, rWorld, bool> vRenderLoopStartSlot;
      uSlot<void, rWorld>       vRenderLoopStopSlot;

      uSlot<void, rWorld>       vPauseRenderLoopSlot;
      uSlot<void, rWorld>       vContinueRenderLoopSlot;

      bool vInitObjSet_B;

      bool vRenderLoopRunning_B;
      bool vRenderLoopShouldRun_B;

      bool          vRenderLoopIsPaused_B;
      bool          vRenderLoopShouldPaused_B;
      boost::mutex  vRenderLoopMutex_BT;
      boost::condition_variable vRenderLoopWait_BT;

      struct {
         bool vNeedUpdate_B;
         unsigned int x;
         unsigned int y;
         unsigned int width;
         unsigned int height;
      } vViewPort;

      struct {
         bool vNeedUpdate_B;
         GLfloat r;
         GLfloat g;
         GLfloat b;
         GLfloat a;
      } vClearColor;

      void renderLoop();

      iInit *vInitPointer;
      boost::thread vRenderLoop_BT;

      void startRenderLoop( bool _wait );
      void stopRenderLoop( );

      void pauseRenderLoop();
      void continueRenderLoop();

      rWorld();

   public:
      void setInitObj( iInit *_init );

      virtual void renderFrame() = 0;

      void updateViewPort( unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height );
      void updateClearColor( GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a );


      rWorld( iInit *_init );
      virtual ~rWorld() {}
};

}

#endif // R_WORLD_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
