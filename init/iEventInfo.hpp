/*!
 * \file iEventInfo.hpp
 * \brief \b Classes: \a iEventInfo
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

namespace e_engine {

enum class EventType { UNKNOWN, KEY, FOCUS, MOUSE, WINDOWCLOSE, RESIZE };

class iInit;

/*!
 * \struct e_engine::iEventInfo
 * \brief Structure for evnet informations
 *
 *
 * \sa iInit uSignal uSlot
 */
struct iEventInfo {
  EventType type         = EventType::UNKNOWN;
  iInit *   iInitPointer = nullptr;

  /*!
   * \struct e_engine::iEventInfo::_eRsize
   * \brief The resize part
   */
  struct _eRsize {
    int          posX   = 0;
    int          posY   = 0;
    unsigned int height = 0;
    unsigned int width  = 0;
  } eResize;

  /*!
   * \struct e_engine::iEventInfo::_eKey
   * \brief The key part
   */
  struct _eKey {
    wchar_t  key   = 0;
    uint16_t state = 0;
  } eKey;

  /*!
   * \struct e_engine::iEventInfo::_iMouse
   * \brief The mouse part
   */
  struct _iMouse {
    unsigned int posX   = 0;
    unsigned int posY   = 0;
    uint16_t     state  = 0;
    E_BUTTON     button = E_MOUSE_UNKNOWN;
  } iMouse;

  struct _eFocus {
    bool hasFocus;
  } eFocus;

  void reset() {
    type            = EventType::UNKNOWN;
    eResize.posX    = 0;
    eResize.posY    = 0;
    eResize.height  = 0;
    eResize.width   = 0;
    eKey.state      = 0;
    eKey.key        = 0;
    iMouse.posX     = 0;
    iMouse.posY     = 0;
    iMouse.state    = 0;
    eFocus.hasFocus = false;
  }

  iEventInfo() {}
  iEventInfo(iInit *_ptr) {
    iInitPointer = _ptr;
    reset();
  }
};
} // namespace e_engine


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
