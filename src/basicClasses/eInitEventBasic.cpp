/*!
 * \file eInitEventBasic.cpp
 * \brief \b Classes: \a eInitEventBasic
 * \sa eInit
 */

#include "eInitEventBasic.hpp"

namespace e_engine {

/*!
 * \brief Removes \c ALL slots from the \c ALL event
 * 
 * \returns Nothing
 */
void eInitEventBasic::removeAllSlots() {
   vWindowClose_SIG.disconnectAll();
   vResize_SIG.disconnectAll();
   vKey_SIG.disconnectAll();
   vMouse_SIG.disconnectAll();
}

eInitEventBasic::~eInitEventBasic() {
   removeAllSlots();
}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
