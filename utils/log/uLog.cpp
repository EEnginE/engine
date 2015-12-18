/*!
 * \file log.cpp
 * \brief \b Classes: \a uLog
 * \sa e_uLog.hpp log_gen_entry.cpp
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

#include "uLog.hpp"
#include "defines.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <ctime>

#include FILESYSTEM_INCLUDE
#include <cstdio> // for fileno
#include <iostream>
#include <sstream>

#ifdef __linux__
#include <unistd.h>
#include <sys/ioctl.h>
#endif // __linux__

namespace e_engine {

uLog LOG;



/*!
 * \brief The standard constructor
 *
 * This only generates ID's for the main modes of this
 * project and generates the output types \c 'I', \c 'W', \c 'E'
 */
uLog::uLog()
    : vStdOut_eSLOT( &uLog::stdOutStandard, this ),
      vStdErr_eSLOT( &uLog::stdErrStandard, this ),
      vStdLog_eSLOT( &uLog::stdLogStandard, this ) {
   vMaxTypeStringLength_usI = 0;

   vIsLogLoopRunning_B = false;
   vLogLoopRun_B       = false;

   vLogFileName_str = "standard_Engine_Log_File";

   vLogEntries.clear();
}

uLog::~uLog() {
   stopLogLoop();
   if ( vLogFileOutput_OS.is_open() ) {
      vLogFileOutput_OS.close();
   }
}

void uLog::devInit() {
   addType( 'E', L"Error", 'R', true );
   addType( 'W', L"Warning", 'Y', false );
   addType( 'I', L"Info", 'G', false );
   addType( 'D', L"Debug", 'W', false );

   connectSlotWith( 'I', vStdOut_eSLOT );
   connectSlotWith( 'D', vStdOut_eSLOT );

   // What shall we do with warnings?
   if ( GlobConf.log.standardWarningsToStdErr ) {
      connectSlotWith( 'W', vStdErr_eSLOT ); // Send them to stderr
   } else {
      connectSlotWith( 'W', vStdOut_eSLOT ); // Send them to stdout
   }

   connectSlotWith( 'E', vStdErr_eSLOT );

   if ( !vLogFileOutput_OS.is_open() )
      openLogFile();

   if ( vLogFileOutput_OS.is_open() ) {
      connectSlotWith( 'I', vStdLog_eSLOT );
      connectSlotWith( 'D', vStdLog_eSLOT );
      connectSlotWith( 'W', vStdLog_eSLOT );
      connectSlotWith( 'E', vStdLog_eSLOT );

      iLOG( "LOGFILE:    ---  ", vLogFielFullPath_str, "  ---" );
   } else {
      wLOG( "Unable to open log file \"", vLogFielFullPath_str, "\" ==> No Log file output" );
   }
}

/*!
 * \brief Add a new log error type
 * \param _type  The character used to identify the log type to use
 * \param _name  The string wich can be displayed at the error output
 * \param _color The color ID from for colored output (can be disabled)
 * \param _bold  Is this error type important
 */
void uLog::addType( char _type, std::wstring _name, char _color, bool _bold ) {
   vLogTypes_V_eLT.emplace_back( _type, _name, _color, _bold );
   if ( _name.size() > vMaxTypeStringLength_usI )
      vMaxTypeStringLength_usI = static_cast<unsigned short int>( _name.size() );
}

/*!
 * \brief Names / renames a thread
 *
 * \param[in] _name the name of the thread
 */
void uLog::nameThread( std::wstring _name ) {
   vThreads[std::this_thread::get_id()] = _name;

   if ( _name.empty() )
      iLOG( L"Removing name from thread" );
   else
      iLOG( L"New Thread name: '", _name, L"'" );
}


bool uLog::openLogFile( uint16_t i ) {
   std::stringstream lThisLog_SS;
   std::stringstream lNextLog_SS;

   vLogFileName_str = GlobConf.log.logFILE.logFileName;

   if ( i == 0 ) {
      lThisLog_SS << vLogFileName_str << ".log";

      lNextLog_SS << vLogFileName_str << '.';
      if ( ( i + 1 ) < 100 ) {
         lNextLog_SS << '0';
      }
      if ( ( i + 1 ) < 10 ) {
         lNextLog_SS << '0';
      }
      lNextLog_SS << ( i + 1 ) << ".log";
   } else {
      lThisLog_SS << vLogFileName_str << '.';
      if ( i < 100 ) {
         lThisLog_SS << '0';
      }
      if ( i < 10 ) {
         lThisLog_SS << '0';
      }
      lThisLog_SS << i << ".log";

      lNextLog_SS << vLogFileName_str << '.';
      if ( ( i + 1 ) < 100 ) {
         lNextLog_SS << '0';
      }
      if ( ( i + 1 ) < 10 ) {
         lNextLog_SS << '0';
      }
      lNextLog_SS << ( i + 1 ) << ".log";
   }


   FILESYSTEM_NAMESPACE::path p( lThisLog_SS.str() );
   FILESYSTEM_NAMESPACE::path n( lNextLog_SS.str() );

   try {
      if ( FILESYSTEM_NAMESPACE::exists( p ) ) {
         if ( FILESYSTEM_NAMESPACE::is_regular_file( p ) ) {
            if ( i < GlobConf.config.maxNumOfLogFileBackshift ) {
               i++;
               openLogFile( i );
               i--;
               FILESYSTEM_NAMESPACE::rename( p, n );
            } else {
               FILESYSTEM_NAMESPACE::remove( p );
               return true;
            }
         }
      }
   } catch ( const FILESYSTEM_NAMESPACE::filesystem_error &ex ) {
      eLOG( ex.what() );
      return false;
   } catch ( ... ) {
      eLOG( "Caught unknown exeption" );
      return false;
   }

   if ( i == 0 ) {
      // Destructor will close it
      vLogFielFullPath_str = lThisLog_SS.str();
      vLogFileOutput_OS.open( vLogFielFullPath_str.c_str() );

      // LOG( __I, "Current log file: " )->add( vLogFielFullPath_str )->end();
   }

   return true;
}



void uLog::stdOutStandard( e_engine::uLogEntryRaw &_e ) {
   _e.data.configure( GlobConf.log.logOUT.colors,
                      GlobConf.log.logOUT.Time,
                      GlobConf.log.logOUT.File,
                      GlobConf.log.logOUT.ErrorType,
                      GlobConf.log.logOUT.Thread,
                      GlobConf.log.width );

#if UNIX
   if ( isatty( fileno( stdout ) ) == 0 ) {
      // No colors supported => no colors
      _e.data.config.vColor_LCT = DISABLED;
   }

   if ( GlobConf.log.width < 0 ) {
      struct winsize winS;
      ioctl( 0, TIOCGWINSZ, &winS );
      _e.data.config.vColumns_I = winS.ws_col;
   }
#endif // UNIX

   _e.data.vResultString_STR.clear();

   _e.defaultEntryGenerator();

   wprintf( _e.data.vResultString_STR.c_str() );
   fflush( stdout );
}


void uLog::stdErrStandard( e_engine::uLogEntryRaw &_e ) {
   _e.data.configure( GlobConf.log.logERR.colors,
                      GlobConf.log.logERR.Time,
                      GlobConf.log.logERR.File,
                      GlobConf.log.logERR.ErrorType,
                      GlobConf.log.logERR.Thread,
                      GlobConf.log.width );

#ifdef __linux__
   if ( isatty( fileno( stderr ) ) == 0 ) {
      // No colors supported => no colors
      _e.data.config.vColor_LCT = DISABLED;
   }

   if ( GlobConf.log.width < 0 ) {
      struct winsize winS;
      ioctl( 0, TIOCGWINSZ, &winS );
      _e.data.config.vColumns_I = winS.ws_col;
   }
#endif // __linux__

   _e.data.vResultString_STR.clear();

   _e.defaultEntryGenerator();

   fwprintf( stderr, _e.data.vResultString_STR.c_str() );
   fflush( stderr );
}

void uLog::stdLogStandard( e_engine::uLogEntryRaw &_e ) {
   if ( !vLogFileOutput_OS.is_open() ) {
      wLOG( "Logfile has closed! Trying to reopen it..." );
      vLogFileOutput_OS.open( vLogFielFullPath_str.c_str() );
   }

   if ( !vLogFileOutput_OS.is_open() ) {
      eLOG( "Failed to reopen log file! Disconnect the Slot" );
      vStdLog_eSLOT.disconnectAll();
      return;
   }

   _e.data.vResultString_STR.clear();

   _e.data.configure( DISABLED, // Impossible to show colors in a Text File
                      GlobConf.log.logFILE.Time,
                      GlobConf.log.logFILE.File,
                      GlobConf.log.logFILE.ErrorType,
                      GlobConf.log.logFILE.Thread,
                      -1 );

   _e.defaultEntryGenerator();

   vLogFileOutput_OS << _e.data.vResultString_STR;
}


void uLog::logLoop() {
   nameThread( L"log" );
   if ( vIsLogLoopRunning_B )
      return;

   vIsLogLoopRunning_B = true;
   std::string lErrorType_STR;
   unsigned int lLogTypeId_uI = 0;
   std::list<uLogEntryRaw> lEntryWorkerList;

   do {
      // Killing the CPU is not our Task
      B_SLEEP( milliseconds, 25 );

      {
         std::lock_guard<std::mutex> lLock( vLogThreadSaveMutex_BT );
         if ( vLogEntries.empty() )
            continue;

         // Transfer all elements (only the pointers) from vLogEntries to lTempList
         lEntryWorkerList.splice( lEntryWorkerList.end(), vLogEntries );
      }

      while ( !lEntryWorkerList.empty() ) {
         lLogTypeId_uI = lEntryWorkerList.front().getLogEntry( vLogTypes_V_eLT, vThreads );
         vLogTypes_V_eLT[lLogTypeId_uI].getSignal()->send( lEntryWorkerList.front() );
         lEntryWorkerList.pop_front();
      }
   } while ( vLogLoopRun_B );
   vIsLogLoopRunning_B = false;
}

bool uLog::startLogLoop() {
   if ( vIsLogLoopRunning_B == true )
      return false;

   if ( GlobConf.log.waitUntilLogEntryPrinted == true ) {
      vLogLoopRun_B = true;
      return true;
   }

   // Slow output fix
   if ( setvbuf( stdout, nullptr, _IOLBF, 4096 ) != 0 ) {
      wLOG( "Cannot set Windows output buffer [stdout]" );
   }
   if ( setvbuf( stderr, nullptr, _IOLBF, 4096 ) != 0 ) {
      wLOG( "Cannot set Windows output buffer [stderr]" );
   }

   vLogLoopRun_B         = true;
   vLogLoopThread_THREAD = std::thread( &uLog::logLoop, this );

   return true;
}

bool uLog::stopLogLoop() {
   // If stopLogLoop() is immediately called after startLogLoop() it can
   // generate strange errors because vIsLogLoopRunning_B can be false
   // when there was too litle time for the thread to check and set
   // vIsLogLoopRunning_B. 10 milliseconds sleeping should be enough
   // in this case.
   B_SLEEP( milliseconds, 10 );

   if ( vIsLogLoopRunning_B == false )
      return false;

   vLogLoopRun_B = false;
   vLogLoopThread_THREAD.join();

   return true;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
