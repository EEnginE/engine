/*!
 * \file rWorld.hpp
 * \brief \b Classes: \a rWorld
 */

#ifndef R_WORLD_H
#define R_WORLD_H

#include "uSignalSlot.hpp"
#include "iInit.hpp"

namespace e_engine {

class rWorld {
      typedef void (*RENDER_FUNC_TD)(iInit *);
   private:
      RENDER_FUNC_TD vRenderFunctionPointer;
      
      uSlot<void, rWorld, bool> vRenderLoopStartSlot;
      uSlot<void, rWorld>       vRenderLoopStopSlot;

      uSlot<void, rWorld>       vPauseRenderLoopSlot;
      uSlot<void, rWorld>       vContinueRenderLoopSlot;

      bool vRenderFunctionPointerSet_B;
      bool vInitObjSet_B;

      bool vRenderLoopRunning_B;
      bool vRenderLoopShouldRun_B;

      bool          vRenderLoopIsPaused_B;
      bool          vRenderLoopShouldPaused_B;
      boost::mutex  vRenderLoopMutex_BT;
      boost::condition_variable vRenderLoopWait_BT;
      
      void renderLoop();

      iInit *vInitPointer;
      boost::thread vRenderLoop_BT;

      void startRenderLoop( bool _wait );
      void stopRenderLoop( );

      void pauseRenderLoop();
      void continueRenderLoop();
      
   public:
      void setInitObj( iInit *_init );
      void setRenderFunc( RENDER_FUNC_TD _func );

      rWorld();
      virtual ~rWorld() {}
};

}

#endif // R_WORLD_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
