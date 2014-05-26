/*!
 * \file eKeyboardBasic.hpp
 *
 * Basic class for setting keys
 */

#ifndef KEYS_BASIC
#define KEYS_BASIC

#include "defines.hpp"

namespace e_engine {

class eKeyboardBasic {
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
         if ( _key < 0 ) {_state = E_KEY_UNKNOWN; return;}
         key_state[( unsigned int )_key] = _state;
      }

   public:
      eKeyboardBasic();
      virtual ~eKeyboardBasic() {}

      /*!
       * \brief Get the key state
       * \param _key The key
       * \returns The key state
       */
      unsigned short int getKeyState( wchar_t _key ) {
         if ( _key < 0 || _key > _E_KEY_LAST ) {return E_KEY_UNKNOWN;}
         return key_state[( unsigned int )_key];
      }
};

}




#endif // KEYS_BASIC
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
