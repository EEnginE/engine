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

iWindowBasic::iWindowBasic(iInit *_init) : vParent(_init) {}
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
    case E_EVENT_RESIZE: vSignals.resize->send(ev); break;
    case E_EVENT_KEY: vSignals.key->send(ev); break;
    case E_EVENT_MOUSE: vSignals.mouse->send(ev); break;
    case E_EVENT_FOCUS: vSignals.focus->send(ev); break;
    case E_EVENT_WINDOWCLOSE: vSignals.windowClose->send(ev); break;
    case E_EVENT_UNKNOWN: break;
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


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
