/*!
 * \file windows/iKeyboard.hpp
 * \brief \b Classes: \a eKeys
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

#ifndef KEYBOARD_WIN
#define KEYBOARD_WIN

#include "defines.hpp"

#include <windows.h>
#include "iKeyboardBasic.hpp"

namespace e_engine {

namespace windows_win32 {

class iKeyboard : public iKeyboardBasic {

 protected:
   /*!
       * \brief Convert a Windows keyevent to a key and set the state of it
       * \param _wparam    The key to process
       * \param _key_state The new state
       * \return The key in wchar
       */
   wchar_t processWindowsKeyInput( WPARAM _wparam, short unsigned int _key_state );

 public:
   iKeyboard() {}
   virtual ~iKeyboard() {}

   unsigned short int getKeyState( wchar_t _key );
};

} // windows_win32

} // e_engine

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
