/*!
 * \file myWorld.hpp
 * \brief Class myWorld
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

#include <engine.hpp>
#include "cmdANDinit.hpp"
#include "myScene.hpp"

#ifndef HANDLER_HPP
#define HANDLER_HPP

using namespace std;
using namespace e_engine;
using namespace OS_NAMESPACE;

class myWorld final : public rWorld, public rFrameCounter {
   typedef uSlot<void, myWorld, iEventInfo const &> _SLOT_;

 private:
   GLfloat vAlpha;

   std::vector<iDisplays> vDisp_RandR;

   myScene vScene;
   iInit *vInitPointer;

   GLfloat vNearZ;
   GLfloat vFarZ;

   _SLOT_ slotWindowClose;
   _SLOT_ slotResize;
   _SLOT_ slotKey;

 public:
   myWorld( cmdANDinit &_cmd, iInit *_init )
       : rWorld( _init ),
         rFrameCounter( this, true ),
         vScene( _init, _cmd ),
         vInitPointer( _init ),
         vNearZ( _cmd.getNearZ() ),
         vFarZ( _cmd.getFarZ() ),
         slotWindowClose( &myWorld::windowClose, this ),
         slotResize( &myWorld::resize, this ),
         slotKey( &myWorld::key, this ) {

      _init->addWindowCloseSlot( &slotWindowClose );
      _init->addResizeSlot( &slotResize );
      _init->addKeySlot( &slotKey );

      vAlpha = 1;
   }

   ~myWorld();
   myWorld() = delete;


   void windowClose( iEventInfo const &info ) {
      iLOG( "User closed window" );
      info.iInitPointer->closeWindow();
   }
   void key( iEventInfo const &info );
   void resize( iEventInfo const &info ) {
      iLOG( "Window resized: W = ", info.eResize.width, ";  H = ", info.eResize.height );
      updateViewPort( 0, 0, GlobConf.win.width, GlobConf.win.height );
      vScene.calculateProjectionPerspective(
            GlobConf.win.width, GlobConf.win.height, vNearZ, vFarZ, 35.0 );
   }

   int initGL();

   virtual void renderFrame() { vScene.renderScene(); }
};


#endif // HANDLER_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
