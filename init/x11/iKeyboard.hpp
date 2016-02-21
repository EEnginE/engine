/*!
 * \file x11/iKeyboard.hpp
 * \brief \b Classes: \a iKeyboard
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

#ifndef E_KEYSYM_TO_UNICODE_HPP
#define E_KEYSYM_TO_UNICODE_HPP

#include <X11/XKBlib.h>
#include "defines.hpp"
#include "iKeyboardBasic.hpp"


namespace e_engine {

namespace unix_x11 {

/*!
 * \brief Stores information about the state of the keys
 */
class INIT_API iKeyboard : public iKeyboardBasic {
 private:
   wchar_t keysym2unicode( KeySym keysym );

 protected:
   /*!
    * \brief Convert a X11 keyevent to a key and set the state of it
    * \param _kEv       The key event
    * \param _key_state The new state
    * \param _display   The connection to the X-Server
    * \return The key in wchar
    */
   wchar_t processX11KeyInput( XKeyPressedEvent _kEv,
                               short unsigned int _key_state,
                               Display *_display );


 public:
   iKeyboard() {}
   virtual ~iKeyboard();

   inline unsigned short int getKeyState( wchar_t _key ) { return getKeyStateArray( _key ); }
};

} // unix_x11

} // e_engine

#endif // E_KEYSYM_TO_UNICODE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
