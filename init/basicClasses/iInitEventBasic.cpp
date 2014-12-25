/*!
 * \file iInitEventBasic.cpp
 * \brief \b Classes: \a iInitEventBasic
 * \sa iInit
 */

#include "iInitEventBasic.hpp"
#include "iInit.hpp"

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

iInitEventBasic::iInitEventBasic( iInit *_init ) :
   vWindowClose_SLOT( &iInit::s_standardWindowClose, _init ),
   vResize_SLOT( &iInit::s_standardResize, _init ),
   vKey_SLOT( &iInit::s_standardKey, _init ),
   vMouse_SLOT( &iInit::s_standardMouse, _init ),
   vFocus_SLOT( &iInit::s_standardFocus, _init ),

   vGrabControl_SLOT( &iInit::s_advancedGrabControl, _init ) {}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
