/*!
 * \file log.cpp
 * \brief \b Classes: \a uLog
 * \sa e_uLog.hpp log_gen_entry.cpp
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

#include "uLog.hpp"
#include "defines.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <ctime>

#include <boost/filesystem.hpp>
#include <cstdio> // for fileno

#ifdef __linux__
#include <unistd.h>
#include <sys/ioctl.h>
#endif // __linux__

namespace e_engine {

uLog LOG;

// =========================================================================================================================
// ==============================================================================================================================================
// =========                                    =============================================================================================================
// =======   Help functions to format log entry   ================================================================================================================
// =========                                    =============================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================



// =========================================================================================================================
// ==============================================================================================================================================
// =========                =================================================================================================================================
// =======   uLog functions   ====================================================================================================================================
// =========                =================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief The standard constructor
 *
 * This only generates ID's for the main modes of this
 * project and generates the output types \c 'I', \c 'W', \c 'E'
 */
uLog::uLog() {
   vMaxTypeStringLength_usI = 0;

   vIsLogLoopRunning_B = false;
   vLogLoopRun_B       = false;

   vLogFileName_str = "standard_Engine_Log_File";

   vStdOut_eSLOT.setFunc( &uLog::stdOutStandard, this );
   vStdErr_eSLOT.setFunc( &uLog::stdErrStandard, this );
   vStdLog_eSLOT.setFunc( &uLog::stdLogStandard, this );

   vLogEntries.clear();
}

void uLog::devInit() {
   addType( 'E', L"Error",   'R', true );
   addType( 'W', L"Warning", 'Y', false );
   addType( 'I', L"Info",    'G', false );
   addType( 'D', L"Debug",   'W', false );

   connectSlotWith( 'I', vStdOut_eSLOT );
   connectSlotWith( 'D', vStdOut_eSLOT );

   // What shall we do with warnings?
   if( GlobConf.log.standardWarningsToStdErr ) {
      connectSlotWith( 'W', vStdErr_eSLOT ); // Send them to stderr
   } else {
      connectSlotWith( 'W', vStdOut_eSLOT ); // Send them to stdout
   }

   connectSlotWith( 'E', vStdErr_eSLOT );

   if( ! vLogFileOutput_OS.is_open() )
      openLogFile();

   if( vLogFileOutput_OS.is_open() ) {
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
 * \returns Nothing
 */
void uLog::addType( char _type, std::wstring _name, char _color, bool _bold ) {
   vLogTypes_V_eLT.push_back( internal::uLogType( _type, _name, _color, _bold ) );
   if( _name.size() > vMaxTypeStringLength_usI )
      vMaxTypeStringLength_usI = (unsigned short int) _name.size();
}

bool uLog::openLogFile( uint16_t i ) {
   std::stringstream lThisLog_SS;
   std::stringstream lNextLog_SS;

   vLogFileName_str = GlobConf.log.logFILE.logFileName;

   if( i == 0 ) {
      lThisLog_SS << vLogFileName_str << ".log";

      lNextLog_SS << vLogFileName_str << '.';
      if( ( i + 1 ) < 100 ) {
         lNextLog_SS << '0';
      } if( ( i + 1 ) < 10 ) {
         lNextLog_SS << '0';
      }
      lNextLog_SS << ( i + 1 ) << ".log";
   } else {
      lThisLog_SS << vLogFileName_str << '.';
      if( i < 100 ) {
         lThisLog_SS << '0';
      } if( i < 10 ) {
         lThisLog_SS << '0';
      }
      lThisLog_SS <<     i     << ".log";

      lNextLog_SS << vLogFileName_str << '.';
      if( ( i + 1 ) < 100 ) {
         lNextLog_SS << '0';
      } if( ( i + 1 ) < 10 ) {
         lNextLog_SS << '0';
      }
      lNextLog_SS << ( i + 1 ) << ".log";
   }


   boost::filesystem::path p( lThisLog_SS.str() );
   boost::filesystem::path n( lNextLog_SS.str() );

   try {
      if( boost::filesystem::exists( p ) ) {
         if( boost::filesystem::is_regular_file( p ) ) {
            if( i < GlobConf.config.maxNumOfLogFileBackshift ) {
               i++;
               openLogFile( i );
               i--;
               boost::filesystem::rename( p, n );
            } else {
               boost::filesystem::remove( p );
               return true;
            }
         }
      }
   } catch( const boost::filesystem::filesystem_error &ex )  {
      eLOG( ex.what() );
      return false;
   } catch( ... ) {
      eLOG( "Caught unknown exeption" );
      return false;
   }

   if( i == 0 ) {
      // Destructor will close it
      vLogFielFullPath_str = lThisLog_SS.str();
      vLogFileOutput_OS.open( vLogFielFullPath_str.c_str() );

      //LOG( __I, "Current log file: " )->add( vLogFielFullPath_str )->end();
   }

   return true;
}



// =========================================================================================================================
// ==============================================================================================================================================
// =========                           ======================================================================================================================
// =======   Standard Output functions   =========================================================================================================================
// =========                           ======================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


void uLog::stdOutStandard( e_engine::uLogEntryRaw &_e ) {
   _e.data.configure( GlobConf.log.logOUT.colors,
         GlobConf.log.logOUT.Time,
         GlobConf.log.logOUT.File,
         GlobConf.log.logOUT.ErrorType,
         GlobConf.log.width );

#if UNIX
   if( isatty( fileno( stdout ) ) == 0 ) {
      // No colors supported => no colors
      _e.data.config.vColor_LCT = DISABLED;
   }

   if( GlobConf.log.width < 0 ) {
      struct winsize winS;
      ioctl( 0, TIOCGWINSZ, &winS );
      _e.data.config.vColumns_uI = winS.ws_col;
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
         GlobConf.log.width );

#ifdef __linux__
   if( isatty( fileno( stderr ) ) == 0 ) {
      // No colors supported => no colors
      _e.data.config.vColor_LCT = DISABLED;
   }

   if( GlobConf.log.width < 0 ) {
      struct winsize winS;
      ioctl( 0, TIOCGWINSZ, &winS );
      _e.data.config.vColumns_uI = winS.ws_col;
   }
#endif // __linux__

   _e.data.vResultString_STR.clear();

   _e.defaultEntryGenerator();

   fwprintf( stderr, _e.data.vResultString_STR.c_str() );
   fflush( stderr );
}

void uLog::stdLogStandard( e_engine::uLogEntryRaw &_e ) {
   if( ! vLogFileOutput_OS.is_open() ) {
      wLOG( "Logfile has closed! Trying to reopen it..." );
      vLogFileOutput_OS.open( vLogFielFullPath_str.c_str() );
   }

   if( ! vLogFileOutput_OS.is_open() ) {
      eLOG( "Failed to reopen log file! Disconnect the Slot" );
      vStdLog_eSLOT.disconnectAll();
      return;
   }

   _e.data.vResultString_STR.clear();

   _e.data.configure( DISABLED,                      // Impossible to show colors in a Text File
         GlobConf.log.logFILE.Time,
         GlobConf.log.logFILE.File,
         GlobConf.log.logFILE.ErrorType,
         -1 );

   _e.defaultEntryGenerator();

   vLogFileOutput_OS << _e.data.vResultString_STR;
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========                      ===========================================================================================================================
// =======   Log Entry generation   ==============================================================================================================================
// =========                      ===========================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

// Moved to log_gen_entry.cpp


// =========================================================================================================================
// ==============================================================================================================================================
// =========                =================================================================================================================================
// =======   Loop functions   ====================================================================================================================================
// =========                =================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


void uLog::logLoop() {
   if( vIsLogLoopRunning_B )
      return;

   vIsLogLoopRunning_B = true;
   std::string  lErrorType_STR;
   unsigned int lLogTypeId_uI = 0;
   do {
      // Killing the CPU is not our Task
      B_SLEEP( milliseconds, 25 );

      while( ! vLogEntries.empty() ) {
         while( ! vLogEntries.front().getIsComplete() )
            B_SLEEP( milliseconds, 50 );

         try {
            lLogTypeId_uI = vLogEntries.front().getLogEntry( vLogTypes_V_eLT );
            vLogTypes_V_eLT[lLogTypeId_uI].getSignal()->sendSignal( vLogEntries.front() );
            vLogEntries.front().endLogWaitAndSetPrinted();
         } catch( std::bad_alloc e ) {
            std::cerr << "Received bad_alloc exeption in log Loop -- FILE: " << __FILE__ << " -- LINE: " << __LINE__ << std::endl;
            std::cerr << e.what() << std::endl;
            return;
         } catch( ... ) {
            // A new log entry in a crashing log loop fuction would be useless
            std::cerr << "Received unknown exeption in log Loop -- FILE: " << __FILE__ << " -- LINE: " << __LINE__ << std::endl;
            return;
         }

         vLogEntries.pop_front();
      }
   } while( vLogLoopRun_B );
   vIsLogLoopRunning_B = false;
}

bool uLog::startLogLoop() {
   if( vIsLogLoopRunning_B == true )
      return false;

   // Slow output fix
   if( setvbuf( stdout, 0, _IOLBF, 4096 ) != 0 ) {
      wLOG( "Cannot set Windows output buffer [stdout]" );
   }
   if( setvbuf( stderr, 0, _IOLBF, 4096 ) != 0 ) {
      wLOG( "Cannot set Windows output buffer [stderr]" );
   }

   vLogLoopRun_B = true;
   vLogLoopThread_THREAD = boost::thread( &uLog::logLoop, this );

   return true;
}

bool uLog::stopLogLoop() {
   // If stopLogLoop() is immediately called after startLogLoop() it can
   // generate strange errors because vIsLogLoopRunning_B can be false
   // when there was too litle time for the thread to check and set
   // vIsLogLoopRunning_B. 10 milliseconds sleeping should be enough
   // in this case.
   B_SLEEP( milliseconds, 10 );

   if( vIsLogLoopRunning_B == false )
      return false;

   vLogLoopRun_B = false;

#if BOOST_VERSION < 105000
   boost::posix_time::time_duration duration = boost::posix_time::seconds( 60 );
   vLogLoopThread_THREAD.timed_join( duration );
#else
   vLogLoopThread_THREAD.try_join_for( boost::chrono::seconds( 60 ) );
#endif

   if( vIsLogLoopRunning_B == true ) {
      vLogLoopThread_THREAD.interrupt(); // 1s should be more than enough, so something went wrong
   }

   return true;
}




}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
