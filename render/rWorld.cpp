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

   vClearColor.vNeedUpdate_B  = false;
   vClearColor.r              = 0;
   vClearColor.g              = 0;
   vClearColor.b              = 0;
   vClearColor.a              = 1;

   vRenderLoopStartSlot.setFunc( &rWorld::startRenderLoop, this );
   vRenderLoopStopSlot.setFunc( &rWorld::stopRenderLoop, this );

   vPauseRenderLoopSlot.setFunc( &rWorld::pauseRenderLoop, this );
   vContinueRenderLoopSlot.setFunc( &rWorld::continueRenderLoop, this );

   setInitObj( _init );
}


void rWorld::renderLoop() {
   iLOG( "Render loop started" );
   vRenderLoopRunning_B = true;
   vInitPointer->makeContextCurrent();  // Only ONE thread can have a context

   if( GlobConf.win.VSync == true )
      vInitPointer->enableVSync();


   glClearColor( vClearColor.r, vClearColor.g, vClearColor.b, vClearColor.a );

//    glEnable( GL_CULL_FACE );
   glEnable( GL_DEPTH_TEST );
   glEnable( GL_MULTISAMPLE );

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
            eLOG( "Failed to make context current ==> Quitting render loop" );
            return;
         }
      }

      if( vViewPort.vNeedUpdate_B ) {
         vViewPort.vNeedUpdate_B = false;
         glViewport( vViewPort.x, vViewPort.y, vViewPort.width, vViewPort.height );
         dLOG( "Viewport updated" );
      }

      if( vClearColor.vNeedUpdate_B ) {
         vClearColor.vNeedUpdate_B = false;
         glClearColor( vClearColor.r, vClearColor.g, vClearColor.b, vClearColor.a );
         dLOG( "Updated clear color: [RGBA] ",  vClearColor.r, "; ", vClearColor.g, "; ", vClearColor.b, "; ", vClearColor.a );
      }

      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );

      renderFrame();
      vInitPointer->swapBuffers();
   }

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
      wLOG( "Render Loop Timeout reached  -->  Killing the thread" );
      vRenderLoopRunning_B = false;
   }

   iLOG( "Render loop finished" );
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

void rWorld::updateClearColor( GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a ) {
   vClearColor.vNeedUpdate_B = true;
   vClearColor.r             = _r;
   vClearColor.g             = _b;
   vClearColor.b             = _g;
   vClearColor.a             = _a;
}

// void rWorld::calculatePerspective( GLfloat _aspactRatio, GLfloat _nearZ, GLfloat _farZ, GLfloat _fofy ) {
//    GLfloat f = ( 1.0f / tan( degToRad( _fofy / 2 ) ) );
// 
//    vProjectionMatrix_MAT.fill( 0 );
//    vProjectionMatrix_MAT.set( 0, 0, f / _aspactRatio );
//    vProjectionMatrix_MAT.set( 1, 1, f );
//    vProjectionMatrix_MAT.set( 2, 2, ( _farZ + _nearZ ) / ( _nearZ - _farZ ) );
//    vProjectionMatrix_MAT.set( 3, 2, ( 2 * _farZ * _nearZ ) / ( _nearZ - _farZ ) );
//    vProjectionMatrix_MAT.set( 2, 3, -1 );
// }
// 
// 
// void rWorld::setCamera( rVec3f &_position, rVec3f &_lookAt, rVec3f &_upVector ) {
//    rVec3f n = _lookAt;
//    rVec3f u = _upVector;
// 
//    rVectorMath::normalize( n );
//    rVectorMath::normalize( u );
// 
//    u = rVectorMath::crossProduct( u, n );
// 
//    rVec3f v = rVectorMath::crossProduct( n, u );
// 
//    rMat4f lMatTranslate;
//    rMat4f lRotateMatrix;
//    lMatTranslate.toIdentityMatrix();
//    lRotateMatrix.toIdentityMatrix();
//    
//    lMatTranslate.set( 3, 0, -_position[0] );
//    lMatTranslate.set( 3, 1, -_position[1] );
//    lMatTranslate.set( 3, 2, -_position[2] );
//    
//    lRotateMatrix.set( 0, 0, u[0] );
//    lRotateMatrix.set( 1, 0, u[1] );
//    lRotateMatrix.set( 2, 0, u[2] );
//    
//    lRotateMatrix.set( 0, 1, v[0] );
//    lRotateMatrix.set( 1, 1, v[1] );
//    lRotateMatrix.set( 2, 1, v[2] );
//    
//    lRotateMatrix.set( 0, 2, n[0] );
//    lRotateMatrix.set( 1, 2, n[1] );
//    lRotateMatrix.set( 2, 2, n[2] );
//    
//    vCameraMatrix_MAT = lRotateMatrix * lMatTranslate;
// }



void rWorld::setInitObj( iInit *_init ) {
   if( vInitObjSet_B ) {
      eLOG( "iInit object is already set and can't be reset! Doing nothing" );
      return;
   }

   vInitPointer = _init;
   vInitPointer->addRenderSlots<e_engine::rWorld>( &vRenderLoopStartSlot, &vRenderLoopStopSlot, &vPauseRenderLoopSlot, &vContinueRenderLoopSlot );

   vInitObjSet_B = true;
}




}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
