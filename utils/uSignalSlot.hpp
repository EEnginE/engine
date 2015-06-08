/*!
 * \file uSignalSlot.hpp
 * \brief \b Classes: \a uSignal, \a uSlot
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


#ifndef U_SGNAL_SLOT_HPP
#define U_SGNAL_SLOT_HPP

#include <list>
#include <vector>
#include <mutex>
#include "defines.hpp"

namespace e_engine {

template <class __R, class __C, class... __A>
class uSlot;

template <class __R, class... __A>
class uSignal;


namespace internal {

template <class __R>
struct __uReturnStruct {
   __R value;
};

template <>
struct __uReturnStruct<void> {};

template <class __R>
struct __uSlotReturnHelper {
   template <class __C, class... __A>
   static __uReturnStruct<__R> call( __R ( __C::*_CALL )( __A... _arg ),
                                     __C *_obj,
                                     __A &&... _arg ) {
      __uReturnStruct<__R> lRet;
      lRet.value = ( *_obj.*_CALL )( std::forward<__A>( _arg )... );
      return lRet;
   }
};

template <>
struct __uSlotReturnHelper<void> {
   template <class __C, class... __A>
   static __uReturnStruct<void> call( void ( __C::*_CALL )( __A... _arg ),
                                      __C *_obj,
                                      __A &&... _arg ) {
      __uReturnStruct<void> lRet;
      ( *_obj.*_CALL )( std::forward<__A>( _arg )... );
      return lRet;
   }
};

//   _____ _       _     _   _      _
//  /  ___| |     | |   | | | |    | |
//  \ `--.| | ___ | |_  | |_| | ___| |_ __   ___ _ __
//   `--. \ |/ _ \| __| |  _  |/ _ \ | '_ \ / _ \ '__|
//  /\__/ / | (_) | |_  | | | |  __/ | |_) |  __/ |
//  \____/|_|\___/ \__| \_| |_/\___|_| .__/ \___|_|
//                                   | |
//                                   |_|

template <class __R, class... __A>
class uSlotBase {
 private:
   typedef uSignal<__R, __A...> SIGNAL;
   typedef __uReturnStruct<__R> RETURN;

 private:
   std::list<SIGNAL *> vSignals; //!< The signals connected to

   void addSignal( SIGNAL *_sig ); //!< Only called from uSignal
   void rmSignal( SIGNAL *_sig );  //!< Only called from uSignal

   bool isConnectedP( SIGNAL const *_sig );

 protected:
   std::mutex vSlotMutex;

   uSlotBase() {}

 public:
   virtual ~uSlotBase() {} //!< disconnectAll in the final class!

   /*
    * template<class... __a>
    * virtual RETURN call( __a && ... _args ) = 0;
    *
    * Unfortunately this is not possible beacuse the function is virtual
    */
   //! \todo enable perfect forwading with rvalue (&&) and std::forward

   virtual RETURN call( __A... _args ) = 0;

   bool connect( SIGNAL *_sig );
   bool disconnect( SIGNAL *_sig );
   bool isConnected( SIGNAL const *_sig );
   void disconnectAll();

   friend class uSignal<__R, __A...>;
};

template <class __R, class... __A>
void uSlotBase<__R, __A...>::addSignal( SIGNAL *_sig ) {
   std::lock_guard<std::mutex> lLock( vSlotMutex );
   vSignals.emplace_back( _sig );
}

template <class __R, class... __A>
void uSlotBase<__R, __A...>::rmSignal( SIGNAL *_sig ) {
   std::lock_guard<std::mutex> lLock( vSlotMutex );

   typename std::list<SIGNAL *>::iterator lIT = vSignals.begin();
   while ( lIT != vSignals.end() ) {
      if ( *lIT == _sig ) {
         vSignals.erase( lIT );
         return;
      }
      ++lIT;
   }
}


template <class __R, class... __A>
bool uSlotBase<__R, __A...>::isConnectedP( SIGNAL const *_sig ) {
   // Asume that mutex is locked
   for ( auto *s : vSignals )
      if ( _sig == s )
         return true;

   return false;
}


/*!
 * \brief Connect with a signal
 *
 * Connects this slot with a \c uSignal.
 *
 * \note A slot can have multiple signal
 *
 * \warning A slot \b must have set a function before connecting
 *
 * \param[in] _sig A Pointer to the \c uSignal object
 * \returns If connection was successfull
 */
template <class __R, class... __A>
bool uSlotBase<__R, __A...>::connect( SIGNAL *_sig ) {
   std::lock_guard<std::mutex> lLock( vSlotMutex );

   if ( isConnectedP( _sig ) )
      return false;

   vSignals.emplace_back( _sig );
   _sig->addSlot( this );

   return true;
}

/*!
 * \brief Break the connection
 *
 * Breaks the connection.
 *
 * \param[in] _sig The \c uSignal object which should break the connection
 * \returns \c SUCCES: \a true -- \c FAIL: \a false
 */
template <class __R, class... __A>
bool uSlotBase<__R, __A...>::disconnect( SIGNAL *_sig ) {
   std::lock_guard<std::mutex> lLock( vSlotMutex );

   if ( !isConnectedP( _sig ) )
      return false;

   typename std::list<SIGNAL *>::iterator lIT = vSignals.begin();
   while ( lIT != vSignals.end() ) {
      if ( *lIT == _sig ) {
         _sig->rmSlot( this );
         vSignals.erase( lIT );
         return true;
      }
   }

   return false;
}

/*!
 * \brief Tests if there is a connection
 *
 * \param[in] _sig A pointer to the \c uSignal object
 * \returns \c Connected: \a true -- \c FAIL: \a false
 */
template <class __R, class... __A>
bool uSlotBase<__R, __A...>::isConnected( SIGNAL const *_sig ) {
   std::lock_guard<std::mutex> lLock( vSlotMutex );
   return isConnectedP( _sig );
}

template <class __R, class... __A>
void uSlotBase<__R, __A...>::disconnectAll() {
   std::lock_guard<std::mutex> lLock( vSlotMutex );

   for ( auto *s : vSignals )
      s->rmSlot( this );

   vSignals.clear();
}
}



//   _____ _                   _   _____ _
//  /  ___(_)                 | | /  __ \ |
//  \ `--. _  __ _ _ __   __ _| | | /  \/ | __ _ ___ ___
//   `--. \ |/ _` | '_ \ / _` | | | |   | |/ _` / __/ __|
//  /\__/ / | (_| | | | | (_| | | | \__/\ | (_| \__ \__ \
//  \____/|_|\__, |_| |_|\__,_|_|  \____/_|\__,_|___/___/
//            __/ |
//           |___/

/*!
 * \class e_engine::uSignal
 * \brief class to manage signals
 *
 * This class stores a \c boost::signals2::signal object and
 * can connect it with a function from a uSlot class.
 *
 * \sa uSlot
 */
template <class __R, class... __A>
class uSignal final {
 public:
   typedef internal::uSlotBase<__R, __A...> SLOT;
   typedef internal::__uReturnStruct<__R> RETURN;

 private:
   std::mutex vSignalMutex;
   std::list<SLOT *> vSlots;

   std::vector<RETURN> vReturns;

   void addSlot( SLOT *_slot ); //!< Only called from uSlot
   void rmSlot( SLOT *_slot );  //!< Only called from uSlot

   bool isConnectedP( SLOT const *_slot );

 public:
   uSignal() {}                    //!< Nothing fancy to do here
   ~uSignal() { disconnectAll(); } //!< Destructor will break all connections

   uSignal( const uSignal &_e ) = delete;
   uSignal( const uSignal &&_e );

   uSignal &operator=( const uSignal &_e ) = delete;
   uSignal &operator=( const uSignal &&_e );

   bool connect( SLOT *_slot );
   bool disconnect( SLOT *_slot );
   bool isConnected( SLOT const *_slot );
   void disconnectAll();

   template <class... __a>
   std::vector<RETURN> &operator()( __a &&... _args ) {
      return send( std::forward<__a>( _args )... );
   }

   template <class... __a>
   std::vector<RETURN> &send( __a &&... _args );

   friend class internal::uSlotBase<__R, __A...>;
};

template <class __R, class... __A>
uSignal<__R, __A...>::uSignal( const uSignal &&_e ) {
   std::lock_guard<std::mutex> lLock2( vSignalMutex );
   vSlots = std::move( _e.vSlots );
   vReturns = std::move( _e.vReturns );
}

template <class __R, class... __A>
uSignal<__R, __A...> &uSignal<__R, __A...>::operator=( const uSignal &&_e ) {
   std::lock_guard<std::mutex> lLock2( vSignalMutex );
   vSlots = std::move( _e.vSlots );
   vReturns = std::move( _e.vReturns );
   return *this;
}


template <class __R, class... __A>
void uSignal<__R, __A...>::addSlot( SLOT *_slot ) {
   std::lock_guard<std::mutex> lLock( vSignalMutex );
   vSlots.emplace_back( _slot );
}

template <class __R, class... __A>
void uSignal<__R, __A...>::rmSlot( SLOT *_slot ) {
   std::lock_guard<std::mutex> lLock( vSignalMutex );

   typename std::list<SLOT *>::iterator lIT = vSlots.begin();
   while ( lIT != vSlots.end() ) {
      if ( *lIT == _slot ) {
         vSlots.erase( lIT );
         return;
      }
      ++lIT;
   }
}

template <class __R, class... __A>
bool uSignal<__R, __A...>::isConnectedP( SLOT const *_slot ) {
   // Asume that mutex is locked
   for ( auto *s : vSlots )
      if ( _slot == s )
         return true;

   return false;
}


/*!
 * \brief Connect with a slot
 *
 * Connects this signal with a \c uSlot.
 *
 * \note A Signal can have multiple slots
 *
 * \warning A slot \b must have set a function before connecting
 *
 * \param[in] _slot A Pointer to the \c uSlot object
 * \returns If connection was successfull
 */
template <class __R, class... __A>
bool uSignal<__R, __A...>::connect( SLOT *_slot ) {
   std::lock_guard<std::mutex> lLock( vSignalMutex );

   if ( isConnectedP( _slot ) )
      return false;

   vSlots.emplace_back( _slot );
   _slot->addSignal( this );
   vReturns.resize( vSlots.size() );

   return true;
}

/*!
 * \brief Break the connection
 *
 * Breaks the connection.
 *
 * \param[in] _slot The \c uSlot object which should break the connection
 * \returns \c SUCCES: \a true -- \c FAIL: \a false
 */
template <class __R, class... __A>
bool uSignal<__R, __A...>::disconnect( SLOT *_slot ) {
   std::lock_guard<std::mutex> lLock( vSignalMutex );

   if ( !isConnectedP( _slot ) )
      return false;

   typename std::list<SLOT *>::iterator lIT = vSlots.begin();
   while ( lIT != vSlots.end() ) {
      if ( *lIT == _slot ) {
         _slot->rmSignal( this );
         vSlots.erase( lIT );
         vReturns.resize( vSlots.size() );
         return true;
      }
   }

   return false;
}

/*!
 * \brief Tests if there is a connection
 * *
 * \param[in] _slot A pointer to the \c uSlot object
 * \returns \c Connected: \a true -- \c FAIL: \a false
 */
template <class __R, class... __A>
bool uSignal<__R, __A...>::isConnected( SLOT const *_slot ) {
   std::lock_guard<std::mutex> lLock( vSignalMutex );
   return isConnectedP( _slot );
}

template <class __R, class... __A>
void uSignal<__R, __A...>::disconnectAll() {
   std::lock_guard<std::mutex> lLock( vSignalMutex );

   for ( auto *s : vSlots )
      s->rmSignal( this );

   vSlots.clear();
   vReturns.resize( 0 );
}



/*!
 * \fn uSignal::operator()
 * \brief Send the signal
 *
 * Sends a signal that causes all functions connected to
 * this signal to be executed and receive the user defined
 * objects \c _atr as argument
 *
 * The result of every slot will be stored in a vector and can be
 * accessed with ::value (if __R is not void)
 *
 * \param _atr What needs to be sent to all connected functions
 * \returns A reference to the result vector
 */

/*!
 * \brief Send the signal
 *
 * Sends a signal that causes all functions connected to
 * this signal to be executed and receive the user defined
 * objects \c _atr as argument
 *
 * The result of every slot will be stored in a vector and can be
 * accessed with ::value (if __R is not void)
 *
 * \param _args What needs to be sent to all connected functions
 * \returns A reference to the result vector
 */
template <class __R, class... __A>
template <class... __a>
std::vector<internal::__uReturnStruct<__R>> &uSignal<__R, __A...>::send( __a &&... _args ) {
   std::lock_guard<std::mutex> lLock( vSignalMutex );

   unsigned int lCounter = 0;

   for ( auto *s : vSlots ) {
      vReturns[lCounter] = s->call( std::forward<__a>( _args )... );
      ++lCounter;
   }

   return vReturns;
}


//   _____ _       _     _____ _
//  /  ___| |     | |   /  __ \ |
//  \ `--.| | ___ | |_  | /  \/ | __ _ ___ ___
//   `--. \ |/ _ \| __| | |   | |/ _` / __/ __|
//  /\__/ / | (_) | |_  | \__/\ | (_| \__ \__ \
//  \____/|_|\___/ \__|  \____/_|\__,_|___/___/

/*!
 * \class e_engine::uSlot
 * \brief class to manage slots
 *
 * This class stores the function pointer and the boost
 * connection object
 *
 * \sa uSignal
 */
template <class __R, class __C, class... __A>
class uSlot final : public internal::uSlotBase<__R, __A...> {
   static_assert( std::is_class<__C>::value, "template argument __C MUST be a class!" );

 public:
   typedef uSignal<__R, __A...> SIGNAL;
   typedef internal::__uReturnStruct<__R> RETURN;

 private:
   __R ( __C::*CALL )( __A... _arg ); //!< This is the member function pointer
   __C *classPointer; //!< This object pointer is needed to call the function pointer

   using internal::uSlotBase<__R, __A...>::vSlotMutex;

 public:
   using internal::uSlotBase<__R, __A...>::disconnectAll;

   uSlot() = delete; //!< We need a function pointer
   uSlot( __R ( __C::*_CALL )( __A... _arg ), __C *_obj ) : CALL( _CALL ), classPointer( _obj ) {}

   ~uSlot() { disconnectAll(); } //!< Break the connection at the end of life

   virtual RETURN call( __A... _args );

   friend class uSignal<__R, __A...>;
};

/*!
 * \brief calls the function pointer
 *
 * \param[in] _args The arguments for the function
 * \returns a return structue with the return value of the function in RETURN::value (if __R !=
 *void)
 */
template <class __R, class __C, class... __A>
typename uSlot<__R, __C, __A...>::RETURN uSlot<__R, __C, __A...>::call( __A... _args ) {
   std::lock_guard<std::mutex> lLock( vSlotMutex );
   return internal::__uSlotReturnHelper<__R>::call(
         CALL, classPointer, std::forward<__A>( _args )... );
}


/*!
 * \fn uSlot::uSlot
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
}

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
