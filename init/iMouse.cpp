/*!
 * \file iMouse.cpp
 *
 * Basic class for setting keys
 */

#include "iMouse.hpp"

namespace e_engine {

iMouse::iMouse() {
   for ( auto &elem : button_state )
      elem = E_RELEASED;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
