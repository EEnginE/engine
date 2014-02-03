/*!
 * \file keyboard.hpp
 * \brief \b Classes: \a eKeys
 * \sa e_keycode_to_key.cpp
 */

#ifndef KEYBOARD_WIN
#define KEYBOARD_WIN

#include "keyboard_basic.hpp"

namespace e_engine {

class eKeyboard : public eKeyboardBasic {
   public:
      eKeyboard() {}
      virtual ~eKeyboard() {}

};

}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
