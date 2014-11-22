/*!
 * \file windows/iKeyboard.hpp
 * \brief \b Classes: \a eKeys
 */

#ifndef KEYBOARD_WIN
#define KEYBOARD_WIN

#include <windows.h>
#include "iKeyboardBasic.hpp"
#include "uLog.hpp"

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

      unsigned short int getKeyState(wchar_t _key);



};

} // windows_win32

} // e_engine

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
