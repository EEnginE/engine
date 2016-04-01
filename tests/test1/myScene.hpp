/*!
 * \file myScene.hpp
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

#ifndef MY_SCENE_HPP
#define MY_SCENE_HPP

#include <engine.hpp>
#include "cmdANDinit.hpp"

using e_engine::rScene;
using e_engine::rCameraHandler;
using e_engine::rSimpleMesh;
using e_engine::rPointLight;
using e_engine::rDirectionalLight;
using e_engine::uSlot;
using e_engine::iInit;
using e_engine::iEventInfo;
using e_engine::rWorld;

class myScene final : public rScene<float>/*, public rCameraHandler<float>*/ {
   typedef uSlot<void, myScene, iEventInfo const &> _SLOT_;

 private:
   std::vector<rSimpleMesh> vObjects;

   rPointLight<float> vLight1;
   rPointLight<float> vLight2;
   rDirectionalLight<float> vLight3;

   std::string vShader_str;
   std::string vNormalShader_str;

   std::string vFilePath;

   _SLOT_ vKeySlot;
   float vRotationAngle;
   bool vRenderNormals;

 public:
   myScene() = delete;

   myScene( rWorld *_world, cmdANDinit &_cmd )
       : rScene( "MAIN SCENE", _world ),
         //rCameraHandler( this, _init ),
         vLight1( this, "L1" ),
         vLight2( this, "L2" ),
         vLight3( "L3", e_engine::rVec3f( 0.5, -1, 0.5 ) ),
         vShader_str( _cmd.getShader() ),
         vNormalShader_str( _cmd.getNormalShader() ),
         vFilePath( _cmd.getMesh() ),
         vKeySlot( &myScene::keySlot, this ),
         vRotationAngle( 0 ),
         vRenderNormals( _cmd.getRenderNormals() ) {
      _world->getInitPtr()->addKeySlot( &vKeySlot );
   }

   int init();

   void keySlot( iEventInfo const &_inf );

   virtual void afterCameraUpdate();
};

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
