/*!
 * \file iKeyboardBasic.hpp
 *
 * Basic class for setting keys
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

class iKeyboardBasic {
 private:
  unsigned short int key_state[_E_KEY_LAST + 1];

 protected:
  /*!
   * \brief Set a key to a specific state
   * \param _key   The key ID
   * \param _state The new key state
   */
  void setKeyState(wchar_t _key, unsigned short int _state) {
#if !WINDOWS
    if (_key < 0)
      return;
#endif

    key_state[static_cast<unsigned int>(_key)] = _state;
  }

  /*!
   * \brief Get the key state
   * \param _key The key
   * \returns The key state
   */
  unsigned short int getKeyStateArray(wchar_t _key) {
#if !WINDOWS
    if (_key < 0 || _key > _E_KEY_LAST) {
      return static_cast<unsigned short int>(E_UNKNOWN);
    }
#else
    if (_key > _E_KEY_LAST) {
      return static_cast<unsigned short int>(E_UNKNOWN);
    }
#endif
    return key_state[static_cast<unsigned int>(_key)];
  }

 public:
  iKeyboardBasic();
  virtual ~iKeyboardBasic();
};
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
