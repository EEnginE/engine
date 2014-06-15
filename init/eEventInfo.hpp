/*!
 * \file eEventInfo.hpp
 * \brief \b Classes: \a eEventInfo
 */

#ifndef E_EVENT_INFO_HPP
#define E_EVENT_INFO_HPP

#include "defines.hpp"

namespace e_engine {

enum EVENT_TYPE { E_EVENT_UNKNOWN, E_EVENT_KEY, E_EVENT_FOCUS, E_EVENT_MOUSE, E_EVENT_WINDOWCLOSE, E_EVENT_RESIZE };

class eInit;

/*!
 * \struct e_engine::eEventInfo
 * \brief Structure for evnet informations
 *
 *
 * \sa eInit uSignal uSlot
 */
struct eEventInfo {
   EVENT_TYPE     type;
   eInit *eInitPointer;

   /*!
    * \struct e_engine::eEventInfo::_eRsize
    * \brief The resize part
    */
   struct _eRsize {
      int          posX;
      int          posY;
      unsigned int height;
      unsigned int width;
   } eResize;

   /*!
    * \struct e_engine::eEventInfo::_eKey
    * \brief The key part
    */
   struct _eKey {
      wchar_t      key;
      unsigned int state;
   } eKey;

   /*!
    * \struct e_engine::eEventInfo::_eMouse
    * \brief The mouse part
    */
   struct _eMouse {
      int       posX;
      int       posY;
      int       state;
      E_BUTTON button;
   } eMouse;

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
      eMouse.posX     = 0;
      eMouse.posY     = 0;
      eMouse.state    = 0;
      eFocus.hasFocus = false;
   }

   eEventInfo() {
      eInitPointer = NULL;
      reset();
   }
   eEventInfo( eInit *_ptr ) {
      eInitPointer = _ptr;
      reset();
   }
};


}

#endif // E_EVENT_INFO_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
