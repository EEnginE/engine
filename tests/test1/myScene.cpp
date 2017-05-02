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

#include "myScene.hpp"

using namespace glm;
using namespace e_engine;

myScene::~myScene() {
  if (vMovementThread.joinable())
    vMovementThread.join();
}

int myScene::init() {
  // updateCamera();

  vPipeline.setDynamicViewports(1)->setDynamicScissors(1)->enableDepthTest()->enableCulling();
  vPipeline.setShader(&vShader);

  vLightPipeline.setDynamicViewports(1)->setDynamicScissors(1); //->enableDepthTest();
  vLightPipeline.setShader(&vLightShader);

  beginInitObject();

  auto lNames = loadFile(vFilePath);
  for (auto const &i : lNames) {
    if (i.type != MESH_3D)
      continue;

    vObjects.emplace_back(std::make_shared<rSimpleMesh>(this, i.name));

    initObject(vObjects.back(), i.index);
    vObjects.back()->setPosition(vec3(0, 0, -5));
  }

  endInitObject();

  vPointLights.emplace_back(std::make_shared<rPointLightF>(this, "L1"));
  //    vPointLights.emplace_back( std::make_shared<rPointLightF>( this, "L2" ) );
  //    vDirectionalLights.emplace_back(
  //          std::make_shared<rDirectionalLightF>( "L3", vec3( 0.5, -1, 0.5 ) ) );

  vPointLights[0]->setPosition(vec3(1, 1, -4));
  //    vPointLights[1]->setPosition( vec3( -1, -1, -4 ) );

  vPointLights[0]->setColor(vec3(1.0f, 0.2f, 0.2f), vec3(0.1f, 0.0f, 0.0f));
  //    vPointLights[1]->setColor( vec3( 0.2f, 0.2f, 1.0f ), vec3( 0.0, 0.0f, 0.1f ) );
  //    vPointLights[2]->setColor( vec3( 0.9f, 0.9f, 0.9f ), vec3( 0.05f, 0.05f, 0.05f ) );

  vPointLights[0]->setAttenuation(0.1f, 0.01f, 0.1f);
  //    vPointLights[1]->setAttenuation( 0.1f, 0.02f, 0.2f );


  for (auto &i : vObjects) {
    i->setPipeline(&vPipeline);
    addObject(i);
  }

  for (auto &i : vPointLights) {
    i->setPipeline(&vLightPipeline);
    addObject(i);
  }

  //    for ( auto &i : vDirectionalLights ) {
  //       i->setPipeline( &vLightPipeline );
  //       addObject( i );
  //    }

  if (!canRenderScene()) {
    eLOG("Cannot render scene!");
    return 2;
  }

  vMovementThread = std::thread(&myScene::objectMoveLoop, this);
  return 0;
}

void myScene::objectMoveLoop() {
  LOG.nameThread(L"move1");
  std::mutex                            lWaitMutex;
  std::chrono::system_clock::time_point lStart = std::chrono::system_clock::now();
  std::chrono::system_clock::time_point lNow;
  std::chrono::milliseconds             lDuration;

  vec3 lAxis(0.0, 1.0, 0.0);

  getWorldPtr()->waitForFrame(lWaitMutex);

  while (getWorldPtr()->isSetup()) {
    lNow      = std::chrono::system_clock::now();
    lDuration = std::chrono::duration_cast<std::chrono::milliseconds>(lNow - lStart);

    float lRotDeg = lDuration.count() / 50.0f;

    std::lock_guard<std::mutex> lLock(vObjAccesMut);
    for (auto &i : vObjects)
      i->setRotation(lAxis, lRotDeg);

    getWorldPtr()->waitForFrame(lWaitMutex);
  }
}

void myScene::keySlot(const iEventInfo &_inf) {
  if (_inf.eKey.state != E_PRESSED)
    return;

  for (auto &i : vObjects) {
    switch (_inf.eKey.key) {
      case L'z':
        vRotationAngle += 0.25;
        i->setRotation(vec3(0, 1, 0), vRotationAngle);
        break;
      case L't':
        vRotationAngle -= 0.25;
        i->setRotation(vec3(0, 1, 0), vRotationAngle);
        break;
    }
  }
}


void myScene::afterCameraUpdate() {
  for (auto &i : vObjects)
    i->updateFinalMatrix();

  //    vLight1.updateFinalMatrix();
  //    vLight2.updateFinalMatrix();
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
