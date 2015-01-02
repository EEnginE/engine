/*!
 * \file iKeyboardBasic.hpp
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

#ifndef KEYS_BASIC
#define KEYS_BASIC

#include "defines.hpp"

namespace e_engine {

class iKeyboardBasic {
 private:
   unsigned short int key_state[_E_KEY_LAST + 1];

 protected:
   /*!
    * \brief Set a key to a specific state
    * \param _key   The key ID
    * \param _state The new key state
    * \returns Nothing
    */
   void setKeyState( wchar_t _key, unsigned short int _state ) {
      if ( _key < 0 )
         return;

      key_state[(unsigned int)_key] = _state;
   }

   /*!
    * \brief Get the key state
    * \param _key The key
    * \returns The key state
    */
   unsigned short int getKeyStateArray( wchar_t _key ) {
      if ( _key < 0 || _key > _E_KEY_LAST ) {
         return E_UNKNOWN;
      }
      return key_state[(unsigned int)_key];
   }

 public:
   iKeyboardBasic();
   virtual ~iKeyboardBasic() {}
};
}




#endif // KEYS_BASIC
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
