/// \file log.hpp
/// \brief \b Classes: \a eLog
/// \sa e_log.cpp
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


#ifndef E_LOG_HPP
#define E_LOG_HPP

#include <fstream>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include "macros.hpp"
#include "log_resources.hpp"

#if BOOST_VERSION < 105000
#define B_SLEEP( chrono_sleep, time ) {boost::posix_time::time_duration duration = boost::posix_time::chrono_sleep( time );\
                                      boost::this_thread::sleep( duration );}
#else
#define B_SLEEP( chrono_sleep, time ) boost::this_thread::sleep_for( boost::chrono::chrono_sleep( time ) )
#endif


namespace e_engine {

const unsigned short int MAX_LOG_TYPES = 8;

/*!
 * \class e_engine::eLog
 * \brief The main log class
 *
 * \par General information
 *
 * This is the main Log class of this project. It is
 * able to write all input in a log-file and to a
 * specific (colored) commandline output.
 *
 * \par Multithreading
 *
 * The output of \b one instance of this class is thread safe.
 * Every time you start the output the internal mutex will
 * be locked and after you end unlocked.
 *
 * \par Usage
 *
 * At first you really should change the standard log file
 * output to a file of your choise with the main config
 * structure \c WinData.config
 *
 * To start a new entry in jour log you can either call the
 * commands \c start, \c add, \c end after each other or
 * connect them.
 *
 * Example 1: After each other
 * \code
 *    LOG.start(5,'I',"Start of the log ");
 *    LOG.add("number ");
 *    LOG.add(1);
 *    LOG.end();
 * \endcode
 *
 * Example 2: Connected
 * \code
 *    LOG.start(5,'I',"Start of the log ")->add("number ")->add(1)->end();
 * \endcode
 *
 * \warning \b ALWAYS run \c end() at the and of a log entry!
 *
 * \warning It is impossible to change the log file after the 1st output!
 *
 * \note Only the <b>single predefined</b> instance \c LOG of this class should be used!
 */
class eLog {
      typedef eSignal<void, eLogEntry> _SIGNAL_;
      typedef eSlot<void, eLog , eLogEntry> _SLOT_;
   private:
      std::vector<e_engine_internal::eLogType> vLogTypes_V_eLT;
      std::string                              vLogFileName_str;
      std::string                              vLogFielFullPath_str;
      std::ofstream                            vLogFileOutput_OS;

      bool vLogLoopRun_B;
      bool vIsLogLoopRunning_B;

      _SLOT_ vStdOut_eSLOT;
      _SLOT_ vStdErr_eSLOT;
      _SLOT_ vStdLog_eSLOT;

      uint16_t vMaxTypeStringLength_usI; //!< The max string length from a \a Error \a type.
      
            
      boost::mutex vOnlyOneLogLoop_MUTEX;
      
      boost::thread vLogLoopThread_THREAD;

      bool openLogFile( uint16_t i = 0 );

      void logLoop();

      void stdOutStandard( e_engine::eLogEntry _e );
      void stdErrStandard( eLogEntry _e );
      void stdLogStandard( eLogEntry _e );

      std::list<e_engine_internal::__eLogStoreHelper> vLogList_L_eLSH;
   public:

      eLog();

      virtual ~eLog() {
         stopLogLoop();
         if ( vLogFileOutput_OS.is_open() ) {
            vLogFileOutput_OS.close();
         }
      }

      void devInit();

      void addType( char _type, std::string _name, char _color, bool _bold );

      void generateEntry( eLogEntry &_rawEntry );

      template<class __C>
      bool connectSlotWith( char _type, eSlot<void, __C, eLogEntry> &_slot );
      
      template<class __C>
      bool disconnectSlotWith( char _type, eSlot<void, __C, eLogEntry> &_slot );
      
      bool startLogLoop();
      bool stopLogLoop();


      /*!
       * \brief Only runs the \c start member function
       *
       * When this function is called it locks the internal mutex first.
       * After that it calls \c printStart to produce a nice introduction
       * for the following Log entry. At last it adds the 1st part of the log
       *
       * \param _id    The id of the mode for the string
       * \param _mode  The type of the output. Use the single character for this
       * \param _first The first part of log entry
       * \param _cmdOut Print to the commandline  \c Default: \a true
       * \param _logOut Write into log file       \c Default: \a true
       * \returns The \c this pinter which is neaded for the connection
       */
      template<class T>
      inline e_engine_internal::__eLogStoreHelper *operator()( char _type, const char* _file, const int _line, T _text ) {
         return start( _type, _file, _line, _text );
      }
      
      
      std::string getLogFileFullPath() {return vLogFielFullPath_str;}


      template<class T>
      inline e_engine_internal::__eLogStoreHelper *start( char _type, const char* _file, const int _line, T _text ) {
         e_engine_internal::__eLogStoreHelper temp( _type, _file, _line );
         temp.add( _text );
         vLogList_L_eLSH.push_back( temp );
         return &vLogList_L_eLSH.back();
      }

};

template<class __C>
bool eLog::connectSlotWith( char _type, eSlot< void, __C, eLogEntry > &_slot ) {
   for ( unsigned int i = 0; i < vLogTypes_V_eLT.size(); ++i ) {
      if ( vLogTypes_V_eLT.at( i ).getType() == _type ) {
         return _slot.connectWith( vLogTypes_V_eLT.at( i ).getSignal() );
      }
   }
   return false;
}

template<class __C>
bool eLog::disconnectSlotWith( char _type, eSlot< void, __C, eLogEntry > &_slot ) {
   for ( unsigned int i = 0; i < vLogTypes_V_eLT.size(); ++i ) {
      if ( vLogTypes_V_eLT.at( i ).getType() == _type ) {
         return _slot.disconnectSlotWith( vLogTypes_V_eLT.at( i ).getSignal() );
      }
   }
   return false;
}



/*!
 * \brief The standard \c eLog object
 *
 * This is the standard \c eLog object which schould
 * be used for \b all Logging operations
 */
extern eLog LOG;

}

#endif // E_LOG_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

