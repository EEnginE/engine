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

#include <vulkan.h>

#include "iKeyboard.hpp"
#include "iRandR.hpp"
#include "iWindowBasic.hpp"


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
}

/*!
 * \class e_engine::unix_x11::iWindow
 * \brief The X11 window creation class
 *
 * This class creates the connection to the X-Server and
 * opens a window via xcb
 */
class iWindow : public iRandR, public iKeyboard, public iWindowBasic {
  using iXCBAtom = internal::iXCBAtom;

 private:
  xcb_connection_t * vConnection_XCB;
  xcb_window_t       vWindow_XCB;
  const xcb_setup_t *vSetup_XCB;
  xcb_screen_t *     vScreen_XCB;

  iXCBAtom vWmProtocol_ATOM;
  iXCBAtom vWmDeleteWindow_ATOM;
  iXCBAtom vNetWmState_ATOM;
  iXCBAtom vNetWmWindowType_ATOM;
  iXCBAtom vNetWmName_ATOM;
  iXCBAtom vNetWmIconName_ATOM;
  iXCBAtom vWmName_ATOM;
  iXCBAtom vWmIconName_ATOM;
  iXCBAtom vMotifWmHints_ATOM;


  bool vWindowHasBorder_B;

  bool vWindowCreated_B;

  bool vIsMouseGrabbed_B;
  bool vIsCursorHidden_B;

  //! \todo create a function for setting an icon

  // bool createIconPixmap();  //! Does NOT work until now
  // void pixmaps2(unsigned int Width, unsigned int Height, const Uint8* Pixels);

  xcb_intern_atom_reply_t *getAtom(std::string _name);

  void setWmProperty(iXCBAtom &_property, xcb_atom_t _type, uint8_t _format, uint32_t _length, const void *_data);
  void setWmPropertyAtom(iXCBAtom &_property, iXCBAtom &_data);
  void setWmPropertyString(iXCBAtom &_property, std::string _data);


  void sendX11Event(
      iXCBAtom &_atom, uint32_t _l0 = 0, uint32_t _l1 = 0, uint32_t _l2 = 0, uint32_t _l3 = 0, uint32_t _l4 = 0);

 protected:
  bool vWindowRecreate_B;

 public:
  iWindow();
  virtual ~iWindow();

  virtual int createWindow();

  void getXCBVersion(int *_major, int *_minor);
  xcb_connection_t *getXCBConnection();
  xcb_atom_t        getWmProtocolAtom() const;
  xcb_atom_t        getWmDeleteWindowAtom() const;

  virtual void destroyWindow();

  virtual void changeWindowConfig(unsigned int _width, unsigned int _height, int _posX, int _posY);

  virtual void setWindowType(WINDOW_TYPE _type);
  virtual void setWindowNames(std::string _windowName, std::string _iconName = "<NONE>");
  virtual void setAttribute(ACTION _action, WINDOW_ATTRIBUTE _type1, WINDOW_ATTRIBUTE _type2 = NONE);

  virtual void fullScreen(ACTION _action, bool _allMonitors = false);
  void fullScreenMultiMonitor();
  void setFullScreenMonitor(iDisplays &_disp);
  virtual void maximize(ACTION _action);
  virtual void setDecoration(ACTION _action);

  virtual bool grabMouse();
  virtual void freeMouse();
  virtual bool getIsMouseGrabbed() const;

  virtual void moveMouse(unsigned int _posX, unsigned int _posY);

  virtual void hideMouseCursor();
  virtual void showMouseCursor();
  virtual bool getIsCursorHidden() const;
  virtual bool getIsWindowCreated() const;

  virtual VkSurfaceKHR getVulkanSurface(VkInstance _instance);
};


} // unix_x11

} // e_engine


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
