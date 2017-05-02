/*!
 * \file rCameraHandler.hpp
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

#pragma once

#include "defines.hpp"

#include "uLog.hpp"
#include "uSignalSlot.hpp"
#include "iInit.hpp"
#include "rMatrixSceneBase.hpp"

namespace e_engine {

template <class T, glm::precision P = glm::precision::highp>
class rCameraHandler {
  typedef iInit::SLOT_C<rCameraHandler> SLOT;

 public:
  enum KEY_MOVEMENT { UP = 0, DOWN, LEFT, RIGHT, FORWARD, BACKWARD, __LAST__ };

 private:
  rMatrixSceneBase<T> *vScene;
  iInit *              vInit;

  glm::tvec3<T, P> vPosition;
  glm::tvec3<T, P> vDirection;
  glm::tvec3<T, P> vUp;

  bool vCameraMovementEnabled;

  SLOT vMouseSlot;
  SLOT vKeySlot;

  wchar_t keys[KEY_MOVEMENT::__LAST__];

  void updateDirectionAndUp();

  void mouse(iEventInfo const &_event);
  void key(iEventInfo const &_event);

  rCameraHandler() {}

 public:
  virtual ~rCameraHandler() {}
  rCameraHandler(rMatrixSceneBase<T, P> *_scene, iInit *_init)
      : vScene(_scene),
        vInit(_init),

        vPosition(static_cast<T>(0), static_cast<T>(0), static_cast<T>(0)),

        vCameraMovementEnabled(true),

        vMouseSlot(&rCameraHandler::mouse, this),
        vKeySlot(&rCameraHandler::key, this) {

    vInit->addMouseSlot(&vMouseSlot);
    vInit->addKeySlot(&vKeySlot);

    keys[UP]       = L'q';
    keys[DOWN]     = L'e';
    keys[LEFT]     = L'a';
    keys[RIGHT]    = L'd';
    keys[FORWARD]  = L'w';
    keys[BACKWARD] = L's';

    updateDirectionAndUp();

    iLOG("Camera movement enabled");
  }

  void setCameraKey(KEY_MOVEMENT _key, wchar_t _what);

  void enableCamera() {
    vCameraMovementEnabled = true;
    iLOG("Camera movement enabled");
  }
  void disableCamera() {
    vCameraMovementEnabled = false;
    iLOG("Camera movement disabled");
  }

  void updateCamera();

  bool getIsCameraEnabled() const { return vCameraMovementEnabled; }

  void printCameraPosition();

  virtual void afterCameraUpdate() = 0;
};

template <class T, glm::precision P>
void rCameraHandler<T, P>::setCameraKey(KEY_MOVEMENT _key, wchar_t _what) {
  if (_key >= __LAST__ || _key < 0) return;

  keys[_key] = _what;
}

template <class T, glm::precision P>
void rCameraHandler<T, P>::updateCamera() {
  if (vCameraMovementEnabled) vScene->setCamera(vPosition, vPosition + vDirection, vUp);

  afterCameraUpdate();
}


template <class T, glm::precision P>
void rCameraHandler<T, P>::key(iEventInfo const &_event) {
  if (!vCameraMovementEnabled || _event.eKey.state == E_RELEASED) return;

  T            lSpeed  = static_cast<T>(GlobConf.camera.movementSpeed);
  KEY_MOVEMENT _action = __LAST__;

  glm::tvec3<T, P> lTempVector;

  for (uint8_t i = 0; i < __LAST__; ++i) {
    if (_event.eKey.key == keys[i]) {
      _action = static_cast<KEY_MOVEMENT>(i);
      break;
    }
  }

  switch (_action) {
    case DOWN: lSpeed *= -1; FALLTHROUGH
    case UP:
      lTempVector = lSpeed * vUp;
      vPosition += lTempVector;
      break;

    case LEFT: lSpeed *= -1; FALLTHROUGH
    case RIGHT:
      lTempVector = glm::cross(vDirection, vUp);
      lTempVector = glm::normalize(lTempVector);
      lTempVector *= lSpeed;
      vPosition += lTempVector;
      break;

    case BACKWARD: lSpeed *= -1; FALLTHROUGH
    case FORWARD:
      lTempVector = lSpeed * vDirection;
      vPosition += lTempVector;
      break;

    case __LAST__: return;
  }

  updateCamera();
}


template <class T, glm::precision P>
void rCameraHandler<T, P>::updateDirectionAndUp() {
  vDirection.x = static_cast<T>(cos(GlobConf.camera.angleVertical) * sin(GlobConf.camera.angleHorizontal));
  vDirection.y = static_cast<T>(sin(GlobConf.camera.angleVertical));
  vDirection.z = static_cast<T>(cos(GlobConf.camera.angleVertical) * cos(GlobConf.camera.angleHorizontal));

  glm::tvec3<T, P> lTempRight;
  lTempRight.y = 0;

#ifdef M_PIl
  lTempRight.x = static_cast<T>(sin(GlobConf.camera.angleHorizontal - static_cast<T>(M_PIl / 2)));
  lTempRight.z = static_cast<T>(cos(GlobConf.camera.angleHorizontal - static_cast<T>(M_PIl / 2)));
#elif defined M_PI
  lTempRight.x = static_cast<T>(sin(GlobConf.camera.angleHorizontal - static_cast<T>(M_PI / 2)));
  lTempRight.z = static_cast<T>(cos(GlobConf.camera.angleHorizontal - static_cast<T>(M_PI / 2)));
#else
#error "M_PI is not defined!"
#endif

  vUp = glm::cross(lTempRight, vDirection);
}


template <class T, glm::precision P>
void rCameraHandler<T, P>::mouse(iEventInfo const &_event) {
  int lDifX = static_cast<int>((GlobConf.win.width / 2) - _event.iMouse.posX);
  int lDifY = static_cast<int>((GlobConf.win.height / 2) - _event.iMouse.posY);
  if ((lDifX == 0 && lDifY == 0) || !vCameraMovementEnabled) return;

  GlobConf.camera.angleHorizontal += GlobConf.camera.mouseSensitivity * lDifX;
  GlobConf.camera.angleVertical += GlobConf.camera.mouseSensitivity * lDifY;
  vInit->getWindow()->moveMouse(GlobConf.win.width / 2, GlobConf.win.height / 2);

  updateDirectionAndUp();

  updateCamera();
}

template <class T, glm::precision P>
void rCameraHandler<T, P>::printCameraPosition() {
  iLOG(L"Camera position:  X = ", vPosition.x, L"; Y = ", vPosition.y, "; Z = ", vPosition.z);
}
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
