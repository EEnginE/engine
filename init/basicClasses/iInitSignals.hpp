/*!
 * \file iInitSignals.hpp
 * \brief \b Classes: \a iInitSignals
 * \sa iInit
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

namespace e_engine {

class iInit;

class iInitSignals {
 public:
  typedef iEventInfo const &SIGNAL_TYPE;
  typedef uSignal<void, SIGNAL_TYPE> SIGNAL;

  template <class __C>
  using SLOT_C = uSlot<void, __C, SIGNAL_TYPE>;
  typedef SLOT_C<iInit> SLOT;

 protected:
  SIGNAL vWindowClose_SIG; //!< The signal for Window close
  SIGNAL vResize_SIG;      //!< The signal for Resize
  SIGNAL vKey_SIG;         //!< The signal for Key
  SIGNAL vMouse_SIG;       //!< The signal for Mouse
  SIGNAL vFocus_SIG;       //!< The signal for focus change

 public:
  iInitSignals() {}
  virtual ~iInitSignals();

  template <class __C>
  bool addWindowCloseSlot(SLOT_C<__C> *_slot) {
    return _slot->connect(&vWindowClose_SIG);
  }
  template <class __C>
  bool addResizeSlot(SLOT_C<__C> *_slot) {
    return _slot->connect(&vResize_SIG);
  }
  template <class __C>
  bool addKeySlot(SLOT_C<__C> *_slot) {
    return _slot->connect(&vKey_SIG);
  }
  template <class __C>
  bool addMouseSlot(SLOT_C<__C> *_slot) {
    return _slot->connect(&vMouse_SIG);
  }
  template <class __C>
  bool addFocusSlot(SLOT_C<__C> *_slot) {
    return _slot->connect(&vFocus_SIG);
  }

  void removeAllSlots();

  template <class __C>
  bool removeWindowCloseSlot(SLOT_C<__C> *_slot) {
    return vWindowClose_SIG.disconnect(_slot);
  }
  template <class __C>
  bool removeResizeSlot(SLOT_C<__C> *_slot) {
    return vResize_SIG.disconnect(_slot);
  }
  template <class __C>
  bool removeKeySlot(SLOT_C<__C> *_slot) {
    return vKey_SIG.disconnect(_slot);
  }
  template <class __C>
  bool removeMouseSlot(SLOT_C<__C> *_slot) {
    return vMouse_SIG.disconnect(_slot);
  }
  template <class __C>
  bool removeFocusSlot(SLOT_C<__C> *_slot) {
    return vFocus_SIG.disconnect(_slot);
  }
};

/*!
 * \fn void iInitSignals::addWindowCloseSlot
 * \brief Adds a slot for the \c WindowClose event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn void iInitSignals::addResizeSlot
 * \brief Adds a slot for the \c Resize event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn void iInitSignals::addKeySlot
 * \brief Adds a slot for the \c Key event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn void iInitSignals::addMouseSlot
 * \brief Adds a slot for the \c Mouse event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn void iInitSignals::addFocusSlot
 * \brief Adds a slot for the \c Focus event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */



/*!
 * \fn bool iInitSignals::removeWindowCloseSlot
 * \brief Removes _slot from the \c WindowClose event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool iInitSignals::removeResizeSlot
 * \brief Removes _slot from the \c Resize event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool iInitSignals::removeKeySlot
 * \brief Removes _slot from the \c Key event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool iInitSignals::removeMouseSlot
 * \brief Removes _slot from the \c Mouse event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn void iInitSignals::removeFocusSlot
 * \brief Removes _slot from the \c Focus event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
