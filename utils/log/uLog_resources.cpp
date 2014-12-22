/*!
 * \file log_resources.cpp
 * \brief \b Classes: \a uLogType, \a __uLogStore, \a uLogEntry, \a __uLogStoreHelper
 */

#include "uLog_resources.hpp"
#include "uLog.hpp"
#include "defines.hpp"
#include <iomanip>

#if UNIX
#define E_COLOR_NO_TERMTEST
#include "eCMDColor.hpp"
#endif

namespace e_engine {


#ifndef LOG_FALLBACK

class Converter : public boost::static_visitor<> {
   public:
      std::wstring theSTR;
      bool   vIsSigned_B;

      void operator()( bool &i ) {
         theSTR += i ? L"true" : L"false";
      }

      void operator()( wchar_t &i ) { // uint8_t == char
         theSTR.append( 1, i );
      }

      void operator()( uint8_t &i ) { // uint8_t == char
         theSTR.append( 1, i );
      }

      void operator()( uint16_t &i ) {
         if( vIsSigned_B )
            theSTR += std::to_wstring( ( int16_t )i );
         else
            theSTR += std::to_wstring( i );
      }

      void operator()( uint32_t &i ) {
         if( vIsSigned_B )
            theSTR += std::to_wstring( ( int32_t )i );
         else
            theSTR += std::to_wstring( i );
      }

      void operator()( uint64_t &i ) {
         if( vIsSigned_B )
            theSTR += std::to_wstring( ( int64_t )i );
         else
            theSTR += std::to_wstring( i );
      }

      void operator()( double &i ) {
         theSTR += std::to_wstring( i );
      }

      void operator()( std::string &i ) {
         theSTR.append( i.begin(), i.end() );
      }

      void operator()( std::wstring &i ) {
         theSTR.append( i );
      }

      void operator()( const char *_str ) {
         for( uint32_t i = 0; _str[i] != 0; ++i )
            theSTR.append( 1, _str[i] );
      }

      void operator()( const wchar_t *_str ) {
         theSTR.append( _str );
      }
};

#endif


void uLogEntryRaw::end() {
   if( !GlobConf.log.waitUntilLogEntryPrinted || !LOG.getIsLogLoopRunning() ) {
      vEndFinished_B = true;
      return;
   }
   {
      std::unique_lock<std::mutex> lLock_BT( vWaitMutex_BT );
      while( ! vIsPrinted_B ) vWaitUntilThisIsPrinted_BT.wait( lLock_BT );
   }
   std::lock_guard<std::mutex> lLockWait_BT( vWaitEndMutex_BT );
   vEndFinished_B = true;
   vWaitUntilEndFinisched_BT.notify_one();
}

void uLogEntryRaw::endLogWaitAndSetPrinted() {
   if( ! GlobConf.log.waitUntilLogEntryPrinted ) {
      vIsPrinted_B = true;
      return;
   }
   {
      std::lock_guard<std::mutex> lLockWait_BT( vWaitMutex_BT );
      vIsPrinted_B = true;
      vWaitUntilThisIsPrinted_BT.notify_one();
   }
   std::unique_lock<std::mutex> lLockWait_BT( vWaitEndMutex_BT );
   while( ! vEndFinished_B ) vWaitUntilEndFinisched_BT.wait( lLockWait_BT );
}

unsigned int uLogEntryRaw::getLogEntry( std::vector< internal::uLogType > &_vLogTypes_V_eLT ) {
   data.raw.vDataString_STR.clear();
   data.raw.vType_STR  = L"UNKNOWN";

#ifdef LOG_FALLBACK
   for (auto & i : vElements) {
	   data.raw.vDataString_STR += i.vData;
   }
#else
   Converter conf;
   for( auto & i : vElements ) {
      conf.vIsSigned_B = i.vIsSigned_B;
      boost::apply_visitor( conf, i.vData );
   }

   data.raw.vDataString_STR = conf.theSTR;
#endif

   if( _vLogTypes_V_eLT.empty() ) {
      eLOG( "No Log type found!! Please add at least one manually or run 'uLog.devInit();', which will be run now to prevent further Errors" );
      LOG.devInit();
   }

   for( unsigned int i = 0; i < _vLogTypes_V_eLT.size(); ++i ) {
      if( _vLogTypes_V_eLT[i].getType() == vType_C ) {
         data.raw.vType_STR     = _vLogTypes_V_eLT[i].getString();
         data.raw.vBasicColor_C = _vLogTypes_V_eLT[i].getColor();
         data.raw.vBold_B       = _vLogTypes_V_eLT[i].getBold();
         return i;
      }
   }

   std::wstring ltemp_STR = L"WARNING!! Log type '";
   ltemp_STR += vType_C;
   ltemp_STR += L"' not Found";
//    vElements.emplace_back( ltemp_STR );

   return 0;
}


void uLogEntryRaw::__DATA__::configure( LOG_COLOR_TYPE _color, LOG_PRINT_TYPE _time, LOG_PRINT_TYPE _file, LOG_PRINT_TYPE _errorType, int _columns ) {
   config.vColor_LCT     = _color;
   config.vTime_LPT      = _time;
   config.vFile_LPT      = _file;
   config.vErrorType_LPT = _errorType;
   config.vColumns_uI    = _columns;
}


}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
