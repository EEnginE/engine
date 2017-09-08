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

#include "cmdANDinit.hpp"
#include "myScene.hpp"
#include <engine.hpp>

#ifndef HANDLER_HPP
#define HANDLER_HPP

using e_engine::rWorld;
using e_engine::rRendererDeferred;
using e_engine::rRendererBasic;
using e_engine::internal::rRendererBase;
using e_engine::rFrameCounter;
using e_engine::iDisplayBasic;
using e_engine::GlobConf;

class myWorld final : public rWorld, public rFrameCounter {
  typedef uSlot<void, myWorld, e_engine::iEventInfo const &> _SLOT_;

 private:
  float vAlpha;

  std::vector<std::shared_ptr<iDisplayBasic>> vDisp_RandR;
  std::shared_ptr<rRendererBase>              vRenderer;

  myScene          vScene;
  e_engine::iInit *vInitPointer;

  float vNearZ;
  float vFarZ;

  _SLOT_ slotWindowClose;
  _SLOT_ slotResize;
  _SLOT_ slotKey;

 public:
  myWorld(cmdANDinit &_cmd, e_engine::iInit *_init)
      : rWorld(_init),
        rFrameCounter(this, true),
        vRenderer(std::make_shared<rRendererBasic>(getInitPtr(), this, L"R1")),
        vScene(this, _cmd),
        vInitPointer(_init),
        vNearZ(_cmd.getNearZ()),
        vFarZ(_cmd.getFarZ()),
        slotWindowClose(&myWorld::windowClose, this),
        slotResize(&myWorld::resize, this),
        slotKey(&myWorld::key, this) {

    _init->addWindowCloseSlot(&slotWindowClose);
    _init->addResizeSlot(&slotResize);
    _init->addKeySlot(&slotKey);

    vAlpha = 1;
  }

  ~myWorld();
  myWorld() = delete;


  void windowClose(e_engine::iEventInfo const &info) {
    iLOG("User closed window");
    getRenderLoop()->stop();
    info.iInitPointer->shutdown();
  }
  void key(e_engine::iEventInfo const &info);
  void resize(e_engine::iEventInfo const &info) {
    iLOG("Window resized: W = ", info.eResize.width, ";  H = ", info.eResize.height);
    updateViewPort(0, 0, static_cast<int>(GlobConf.win.width), static_cast<int>(GlobConf.win.height));
    vScene.calculateProjectionPerspective(GlobConf.win.width, GlobConf.win.height, vNearZ, vFarZ, glm::radians(60.0f));
  }

  int initGL();
};


#endif // HANDLER_HPP
// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
