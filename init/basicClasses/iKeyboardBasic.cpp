/*!
 * \file iKeyboardBasic.hpp
 *
 * Basic class for setting keys
 */

#include "iKeyboardBasic.hpp"

namespace e_engine {

iKeyboardBasic::iKeyboardBasic()  {
   for ( unsigned short int i = 0; i < ( _E_KEY_LAST + 1 ); i++ )
      key_state[i] = E_RELEASED;
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
