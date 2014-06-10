/*!
 * \file windows/keyboard.hpp
 * \brief \b Classes: \a eKeys
 * \sa e_keycode_to_key.cpp
 */

#include "keyboard.hpp"

namespace e_engine {

namespace windows_win32 {

short unsigned int eKeyboard::getKeyState( wchar_t _key ) {
   if ( _key >= E_KEY_BACKSPACE && _key <= E_KEY_KP_NUM_LOCK )
      return getKeyStateArray( _key );

   // If the Key is pressed, GetKeyState will return -1
   return ( GetKeyState( VkKeyScan( _key ) ) >> 16 == -1 || GetKeyState( VkKeyScan( _key ) ) >> 16 == 1 ) ? E_PRESSED : E_RELEASED;
}


wchar_t eKeyboard::processWindowsKeyInput( WPARAM _wparam, short unsigned int _key_state ) {

   wchar_t _charToCheck = ( wchar_t ) _wparam; //Convert _wparam to a wide char

   switch ( _charToCheck ) {
         //Check for special keys, see http://msdn.microsoft.com/en-us/library/ms927178.aspx

      case VK_BACK:              setKeyState( E_KEY_BACKSPACE,   _key_state );  return E_KEY_BACKSPACE;
      case VK_TAB:               setKeyState( E_KEY_TAB,         _key_state );  return E_KEY_TAB;
      case VK_CLEAR:             setKeyState( E_KEY_CLEAR,       _key_state );  return E_KEY_CLEAR;
      case VK_RETURN:            setKeyState( E_KEY_RETURN,      _key_state );  return E_KEY_RETURN;
      case VK_PAUSE:             setKeyState( E_KEY_PAUSE,       _key_state );  return E_KEY_PAUSE;

         //Non existant under windows:
         //case XK_Sys_Req:           setKeyState( E_KEY_SYS_REQ,     _key_state );  return E_KEY_SYS_REQ;
      case VK_SNAPSHOT:          setKeyState( E_KEY_SYS_REQ,     _key_state );  return E_KEY_SYS_REQ;

      case VK_ESCAPE:            setKeyState( E_KEY_ESCAPE,      _key_state );  return E_KEY_ESCAPE;
      case VK_DELETE:            setKeyState( E_KEY_DELETE,      _key_state );  return E_KEY_DELETE;

      case VK_SCROLL:            setKeyState( E_KEY_SCROLL_LOCK, _key_state );  return E_KEY_SCROLL_LOCK;
      case VK_NUMLOCK:           setKeyState( E_KEY_KP_NUM_LOCK, _key_state );  return E_KEY_KP_NUM_LOCK;
      case VK_CAPITAL:           setKeyState( E_KEY_CAPS_LOCK,   GetKeyState( VK_CAPITAL ) );  return E_KEY_CAPS_LOCK;   //Set the local capslock state to the global capslock state


      case VK_F1:                setKeyState( E_KEY_F1,          _key_state );  return E_KEY_F1;
      case VK_F2:                setKeyState( E_KEY_F2,          _key_state );  return E_KEY_F2;
      case VK_F3:                setKeyState( E_KEY_F3,          _key_state );  return E_KEY_F3;
      case VK_F4:                setKeyState( E_KEY_F4,          _key_state );  return E_KEY_F4;
      case VK_F5:                setKeyState( E_KEY_F5,          _key_state );  return E_KEY_F5;
      case VK_F6:                setKeyState( E_KEY_F6,          _key_state );  return E_KEY_F6;
      case VK_F7:                setKeyState( E_KEY_F7,          _key_state );  return E_KEY_F7;
      case VK_F8:                setKeyState( E_KEY_F8,          _key_state );  return E_KEY_F8;
      case VK_F9:                setKeyState( E_KEY_F9,          _key_state );  return E_KEY_F9;
      case VK_F10:               setKeyState( E_KEY_F10,         _key_state );  return E_KEY_F10;
      case VK_F11:               setKeyState( E_KEY_F11,         _key_state );  return E_KEY_F11;
      case VK_F12:               setKeyState( E_KEY_F12,         _key_state );  return E_KEY_F12;
      case VK_F13:               setKeyState( E_KEY_F13,         _key_state );  return E_KEY_F13;
      case VK_F14:               setKeyState( E_KEY_F14,         _key_state );  return E_KEY_F14;
      case VK_F15:               setKeyState( E_KEY_F15,         _key_state );  return E_KEY_F15;
      case VK_F16:               setKeyState( E_KEY_F16,         _key_state );  return E_KEY_F16;
      case VK_F17:               setKeyState( E_KEY_F17,         _key_state );  return E_KEY_F17;
      case VK_F18:               setKeyState( E_KEY_F18,         _key_state );  return E_KEY_F18;
      case VK_F19:               setKeyState( E_KEY_F19,         _key_state );  return E_KEY_F19;
      case VK_F20:               setKeyState( E_KEY_F20,         _key_state );  return E_KEY_F20;
      case VK_F21:               setKeyState( E_KEY_F21,         _key_state );  return E_KEY_F21;
      case VK_F22:               setKeyState( E_KEY_F22,         _key_state );  return E_KEY_F22;
      case VK_F23:               setKeyState( E_KEY_F23,         _key_state );  return E_KEY_F23;
      case VK_F24:               setKeyState( E_KEY_F24,         _key_state );  return E_KEY_F24;

         //Nonexistant under windows:
         //case VK_F25:               setKeyState( E_KEY_F25,         _key_state );  return E_KEY_F25;

      case VK_UP:                setKeyState( E_KEY_UP,          _key_state );  return E_KEY_UP;
      case VK_DOWN:              setKeyState( E_KEY_DOWN,        _key_state );  return E_KEY_DOWN;
      case VK_LEFT:              setKeyState( E_KEY_LEFT,        _key_state );  return E_KEY_LEFT;
      case VK_RIGHT:             setKeyState( E_KEY_RIGHT,       _key_state );  return E_KEY_RIGHT;

      case VK_SHIFT:
      case VK_LSHIFT:            setKeyState( E_KEY_L_SHIFT,     _key_state );  return E_KEY_L_SHIFT;
      case VK_RSHIFT:            setKeyState( E_KEY_R_SHIFT,     _key_state );  return E_KEY_R_SHIFT;

      case VK_CONTROL:
      case VK_LCONTROL:          setKeyState( E_KEY_L_CTRL,      _key_state );  return E_KEY_L_CTRL;
      case VK_RCONTROL:          setKeyState( E_KEY_R_CTRL,      _key_state );  return E_KEY_R_CTRL;

      case VK_MENU:
      case VK_LMENU:             setKeyState( E_KEY_L_ALT,       _key_state );  return E_KEY_L_ALT;
      case VK_RMENU:             setKeyState( E_KEY_R_ALT,       _key_state );  return E_KEY_R_ALT;

      case VK_APPS:              setKeyState( E_KEY_MENU,        _key_state );  return E_KEY_MENU;

      case VK_LWIN:              setKeyState( E_KEY_L_SUPER,     _key_state );  return E_KEY_L_SUPER;
      case VK_RWIN:              setKeyState( E_KEY_R_SUPER,     _key_state );  return E_KEY_R_SUPER;

      case VK_HOME:              setKeyState( E_KEY_HOME,        _key_state );  return E_KEY_HOME;
      case VK_INSERT:            setKeyState( E_KEY_INSERT,      _key_state );  return E_KEY_INSERT;
      case VK_PRIOR :            setKeyState( E_KEY_PAGE_UP,     _key_state );  return E_KEY_PAGE_UP;
      case VK_NEXT:              setKeyState( E_KEY_PAGE_DOWN,   _key_state );  return E_KEY_PAGE_DOWN;
      case VK_END:               setKeyState( E_KEY_END,         _key_state );  return E_KEY_END;

      case VK_DIVIDE:            setKeyState( E_KEY_KP_DIVIDE,   _key_state );  return E_KEY_KP_DIVIDE;
      case VK_MULTIPLY:          setKeyState( E_KEY_KP_MULTIPLY, _key_state );  return E_KEY_KP_MULTIPLY;
      case VK_SUBTRACT:          setKeyState( E_KEY_KP_SUBTRACT, _key_state );  return E_KEY_KP_SUBTRACT;
      case VK_ADD:               setKeyState( E_KEY_KP_ADD,      _key_state );  return E_KEY_KP_ADD;

         //Nonexistant under windows
         //case XK_KP_Equal:          setKeyState( E_KEY_KP_EQUAL,    _key_state );  return E_KEY_KP_EQUAL;

         //Already registered earlier, no difference between normal and Keypad
         //case XK_KP_Enter:          setKeyState( E_KEY_KP_ENTER,    _key_state );  return E_KEY_KP_ENTER;

      case VK_NUMPAD0:              setKeyState( E_KEY_KP_0,        _key_state );  return E_KEY_KP_0;
      case VK_NUMPAD1:              setKeyState( E_KEY_KP_1,        _key_state );  return E_KEY_KP_1;
      case VK_NUMPAD2:              setKeyState( E_KEY_KP_2,        _key_state );  return E_KEY_KP_2;
      case VK_NUMPAD3:              setKeyState( E_KEY_KP_3,        _key_state );  return E_KEY_KP_3;
      case VK_NUMPAD4:              setKeyState( E_KEY_KP_4,        _key_state );  return E_KEY_KP_4;
      case VK_NUMPAD5:              setKeyState( E_KEY_KP_5,        _key_state );  return E_KEY_KP_5;
      case VK_NUMPAD6:              setKeyState( E_KEY_KP_6,        _key_state );  return E_KEY_KP_6;
      case VK_NUMPAD7:              setKeyState( E_KEY_KP_7,        _key_state );  return E_KEY_KP_7;
      case VK_NUMPAD8:              setKeyState( E_KEY_KP_8,        _key_state );  return E_KEY_KP_8;
      case VK_NUMPAD9:              setKeyState( E_KEY_KP_9,        _key_state );  return E_KEY_KP_9;

      default:
         return 0; //Return 0 to skip sending a signal
   }



   return _charToCheck;
}


} // windows_win32

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
