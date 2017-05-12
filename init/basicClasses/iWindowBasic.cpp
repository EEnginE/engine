/*!
 * \file iWindowBasic.cpp
 *
 * Basic class for different wm.
 *
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

#include "iWindowBasic.hpp"
#include "uLog.hpp"

using namespace e_engine;

iWindowBasic::iWindowBasic(iInit *_init) : vParent(_init) {
  for (auto &elem : vButtonState)
    elem = E_RELEASED;

  for (auto &elem : vKeyState)
    elem = E_RELEASED;
}
iWindowBasic::~iWindowBasic() {
  if (vEventLoopThread.joinable())
    vEventLoopThread.join();
}

void iWindowBasic::evLoopWrapper() {
  {
    std::unique_lock<std::mutex> lLock(vEventThreadControlMutex);
    vIsLoopRunning = true;
    vLoopResponseCond.notify_all();
  }

  LOG.nameThread(L"EVENT");
  iLOG("Event loop started");

  while (vKeepLoopRunning)
    eventLoop();

  iLOG("Event Loop finished");

  std::unique_lock<std::mutex> lLock(vEventThreadControlMutex);
  vIsLoopRunning = false;
  vLoopResponseCond.notify_all();
}

void iWindowBasic::startEventLoop(iSignalReference _signals) {
  std::unique_lock<std::mutex> lLock(vEventThreadControlMutex);

  if (vIsLoopRunning)
    return;

  vKeepLoopRunning = true;
  vEventLoopThread = std::thread(&iWindowBasic::evLoopWrapper, this);

  vSignals = _signals;

  while (!vIsLoopRunning)
    vLoopResponseCond.wait(lLock);
}

void iWindowBasic::stopEventLoop() {
  std::unique_lock<std::mutex> lLock(vEventThreadControlMutex);

  if (!vIsLoopRunning)
    return;

  vKeepLoopRunning = false;

  while (vIsLoopRunning)
    vLoopResponseCond.wait(lLock);
}

void iWindowBasic::sendEvent(iEventInfo &ev) noexcept {
  if (!vWindowCreated)
    return;

  switch (ev.type) {
    case EventType::RESIZE: vSignals.resize->send(ev); break;
    case EventType::KEY:
      setMousebuttonState(ev.iMouse.button, ev.iMouse.state);
      vSignals.key->send(ev);
      break;
    case EventType::MOUSE:
      setKeyState(ev.eKey.key, ev.eKey.state);
      vSignals.mouse->send(ev);
      break;
    case EventType::FOCUS: vSignals.focus->send(ev); break;
    case EventType::WINDOWCLOSE: vSignals.windowClose->send(ev); break;
    case EventType::UNKNOWN: break;
  }
}

void iWindowBasic::waitForWindowToClose() {
  std::unique_lock<std::mutex> lLock(vWindowCloseMutex);

  while (vWindowCreated)
    vWindowCloseCond.wait(lLock);
}

bool iWindowBasic::isLoopRunning() { return vIsLoopRunning; }
bool iWindowBasic::getRunEventLoop() const noexcept { return vKeepLoopRunning; }
bool iWindowBasic::getIsWindowCreated() const noexcept { return vWindowCreated; }

void iWindowBasic::setWindowCreated(bool _isCreated) noexcept {
  std::unique_lock<std::mutex> lLock(vWindowCloseMutex);

  vWindowCreated = _isCreated;

  if (vWindowCreated == false)
    vWindowCloseCond.notify_all();
}


/*!
 * \brief Set a mouse button to a specific state
 * \param _button   The mouse button ID
 * \param _state The new mouse button state
 */
void iWindowBasic::setMousebuttonState(E_BUTTON _button, uint16_t _state) noexcept {
  if (_button < 0)
    return;

  vButtonState[static_cast<unsigned int>(_button)] = _state;
}

/*!
 * \brief Set a key to a specific state
 * \param _key   The key ID
 * \param _state The new key state
 */
void iWindowBasic::setKeyState(wchar_t _key, uint16_t _state) noexcept {
#if !WINDOWS
  if (_key < 0)
    return;
#endif

  vKeyState[static_cast<unsigned int>(_key)] = _state;
}

/*!
 * \brief Get the mouse button's state
 * \param _button The mouse button to check
 * \returns The mouse button state
 */
uint16_t iWindowBasic::getMousebuttonState(E_BUTTON _button) const noexcept {
  if (_button < 0 || _button > E_MOUSE_UNKNOWN) {
    return static_cast<uint16_t>(E_UNKNOWN);
  }
  return vButtonState[static_cast<unsigned int>(_button)];
}

/*!
 * \brief Get the key state
 * \param _key The key
 * \returns The key state
 */
uint16_t iWindowBasic::getKeyState(wchar_t _key) const noexcept {
#if !WINDOWS
  if (_key < 0 || _key > _E_KEY_LAST) {
    return static_cast<uint16_t>(E_UNKNOWN);
  }
#else
  if (_key > _E_KEY_LAST) {
    return static_cast<uint16_t>(E_UNKNOWN);
  }
#endif
  return vKeyState[static_cast<unsigned int>(_key)];
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
