/*!
 * \file iWindowBasic.hpp
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

#pragma once

#include "defines.hpp"
#include "uSignalSlot.hpp"
#include "iEventInfo.hpp"
#include "iRandRBasic.hpp"
#include <condition_variable>
#include <string>
#include <thread>
#include <vulkan.h>


namespace e_engine {

class iInit;

/*!
 * \class e_engine::iWindowBasic
 * \brief The wm basic window class
 *
 * This class provides an interface to create a
 * connection to a wm and create a window.
 */
class iWindowBasic {
 public:
  enum ErrorCode {
    OK = 0,
    WINDOW_ALREADY_CREATED,

    // XCB specific
    XCB_BAD_CONNECTION = 100
  };

  typedef iEventInfo const &         SIGNAL_TYPE;
  typedef uSignal<void, SIGNAL_TYPE> SIGNAL;

  struct iSignalReference {
    SIGNAL *windowClose = nullptr; //!< The signal for Window close
    SIGNAL *resize      = nullptr; //!< The signal for Resize
    SIGNAL *key         = nullptr; //!< The signal for Key
    SIGNAL *mouse       = nullptr; //!< The signal for Mouse
    SIGNAL *focus       = nullptr; //!< The signal for focus change

    iSignalReference() = default;
  };

 private:
  bool                    vWindowCreated   = false;
  bool                    vKeepLoopRunning = false;
  bool                    vIsLoopRunning   = false;
  std::thread             vEventLoopThread;
  std::mutex              vEventThreadControlMutex;
  std::mutex              vWindowCloseMutex;
  std::condition_variable vLoopResponseCond;
  std::condition_variable vWindowCloseCond;

  iSignalReference vSignals;

  // Keyboard and mouse state tracking
  uint16_t vButtonState[E_MOUSE_UNKNOWN];
  uint16_t vKeyState[_E_KEY_LAST + 1];

  void evLoopWrapper();

 protected:
  virtual void eventLoop() = 0;
  bool         getRunEventLoop() const noexcept;
  void         sendEvent(iEventInfo &ev) noexcept;
  void         setWindowCreated(bool _isCreated) noexcept;

  void setMousebuttonState(E_BUTTON _button, uint16_t _state) noexcept;
  void setKeyState(wchar_t _key, unsigned short int _state) noexcept;

  iInit *vParent = nullptr;

 public:
  iWindowBasic() = delete;
  iWindowBasic(iInit *_init);
  virtual ~iWindowBasic();

  virtual ErrorCode createWindow()  = 0;
  virtual void      destroyWindow() = 0;

  virtual void changeWindowConfig(unsigned int _width, unsigned int _height, int _posX, int _posY) = 0;

  virtual void setWindowType(WINDOW_TYPE _type)                                                      = 0;
  virtual void setWindowNames(std::string _windowName, std::string _iconName = "<NONE>")             = 0;
  virtual void setAttribute(ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE) = 0;

  virtual void fullScreen(ACTION _action, bool _allMonitors = false) = 0;
  virtual void setFullScreenMonitor(iDisplayBasic *_disp)            = 0;
  virtual void maximize(ACTION _action)                              = 0;
  virtual void setDecoration(ACTION _action)                         = 0;

  virtual bool grabMouse()               = 0;
  virtual void freeMouse()               = 0;
  virtual bool getIsMouseGrabbed() const = 0;

  virtual void moveMouse(unsigned int _posX, unsigned int _posY) = 0;

  virtual void hideMouseCursor()         = 0;
  virtual void showMouseCursor()         = 0;
  virtual bool getIsCursorHidden() const = 0;
  bool         getIsWindowCreated() const noexcept;

  virtual iRandRBasic *getRandRManager() = 0;

  void startEventLoop(iSignalReference _signals);
  void stopEventLoop();
  bool isLoopRunning();

  uint16_t getMousebuttonState(E_BUTTON _button) const noexcept;
  uint16_t getKeyState(wchar_t _key) const noexcept;

  virtual VkSurfaceKHR getVulkanSurface(VkInstance _instance) = 0;
};

/*!
 * \fn void iWindowBasic::createWindow
 * \brief Creates a PLAIN window
 *
 * \note This function is intended to be called from iInit::init
 */

/*!
 * \fn void iWindowBasic::destroyWindow
 * \brief Closes a PLAIN window
 *
 * \note This function is intended to be called from iInit::init
 * \warning do NOT call this function if you created the window with iInit::init
 */

} // namespace e_engine


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
