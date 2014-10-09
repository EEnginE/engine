/*!
 * \file iEventInfo.hpp
 * \brief \b Classes: \a iEventInfo
 */

#ifndef E_EVENT_INFO_HPP
#define E_EVENT_INFO_HPP

#include "defines.hpp"
#include "engine_init_Export.hpp"

namespace e_engine {

enum EVENT_TYPE { E_EVENT_UNKNOWN, E_EVENT_KEY, E_EVENT_FOCUS, E_EVENT_MOUSE, E_EVENT_WINDOWCLOSE, E_EVENT_RESIZE };

class iInit;

/*!
 * \struct e_engine::iEventInfo
 * \brief Structure for evnet informations
 *
 *
 * \sa iInit uSignal uSlot
 */
struct iEventInfo {
   EVENT_TYPE     type;
   iInit *iInitPointer;

   /*!
    * \struct e_engine::iEventInfo::_eRsize
    * \brief The resize part
    */
   struct _eRsize {
      int          posX;
      int          posY;
      unsigned int height;
      unsigned int width;
   } eResize;

   /*!
    * \struct e_engine::iEventInfo::_eKey
    * \brief The key part
    */
   struct _eKey {
      wchar_t      key;
      unsigned int state;
   } eKey;

   /*!
    * \struct e_engine::iEventInfo::_iMouse
    * \brief The mouse part
    */
   struct _iMouse {
      int       posX;
      int       posY;
      int       state;
      E_BUTTON button;
   } iMouse;

   struct _eFocus {
      bool hasFocus;
   } eFocus ;

   void reset() {
      type            = E_EVENT_UNKNOWN;
      eResize.posX    = 0;
      eResize.posY    = 0;
      eResize.height  = 0;
      eResize.width   = 0;
      eKey.state      = 0;
      eKey.key        = 0;
      iMouse.posX     = 0;
      iMouse.posY     = 0;
      iMouse.state    = 0;
      eFocus.hasFocus = false;
   }

   iEventInfo() {
      iInitPointer = NULL;
      reset();
   }
   iEventInfo( iInit *_ptr ) {
      iInitPointer = _ptr;
      reset();
   }
};


}

#endif // E_EVENT_INFO_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
