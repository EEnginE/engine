/*!
 * \file rWorld.cpp
 * \brief \b Classes: \a rWorld
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "rWorld.hpp"
#include "uLog.hpp"
#include "math.h"

namespace e_engine {

rWorld::rWorld( iInit *_init )
    : vRenderLoopStartSlot( &rWorld::startRenderLoop, this ),
      vRenderLoopStopSlot( &rWorld::stopRenderLoop, this ),

      vPauseRenderLoopSlot( &rWorld::pauseRenderLoop, this ),
      vContinueRenderLoopSlot( &rWorld::continueRenderLoop, this ) {


   vInitObjSet_B = false; // Will be set true in setInitObj

   vRenderLoopRunning_B = false;
   vRenderLoopShouldRun_B = false;

   vRenderLoopIsPaused_B = false;
   vRenderLoopShouldPaused_B = false;

   vViewPort.vNeedUpdate_B = false;
   vViewPort.x = 0;
   vViewPort.y = 0;
   vViewPort.width = 0;
   vViewPort.height = 0;

   vClearColor.vNeedUpdate_B = false;
   vClearColor.r = 0;
   vClearColor.g = 0;
   vClearColor.b = 0;
   vClearColor.a = 1;

   vRenderedFrames = 0;

   setInitObj( _init );
}


void rWorld::renderLoop() {
   LOG.nameThread( L"RENDER" );
   iLOG( "Render loop started" );
   vRenderLoopRunning_B = true;

   vInitPointer->makeContextCurrent(); // Only ONE thread can have a context

   if ( GlobConf.win.VSync == true )
      vInitPointer->enableVSync();


   glClearColor( vClearColor.r, vClearColor.g, vClearColor.b, vClearColor.a );

   glEnable( GL_CULL_FACE );
   glEnable( GL_DEPTH_TEST );
   glEnable( GL_MULTISAMPLE );

   while ( vRenderLoopShouldRun_B ) {
      if ( vRenderLoopShouldPaused_B ) {
         std::unique_lock<std::mutex> lLock_BT( vRenderLoopMutex_BT );
         vInitPointer->makeNOContextCurrent();
         vRenderLoopIsPaused_B = true;
         while ( vRenderLoopShouldPaused_B )
            vRenderLoopWait_BT.wait( lLock_BT );
         while ( !vInitPointer->getHaveContext() /*|| vInitPointer->vWindowRecreate_B*/ )
            B_SLEEP( milliseconds, 10 );
#if WINDOWS
         B_SLEEP( milliseconds, 100 ); //!< \todo Remove this workaround for Windows (problem with
// iContext::makeContextCurrent)
#endif
         vRenderLoopIsPaused_B = false;
         if ( !vInitPointer->makeContextCurrent() ) {
            eLOG( "Failed to make context current ==> Quitting render loop" );
            return;
         }
      }

      if ( vViewPort.vNeedUpdate_B ) {
         vViewPort.vNeedUpdate_B = false;
         glViewport( vViewPort.x, vViewPort.y, vViewPort.width, vViewPort.height );
         dLOG( "Viewport updated" );
      }

      if ( vClearColor.vNeedUpdate_B ) {
         vClearColor.vNeedUpdate_B = false;
         glClearColor( vClearColor.r, vClearColor.g, vClearColor.b, vClearColor.a );
         dLOG( "Updated clear color: [RGBA] ",
               vClearColor.r,
               "; ",
               vClearColor.g,
               "; ",
               vClearColor.b,
               "; ",
               vClearColor.a );
      }

      glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT );


      ++vRenderedFrames;
      renderFrame();
      vInitPointer->swapBuffers();
   }

   if ( vInitPointer->getHaveContext() )
      vInitPointer->makeNOContextCurrent();

   iLOG( "Render Loop finished" );
   vRenderLoopRunning_B = false;
}


void rWorld::startRenderLoop( bool _wait ) {
   vRenderLoopShouldRun_B = true;

   vInitPointer->makeNOContextCurrent();
   vRenderLoop_BT = std::thread( &rWorld::renderLoop, this );

   if ( _wait ) {
      vRenderLoop_BT.join();
      if ( vInitPointer->getHaveContext() )
         vInitPointer->makeContextCurrent(); // Only ONE thread can have a context
   }
}

void rWorld::stopRenderLoop() {
   vRenderLoopShouldRun_B = false;

#if 0
   if( vRenderLoop_BT.joinable() )
      vRenderLoop_BT.join(); // Segfault here. Why!?
#else
   while ( vRenderLoopRunning_B )
      B_SLEEP( milliseconds, 100 ); /// \todo Fix this workaround
#endif

   if ( vRenderLoopRunning_B ) {
      wLOG( "Render Loop thread finished abnormaly" );
      vRenderLoopRunning_B = false;
   }
}

void rWorld::pauseRenderLoop() {
   if ( !vRenderLoopShouldRun_B )
      return;

   vRenderLoopShouldPaused_B = true;


   while ( !vRenderLoopIsPaused_B )
      B_SLEEP( milliseconds, 10 );
}

void rWorld::continueRenderLoop() {
   std::lock_guard<std::mutex> lLockMain_BT( vRenderLoopMutex_BT );
   vRenderLoopShouldPaused_B = false;
   vRenderLoopWait_BT.notify_one();
}


void rWorld::updateViewPort( unsigned int _x,
                             unsigned int _y,
                             unsigned int _width,
                             unsigned int _height ) {
   vViewPort.vNeedUpdate_B = true;
   vViewPort.x = _x;
   vViewPort.y = _y;
   vViewPort.width = _width;
   vViewPort.height = _height;
}

void rWorld::updateClearColor( GLfloat _r, GLfloat _g, GLfloat _b, GLfloat _a ) {
   vClearColor.vNeedUpdate_B = true;
   vClearColor.r = _r;
   vClearColor.g = _b;
   vClearColor.b = _g;
   vClearColor.a = _a;
}


void rWorld::setInitObj( iInit *_init ) {
   if ( vInitObjSet_B ) {
      eLOG( "iInit object is already set and can't be reset! Doing nothing" );
      return;
   }

   vInitPointer = _init;
   vInitPointer->addRenderSlots<e_engine::rWorld>( &vRenderLoopStartSlot,
                                                   &vRenderLoopStopSlot,
                                                   &vPauseRenderLoopSlot,
                                                   &vContinueRenderLoopSlot );

   vInitObjSet_B = true;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
