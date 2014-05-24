/*!
 * \file windows/keyboard.hpp
 * \brief \b Classes: \a eKeys
 * \sa e_keycode_to_key.cpp
 */

#ifndef KEYBOARD_WIN
#define KEYBOARD_WIN

#include "eKeyboardBasic.hpp"

namespace e_engine {

namespace windows_win32 {

class eKeyboard : public eKeyboardBasic {
   public:
      eKeyboard() {}
      virtual ~eKeyboard() {}

};

} // windows_win32

} // e_engine

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
