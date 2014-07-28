/*!
 * \file rWorld.cpp
 * \brief \b Classes: \a rWorld
 */

#include "rWorld.hpp"
#include "uLog.hpp"
#include "math.h"

namespace e_engine {

rWorld::rWorld() {
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

   vProjectionMatrix_MAT.set( 0, 0, 1 );
   vProjectionMatrix_MAT.set( 0, 1, 0 );
   vProjectionMatrix_MAT.set( 0, 2, 0 );
   vProjectionMatrix_MAT.set( 0, 3, 0 );

   vProjectionMatrix_MAT.set( 1, 0, 0 );
   vProjectionMatrix_MAT.set( 1, 1, 1 );
   vProjectionMatrix_MAT.set( 1, 2, 0 );
   vProjectionMatrix_MAT.set( 1, 3, 0 );

   vProjectionMatrix_MAT.set( 2, 0, 0 );
   vProjectionMatrix_MAT.set( 2, 1, 0 );
   vProjectionMatrix_MAT.set( 2, 2, 1 );
   vProjectionMatrix_MAT.set( 2, 3, 0 );

   vProjectionMatrix_MAT.set( 3, 0, 0 );
   vProjectionMatrix_MAT.set( 3, 1, 0 );
   vProjectionMatrix_MAT.set( 3, 2, 0 );
   vProjectionMatrix_MAT.set( 3, 3, 1 );
}

rWorld::rWorld( iInit *_init ) {
   vInitObjSet_B               = false; // Will be set true in setInitObj

   vRenderLoopRunning_B        = false;
   vRenderLoopShouldRun_B      = false;

   vRenderLoopIsPaused_B       = false;
   vRenderLoopShouldPaused_B   = false;

   vViewPort.vNeedUpdate_B     = false;
   vViewPort.x                 = 0;
   vViewPort.y                 = 0;
   vViewPort.width             = 0;
   vViewPort.height            = 0;

   vClearCollor.vNeedUpdate_B  = false;
   vClearCollor.r              = 0;
   vClearCollor.g              = 0;
   vClearCollor.b              = 0;
   vClearCollor.a              = 1;

   vRenderLoopStartSlot.setFunc( &rWorld::startRenderLoop, this );
   vRenderLoopStopSlot.setFunc( &rWorld::stopRenderLoop, this );

   vPauseRenderLoopSlot.setFunc( &rWorld::pauseRenderLoop, this );
   vContinueRenderLoopSlot.setFunc( &rWorld::continueRenderLoop, this );

   setInitObj( _init );

   vProjectionMatrix_MAT.set( 0, 0, 1 );
   vProjectionMatrix_MAT.set( 0, 1, 0 );
   vProjectionMatrix_MAT.set( 0, 2, 0 );
   vProjectionMatrix_MAT.set( 0, 3, 0 );

   vProjectionMatrix_MAT.set( 1, 0, 0 );
   vProjectionMatrix_MAT.set( 1, 1, 1 );
   vProjectionMatrix_MAT.set( 1, 2, 0 );
   vProjectionMatrix_MAT.set( 1, 3, 0 );

   vProjectionMatrix_MAT.set( 2, 0, 0 );
   vProjectionMatrix_MAT.set( 2, 1, 0 );
   vProjectionMatrix_MAT.set( 2, 2, 1 );
   vProjectionMatrix_MAT.set( 2, 3, 0 );

   vProjectionMatrix_MAT.set( 3, 0, 0 );
   vProjectionMatrix_MAT.set( 3, 1, 0 );
   vProjectionMatrix_MAT.set( 3, 2, 0 );
   vProjectionMatrix_MAT.set( 3, 3, 1 );
}


void rWorld::renderLoop() {
   iLOG "Render loop started" END
   vRenderLoopRunning_B = true;
   vInitPointer->makeContextCurrent();  // Only ONE thread can have a context

   if( GlobConf.win.VSync == true )
      vInitPointer->enableVSync();

//    GLuint lVertexArray_OGL;

//    glGenVertexArrays( 1, &lVertexArray_OGL );
//    glBindVertexArray( lVertexArray_OGL );

   glClearColor( vClearCollor.r, vClearCollor.g, vClearCollor.b, vClearCollor.a );

   glEnable( GL_CULL_FACE );
//    glEnable( GL_DEPTH_TEST );

//    glBindVertexArray( vInitPointer->getVertexArrayOpenGL() );

   while( vRenderLoopShouldRun_B ) {
      if( vRenderLoopShouldPaused_B ) {
         boost::unique_lock<boost::mutex> lLock_BT( vRenderLoopMutex_BT );
         vInitPointer->makeNOContextCurrent();
         vRenderLoopIsPaused_B = true;
         while( vRenderLoopShouldPaused_B ) vRenderLoopWait_BT.wait( lLock_BT );
         while( !vInitPointer->getHaveContext() /*|| vInitPointer->vWindowRecreate_B*/ ) B_SLEEP( milliseconds, 10 );
#if WINDOWS
         B_SLEEP( milliseconds, 100 ); //!< \todo Remove this workaround for Windows (problem with iContext::makeContextCurrent)
#endif
         vRenderLoopIsPaused_B = false;
         if( ! vInitPointer->makeContextCurrent() ) {
            eLOG "Failed to make context current ==> quit render loop" END
            return;
         }
      }

      if( vViewPort.vNeedUpdate_B ) {
         vViewPort.vNeedUpdate_B = false;
         glViewport( vViewPort.x, vViewPort.y, vViewPort.width, vViewPort.height );
         dLOG "Viewport updated" END
      }

      if( vClearCollor.vNeedUpdate_B ) {
         vClearCollor.vNeedUpdate_B = false;
         glClearColor( vClearCollor.r, vClearCollor.g, vClearCollor.b, vClearCollor.a );
         dLOG "Updatad clear collor: [RGBA] " ADD  vClearCollor.r ADD "; " ADD vClearCollor.g ADD "; " ADD vClearCollor.b ADD "; " ADD vClearCollor.a END
      }

      renderFrame();
      vInitPointer->swapBuffers();
   }

//    glDeleteVertexArrays( 1, &lVertexArray_OGL );

   if( vInitPointer->getHaveContext() )
      vInitPointer->makeNOContextCurrent();

   vRenderLoopRunning_B = false;

}


void rWorld::startRenderLoop( bool _wait ) {
   vRenderLoopShouldRun_B = true;

   vInitPointer->makeNOContextCurrent();
   vRenderLoop_BT = boost::thread( &rWorld::renderLoop, this );

   if( _wait ) {
      vRenderLoop_BT.join();
      if( vInitPointer->getHaveContext() ) vInitPointer->makeContextCurrent();   // Only ONE thread can have a context
   }

}

void rWorld::stopRenderLoop() {
   vRenderLoopShouldRun_B = false;


   if( vRenderLoopRunning_B )
#if BOOST_VERSION < 105000
   {
      boost::posix_time::time_duration duration = boost::posix_time::milliseconds( GlobConf.timeoutForMainLoopThread_mSec );
      vRenderLoop_BT.timed_join( duration );
   }
#else
      vRenderLoop_BT.try_join_for( boost::chrono::milliseconds( GlobConf.timeoutForMainLoopThread_mSec ) );
#endif

   if( vRenderLoopRunning_B ) {
      vRenderLoop_BT.interrupt();
      wLOG "Render Loop Timeout reached  -->  kill the thread" END
      vRenderLoopRunning_B = false;
   }

   iLOG "Render loop finished" END
}

void rWorld::pauseRenderLoop() {
   if( ! vRenderLoopShouldRun_B )
      return;

   vRenderLoopShouldPaused_B = true;


   while( !vRenderLoopIsPaused_B )
      B_SLEEP( milliseconds, 10 );
}

void rWorld::continueRenderLoop() {
   boost::lock_guard<boost::mutex> lLockMain_BT( vRenderLoopMutex_BT );
   vRenderLoopShouldPaused_B = false;
   vRenderLoopWait_BT.notify_one();
}


void rWorld::updateViewPort( unsigned int _x, unsigned int _y, unsigned int _width, unsigned int _height ) {
   vViewPort.vNeedUpdate_B = true;
   vViewPort.x             = _x;
   vViewPort.y             = _y;
   vViewPort.width         = _width;
   vViewPort.height        = _height;
}

void rWorld::updateClearCollor( GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a ) {
   vClearCollor.vNeedUpdate_B = true;
   vClearCollor.r             = _r;
   vClearCollor.g             = _b;
   vClearCollor.b             = _g;
   vClearCollor.a             = _a;
}

namespace {

float degToRad( float _degree ) { return _degree * ( M_PI / 180.0 ); }

}

void rWorld::calculatePerspective( GLfloat _aspactRatio, GLfloat _nearZ, GLfloat _farZ, GLfloat _angle ) {
   GLfloat lRange  = tan( degToRad( _angle / 2.0 ) ) * _nearZ;
   GLfloat lLeft   = (-lRange) * _aspactRatio;
   GLfloat lRight  = lRange * _aspactRatio;
   GLfloat lBottom = (-lRange);
   GLfloat lTop    = lRange;

   vProjectionMatrix_MAT.set( 0, 0, ( 2.0 * _nearZ ) / ( lRight - lLeft ) );
   vProjectionMatrix_MAT.set( 1, 1, ( 2.0 * _nearZ ) / ( lTop - lBottom ) );
   vProjectionMatrix_MAT.set( 2, 0, ( lRight + lLeft ) / ( lRight - lLeft ) );
   vProjectionMatrix_MAT.set( 2, 1, ( lTop + lBottom ) / ( lTop - lBottom ) );
   vProjectionMatrix_MAT.set( 2, 2, - ( _farZ + _nearZ ) / ( _farZ - _nearZ ) );
   vProjectionMatrix_MAT.set( 2, 3, -1.0 );
   vProjectionMatrix_MAT.set( 3, 2, - ( 2.0 * _farZ * _nearZ ) / ( _farZ - _nearZ ) );
   vProjectionMatrix_MAT.set( 3, 3, 0 );
}


void rWorld::setInitObj( iInit *_init ) {
   if( vInitObjSet_B ) {
      eLOG "iInit object is already set and can't be reset! Do nothing" END
      return;
   }

   vInitPointer = _init;
   vInitPointer->addRenderSlots( &vRenderLoopStartSlot, &vRenderLoopStopSlot, &vPauseRenderLoopSlot, &vContinueRenderLoopSlot );

   vInitObjSet_B = true;
}




}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
