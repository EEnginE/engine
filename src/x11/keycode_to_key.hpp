/// \file keycode_to_key.hpp
/// \brief \b Classes: \a eKeys
/// \sa e_keycode_to_key.cpp
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

#include <platform.hpp>

namespace e_engine {

class eInit;

/*!
 * \class e_engine::eKeys
 * \brief Stores information about the state of the keys
 */
class eKeys {
   private:
      unsigned short int key_state[_E_KEY_LAST + 1];
      wchar_t keysym2unicode( KeySym keysym );

      /*!
       * \brief Set a key to a specific state
       * \param _key   The key ID
       * \param _state The new key state
       * \returns Nothing
       */
      inline void setKeyState( wchar_t _key, unsigned short int _state ) {
         if ( _key < 0 ) {_state = E_KEY_UNKNOWN; return;}
         key_state[( unsigned int )_key] = _state;
      }
      
      /*!
       * \brief Convert a X11 keyevent to a key and set the state of it
       * \param _kEv       The key event
       * \param _key_state The new state
       * \param _display   The connection to the X-Server
       * \return The key in wchar
       */
      wchar_t keycode_to_key( XKeyPressedEvent _kEv, short unsigned int _key_state, Display *_display );


   public:
      //! The constructor
      eKeys() {
         for ( unsigned short int i = 0; i < ( _E_KEY_LAST + 1 ); i++ ) {
            key_state[i] = E_KEY_RELEASED;
         }
      }

      /*!
       * \brief Get the key state
       * \param _key The key
       * \returns The key state
       */
      inline unsigned short int getKeyState( wchar_t _key ) {
         if ( _key < 0 || _key > _E_KEY_LAST ) {return E_KEY_UNKNOWN;}
         return key_state[( unsigned int )_key];
      }
      
      friend class eInit;
};

}

#endif // E_KEYSYM_TO_UNICODE_HPP
