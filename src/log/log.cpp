/*!
 * \file log.cpp
 * \brief \b Classes: \a eLog
 * \sa e_log.hpp log_gen_entry.cpp
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

#include "log.hpp"

#define BOOST_FILESYSTEM_NO_DEPRECATED

#include <ctime>

#include <boost/filesystem.hpp>
#include <cstdio> // for fileno

#ifdef __linux__
#include <unistd.h>
#include <sys/ioctl.h>
#endif // __linux__

namespace e_engine {

eLog LOG;

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
// =======   eLog functions   ====================================================================================================================================
// =========                =================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief The standard constructor
 *
 * This only generates ID's for the main modes of this
 * project and generates the output types \c 'I', \c 'W', \c 'E'
 */
eLog::eLog() {
   vMaxTypeStringLength_usI = 0;

   vIsLogLoopRunning_B = false;
   vLogLoopRun_B       = false;

   vLogFileName_str = "standard_Engine_Log_File";

   vStdOut_eSLOT.setFunc( &eLog::stdOutStandard, this );
   vStdErr_eSLOT.setFunc( &eLog::stdErrStandard, this );
   vStdLog_eSLOT.setFunc( &eLog::stdLogStnagard, this );

   vLogList_L_eLSH.clear();
}

void eLog::devInit() {
   addType( 'E', "Error",   'R', true );
   addType( 'W', "Warning", 'Y', false );
   addType( 'I', "Info",    'G', false );

   connectSlotWith( 'I', vStdOut_eSLOT );

   // What shall we do with warnings?
   if ( WinData.log.standardWarningsToStdErr ) {
      connectSlotWith( 'W', vStdErr_eSLOT ); // Send them to stderr
   } else {
      connectSlotWith( 'W', vStdOut_eSLOT ); // Send them to stdout
   }

   connectSlotWith( 'E', vStdErr_eSLOT );

   if ( ! vLogFileOutput_OS.is_open() )
      openLogFile();

   if ( vLogFileOutput_OS.is_open() ) {
      connectSlotWith( 'I', vStdLog_eSLOT );
      connectSlotWith( 'W', vStdLog_eSLOT );
      connectSlotWith( 'E', vStdLog_eSLOT );
   } else {
      wLOG "Unable to open log file \"" ADD vLogFielFullPath_str ADD "\" ==> No Log file output" END
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
void eLog::addType( char _type, std::string _name, char _color, bool _bold ) {
   vLogTypes_V_eLT.push_back( e_engine_internal::eLogType( _type, _name, _color, _bold ) );
   if ( _name.size() > vMaxTypeStringLength_usI )
      vMaxTypeStringLength_usI = _name.size();
}

bool eLog::openLogFile( uint16_t i ) {
   std::stringstream lThisLog_SS;
   std::stringstream lNextLog_SS;

   vLogFileName_str = WinData.log.logFILE.logFileName;

   if ( i == 0 ) {
      lThisLog_SS << vLogFileName_str << ".log";

      lNextLog_SS << vLogFileName_str << '.';
      if ( ( i + 1 ) < 100 ) {
         lNextLog_SS << '0';
      } if ( ( i + 1 ) < 10 ) {
         lNextLog_SS << '0';
      }
      lNextLog_SS << ( i + 1 ) << ".log";
   } else {
      lThisLog_SS << vLogFileName_str << '.';
      if ( i < 100 ) {
         lThisLog_SS << '0';
      } if ( i < 10 ) {
         lThisLog_SS << '0';
      }
      lThisLog_SS <<     i     << ".log";

      lNextLog_SS << vLogFileName_str << '.';
      if ( ( i + 1 ) < 100 ) {
         lNextLog_SS << '0';
      } if ( ( i + 1 ) < 10 ) {
         lNextLog_SS << '0';
      }
      lNextLog_SS << ( i + 1 ) << ".log";
   }


   boost::filesystem::path p( lThisLog_SS.str() );
   boost::filesystem::path n( lNextLog_SS.str() );

   try {
      if ( boost::filesystem::exists( p ) ) {
         if ( boost::filesystem::is_regular_file( p ) ) {
            if ( i < WinData.config.maxNumOfLogFileBackshift ) {
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
   } catch ( const boost::filesystem::filesystem_error &ex )  {
      eLOG ex.what() END
      return false;
   } catch ( ... ) {
      eLOG "Caught unknown exeption" END
      return false;
   }

   if ( i == 0 ) {
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


void eLog::stdOutStandard( e_engine::eLogEntry _e ) {
#ifdef WIN32
   // Sorry, no Color support for Windows
   _e.config.vColor_LCT = DISABLED;
#endif

   _e.configure( WinData.log.logOUT.colors,
                 WinData.log.logOUT.Time,
                 WinData.log.logOUT.File,
                 WinData.log.logOUT.ErrorType,
                 WinData.log.width );

#ifdef __linux__
   if ( isatty( fileno( stdout ) ) == 0 ) {
      // No colors supported => no colors
      _e.config.vColor_LCT = DISABLED;
   }

   if ( WinData.log.width < 0 ) {
      struct winsize winS;
      ioctl( 0, TIOCGWINSZ, &winS );
      _e.config.vColumns_uI = winS.ws_col;
   }
#endif // __linux__


   generateEntry( _e );

   std::cout << _e.vResultStrin_STR;
}


void eLog::stdErrStandard( eLogEntry _e ) {
#ifdef WIN32
   // Sorry, no Color support for Windows
   _e.config.vColor_LCT = DISABLED;
#endif

   _e.configure( WinData.log.logERR.colors,
                 WinData.log.logERR.Time,
                 WinData.log.logERR.File,
                 WinData.log.logERR.ErrorType,
                 WinData.log.width );

#ifdef __linux__
   if ( isatty( fileno( stderr ) ) == 0 ) {
      // No colors supported => no colors
      _e.config.vColor_LCT = DISABLED;
   }

   if ( WinData.log.width < 0 ) {
      struct winsize winS;
      ioctl( 0, TIOCGWINSZ, &winS );
      _e.config.vColumns_uI = winS.ws_col;
   }
#endif // __linux__

   generateEntry( _e );

   std::cerr << _e.vResultStrin_STR;
}

void eLog::stdLogStnagard( e_engine::eLogEntry _e ) {
   if ( ! vLogFileOutput_OS.is_open() ) {
      wLOG "Logfile has closed! Trying to reopen it..." END
      vLogFileOutput_OS.open( vLogFielFullPath_str.c_str() );
   }

   if ( ! vLogFileOutput_OS.is_open() ) {
      eLOG "Failed to reopen log file! Disconnect the Slot" END
      vStdLog_eSLOT.disconnectAll();
      return;
   }

   _e.configure( DISABLED,                      // Impossible to schow colors in a Text File
                 WinData.log.logFILE.Time,
                 WinData.log.logFILE.File,
                 WinData.log.logFILE.ErrorType,
                 -1 );

   generateEntry( _e );

   vLogFileOutput_OS << _e.vResultStrin_STR;
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


void eLog::logLoop() {
   if ( vIsLogLoopRunning_B )
      return;

   vIsLogLoopRunning_B = true;
   std::string  lErrorType_STR;
   eLogEntry    lEntry_eLE;
   unsigned int lLogTypeId_uI = 0;
   do {
      // Killing the CPU is not our Task
      B_SLEEP( milliseconds, 25 );


      while ( ! vLogList_L_eLSH.empty() ) {

         for ( unsigned int i = 0; i < 100 && ! vLogList_L_eLSH.front().getIsComplete(); ++i )
            B_SLEEP( milliseconds, 50 );

         // All in all we have 5 seconds to complete a log entry
         if ( ! vLogList_L_eLSH.front().getIsComplete() ) {
            vLogList_L_eLSH.front().add( 'B', 'R', "  <== [eLog] Forgotten '->end();' ?" )->end();
         }


         try {
            lLogTypeId_uI = vLogList_L_eLSH.front().getLogEntry( vLogTypes_V_eLT, lEntry_eLE );
            vLogTypes_V_eLT[lLogTypeId_uI].getSignal()->sendSignal( lEntry_eLE );
         } catch ( ... ) {
            // A new log entry in a crashing log loop fuction would be a bit sensless
            std::cerr << "Received unknown exeption in log Loop -- FILE: " << __FILE__ << " -- LINE: " << __LINE__ << std::endl;
            return;
         }

         vLogList_L_eLSH.pop_front();
      }
   } while ( vLogLoopRun_B );
   vIsLogLoopRunning_B = false;
}

bool eLog::startLogLoop() {
   if ( vIsLogLoopRunning_B == true )
      return false;

   vLogLoopRun_B = true;
   vLogLoopThread_THREAD = boost::thread( &eLog::logLoop, this );

   return true;
}

bool eLog::stopLogLoop() {
   // If stopLogLoop() is immediately called after startLogLoop() it can
   // generate strange errors because vIsLogLoopRunning_B can be false
   // when there was too litle time for the thread to check and set
   // vIsLogLoopRunning_B. 10 milliseconds sleeping schould be enough
   // in this case.
   B_SLEEP( milliseconds, 10 );

   if ( vIsLogLoopRunning_B == false )
      return false;

   vLogLoopRun_B = false;

   vLogLoopThread_THREAD.try_join_for( boost::chrono::seconds( 30 ) );

   if ( vIsLogLoopRunning_B == true ) {
      vLogLoopThread_THREAD.interrupt(); // 1s should be more than enough, so something went wrong
   }

   return true;
}




}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
