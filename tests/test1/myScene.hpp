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

#include "SPIRV_triangle1.hpp"
#include "cmdANDinit.hpp"
#include <engine.hpp>
#include "SPIRV_deferred1.hpp"
#include "SPIRV_deferred2.hpp"

using e_engine::SPIRV_deferred1;
using e_engine::SPIRV_deferred2;
using e_engine::SPIRV_triangle1;
using e_engine::iEventInfo;
using e_engine::iInit;
using e_engine::rCameraHandler;
using e_engine::rDirectionalLightF;
using e_engine::rPipeline;
using e_engine::rPointLightF;
using e_engine::rScene;
using e_engine::rSimpleMesh;
using e_engine::rWorld;
using e_engine::uSlot;

class myScene final : public rScene<float>, public rCameraHandler<float> {
  typedef uSlot<void, myScene, iEventInfo const &> _SLOT_;

 private:
  OBJECTS<rSimpleMesh>        vObjects;
  OBJECTS<rPointLightF>       vPointLights;
  OBJECTS<rDirectionalLightF> vDirectionalLights;

  rPipeline vPipeline;
  //   rPipeline       vLightPipeline;
  SPIRV_triangle1 vShader;
  //   SPIRV_deferred2 vLightShader;

  std::string vShader_str;
  std::string vNormalShader_str;

  std::string vFilePath;

  std::thread vMovementThread;
  std::mutex  vObjAccesMut;

  _SLOT_ vKeySlot;
  float  vRotationAngle;
  bool   vRenderNormals;
  bool   vRunMovementThread = true;

  void objectMoveLoop();

 public:
  myScene() = delete;
  ~myScene();

  myScene(rWorld *_world, cmdANDinit &_cmd)
      : rScene("MAIN SCENE", _world),
        rCameraHandler(this, _world->getInitPtr()),
        vShader(_world->getDevice()),
        //         vLightShader(_world),
        vShader_str(_cmd.getShader()),
        vNormalShader_str(_cmd.getNormalShader()),
        vFilePath(_cmd.getMesh()),
        vKeySlot(&myScene::keySlot, this),
        vRotationAngle(0),
        vRenderNormals(_cmd.getRenderNormals()) {
    _world->getInitPtr()->addKeySlot(&vKeySlot);
  }

  int  init();
  void destroy();

  void keySlot(iEventInfo const &_inf);

  virtual void afterCameraUpdate();
};

#endif

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
