/// \file signal_slot.hpp
/// \brief \b Classes: \a eSignal, \a eSlot
/*
 *  E Engine
 *  Copyright (C) 2013 Daniel Mensinger
 *
 *  This library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef E_SGNAL_SLOT_HPP
#define E_SGNAL_SLOT_HPP

#include <boost/bind.hpp>
#include <boost/signals2.hpp>
#include <vector>

namespace e_engine {

template<class __R, class __C, class __A>
class eSlot;

template<class __R, class __A>
class eSignal;


namespace e_engine_internal {

/*!
 * \brief Moves eLog logging to \c signal_slot.cpp
 *
 * eLog needs Signals and Slots, so it is impossible to use eLog in this File
 */
class __eSigSlotLogFunctionClass {
   public:
      static void sigSlotLogFunktion( std::string _errStr, const char* _file, const int _line );
};



class __eSigSlotConnection {
      typedef typename boost::signals2::connection connection_TD;
   private:
      //! Counter for Signals
      static unsigned int vSignalsCounter_uI;

      __eSigSlotConnection() {}

      connection_TD vConnection_CON;
      unsigned int  vSignalID_uI;
   public:

      __eSigSlotConnection( connection_TD _connection, unsigned int _signalID ) : vConnection_CON( _connection ), vSignalID_uI( _signalID ) {}
      connection_TD getConnection() {return vConnection_CON;}
      unsigned int  getSignalId()   {return vSignalID_uI;}

      static void         increment()  {vSignalsCounter_uI++;}
      static unsigned int getCounter() {return vSignalsCounter_uI;}
};

}

/*!
 * \class e_engine::eSignal
 * \brief Class to manage signals
 *
 * This class stores a \c boost::signals2::signal object and
 * can connect it with a function from a eSlot class.
 *
 * \sa eSlot
 */
template<class __R, class __A>
class eSignal {
      typedef typename boost::signals2::signal<__R( __A )>            signal_TD;
      typedef typename boost::signals2::signal<__R( __A )>::slot_type slotType_TD;
      typedef typename boost::signals2::connection                    connection_TD;
   private:

      //! The signal
      signal_TD    vSignal_SIG;

      bool         vShowCopyError_B;

      unsigned int vThisSignalId_uI;

   public:
      //! The Constructor of the class
      eSignal( bool _showCopyError = true ) : vShowCopyError_B( _showCopyError ) {
         vThisSignalId_uI = e_engine_internal::__eSigSlotConnection::getCounter();
         e_engine_internal::__eSigSlotConnection::increment();
      }

      eSignal( const eSignal &_e ) {
         vShowCopyError_B = _e.vShowCopyError_B;
         vThisSignalId_uI = e_engine_internal::__eSigSlotConnection::getCounter();
         e_engine_internal::__eSigSlotConnection::increment();
         if ( vShowCopyError_B )
            e_engine_internal::__eSigSlotLogFunctionClass::sigSlotLogFunktion
            ( "Because it is impossible to copy a boost::signals2::signal a compleatly new one will be created! (Constructor)", __FILE__, __LINE__ );
         // signal = _e.signal; => Compiler ERROR: Can not coppy signal_TD objects
      }
      eSignal &operator=( const eSignal &_e ) {
         vShowCopyError_B = _e.vShowCopyError_B;
         vThisSignalId_uI = e_engine_internal::__eSigSlotConnection::getCounter();
         e_engine_internal::__eSigSlotConnection::increment();
         if ( vShowCopyError_B )
            e_engine_internal::__eSigSlotLogFunctionClass::sigSlotLogFunktion
            ( "Because it is impossible to copy a boost::signals2::signal a compleatly new one will be created! (operator=)", __FILE__, __LINE__ );
         // signal = _e.signal; => Compiler ERROR: Can not coppy signal_TD objects
         return *this;
      }

      //! Destructor will break all connections
      virtual ~eSignal() {
         disconnectAll();
      }

      /*!
       * \brief Connect with a slot
       *
       * From the \c eSlot object we get a boost function pointer for
       * the member function pointer stored in th slot object and
       * say the boost signal to connect with this function pointer.
       * The connection is finaly send to the slot object, where it will
       * be stored.
       *
       * \param slot The \c eSlot object
       * \returns The conne object
       */
      template<class __C>
      bool connectWith( eSlot<__R, __C, __A> *slot ) {
         if ( slot->testIfAlreadyConnected( vThisSignalId_uI ) )
            return false;

         connection_TD con;
         con = vSignal_SIG.connect( slot->getSlotType() );
         slot->setConnection( &con, vThisSignalId_uI );
         return true;
      }

      /*!
       * \brief Send the signal
       *
       * Send a signal, what causes that all functions connected to
       * this signal will be executed and receive the user defined
       * object \c atr as argument
       *
       * \param atr What to all connected functions will be send
       * \returns The return value of one fnction
       */
      __R operator()( __A _atr ) {
         return vSignal_SIG( _atr );
      }

      /*!
       * \brief Send the signal
       *
       * Send a signal, what causes that all functions connected to
       * this signal will be executed and receive the user defined
       * object \c atr as argument
       *
       * \param atr What to all connected functions will be send
       * \returns The return value of one fnction
       */
      __R sendSignal( __A _atr ) {
         return vSignal_SIG( _atr );
      }

      /*!
       * \brief Break the connection
       *
       * Says the \c eSlot object to break the connection it has stored
       *
       * \param slot The \c eSlot object which should break the connection
       * \returns \c SUCCES: \a true -- \c FAIL: \a false
       */
      template<class __C>
      bool disconnect( eSlot<__R, __C, __A> *slot ) {
         return slot->disconnectSignal( this );
      }

      /*!
       * \brief Disconnect all slots
       * \returns Nothing
       */
      void disconnectAll() {
         vSignal_SIG.disconnect_all_slots();
      }

      unsigned int getSignalId() const { return vThisSignalId_uI; }
};


/*!
 * \class e_engine::eSlot
 * \brief Class to manage slots
 *
 * This class stores the function pointer and the boost
 * connection object
 *
 * \sa eSignal
 */
template<class __R, class __C, class __A>
class eSlot {
      typedef typename boost::signals2::signal<__R( __A )>            signal_TD;
      typedef typename boost::signals2::signal<__R( __A )>::slot_type slotType_TD;
      typedef typename boost::signals2::connection                    connection_TD;
   private:
      //! The signal connection
      std::vector<e_engine_internal::__eSigSlotConnection> vConnections_eCON;

      //! This object pointer is needed to call the function pointer
      __C *classObjPointer;

      //! This is the member function pointer
      __R( __C::*CALL )( __A _arg );

      bool functionSet;

      slotType_TD getSlotType() {
         return boost::bind( CALL, classObjPointer, _1 );
      }

      inline bool testIfAlreadyConnected( unsigned int _signalID ) {
         for ( size_t i = 0; i < vConnections_eCON.size(); ++i ) {
            if ( vConnections_eCON[i].getSignalId() == _signalID ) {
               e_engine_internal::__eSigSlotLogFunctionClass::sigSlotLogFunktion
               ( "The Signal is already connected => Ignore", __FILE__, __LINE__ );
               return true;
            }
         }
         return false;
      }

      void setConnection( connection_TD *_con, unsigned int _signalID ) {
         vConnections_eCON.push_back( e_engine_internal::__eSigSlotConnection( *_con, _signalID ) );
      }

      void removeOld() {
         for ( size_t i = 0; i < vConnections_eCON.size(); ++i ) {
            if ( ! vConnections_eCON[i].getConnection().connected() ) {
               vConnections_eCON.erase( vConnections_eCON.begin() + i );
            }
         }
      }

   public:
      //! The most basic constructor
      eSlot() {
         functionSet = false;
      }

      /*!
       * \brief Constructor
       *
       * Constructor sets the function pointer.
       * Pointer format:
       * \code
       * &Class::function // No brackets!
       * \endcode
       *
       * \param _CALL The function pointer
       * \param _obj  Object pointer, which is needed to call the function pointer
       */
      eSlot( __R( __C::*_CALL )( __A _arg ), __C *_obj ) {
         CALL            = _CALL;
         classObjPointer = _obj;
         functionSet     = true;
      }

      //! Break the connection at the end of life
      virtual ~eSlot() {
         disconnectAll();
      }

      /*!
       * \brief Set a (new) function pointer
       *
       * Pointer format:
       * \code
       * &Class::function // No brackets!
       * \endcode
       *
       * \param _CALL The function pointer
       * \param _obj  Object pointer, which is needed to call the function pointer
       * \returns Nothing
       */
      inline void setFunc( __R( __C::*_CALL )( __A _arg ), __C *_obj ) {
         CALL            = _CALL;
         classObjPointer = _obj;
         functionSet     = true;
      }

      /*!
       * \brief Connect with a \c eSignal object
       *
       * This function checks if the slot is connectable and then tells
       * the \c eSignal object to connect with \a this slot.
       *
       * \sa eSlot::connectWith
       *
       * \param signal The \c eSignal object
       * \returns \c SUCCES: \a true -- \c FAIL: \a false
       */
      bool connectWith( eSignal<__R, __A> *_signal ) {
         if ( !functionSet ) {
            e_engine_internal::__eSigSlotLogFunctionClass::sigSlotLogFunktion
            ( "SLOT Function Pointer is undefined! Do nothing!", __FILE__, __LINE__ );
            return false;
         }
         _signal->connectWith( this ); // Sets the connection automatecally
         return true;
      }

      /*!
       * \brief Break the connection
       * \returns \c SUCCES: \a true -- \c FAIL: \a false
       */
      bool disconnectSignal( eSignal<__R, __A> *_signal ) {
         bool lSuccess_B = false;
         for ( size_t i = 0; i < vConnections_eCON.size(); ++i ) {
            if ( vConnections_eCON[i].getSignalId() == _signal->getSignalId() ) {
               if ( vConnections_eCON[i].getConnection().connected() ) {
                  vConnections_eCON[i].getConnection().disconnect();
                  lSuccess_B = true;
               }
            }
         }
         removeOld();
         return  lSuccess_B;
      }

      bool disconnectAll() {
         if ( vConnections_eCON.empty() )
            return false;
         for ( size_t i = 0; i < vConnections_eCON.size(); ++i ) {
            if ( vConnections_eCON[i].getConnection().connected() )
               vConnections_eCON[i].getConnection().disconnect();
         }
         vConnections_eCON.clear();
         return true;
      }

      //! Get is the slot is already connected  \returns Whether the slot is connected or not

      friend class eSignal<__R, __A>;
};

}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 


