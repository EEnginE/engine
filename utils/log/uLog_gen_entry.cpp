/*!
 * \file log.cpp
 * \brief \b Classes: \a uLog (only log entry generation)
 * \sa e_uLog.hpp e_log.cpp
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

#include "eCMDColor.hpp"

#include <regex>
#include <algorithm>


namespace e_engine {

void testLogSize( uLogEntryRaw *data, unsigned int _maxTypeStringLength );

template <class T>
inline std::wstring numToSizeString( T _val, unsigned int _size, wchar_t _fill ) {
   std::wstring lResult_STR = std::to_wstring( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.insert( 0, ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

void testLogSize( uLogEntryRaw *data, unsigned int _maxTypeStringLength ) {
   static const unsigned int lFullTimeSize_cuI    = 19; // 4 + ( 2 * 2 ) + ( 3 * 2 ) + 5
   static const unsigned int lReducedTimeSize_cuI = 8;  //                 ( 3 * 2 ) + 2

   static const unsigned int lFullFileSize_cuI = 20 + 25 + 2 + 4; // Default max filename length +
                                                                  // Default max functionname length
                                                                  // + 2 * ':' + Line (4 chars)
   static const unsigned int lReducedFileSize_cuI = 20;           // Default max filename length

   static const unsigned int lThreadSize_cuI = GlobConf.log.threadNameWidth + 2 + 2;

   unsigned int lTimeSize_uI     = 0;
   unsigned int lTimeSizeNext_uI = 0;

   unsigned int lFileSize_uI     = 0;
   unsigned int lFileSizeNext_uI = 0;

   unsigned int lThreadSize_uI = ( data->data.config.vThread_LPT != OFF ) ? lThreadSize_cuI : 0;

   LOG_PRINT_TYPE lTimeNext_LPT = OFF, lFileNext_LPT = OFF;

   switch ( data->data.config.vTime_LPT ) {
      case LEFT_FULL: lTimeNext_LPT = LEFT_REDUCED; FALLTHROUGH
      case RIGHT_FULL:
         lTimeSize_uI     = lFullTimeSize_cuI;
         lTimeSizeNext_uI = lReducedTimeSize_cuI;
         if ( lTimeNext_LPT != LEFT_REDUCED )
            lTimeNext_LPT = RIGHT_REDUCED;
         break;
      case LEFT_REDUCED: FALLTHROUGH
      case RIGHT_REDUCED:
         lTimeSize_uI  = lReducedTimeSize_cuI;
         lTimeNext_LPT = OFF;
         break;
      case OFF: break; // Only because of -Wswitch
   }

   switch ( data->data.config.vFile_LPT ) {
      case LEFT_FULL: lFileNext_LPT = LEFT_REDUCED; FALLTHROUGH
      case RIGHT_FULL:
         lFileSize_uI     = lFullFileSize_cuI;
         lFileSizeNext_uI = lReducedFileSize_cuI;
         if ( lFileNext_LPT != LEFT_REDUCED )
            lFileNext_LPT = RIGHT_REDUCED;
         break;
      case LEFT_REDUCED:
      case RIGHT_REDUCED:
         lFileSize_uI  = lReducedFileSize_cuI;
         lFileNext_LPT = OFF;
         break;
      case OFF: break; // Only because of -Wswitch
   }

   unsigned int lThisSize = lTimeSize_uI + lFileSize_uI + lThreadSize_uI + _maxTypeStringLength +
                            2 + // '[' and ']'
                            75; // The main Message

   if ( lThisSize > static_cast<unsigned int>( data->data.config.vColumns_I ) ) {

      data->data.config.vThread_LPT = OFF;

      lThisSize = lTimeSizeNext_uI + lFileSizeNext_uI + _maxTypeStringLength + 2 + // '[' and ']'
                  75; // The main Message
      if ( lThisSize > static_cast<unsigned int>( data->data.config.vColumns_I ) ) {
         data->data.config.vTime_LPT = OFF;
         data->data.config.vFile_LPT = OFF;
      } else {
         data->data.config.vTime_LPT = lTimeNext_LPT;
         data->data.config.vFile_LPT = lFileNext_LPT;
      }
   }
}


void uLogEntryRaw::defaultEntryGenerator() {
   std::wstring lDefCol_STR    = L""; // Default color escape sequence string
   std::wstring lResetColl_STR = L""; // Default color reset escape sequence

   std::wstring lTime_str, lFile_str, lErrorType_str, lThread_str;

   if ( data.config.vColor_LCT != DISABLED )
      lResetColl_STR = eCMDColor::RESET;

   if ( data.config.vColor_LCT == FULL && data.raw.vBold_B == false ) {
      lDefCol_STR = eCMDColor::color( 'O', data.raw.vBasicColor_C );
   }

   else if ( ( !( data.config.vColor_LCT == DISABLED ) ) && data.raw.vBold_B == true ) {
      lDefCol_STR = eCMDColor::color( 'B', data.raw.vBasicColor_C );
   }


   // Is there enough space for the log Message (if we take a message string with 100 chars)?
   if ( data.config.vColumns_I > 0 )
      testLogSize( this, LOG.getMaxTypeStingLength() );

   // ========= Generate The Time Entry
   // =============================================================================================

   if ( data.config.vTime_LPT != OFF ) {
      if ( data.config.vColor_LCT == FULL && data.raw.vBold_B == false )
         lTime_str += eCMDColor::color( 'O', GlobConf.log.standardTimeColor );

      else if ( data.config.vColor_LCT == FULL && data.raw.vBold_B == true )
         lTime_str += eCMDColor::color( 'B', GlobConf.log.standardTimeColor );

      else if ( data.config.vColor_LCT == REDUCED )
         lTime_str += eCMDColor::RESET;

      struct tm *ltemp_TM;
      ltemp_TM = std::localtime( &data.raw.vTime_lI );

      if ( data.config.vTime_LPT == LEFT_FULL || data.config.vTime_LPT == RIGHT_FULL ) {
         lTime_str += numToSizeString( ltemp_TM->tm_year + 1900, 4, L'0' ) + L'-' +
                      numToSizeString( ltemp_TM->tm_mon + 1, 2, L'0' ) + L'-' +
                      numToSizeString( ltemp_TM->tm_mday, 2, L'0' ) + L' ';
      }

      lTime_str += numToSizeString( ltemp_TM->tm_hour, 2, L'0' ) + L':' +
                   numToSizeString( ltemp_TM->tm_min, 2, L'0' ) + L':' +
                   numToSizeString( ltemp_TM->tm_sec, 2, L'0' ) + lResetColl_STR;
   }


   // ========= Generate The File Entry
   // =============================================================================================

   if ( data.config.vFile_LPT != OFF ) {
      std::wregex lReplace_EX( L"^(.+[/\\\\])*" );
      const wchar_t *lReplaceChar = L"";

      std::wstring lFilename_STR =
            std::regex_replace( data.raw.vFilename_STR, lReplace_EX, lReplaceChar );

      if ( lFilename_STR.size() > GlobConf.log.maxFilenameSize )
         lFilename_STR.resize( GlobConf.log.maxFilenameSize );

      if ( data.config.vColor_LCT == FULL ||
           ( data.raw.vBold_B == true && data.config.vColor_LCT != DISABLED ) ) {
         lFile_str += eCMDColor::color( data.raw.vBold_B ? eCMDColor::BOLD : eCMDColor::OFF,
                                        eCMDColor::RED );
      }

      lFile_str += lFilename_STR + lResetColl_STR;

      // Make a (yellow) ':' ; add the function name; add a (yellow) ':'; add the line number
      if ( data.config.vFile_LPT == LEFT_FULL || data.config.vFile_LPT == RIGHT_FULL ) {
         if ( data.raw.vFunctionName_STR.size() > GlobConf.log.maxFunctionNameSize )
            data.raw.vFunctionName_STR.resize( GlobConf.log.maxFunctionNameSize );

         if ( data.config.vColor_LCT == FULL ||
              ( data.raw.vBold_B == true && data.config.vColor_LCT != DISABLED ) ) {
            lFile_str += eCMDColor::color( eCMDColor::BOLD, eCMDColor::YELLOW ) + L':' +
                         eCMDColor::color( data.raw.vBold_B ? eCMDColor::BOLD : eCMDColor::OFF,
                                           eCMDColor::CYAN ) +
                         data.raw.vFunctionName_STR +
                         eCMDColor::color( eCMDColor::BOLD, eCMDColor::YELLOW ) + L':' +
                         eCMDColor::color( eCMDColor::BOLD, eCMDColor::GREEN ) +
                         numToSizeString( data.raw.vLine_I,
                                          4,
                                          L'0' ) + // More than 9999 lines of code are very rare
                         lResetColl_STR;
         } else {
            lFile_str += L':' + data.raw.vFunctionName_STR + L':' +
                         numToSizeString( data.raw.vLine_I,
                                          4,
                                          L'0' ); // More than 9999 lines of code are very rare
         }
      }
   }

   // ========= Generate The Error Type Entry
   // =============================================================================================
   if ( data.config.vErrorType_LPT != OFF ) {
      std::wstring lTemp = data.raw.vType_STR;
      std::transform( lTemp.begin(), lTemp.end(), lTemp.begin(), ::toupper );
      lErrorType_str = lDefCol_STR + lTemp + lResetColl_STR;

   } else
      lErrorType_str = L"";


   // ========= Generate The Thread Entry
   // =============================================================================================

   if ( data.config.vThread_LPT != OFF ) {
      std::wstring lTempThread_str = data.raw.vThreadName_STR;

      if ( lTempThread_str.size() > GlobConf.log.threadNameWidth )
         lTempThread_str.resize( GlobConf.log.threadNameWidth );

      if ( data.config.vColor_LCT == FULL ||
           ( data.raw.vBold_B == true && data.config.vColor_LCT != DISABLED ) ) {
         lThread_str = eCMDColor::color( eCMDColor::BOLD, eCMDColor::BLUE ) + L" [" +
                       eCMDColor::color( data.raw.vBold_B ? eCMDColor::BOLD : eCMDColor::OFF,
                                         eCMDColor::YELLOW ) +
                       lTempThread_str + eCMDColor::color( eCMDColor::BOLD, eCMDColor::BLUE ) +
                       L"] ";
      } else {
         lThread_str = L" [" + lTempThread_str + L"] ";
      }

      for ( size_t i = lTempThread_str.size(); i < GlobConf.log.threadNameWidth; ++i )
         lThread_str.append( 1, L' ' );
   }

// ========= Prepare Variables
// ================================================================================================

   std::wregex lRmExcape_REGEX( L"\x1b\\[[0-9;]+m" );
   const wchar_t *lRegexReplace_CSTR = L"";

   std::wstring BR_OPEN  = L"[";
   std::wstring BR_CLOSE = L"]";

   if ( data.config.vColor_LCT == FULL ||
        ( data.config.vColor_LCT == REDUCED && data.raw.vBold_B ) ) {
      BR_OPEN  = eCMDColor::color( eCMDColor::BOLD, eCMDColor::CYAN ) + L'[';
      BR_CLOSE = eCMDColor::color( eCMDColor::BOLD, eCMDColor::CYAN ) + L']' + eCMDColor::RESET;
   }

   std::wstring lL_STR = L"";
   std::wstring lR_STR = L"";

   size_t lErrTypeL_uI;
   size_t lFileL_uI;

   size_t lLeftL_uI;
   size_t lRightL_uI;

   unsigned int lErrorTypeUpdatedStringLength_uI = LOG.getMaxTypeStingLength();

   std::vector<unsigned int> lColorPossitions_uI;
   std::vector<std::wstring> lMessage_VEC;

   unsigned int lMaxFileSize =
         GlobConf.log.maxFilenameSize +
         ( ( data.config.vFile_LPT == LEFT_FULL || data.config.vFile_LPT == RIGHT_FULL )
                 ? 6 + GlobConf.log.maxFunctionNameSize
                 : 0 );

   // Directions: -1-Left ; 0-Off ; 1-Right
   int lErrTypeD_I = 0;
   int lTimeD_I    = 0;
   int lFileD_I    = 0;
   int lThread_I   = 0;

   if ( data.config.vColumns_I < 0 ) {
      lErrTypeD_I = -1;
      lTimeD_I    = -1;
      lFileD_I    = -1;
      lThread_I   = -1;
   } else {
      lErrTypeD_I =
            data.config.vErrorType_LPT == LEFT_FULL || data.config.vErrorType_LPT == LEFT_REDUCED
                  ? -1
                  : 0;
      lErrTypeD_I =
            data.config.vErrorType_LPT == RIGHT_FULL || data.config.vErrorType_LPT == RIGHT_REDUCED
                  ? 1
                  : lErrTypeD_I;

      lTimeD_I =
            data.config.vTime_LPT == LEFT_FULL || data.config.vTime_LPT == LEFT_REDUCED ? -1 : 0;
      lTimeD_I = data.config.vTime_LPT == RIGHT_FULL || data.config.vTime_LPT == RIGHT_REDUCED
                       ? 1
                       : lTimeD_I;

      lFileD_I =
            data.config.vFile_LPT == LEFT_FULL || data.config.vFile_LPT == LEFT_REDUCED ? -1 : 0;
      lFileD_I = data.config.vFile_LPT == RIGHT_FULL || data.config.vFile_LPT == RIGHT_REDUCED
                       ? 1
                       : lFileD_I;

      lThread_I = data.config.vThread_LPT == LEFT_FULL || data.config.vThread_LPT == LEFT_REDUCED
                        ? -1
                        : 0;
      lThread_I = data.config.vThread_LPT == RIGHT_FULL || data.config.vThread_LPT == RIGHT_REDUCED
                        ? 1
                        : lThread_I;
   }

   if ( lFileD_I != 0 ) {
      lFile_str.insert( 0, BR_OPEN );
      lFile_str.append( BR_CLOSE );

      lMaxFileSize += 2;
   }

   if ( lErrTypeD_I == ( lFileD_I * -1 ) ) {
      lErrorType_str.insert( 0, BR_OPEN );
      lErrorType_str.append( BR_CLOSE );

      lErrorTypeUpdatedStringLength_uI += 2;
   }

   // Get Size information (without escape sequences)
   if ( data.config.vColor_LCT != DISABLED ) {
      lErrTypeL_uI =
            std::regex_replace( lErrorType_str, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lFileL_uI = std::regex_replace( lFile_str, lRmExcape_REGEX, lRegexReplace_CSTR ).size();

   } else {
      lErrTypeL_uI = lErrorType_str.size();
      lFileL_uI    = lFile_str.size();
   }



   // ========= Generate Left String
   // =============================================================================================
   if ( lTimeD_I == -1 ) {
      lL_STR += lTime_str;
   }

   if ( lThread_I == -1 ) {
      lL_STR += lThread_str;
   }

   if ( lErrTypeD_I == -1 ) {
      if ( lTimeD_I == -1 ) {
         lL_STR += L' ';
      }
      // Place the string as much in the center as possible
      int lFillBeforeString = static_cast<int>( lErrorTypeUpdatedStringLength_uI / 2 ) -
                              static_cast<int>( lErrTypeL_uI / 2 );
      int lFillAfterString =
            static_cast<int>( lErrorTypeUpdatedStringLength_uI + 2 - lErrTypeL_uI ) -
            lFillBeforeString;
      lL_STR.append( lFillBeforeString < 0 ? 0 : static_cast<size_t>( lFillBeforeString ), ' ' );

      lL_STR += lErrorType_str;
      lL_STR.append( lFillAfterString < 0 ? 0 : static_cast<size_t>( lFillAfterString ), ' ' );
   }


   if ( lFileD_I == -1 ) {
      if ( lErrTypeD_I == -1 ) {
         lL_STR += L' ';
      } else if ( lTimeD_I == -1 ) {
         lL_STR += L' ';
      }
      lL_STR.append( ( ( lMaxFileSize / 2 ) - ( lFileL_uI / 2 ) ), L' ' );
      lL_STR += lFile_str;
      lL_STR.append( lMaxFileSize - ( ( ( lMaxFileSize / 2 ) - ( lFileL_uI / 2 ) ) + lFileL_uI ),
                     L' ' );
   }



   // ========= Generate Right String
   // =============================================================================================
   if ( lTimeD_I == 1 ) {
      lR_STR += lTime_str;
   }

   if ( lErrTypeD_I == 1 ) {
      if ( lTimeD_I == 1 ) {
         lR_STR += ' ';
      }
      // Place the string as much in the center as possible
      unsigned int lFillBeforeString = ( lErrorTypeUpdatedStringLength_uI / 2 ) -
                                       ( static_cast<unsigned int>( lErrTypeL_uI ) / 2 );
      lR_STR.append( lFillBeforeString, ' ' );

      lR_STR += lErrorType_str;
      lR_STR.append( static_cast<size_t>( ( lErrorTypeUpdatedStringLength_uI + 2 ) - lErrTypeL_uI -
                                          lFillBeforeString ),
                     ' ' );
   }

   if ( lThread_I == 1 ) {
      lR_STR += lThread_str;
   }

   if ( lFileD_I == 1 ) {
      if ( lErrTypeD_I == 1 ) {
         lR_STR += L' ';
      } else if ( lTimeD_I == 1 ) {
         lR_STR += L' ';
      }
      lR_STR.append( ( ( lMaxFileSize + 1 ) - lFileL_uI ), L' ' );
      lR_STR += lFile_str;
   }



   // ========= Prepare Variables
   // =============================================================================================
   size_t lMaxMessageSize_uI = std::numeric_limits<size_t>::max();

   if ( data.config.vColor_LCT != DISABLED ) {
      lLeftL_uI  = std::regex_replace( lL_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lRightL_uI = std::regex_replace( lR_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();

   } else {
      lLeftL_uI  = lL_STR.size();
      lRightL_uI = lR_STR.size();
   }

   if ( ( static_cast<int>( data.config.vColumns_I ) - static_cast<int>( lLeftL_uI ) -
          static_cast<int>( lRightL_uI ) ) < 0 &&
        !( data.config.vColumns_I < 0 ) ) {

      lL_STR.clear();
      lR_STR.clear();
      lLeftL_uI          = 0;
      lRightL_uI         = 0;
      lMaxMessageSize_uI = static_cast<size_t>( data.config.vColumns_I );
   }

   if ( data.config.vColumns_I > 0 )
      lMaxMessageSize_uI =
            ( static_cast<size_t>( data.config.vColumns_I ) - lLeftL_uI - lRightL_uI );



   // ========= Generate The Message Strings
   // =============================================================================================
   std::wstring lTempMessageString_STR;
   unsigned int lCurrentStringSize = 0;
   std::wstring lTempColor_STR     = lDefCol_STR;



   // Remove color escape sequences from string
   if ( data.config.vColor_LCT == DISABLED ) {
      data.raw.vDataString_STR =
            std::regex_replace( data.raw.vDataString_STR, lRmExcape_REGEX, lRegexReplace_CSTR );
   }

   lMessage_VEC.emplace_back( lDefCol_STR );

   for ( auto c = data.raw.vDataString_STR.begin(); c != data.raw.vDataString_STR.end(); ++c ) {

      if ( *c == L'\x1B' )
         for ( ; *c != L'm' && c != data.raw.vDataString_STR.end(); ++c )
            lMessage_VEC.back().append( 1, *c );

      // break the string if it is to long
      if ( *c == L'\n' || lCurrentStringSize >= lMaxMessageSize_uI ) {
         lMessage_VEC.emplace_back( lDefCol_STR );
         lCurrentStringSize = 0;
         continue;
      }

      lMessage_VEC.back().append( 1, *c );
      ++lCurrentStringSize;
   }

   if ( !lTempMessageString_STR.empty() )
      lMessage_VEC.push_back( lTempMessageString_STR );


   if ( data.config.vTextOnly_B ) {
      lL_STR.clear();
      lR_STR.clear();

      lL_STR.append( lLeftL_uI, L' ' );
      lR_STR.append( lRightL_uI, L' ' );
   }



   // ========= Put Everything Together
   // =============================================================================================
   for ( unsigned int i = 0; i < lMessage_VEC.size(); ++i ) {
      size_t lTempMessageSize_uI;
      if ( data.config.vColor_LCT != DISABLED ) {
         lTempMessageSize_uI =
               std::regex_replace( lMessage_VEC[i], lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      } else {
         lTempMessageSize_uI = lMessage_VEC[i].size();
      }

#if UNIX
      if ( isatty( fileno( stdout ) ) != 0 && data.config.vColor_LCT != DISABLED ) {
         // Clear the current line
         data.vResultString_STR += L"\x1B[2K\x1b[0G";
      }
#endif // __linux__

      data.vResultString_STR += lL_STR + lMessage_VEC[i];
      if ( data.config.vColumns_I > 0 )
         data.vResultString_STR.append( ( lMaxMessageSize_uI - lTempMessageSize_uI ), L' ' );

      else
         data.vResultString_STR += L"  ";

      data.vResultString_STR += lR_STR + lResetColl_STR + L'\n';

      if ( i == 0 && lMessage_VEC.size() > 1 ) {
/*
 * IMHO, an empty field is the best solution
 */
#if 0
         unsigned lLeftNewL_uI;
         unsigned lRightNewL_uI;

         if( data.config.vColor_LCT != DISABLED ) {
            lLeftNewL_uI  = std::regex_replace( lL_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
            lRightNewL_uI = std::regex_replace( lR_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
         } else {
            lLeftNewL_uI  = lL_STR.size();
            lRightNewL_uI = lR_STR.size();
         }

         if( lLeftNewL_uI > 2 ) {
            lL_STR = BR_OPEN;
            lL_STR.append( lLeftNewL_uI - 2, '-' );
            lL_STR += BR_CLOSE;
            lL_STR.append( lLeftL_uI - lLeftNewL_uI, ' ' );
         }

         if( lRightNewL_uI > 2 ) {
            lR_STR.clear();
            lR_STR.append( lRightL_uI - lRightNewL_uI, ' ' );
            lR_STR += BR_OPEN;
            lR_STR.append( lRightNewL_uI - 2, '-' );
            lR_STR += BR_CLOSE;
         }
#else

         lL_STR.clear();
         lR_STR.clear();

         lL_STR.append( lLeftL_uI, L' ' );
         lR_STR.append( lRightL_uI, L' ' );
#endif
      }
   }
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
