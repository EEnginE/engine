/*!
 * \file uSignalSlot.hpp
 * \brief \b Classes: \a uSignal, \a uSlot
 */
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


#ifndef U_SGNAL_SLOT_HPP
#define U_SGNAL_SLOT_HPP

#include <functional>

// This stuff is needed for our std::bind stuff so that we can use "Variadic templates"
// More information https://stackoverflow.com/questions/21192659/variadic-templates-and-stdbind
template<int...>           struct int_sequence                                                            {};
template<int N, int... Is> struct make_int_sequence           : make_int_sequence < N - 1, N - 1, Is... > {};
template<int... Is>        struct make_int_sequence<0, Is...> : int_sequence<Is...>                       {};
template<int>              struct placeholder_template                                                    {};


namespace std {
template<int N> struct is_placeholder< placeholder_template<N> > : integral_constant < int, N + 1 >       {};
}


#include <boost/signals2.hpp>
#include <vector>

#include "defines.hpp"
#include "engine_utils_Export.hpp"

namespace e_engine {

template<class __R, class __C, class... __A>
class uSlot;

template<class __R, class... __A>
class uSignal;


namespace internal {

/*!
 * \brief Moves uLog logging to \c signal_slot.cpp
 *
 * uLog needs Signals and Slots, so it is impossible to use uLog in this File
 */
class __uSigSlotLogFunctionClass {
   public:
      static utils_EXPORT void sigSlotLogFunktion( std::string _errStr, const char *_file, const int _line, const char *_function );
};



class __uSigSlotConnection {
	#ifndef _MSC_VER
		typedef typename boost::signals2::connection connection_TD;
	#else
		typedef boost::signals2::connection connection_TD;
	#endif
   private:
      //! Counter for Signals
      static utils_EXPORT unsigned int vSignalsCounter_uI;

      __uSigSlotConnection() {}

      connection_TD vConnection_CON;
      unsigned int  vSignalID_uI;
   public:

      __uSigSlotConnection( connection_TD _connection, unsigned int _signalID ) : vConnection_CON( _connection ), vSignalID_uI( _signalID ) {}
      connection_TD getConnection() {return vConnection_CON;}
      unsigned int  getSignalId()   {return vSignalID_uI;}

      static void         increment()  {vSignalsCounter_uI++;}
      static unsigned int getCounter() {return vSignalsCounter_uI;}
};

}

/*!
 * \class e_engine::uSignal
 * \brief class to manage signals
 *
 * This class stores a \c boost::signals2::signal object and
 * can connect it with a function from a uSlot class.
 *
 * \sa uSlot
 */
template<class __R, class... __A>
class uSignal {
      typedef typename boost::signals2::signal<__R( __A... )>            signal_TD;
      typedef typename boost::signals2::signal<__R( __A... )>::slot_type slotType_TD;
      typedef typename boost::signals2::connection                       connection_TD;
      typedef typename signal_TD::result_type                            signalReturn_TD;
   private:

      //! The signal
      signal_TD    vSignal_SIG;

      bool         vShowCopyError_B;

      unsigned int vThisSignalId_uI;

   public:
      //! The Constructor of the class
      uSignal( bool _showCopyError = true ) : vShowCopyError_B( _showCopyError ) {
         vThisSignalId_uI = internal::__uSigSlotConnection::getCounter();
         internal::__uSigSlotConnection::increment();
      }

      uSignal( const uSignal &_e ) {
         vShowCopyError_B = _e.vShowCopyError_B;
         vThisSignalId_uI = internal::__uSigSlotConnection::getCounter();
         internal::__uSigSlotConnection::increment();
         if ( vShowCopyError_B )
            internal::__uSigSlotLogFunctionClass::sigSlotLogFunktion
            ( "Because it is impossible to copy a boost::signals2::signal a compleatly new one will be created! (Constructor)", __FILE__, __LINE__, LOG_FUNCTION_NAME );
         // signal = _e.signal; => Compiler ERROR: Can not copy signal_TD objects
      }
      uSignal &operator=( const uSignal &_e ) {
         vShowCopyError_B = _e.vShowCopyError_B;
         vThisSignalId_uI = internal::__uSigSlotConnection::getCounter();
         internal::__uSigSlotConnection::increment();
         if ( vShowCopyError_B )
            internal::__uSigSlotLogFunctionClass::sigSlotLogFunktion
            ( "Because it is impossible to copy a boost::signals2::signal a compleatly new one will be created! (operator=)", __FILE__, __LINE__, LOG_FUNCTION_NAME );
         // signal = _e.signal; => Compiler ERROR: Can not copy signal_TD objects
         return *this;
      }

      //! Destructor will break all connections
      virtual ~uSignal() {
         disconnectAll();
      }

      /*!
       * \brief Connect with a slot
       *
       * From the \c uSlot object we get a boost function pointer for
       * the member function pointer stored in th slot object and
       * say the boost signal to connect with this function pointer.
       * The connection is finally sent to the slot object,
       * where it will be stored.
       *
       * \param slot The \c uSlot object
       * \returns The conne object
       */
      template<class __C>
      bool connectWith( uSlot<__R, __C, __A...> *slot ) {
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
       * Sends a signal that causes all functions connected to
       * this signal to be executed and receive the user defined
       * object \c atr as an argument
       *
       * \param atr What needs to be sent to all connected functions
       * \returns The return value of one function
       */
       signalReturn_TD operator()( __A... _atr ) {
         return vSignal_SIG( _atr... );
      }

      /*!
       * \brief Send the signal
       *
       * Sends a signal that causes all functions connected to
       * this signal to be executed and receive the user defined
       * object \c atr as argument
       *
       * \param atr What needs to be sent to all connected functions
       * \returns The return value of one fnction
       */
      signalReturn_TD sendSignal( __A... _atr ) {
         return vSignal_SIG( _atr... );
      }

      /*!
       * \brief Break the connection
       *
       * Tells the \c uSlot object to break the connection it has stored
       *
       * \param slot The \c uSlot object which should break the connection
       * \returns \c SUCCES: \a true -- \c FAIL: \a false
       */
      template<class __C>
      bool disconnect( uSlot<__R, __C, __A...> *slot ) {
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
 * \class e_engine::uSlot
 * \brief class to manage slots
 *
 * This class stores the function pointer and the boost
 * connection object
 *
 * \sa uSignal
 */
template<class __R, class __C, class... __A>
class uSlot {
      typedef typename boost::signals2::signal<__R( __A... )>            signal_TD;
      typedef typename boost::signals2::signal<__R( __A... )>::slot_type slotType_TD;
      typedef typename boost::signals2::connection                    connection_TD;
//       typedef __R( __C::*CALL_TYPE )( __A... _arg );
   private:
      //! The signal connection
      std::vector<internal::__uSigSlotConnection> vConnections_eCON;

      //! This object pointer is needed to call the function pointer
      __C *classObjPointer;

      //! This is the member function pointer
      __R( __C::*CALL )( __A... _arg );

      bool functionSet;

      template<int... Is>
      slotType_TD bindHelper( __R( __C::*CLASS_2 )( __A... ), int_sequence<Is...> ) {
         return std::bind( CLASS_2, classObjPointer, placeholder_template<Is> {} ... );
      }

      slotType_TD getSlotType() {
         return bindHelper( CALL, make_int_sequence< sizeof...( __A ) > {} );
      }


      inline bool testIfAlreadyConnected( unsigned int _signalID ) {
         for ( size_t i = 0; i < vConnections_eCON.size(); ++i ) {
            if ( vConnections_eCON[i].getSignalId() == _signalID ) {
               internal::__uSigSlotLogFunctionClass::sigSlotLogFunktion
               ( "The Signal is already connected => Ignore", __FILE__, __LINE__, LOG_FUNCTION_NAME );
               return true;
            }
         }
         return false;
      }

      void setConnection( connection_TD *_con, unsigned int _signalID ) {
         vConnections_eCON.push_back( internal::__uSigSlotConnection( *_con, _signalID ) );
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
      uSlot() {
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
      uSlot( __R( __C::*_CALL )( __A... _arg ), __C *_obj ) {
         CALL            = _CALL;
         classObjPointer = _obj;
         functionSet     = true;
      }

      //! Break the connection at the end of life
      virtual ~uSlot() {
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
      inline void setFunc( __R( __C::*_CALL )( __A... _arg ), __C *_obj ) {
         CALL            = _CALL;
         classObjPointer = _obj;
         functionSet     = true;
      }

      /*!
       * \brief Connect with a \c uSignal object
       *
       * This function checks if the slot can be connected to and then tells
       * the \c uSignal object to connect with \a this slot.
       *
       * \sa uSlot::connectWith
       *
       * \param signal The \c uSignal object
       * \returns \c SUCCESS: \a true -- \c FAIL: \a false
       */
      bool connectWith( uSignal<__R, __A...> *_signal ) {
         if ( !functionSet ) {
            internal::__uSigSlotLogFunctionClass::sigSlotLogFunktion
            ( "SLOT Function Pointer is undefined! Do nothing!", __FILE__, __LINE__, LOG_FUNCTION_NAME );
            return false;
         }
         _signal->connectWith( this ); // Sets the connection automatically
         return true;
      }

      /*!
       * \brief Break the connection
       * \returns \c SUCCESS: \a true -- \c FAIL: \a false
       */
      bool disconnectSignal( uSignal<__R, __A...> *_signal ) {
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

      //! Get if the slot is already connected  \returns Whether the slot is connected or not

      friend class uSignal<__R, __A...>;
};

}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 


