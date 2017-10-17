/*!
 * \file x11/iWindow.hpp
 * \brief \b Classes: \a iWindow
 *
 * This file contains the class \b iWindow which creates
 * the window and the Vulkan surface.
 *
 * \sa e_context.cpp e_iInit.cpp
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
#include "iWindowBasic.hpp"
#include <vulkan.h>

namespace e_engine {

namespace unix_x11 {

namespace internal {
class iXCBAtom {
 private:
  xcb_intern_atom_reply_t *vAtomReply_XCB = nullptr;

 public:
  iXCBAtom() = default;
  iXCBAtom(xcb_connection_t *_connection, std::string _name);
  ~iXCBAtom();

  iXCBAtom(iXCBAtom &)  = delete;
  iXCBAtom(iXCBAtom &&) = default;

  bool genAtom(xcb_connection_t *_connection, std::string _name);

  xcb_atom_t  getAtom() const { return vAtomReply_XCB->atom; }
  xcb_atom_t *getAtomRef() { return &vAtomReply_XCB->atom; }
};
} // namespace internal

/*!
 * \class e_engine::unix_x11::iWindow
 * \brief The X11 window creation class
 *
 * This class creates the connection to the X-Server and
 * opens a window via xcb
 */
class iWindow : public iWindowBasic {
  using iXCBAtom = internal::iXCBAtom;

 private:
  xcb_connection_t * vConnection_XCB = nullptr;
  xcb_window_t       vRootWindow_XCB;
  xcb_window_t       vWindow_XCB;
  const xcb_setup_t *vSetup_XCB  = nullptr;
  xcb_screen_t *     vScreen_XCB = nullptr;

  iRandRBasic *vRandR = nullptr;

  iXCBAtom vWmProtocol_ATOM;
  iXCBAtom vWmDeleteWindow_ATOM;
  iXCBAtom vNetWmState_ATOM;
  iXCBAtom vNetWmWindowType_ATOM;
  iXCBAtom vNetWmName_ATOM;
  iXCBAtom vNetWmIconName_ATOM;
  iXCBAtom vWmName_ATOM;
  iXCBAtom vWmIconName_ATOM;
  iXCBAtom vMotifWmHints_ATOM;


  bool vWindowHasBorder_B = true;

  bool vXCBConnectionHasError_B = false;
  bool vIsRandrSupported_B      = false;

  bool vIsMouseGrabbed_B = false;
  bool vIsCursorHidden_B = false;

  //! \todo create a function for setting an icon

  // bool createIconPixmap();  //! Does NOT work until now
  // void pixmaps2(unsigned int Width, unsigned int Height, const Uint8* Pixels);

  xcb_intern_atom_reply_t *getAtom(std::string _name);

  void setWmProperty(iXCBAtom &_property, xcb_atom_t _type, uint8_t _format, uint32_t _length, const void *_data);
  void setWmPropertyAtom(iXCBAtom &_property, iXCBAtom &_data);
  void setWmPropertyString(iXCBAtom &_property, std::string _data);

  void sendX11Event(
      iXCBAtom &_atom, uint32_t _l0 = 0, uint32_t _l1 = 0, uint32_t _l2 = 0, uint32_t _l3 = 0, uint32_t _l4 = 0);

  static wchar_t keysym2unicode(xcb_keysym_t keysym) noexcept;
  wchar_t        processX11KeyInput(xcb_keycode_t _kEv, uint16_t _modMask) noexcept;

  struct Config {
    int64_t eventTimeoutSeconds     = 0;
    int64_t eventTimeoutNanoSeconds = 100000000;
  } cfg;

 protected:
  void eventLoop() override;

 public:
  iWindow(iInit *_init);
  ~iWindow() override;

  ErrorCode createWindow() override;

  void getXCBVersion(int *_major, int *_minor);

  void destroyWindow() override;

  void changeWindowConfig(unsigned int _width, unsigned int _height, int _posX, int _posY) override;

  void setWindowType(WINDOW_TYPE _type) override;
  void setWindowNames(std::string _windowName, std::string _iconName = "<NONE>") override;
  void setAttribute(ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE) override;

  void fullScreen(ACTION _action, bool _allMonitors = false) override;
  void fullScreenMultiMonitor();
  void setFullScreenMonitor(iDisplayBasic *_disp) override;
  void maximize(ACTION _action) override;
  void setDecoration(ACTION _action) override;

  bool grabMouse() override;
  void freeMouse() override;
  bool getIsMouseGrabbed() const override;

  void moveMouse(unsigned int _posX, unsigned int _posY) override;

  void hideMouseCursor() override;
  void showMouseCursor() override;
  bool getIsCursorHidden() const override;

  iRandRBasic *getRandRManager() override;

  VkSurfaceKHR getVulkanSurface(VkInstance _instance) override;
};


} // namespace unix_x11

} // namespace e_engine


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
