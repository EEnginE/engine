/*!
 * \file iMouse.hpp
 *
 * Basic class for setting keys
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

#ifndef MOUSE
#define MOUSE

#include "defines.hpp"

namespace e_engine {

class iMouse {
 private:
   unsigned short int button_state[E_MOUSE_UNKNOWN];

 protected:
   /*!
    * \brief Set a mouse button to a specific state
    * \param _button   The mouse button ID
    * \param _state The new mouse button state
    * \returns Nothing
    */
   void setMousebuttonState( E_BUTTON _button, unsigned short int _state ) {
      if ( _button < 0 )
         return;

      button_state[(unsigned int)_button] = _state;
   }

 public:
   iMouse();
   virtual ~iMouse() {}

   /*!
    * \brief Get the mouse button's state
    * \param _button The mouse button to check
    * \returns The mouse button state
    */
   unsigned short int getMousebuttonState( E_BUTTON _button ) {
      if ( _button < 0 || _button > E_MOUSE_UNKNOWN ) {
         return E_UNKNOWN;
      }
      return button_state[(unsigned int)_button];
   }
};
}




#endif // MOUSE
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
