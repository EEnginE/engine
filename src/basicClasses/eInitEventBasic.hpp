/*!
 * \file eInitEventBasic.hpp
 * \brief \b Classes: \a eInitEventBasic
 * \sa eInit
 */

#ifndef E_INIT_EVENT_BASIC_HPP
#define E_INIT_EVENT_BASIC_HPP

#include "signal_slot.hpp"
#include "eWindowData.hpp"

namespace e_engine {

class eInit;

class eInitEventBasic {
      typedef eSignal<void, eWinInfo>      _SIGNAL_;
      typedef eSlot<void, eInit, eWinInfo> _SLOT_;

   private:
      virtual void makeEInitEventBasicAbstract() = 0;

   protected:
      _SIGNAL_ vWindowClose_SIG;     //!< The signal for Window close
      _SIGNAL_ vResize_SIG;          //!< The signal for Resize
      _SIGNAL_ vKey_SIG;             //!< The signal for Key
      _SIGNAL_ vMouse_SIG;           //!< The signal for Mouse
      _SIGNAL_ vFocus_SIG;           //!< The signal for focus change

      _SLOT_   vWindowClose_SLOT;    //!< The standard slot for Window close
      _SLOT_   vResize_SLOT;         //!< The standard slot for Resize
      _SLOT_   vKey_SLOT;            //!< The standard slot for Key
      _SLOT_   vMouse_SLOT;          //!< The standard slot for Mouse
      _SLOT_   vFocus_SLOT;          //!< The standard slot for focus change

   public:

      virtual ~eInitEventBasic();

      template<class __C>
      void addWindowCloseSlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vWindowClose_SLOT.disconnectAll();
         _slot->connectWith( &vWindowClose_SIG );
      }

      template<class __C>
      void addResizeSlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vResize_SLOT.disconnectAll();
         _slot->connectWith( &vResize_SIG );
      }

      template<class __C>
      void addKeySlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vKey_SLOT.disconnectAll();
         _slot->connectWith( &vKey_SIG );
      }

      template<class __C>
      void addMouseSlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vMouse_SLOT.disconnectAll();
         _slot->connectWith( &vMouse_SIG );
      }
      
      template<class __C>
      void addFocusSlot( eSlot<void, __C, eWinInfo> *_slot ) {
         vFocus_SLOT.disconnectAll();
         _slot->connectWith( &vFocus_SIG );
      }

      void removeAllSlots();

      template<class __C> bool removeWindowCloseSlot( eSlot<void, __C, eWinInfo> *_slot ) {return vWindowClose_SIG.disconnect( _slot );}
      template<class __C> bool removeResizeSlot( eSlot<void, __C, eWinInfo> *_slot )      {return vResize_SIG.disconnect( _slot );}
      template<class __C> bool removeKeySlot( eSlot<void, __C, eWinInfo> *_slot )         {return vKey_SIG.disconnect( _slot );}
      template<class __C> bool removeMouseSlot( eSlot<void, __C, eWinInfo> *_slot )       {return vMouse_SIG.disconnect( _slot );}
      template<class __C> bool removeFocusSlot( eSlot<void, __C, eWinInfo> *_slot )       {return vFocus_SIG.disconnect( _slot );}
};

/*!
 * \fn void eInitEventBasic::makeEInitEventBasicAbstract
 * \brief PRIVATE constructor
 * 
 * This function only exists, so that you can never have an
 * instance of this class.
 * 
 * You will also never need an instance of this class
 */



/*!
 * \fn void eInitEventBasic::addWindowCloseSlot
 * \brief Adds a slot for the \c WindowClose event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */

/*!
 * \fn void eInitEventBasic::addResizeSlot
 * \brief Adds a slot for the \c Resize event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */

/*!
 * \fn void eInitEventBasic::addKeySlot
 * \brief Adds a slot for the \c Key event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */

/*!
 * \fn void eInitEventBasic::addMouseSlot
 * \brief Adds a slot for the \c Mouse event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */



/*!
 * \fn bool eInitEventBasic::removeWindowCloseSlot
 * \brief Removes _slot from the \c WindowClose event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool eInitEventBasic::removeResizeSlot
 * \brief Removes _slot from the \c Resize event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool eInitEventBasic::removeKeySlot
 * \brief Removes _slot from the \c Key event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool eInitEventBasic::removeMouseSlot
 * \brief Removes _slot from the \c Mouse event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */


}

#endif // E_INIT_EVENT_BASIC_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
