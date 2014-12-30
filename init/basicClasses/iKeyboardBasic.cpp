/*!
 * \file iKeyboardBasic.hpp
 *
 * Basic class for setting keys
 */

#include "iKeyboardBasic.hpp"

namespace e_engine {

iKeyboardBasic::iKeyboardBasic()  {
   for (auto & elem : key_state)
      elem = E_RELEASED;
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
