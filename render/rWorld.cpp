/*!
 * \file rWorld.cpp
 * \brief \b Classes: \a rWorld
 */

#include "rWorld.hpp"
#include "uLog.hpp"

namespace e_engine {

rWorld::rWorld() {
   vRenderFunctionPointerSet_B = false;
   vInitObjSet_B               = false;

   vRenderLoopRunning_B        = false;
   vRenderLoopShouldRun_B      = false;

   vRenderLoopIsPaused_B       = false;
   vRenderLoopShouldPaused_B   = false;
   
   vViewPort.vNeedUpdate_B     = false;
   vViewPort.x                 = 0;
   vViewPort.y                 = 0;
   vViewPort.width             = 0;
   vViewPort.height            = 0;
   
   vRenderLoopStartSlot.setFunc( &rWorld::startRenderLoop, this );
   vRenderLoopStopSlot.setFunc( &rWorld::stopRenderLoop, this );

   vPauseRenderLoopSlot.setFunc( &rWorld::pauseRenderLoop, this );
   vContinueRenderLoopSlot.setFunc( &rWorld::continueRenderLoop, this );
}

void rWorld::renderLoop() {
   iLOG "Render loop started" END
   vRenderLoopRunning_B = true;
   vInitPointer->makeContextCurrent();  // Only ONE thread can have a context

   if ( GlobConf.win.VSync == true )
      vInitPointer->enableVSync();
   
   while ( vRenderLoopShouldRun_B ) {
      if ( vRenderLoopShouldPaused_B ) {
         boost::unique_lock<boost::mutex> lLock_BT( vRenderLoopMutex_BT );
         vInitPointer->makeNOContextCurrent();
         vRenderLoopIsPaused_B = true;
         while ( vRenderLoopShouldPaused_B ) vRenderLoopWait_BT.wait( lLock_BT );
         while ( !vInitPointer->getHaveContext() /*|| vInitPointer->vWindowRecreate_B*/ ) B_SLEEP( milliseconds, 10 );
#if WINDOWS
         B_SLEEP( milliseconds, 100 ); //!< \todo Remove this workaround for Windows (problem with iContext::makeContextCurrent)
#endif
         vRenderLoopIsPaused_B = false;
         if ( ! vInitPointer->makeContextCurrent() ) {
            eLOG "Failed to make context current ==> quit render loop" END
            return;
         }
      }
      
      if( vViewPort.vNeedUpdate_B ) {
         vViewPort.vNeedUpdate_B = false;
         glViewport( vViewPort.x, vViewPort.y, vViewPort.width, vViewPort.height );
         dLOG "Viewport updated" END
      }

      vRenderFunctionPointer( vInitPointer );
   }

   if ( vInitPointer->getHaveContext() )
      vInitPointer->makeNOContextCurrent();

   vRenderLoopRunning_B = false;

}


void rWorld::startRenderLoop( bool _wait ) {
   if( !vRenderFunctionPointerSet_B ) {
      eLOG "The render function is not set! DO NOT START THE RENDER LOOP" END
      return;
   }
   vRenderLoopShouldRun_B = true;

   vInitPointer->makeNOContextCurrent();
   vRenderLoop_BT = boost::thread( &rWorld::renderLoop, this );

   if ( _wait ) {
      vRenderLoop_BT.join();
      if ( vInitPointer->getHaveContext() ) vInitPointer->makeContextCurrent();  // Only ONE thread can have a context
   }
   
}

void rWorld::stopRenderLoop() {
   vRenderLoopShouldRun_B = false;
   

   if ( vRenderLoopRunning_B )
#if BOOST_VERSION < 105000
   {
      boost::posix_time::time_duration duration = boost::posix_time::milliseconds( GlobConf.timeoutForMainLoopThread_mSec );
      vRenderLoop_BT.timed_join( duration );
   }
#else
      vRenderLoop_BT.try_join_for( boost::chrono::milliseconds( GlobConf.timeoutForMainLoopThread_mSec ) );
#endif

   if ( vRenderLoopRunning_B ) {
      vRenderLoop_BT.interrupt();
      wLOG "Render Loop Timeout reached  -->  kill the thread" END
      vRenderLoopRunning_B = false;
   }

   iLOG "Render loop finished" END
}

void rWorld::pauseRenderLoop() {
   if ( ! vRenderLoopShouldRun_B )
      return;

   vRenderLoopShouldPaused_B = true;
   

   while ( !vRenderLoopIsPaused_B )
      B_SLEEP( milliseconds, 10 );
}

void rWorld::continueRenderLoop() {
   boost::lock_guard<boost::mutex> lLockMain_BT( vRenderLoopMutex_BT );
   vRenderLoopShouldPaused_B = false;
   vRenderLoopWait_BT.notify_one();
}

void rWorld::setRenderFunc( rWorld::RENDER_FUNC_TD _func ) {
   vRenderFunctionPointer      = _func;
   vRenderFunctionPointerSet_B = true;
}

void rWorld::updateViewPort( unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height ) {
   vViewPort.vNeedUpdate_B = true;
   vViewPort.x             = _x;
   vViewPort.y             = _y;
   vViewPort.width         = _width;
   vViewPort.height        = _height;
   return;
}





void rWorld::setInitObj( iInit *_init ) {
   if ( vInitObjSet_B ) {
      eLOG "iInit object is already set and can't be reset! Do nothing" END
      return;
   }

   vInitPointer = _init;
   vInitPointer->addRenderSlots( &vRenderLoopStartSlot, &vRenderLoopStopSlot, &vPauseRenderLoopSlot, &vContinueRenderLoopSlot );

   vInitObjSet_B = true;
}




}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
