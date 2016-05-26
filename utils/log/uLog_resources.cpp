/*!
 * \file log_resources.cpp
 * \brief \b Classes: \a uLogType, \a __uLogStore, \a uLogEntry, \a __uLogStoreHelper
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

#include "defines.hpp"
#include "uLog_resources.hpp"
#include "uLog.hpp"
#include <iomanip>
#include <iostream>

#if UNIX
#define E_COLOR_NO_TERMTEST
#include "eCMDColor.hpp"
#endif

namespace e_engine {

uLogEntryRaw::~uLogEntryRaw() {}

unsigned int uLogEntryRaw::getLogEntry( std::vector<internal::uLogType> &_vLogTypes_V_eLT,
                                        std::map<std::thread::id, std::wstring> &_threads ) {
   data.raw.vType_STR         = L"UNKNOWN";
   data.raw.vThreadName_STR   = L"noname";
   data.raw.vFunctionName_STR = std::wstring( data.raw.vFunctionNameTemp_STR.begin(),
                                              data.raw.vFunctionNameTemp_STR.end() );

   if ( _vLogTypes_V_eLT.empty() ) {
      LOG.devInit();
      eLOG( "No Log type found!! Please add at least one manually or run 'uLog.devInit();', which "
            "will be run now to prevent further Errors" );
   }

   if ( _threads.find( vThreadId ) != _threads.end() )
      if ( !_threads[vThreadId].empty() )
         data.raw.vThreadName_STR = _threads[vThreadId];


   for ( unsigned int i = 0; i < _vLogTypes_V_eLT.size(); ++i ) {
      if ( _vLogTypes_V_eLT[i].getType() == vType_C ) {
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


void uLogEntryRaw::__DATA__::configure( e_engine::LOG_COLOR_TYPE _color,
                                        e_engine::LOG_PRINT_TYPE _time,
                                        e_engine::LOG_PRINT_TYPE _file,
                                        e_engine::LOG_PRINT_TYPE _errorType,
                                        e_engine::LOG_PRINT_TYPE _thread,
                                        int                      _columns ) {
   config.vColor_LCT     = _color;
   config.vTime_LPT      = _time;
   config.vFile_LPT      = _file;
   config.vErrorType_LPT = _errorType;
   config.vColumns_I     = _columns;
   config.vThread_LPT    = _thread;
}
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
