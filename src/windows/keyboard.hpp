/*!
 * \file windows/keyboard.hpp
 * \brief \b Classes: \a eKeys
 * \sa e_keycode_to_key.cpp
 */

#ifndef KEYBOARD_WIN
#define KEYBOARD_WIN

#include <windows.h>
#include "eKeyboardBasic.hpp"

namespace e_engine {

namespace windows_win32 {

class eKeyboard : public eKeyboardBasic {

   protected:
      /*!
          * \brief Convert a Windows keyevent to a key and set the state of it
          * \param _wparam    The key to process
          * \param _key_state The new state
          * \return The key in wchar
          */
      wchar_t processWindowsKeyInput( WPARAM _wparam, short unsigned int _key_state );

   public:
      eKeyboard() {}
      virtual ~eKeyboard() {}
      
      unsigned short int getKeyState(wchar_t _key);

      

};

} // windows_win32

} // e_engine

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
