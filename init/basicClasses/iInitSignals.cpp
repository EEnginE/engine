/*!
 * \file iInitSignals.cpp
 * \brief \b Classes: \a iInitSignals
 * \sa iInit
 */

#include "iInitSignals.hpp"
#include "iInit.hpp"

namespace e_engine {

/*!
 * \brief Removes \c ALL slots from the \c ALL events
 *
 * \returns Nothing
 */
void iInitSignals::removeAllSlots() {
   vWindowClose_SIG.disconnectAll();
   vResize_SIG.disconnectAll();
   vKey_SIG.disconnectAll();
   vMouse_SIG.disconnectAll();
   vFocus_SIG.disconnectAll();
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
