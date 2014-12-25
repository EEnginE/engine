/*!
 * \file uLog.hpp
 * \brief \b Classes: \a uLog
 * \sa e_log.cpp
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


#ifndef E_LOG_HPP
#define E_LOG_HPP

#include <fstream>
#include <chrono>
#include "uLog_resources.hpp"
#include "uMacros.hpp"

#define B_SLEEP( chrono_sleep, time ) std::this_thread::sleep_for( std::chrono::chrono_sleep( time ) )


namespace e_engine {


/*!
 * \class e_engine::uLog
 * \brief The main log class
 *
 * \par General information
 *
 * This is the main Log class of this project. It is
 * able to write all input into a log-file and to a
 * specific (colored) commandline output.
 *
 * \par Multithreading
 *
 * The output of \b one instance of this class is thread safe.
 * This class prints everything in one seperated thread.
 *
 * \par Usage
 *
 * At first you should change the standard log file
 * output to a file of your choice with the main config
 * structure \c GlobConf.log
 *
 * To start a new log entry run either LOG( < type macro >, a comma, seperated list, of data );
 * or < type >LOG( a comma, seperated list, of data );
 *
 * It consists of a '_' and a single capital letter.
 * The letter defines the log type.
 * There are 3 predefined ones but you can add more by using
 *
 * \code
 * addType( the character, the long name (something like INFO), the color, if it should be printed bold )
 * connectSlotWith( the character, your slot ( e_engine::uSlot ) with your log generating function )
 * \endcode
 *
 * Prdefined modes are:
 *
 * | Mode | Name    |
 * | :--: | :-----: |
 * | _I   | INFO    |
 * | _W   | WARNING |
 * | _E   | ERROR   |
 * | _D   | DEBUG   |
 *
 * You can also change the color by putting single chars ( e_engine::eCMDColor for more information )
 * in front of the actual log message.
 *
 * Instead of using the functions directly you can use the macros like in Example 2.
 *
 *
 * Example 1: Normal
 * \code
 *    LOG( _I, "Start of the log ", L"number ", 1, eCMDColor::color( 'B', 'G' ), " I am colored" );
 * \endcode
 *
 * Example 2: Macros
 * \code
 *    iLOG( "Start of the log ", L"number ", 1, eCMDColor::color( 'B', 'G' ), " I am colored" );
 * \endcode
 *
 * \warning It is impossible to change the log file after the 1st output!
 *
 * \note Only the <b>single predefined</b> instance \c LOG of this class should be used!
 */
class uLog {
      typedef uSignal<void, uLogEntryRaw &> _SIGNAL_;
      typedef uSlot<void, uLog , uLogEntryRaw &> _SLOT_;
   private:
      std::vector<internal::uLogType> vLogTypes_V_eLT;
      std::string                     vLogFileName_str;
      std::string                     vLogFielFullPath_str;
      std::wofstream                  vLogFileOutput_OS;

      std::mutex vLogMutex_BT;

      std::mutex vLogThreadSaveMutex_BT;

      bool vLogLoopRun_B;
      bool vIsLogLoopRunning_B;

      _SLOT_ vStdOut_eSLOT;
      _SLOT_ vStdErr_eSLOT;
      _SLOT_ vStdLog_eSLOT;

      uint16_t vMaxTypeStringLength_usI; //!< The max string length of an \a Error \a type.

      std::thread vLogLoopThread_THREAD;

      bool openLogFile( uint16_t i = 0 );

      void logLoop();

      void stdOutStandard( uLogEntryRaw &_e );
      void stdErrStandard( uLogEntryRaw &_e );
      void stdLogStandard( uLogEntryRaw &_e );

      std::list<uLogEntryRaw>      vLogEntries;
   public:

      uLog();

      virtual ~uLog() {
         stopLogLoop();
         if( vLogFileOutput_OS.is_open() ) {
            vLogFileOutput_OS.close();
         }
      }

      uint16_t getMaxTypeStingLength() { return vMaxTypeStringLength_usI; }
      bool     getIsLogLoopRunning()   { return vIsLogLoopRunning_B; }

      void devInit();

      void addType( char _type, std::wstring _name, char _color, bool _bold );

      template<class __C>
      bool connectSlotWith( char _type, uSlot<void, __C, uLogEntryRaw &> &_slot );

      template<class __C>
      bool disconnectSlotWith( char _type, uSlot<void, __C, uLogEntryRaw &> &_slot );

      bool startLogLoop();
      bool stopLogLoop();


      template<class... ARGS>
      inline void operator()( char _type, bool _onlyText, const char *_file, const int _line, const char *_function, ARGS && ... _data ) {
         std::lock_guard<std::mutex> lLock( vLogThreadSaveMutex_BT );
         vLogEntries.emplace_back( _type, _onlyText, _file, _line, _function, std::forward<ARGS>( _data )... );
         vLogEntries.back().end();
      }

      template<class... ARGS>
      inline void addLogEntry( char _type, bool _onlyText, const char *_file, const int _line, const char *_function, ARGS && ... _data ) {
         std::lock_guard<std::mutex> lLock( vLogThreadSaveMutex_BT );
         vLogEntries.emplace_back( _type, _onlyText, _file, _line, _function, std::forward<ARGS>( _data )... );
         vLogEntries.back().end();
      }

      std::string getLogFileFullPath() {return vLogFielFullPath_str;}

};

template<class __C>
bool uLog::connectSlotWith( char _type, uSlot< void, __C, uLogEntryRaw & > &_slot ) {
   for( unsigned int i = 0; i < vLogTypes_V_eLT.size(); ++i ) {
      if( vLogTypes_V_eLT.at( i ).getType() == _type ) {
         return _slot.connect( vLogTypes_V_eLT.at( i ).getSignal() );
      }
   }
   return false;
}

template<class __C>
bool uLog::disconnectSlotWith( char _type, uSlot< void, __C, uLogEntryRaw & > &_slot ) {
   for( unsigned int i = 0; i < vLogTypes_V_eLT.size(); ++i ) {
      if( vLogTypes_V_eLT.at( i ).getType() == _type ) {
         return _slot.disconnect( vLogTypes_V_eLT.at( i ).getSignal() );
      }
   }
   return false;
}

/*!
 * \fn uLog::operator()
 * \brief Only runs the \c start member function
 *
 * When this function is called it locks the internal mutex first.
 * After that it calls \c printStart to produce a nice introduction
 * for the following Log entry.
 * At last it adds the 1st part of the log
 *
 * \param _id    The id of the mode for the string
 * \param _mode  The type of the output. Use the single character for this
 * \param _first The first part of log entry
 * \param _cmdOut Print to the commandline  \c Default: \a true
 * \param _logOut Write into log file       \c Default: \a true
 * \returns The \c this pinter which is needed for the connection
 */



/*!
 * \brief The standard \c uLog object
 *
 * This is the standard \c uLog object which should
 * be used for \b all Logging operations
 */
extern uLog LOG;

}

#endif // E_LOG_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;

