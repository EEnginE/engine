/*!
 * \file iInitEventBasic.cpp
 * \brief \b Classes: \a iInitEventBasic
 * \sa iInit
 */

#include "iInitEventBasic.hpp"

namespace e_engine {

/*!
 * \brief Removes \c ALL slots from the \c ALL event
 * 
 * \returns Nothing
 */
void iInitEventBasic::removeAllSlots() {
   vWindowClose_SIG.disconnectAll();
   vResize_SIG.disconnectAll();
   vKey_SIG.disconnectAll();
   vMouse_SIG.disconnectAll();
}

iInitEventBasic::~iInitEventBasic() {
   removeAllSlots();
}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
