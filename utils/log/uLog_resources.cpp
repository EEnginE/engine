/*!
 * \file log_resources.cpp
 * \brief \b Classes: \a uLogType, \a __uLogStore, \a uLogEntry, \a __uLogStoreHelper
 */

#include "uLog_resources.hpp"
#include "uLog.hpp"
#include "defines.hpp"
#include <iomanip>
#include <iostream>

#if UNIX
#define E_COLOR_NO_TERMTEST
#include "eCMDColor.hpp"
#endif

namespace e_engine {


uLogEntryRaw::~uLogEntryRaw() {
   if( vElements )
      delete vElements;
}


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

   data.raw.vDataString_STR += vElements->get();

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


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
