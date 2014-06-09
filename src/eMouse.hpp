/*!
 * \file eMouse.hpp
 *
 * Basic class for setting keys
 */

#ifndef MOUSE
#define MOUSE

#include "defines.hpp"

namespace e_engine {

class eMouse {
   private:
      unsigned short int button_state[E_MOUSE_UNKNOWN];

   protected:

      /*!
       * \brief Set a mouse button to a specific state
       * \param _button   The mouse button ID
       * \param _state The new mouse button state
       * \returns Nothing
       */
      void setMousebuttonState( wchar_t _button, unsigned short int _state ) {
         if ( _button < 0 ) {_state = E_UNKNOWN; return;}
         button_state[( unsigned int )_button] = _state;
      }

   public:
      eMouse();
      virtual ~eMouse() {}

      /*!
       * \brief Get the mouse button's state
       * \param _button The mouse button to check
       * \returns The mouse button state
       */
      unsigned short int getMousebuttonState( wchar_t _button ) {
         if ( _button < 0 || _button > E_MOUSE_UNKNOWN ) {return E_UNKNOWN;}
         return button_state[( unsigned int )_button];
      }
};

}




#endif // MOUSE
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
