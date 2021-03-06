/*!
 * \file myWorld.cpp
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

#include "myWorld.hpp"
#include "config.hpp"

#if UNIX_X11
#include "iWindow.hpp"
#endif

using namespace e_engine;


myWorld::~myWorld() { shutdown(); }


void myWorld::key(iEventInfo const &info) {
  if (vDisp_RandR.empty())
    vDisp_RandR = info.iInitPointer->getWindow()->getRandRManager()->getDisplayResolutions();

  if (info.eKey.state == E_PRESSED) {
    switch (info.eKey.key) {
      // Handle fullscreen
      case E_KEY_F8: info.iInitPointer->getWindow()->setAttribute(C_TOGGLE, MAXIMIZED_VERT, MAXIMIZED_HORZ); break;
#if UNIX_X11
      case E_KEY_F9: static_cast<unix_x11::iWindow *>(info.iInitPointer->getWindow())->fullScreenMultiMonitor(); break;
#endif
      case E_KEY_F11: info.iInitPointer->getWindow()->fullScreen(e_engine::C_TOGGLE); break;
      case E_KEY_F10:
        if (vDisp_RandR.size() > 0)
          info.iInitPointer->getWindow()->setFullScreenMonitor(vDisp_RandR[0].get());
        break;
      case E_KEY_F12:
        if (vDisp_RandR.size() > 1)
          info.iInitPointer->getWindow()->setFullScreenMonitor(vDisp_RandR[1].get());
        break;

      // Mouse control
      case L'g': info.iInitPointer->getWindow()->grabMouse(); break;
      case L'G': info.iInitPointer->getWindow()->freeMouse(); break;
      case L'c': info.iInitPointer->getWindow()->hideMouseCursor(); break;
      case L'C':
        info.iInitPointer->getWindow()->showMouseCursor();
        break;

      // Object control
      //          case E_KEY_UP:        vObjects.addPositionDelta( rVec3f( 0, 0, -0.1 ) );
      //          vObjects.updateFinalMatrix(); break;
      //          case E_KEY_DOWN:      vObjects.addPositionDelta( rVec3f( 0, 0, 0.1 ) );
      //          vObjects.updateFinalMatrix(); break;
      //          case E_KEY_LEFT:      vObjects.addPositionDelta( rVec3f( -0.01, 0, 0 ) );
      //          vObjects.updateFinalMatrix(); break;
      //          case E_KEY_RIGHT:     vObjects.addPositionDelta( rVec3f( 0.01, 0, 0 ) );
      //          vObjects.updateFinalMatrix(); break;
      //          case E_KEY_PAGE_UP:   vObjects.addPositionDelta( rVec3f( 0, 0.01, 0 ) );
      //          vObjects.updateFinalMatrix(); break;
      //          case E_KEY_PAGE_DOWN: vObjects.addPositionDelta( rVec3f( 0, -0.01, 0 ) );
      //          vObjects.updateFinalMatrix(); break;

      // Window Border
      case L'b':
      case L'B':
        info.iInitPointer->getWindow()->setDecoration(e_engine::C_TOGGLE);
        break;

      // Quit
      case L'Q':
      case L'q':
      case E_KEY_ESCAPE: disconnectSlotsAndContinue(); break;
    }
  }
}


int myWorld::initGL() {
  // vInitPointer->fullScreen( C_ADD );
  int lReturn = vScene.init();
  updateClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  vRenderer->renderScene(&vScene);
  addRenderer(vRenderer);
  init(); // Initializes everything
  getRenderLoop()->start();

  vInitPointer->getWindow()->moveMouse(GlobConf.win.width / 2, GlobConf.win.height / 2);
  // vInitPointer->hideMouseCursor();
  return lReturn;
}

void myWorld::disconnectSlotsAndContinue() {
  slotKey.disconnectAll();
  slotResize.disconnectAll();
  slotWindowClose.disconnectAll();
  vScene.disableCamera();

  std::unique_lock<std::mutex> lLock(vWaitMutex);
  vKeepWaiting = false;
  vWaitCond.notify_all();
}

void myWorld::waitForEnd() {
  std::unique_lock<std::mutex> lLock(vWaitMutex);

  while (vKeepWaiting)
    vWaitCond.wait(lLock);
}




// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
