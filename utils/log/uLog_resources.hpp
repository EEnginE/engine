/*!
 * \file uLog_resources.hpp
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

#pragma once

#include "defines.hpp"

#include "uConfig.hpp" // Only for internal::LOG_COLOR_TYPE and internal::LOG_PRINT_TYPE
#include "uLog_converters.hpp"
#include "uSignalSlot.hpp"
#include <condition_variable>
#include <map>
#include <thread>
#include <tuple>


namespace e_engine {

class uLogEntryRaw;

namespace internal {

enum LOG_OBJECT_TYPE { STRING, NEW_LINE, NEW_POINT };


/*!
 * \struct e_engine::internal::uLogType
 * \brief Holds information about a (new) output type for class \c uLog
 *
 * This structure defines all variables which are important
 * for a (new) output type for the class \c uLog
 *
 * \sa uLog
 */
class uLogType {
   typedef uSignal<void, uLogEntryRaw &> _SIGNAL_;

 private:
   char         vType_C; //!< The character witch is associated with color and output mode
   std::wstring vType_STR;
   char         vColor_C; //!< The ID from struct \c eCMDColor for the color which should be used
   bool         vBold_B;


   _SIGNAL_ vSignal_eSIG; //!< \warning The connections will never copy!

   uLogType() {}

 public:
   uLogType( char _type, std::wstring _typeString, char _color, bool _bold )
       : vType_C( _type ), vType_STR( _typeString ), vColor_C( _color ), vBold_B( _bold ) {}

   inline char         getType() const { return vType_C; }
   inline std::wstring getString() const { return vType_STR; }
   inline char         getColor() const { return vColor_C; }
   inline bool         getBold() const { return vBold_B; }

   inline _SIGNAL_ *getSignal() { return &vSignal_eSIG; }

   // void send( uLogEntryStruct _data )   { vSignal_eSIG( _data ); }
};


template <class __A, class... __T>
struct uConverter {
   static void convert( std::wstring &_str, __A &&_toConvert, __T &&... _rest ) {
      uLogConverter<__A>::convert( _str, std::forward<__A>( _toConvert ) );
      uConverter<__T...>::convert( _str, std::forward<__T>( _rest )... );
   }
};

template <class __A>
struct uConverter<__A> {
   static void convert( std::wstring &_str, __A &&_toConvert ) {
      uLogConverter<__A>::convert( _str, std::forward<__A>( _toConvert ) );
   }
};
}

class uLog;

class UTILS_API uLogEntryRaw {
 public:
   struct __DATA__ {
      std::wstring vResultString_STR;

      struct __DATA_RAW__ {
         std::wstring vDataString_STR;
         std::wstring vFilename_STR;
         std::string  vFunctionNameTemp_STR;
         std::wstring vFunctionName_STR;
         std::wstring vThreadName_STR;
         std::wstring vType_STR;
         char         vBasicColor_C;
         bool         vBold_B;
         int          vLine_I;
         std::time_t  vTime_lI;

         __DATA_RAW__( std::wstring &_filename, int &_line, std::string &_funcName )
             : vFilename_STR( _filename ), vFunctionNameTemp_STR( _funcName ), vLine_I( _line ) {

            std::time( &vTime_lI );
         }
      } raw;

      struct __DATA_CONF__ {
         LOG_COLOR_TYPE vColor_LCT;
         LOG_PRINT_TYPE vTime_LPT;
         LOG_PRINT_TYPE vFile_LPT;
         LOG_PRINT_TYPE vErrorType_LPT;
         LOG_PRINT_TYPE vThread_LPT;
         int            vColumns_I;
         uint16_t       vMaxTypeStringLength_usI;
         bool           vTextOnly_B;
         __DATA_CONF__( bool &_onlyText ) : vTextOnly_B( _onlyText ) {}
      } config;

      void configure( e_engine::LOG_COLOR_TYPE _color,
                      e_engine::LOG_PRINT_TYPE _time,
                      e_engine::LOG_PRINT_TYPE _file,
                      e_engine::LOG_PRINT_TYPE _errorType,
                      e_engine::LOG_PRINT_TYPE _thread,
                      int                      _columns );

      __DATA__( std::wstring &_filename, int &_line, std::string &_funcName, bool &_textOnly )
          : raw( _filename, _line, _funcName ), config( _textOnly ) {}
   } data;

 private:
   char            vType_C;
   std::thread::id vThreadId;

   const unsigned int vSize;

 public:
   template <class... ARGS>
   uLogEntryRaw( char              _type,
                 bool              _onlyText,
                 std::wstring &&   _rawFilename,
                 int               _logLine,
                 std::string &&    _functionName,
                 std::thread::id &&_thread,
                 ARGS &&... _args )
       : data( _rawFilename, _logLine, _functionName, _onlyText ),
         vType_C( _type ),
         vThreadId( _thread ),
         vSize( sizeof...( ARGS ) ) {

      internal::uConverter<ARGS...>::convert( data.raw.vDataString_STR,
                                              std::forward<ARGS>( _args )... );
   }

   uLogEntryRaw()                  = delete;
   uLogEntryRaw( uLogEntryRaw & )  = delete;
   uLogEntryRaw( uLogEntryRaw && ) = default;

   ~uLogEntryRaw();

   inline size_t getElementsSize() const { return vSize; }
   unsigned int getLogEntry( std::vector<e_engine::internal::uLogType> &_vLogTypes_V_eLT,
                             std::map<std::thread::id, std::wstring> &_threads );

   void defaultEntryGenerator();
};
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
