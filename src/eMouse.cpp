/*!
 * \file eMouse.cpp
 *
 * Basic class for setting keys
 */

#include "eMouse.hpp"

namespace e_engine {

eMouse::eMouse()  {
   for ( unsigned short int i = 0; i < E_MOUSE_UNKNOWN; i++ )
      button_state[i] = E_RELEASED;
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
