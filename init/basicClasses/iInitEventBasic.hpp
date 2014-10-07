/*!
 * \file iInitEventBasic.hpp
 * \brief \b Classes: \a iInitEventBasic
 * \sa iInit
 */

#ifndef E_INIT_EVENT_BASIC_HPP
#define E_INIT_EVENT_BASIC_HPP

#include "uSignalSlot.hpp"
#include "iEventInfo.hpp"

namespace e_engine {

class iInit;

class iInitEventBasic {
      typedef uSignal<void, iEventInfo>      _SIGNAL_;
      typedef uSlot<void, iInit, iEventInfo> _SLOT_;

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

      _SLOT_   vGrabControl_SLOT;   //!< Slot for grab control \sa iInit::s_advancedGrabControl

   public:

      virtual ~iInitEventBasic();

      template<class __C>
      void addWindowCloseSlot( uSlot<void, __C, iEventInfo> *_slot ) {
         vWindowClose_SLOT.disconnectAll();
         _slot->connectWith( &vWindowClose_SIG );
      }

      template<class __C>
      void addResizeSlot( uSlot<void, __C, iEventInfo> *_slot ) {
         vResize_SLOT.disconnectAll();
         _slot->connectWith( &vResize_SIG );
      }

      template<class __C>
      void addKeySlot( uSlot<void, __C, iEventInfo> *_slot ) {
         vKey_SLOT.disconnectAll();
         _slot->connectWith( &vKey_SIG );
      }

      template<class __C>
      void addMousuSlot( uSlot<void, __C, iEventInfo> *_slot ) {
         vMouse_SLOT.disconnectAll();
         _slot->connectWith( &vMouse_SIG );
      }

      template<class __C>
      void addFocusSlot( uSlot<void, __C, iEventInfo> *_slot ) {
         vFocus_SLOT.disconnectAll();
         _slot->connectWith( &vFocus_SIG );
      }

      void removeAllSlots();

      _SLOT_ *getAdvancedGrabControlSlot() {return &vGrabControl_SLOT;}

      template<class __C> bool removeWindowCloseSlot( uSlot<void, __C, iEventInfo> *_slot ) {return vWindowClose_SIG.disconnect( _slot );}
      template<class __C> bool removeResizeSlot( uSlot<void, __C, iEventInfo> *_slot )      {return vResize_SIG.disconnect( _slot );}
      template<class __C> bool removeKeySlot( uSlot<void, __C, iEventInfo> *_slot )         {return vKey_SIG.disconnect( _slot );}
      template<class __C> bool removeMousuSlot( uSlot<void, __C, iEventInfo> *_slot )       {return vMouse_SIG.disconnect( _slot );}
      template<class __C> bool removeFocusSlot( uSlot<void, __C, iEventInfo> *_slot )       {return vFocus_SIG.disconnect( _slot );}
};

/*!
 * \fn void iInitEventBasic::makeEInitEventBasicAbstract
 * \brief PRIVATE constructor
 *
 * This function only exists, so that you can never have an
 * instance of this class.
 *
 * You will also never need an instance of this class
 */



/*!
 * \fn void iInitEventBasic::addWindowClosuSlot
 * \brief Adds a slot for the \c WindowClose event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */

/*!
 * \fn void iInitEventBasic::addResizuSlot
 * \brief Adds a slot for the \c Resize event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */

/*!
 * \fn void iInitEventBasic::addKeySlot
 * \brief Adds a slot for the \c Key event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */

/*!
 * \fn void iInitEventBasic::addMousuSlot
 * \brief Adds a slot for the \c Mouse event
 *
 * \param[in] _slot The Slot for the event
 * \returns Nothing
 */


/*!
 * \fn void iInitEventBasic::getAdvancedGrabControlSlot
 * \brief Returns the slot for the advanced grabb controll
 *
 * \returns The adress of the slot
 *
 * \sa iInit::s_advancedGrabControl
 */



/*!
 * \fn bool iInitEventBasic::removeWindowClosuSlot
 * \brief Removes _slot from the \c WindowClose event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool iInitEventBasic::removeResizuSlot
 * \brief Removes _slot from the \c Resize event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool iInitEventBasic::removeKeySlot
 * \brief Removes _slot from the \c Key event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */

/*!
 * \fn bool iInitEventBasic::removeMousuSlot
 * \brief Removes _slot from the \c Mouse event
 *
 * \param[in] _slot The Slot for the event
 * \returns true  when successfull
 * \returns false when not
 */


}

#endif // E_INIT_EVENT_BASIC_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
