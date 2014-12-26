/*!
 * \file x11/iKeyboard.hpp
 * \brief \b Classes: \a iKeyboard
 */
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef E_KEYSYM_TO_UNICODE_HPP
#define E_KEYSYM_TO_UNICODE_HPP

#include "defines.hpp"

#include "iKeyboardBasic.hpp"
#include <X11/XKBlib.h>


namespace e_engine {

namespace unix_x11 {

/*!
 * \brief Stores information about the state of the keys
 */
class iKeyboard : public iKeyboardBasic {
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
      wchar_t processX11KeyInput( XKeyPressedEvent _kEv, short unsigned int _key_state, Display *_display );


   public:
      iKeyboard() {}
      virtual ~iKeyboard() {}

      inline unsigned short int getKeyState( wchar_t _key ) { return getKeyStateArray( _key ); }
};

} // unix_x11

} // e_engine

#endif // E_KEYSYM_TO_UNICODE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
