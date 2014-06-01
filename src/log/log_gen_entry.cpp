/*!
 * \file log.cpp
 * \brief \b Classes: \a eLog (only log entry generation)
 * \sa e_log.hpp e_log.cpp
 */

#include "log.hpp"
#include "defines.hpp"

#if UNIX
#define E_COLOR_NO_TERMTEST
#include "color.hpp"
#endif

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>


namespace e_engine {


template<class T>
inline std::wstring numToSizeString( T _val, unsigned int _size, wchar_t _fill ) {
   std::wstring lResult_STR = boost::lexical_cast<std::wstring>( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.insert( 0, ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

void testLogSize( eLogEntry &_rawEntry, unsigned int _maxTypeStringLength ) {
   static const unsigned int lFullTimeSize_cuI    = 19; // 4 + ( 2 * 2 ) + ( 3 * 2 ) + 5
   static const unsigned int lReducedTimeSize_cuI = 8;  //                 ( 3 * 2 ) + 2

   static const unsigned int lFullFileSize_cuI    = 20 + 25 + 2 + 4; // Default max filename length + Default max functionname length + 2 * ':' + Line (4 chars)
   static const unsigned int lReducedFileSize_cuI = 20; // Default max filename length

   unsigned int lTimeSize_uI     = 0;
   unsigned int lTimeSizeNext_uI = 0;

   unsigned int lFileSize_uI     = 0;
   unsigned int lFileSizeNext_uI = 0;

   LOG_PRINT_TYPE lTimeNext_LPT, lFileNext_LPT;

   switch ( _rawEntry.config.vTime_LPT ) {
      case LEFT_FULL:
         lTimeNext_LPT = LEFT_REDUCED;
      case RIGHT_FULL:
         lTimeSize_uI     = lFullTimeSize_cuI;
         lTimeSizeNext_uI = lReducedTimeSize_cuI;
         if ( lTimeNext_LPT != LEFT_REDUCED ) lTimeNext_LPT = RIGHT_REDUCED;
         break;
      case LEFT_REDUCED:
      case RIGHT_REDUCED:
         lTimeSize_uI = lReducedTimeSize_cuI;
         lTimeNext_LPT = OFF;
         break;
      case OFF: break; // Only because of -Wswitch
   }

   switch ( _rawEntry.config.vFile_LPT ) {
      case LEFT_FULL:
         lFileNext_LPT = LEFT_REDUCED;
      case RIGHT_FULL:
         lFileSize_uI     = lFullFileSize_cuI;
         lFileSizeNext_uI = lReducedFileSize_cuI;
         if ( lFileNext_LPT != LEFT_REDUCED ) lFileNext_LPT = RIGHT_REDUCED;
         break;
      case LEFT_REDUCED:
      case RIGHT_REDUCED:
         lFileSize_uI = lReducedFileSize_cuI;
         lFileNext_LPT = OFF;
         break;
      case OFF: break; // Only because of -Wswitch
   }

   unsigned int lThisSize = lTimeSize_uI + lFileSize_uI +
                            _maxTypeStringLength +
                            2 +  // '[' and ']'
                            75; // The main Message

   if ( lThisSize > ( unsigned int )_rawEntry.config.vColumns_uI ) {

      lThisSize = lTimeSizeNext_uI + lFileSizeNext_uI +
                  _maxTypeStringLength +
                  2 +  // '[' and ']'
                  75; // The main Message
      if ( lThisSize > ( unsigned int )_rawEntry.config.vColumns_uI ) {
         _rawEntry.config.vTime_LPT = OFF;
         _rawEntry.config.vFile_LPT = OFF;
      } else {
         _rawEntry.config.vTime_LPT = lTimeNext_LPT;
         _rawEntry.config.vFile_LPT = lFileNext_LPT;
      }
   }
}


void eLog::generateEntry( eLogEntry &_rawEntry ) {
   std::wstring lDefCol1_STR = L"";    // Default color escape sequence string -- Shown  when REDUCED
   std::wstring lDefCol2_STR = L"";    // Default color escape sequence string -- Hidden when REDUCED
   std::wstring lResetColl_STR = L"";  // Default color reset escape sequence

#if  ! UNIX
   _rawEntry.config.vColor_LCT = DISABLED;
#endif

#if UNIX
   if ( _rawEntry.config.vColor_LCT != DISABLED )
      lResetColl_STR = eCMDColor::RESET;

   if ( _rawEntry.config.vColor_LCT == FULL && _rawEntry.data.vBold_B == false ) {
      if ( _rawEntry.data.vNewColor_STR.empty() ) {
         lDefCol2_STR = lDefCol1_STR = eCMDColor::color( eCMDColor::OFF,  _rawEntry.data.vBasicColor_C );
      } else {
         lDefCol2_STR = _rawEntry.data.vNewColor_STR;
         lDefCol1_STR = eCMDColor::color( eCMDColor::OFF,  _rawEntry.data.vBasicColor_C );
      }
   }

   else if ( ( !( _rawEntry.config.vColor_LCT == DISABLED ) ) && _rawEntry.data.vBold_B == true ) {
      if ( _rawEntry.data.vNewColor_STR.empty() ) {
         lDefCol2_STR = lDefCol1_STR = eCMDColor::color( eCMDColor::BOLD, _rawEntry.data.vBasicColor_C );
      } else {
         lDefCol2_STR = _rawEntry.data.vNewColor_STR;
         lDefCol1_STR = eCMDColor::color( eCMDColor::OFF,  _rawEntry.data.vBasicColor_C );
      }
   }
#endif



   // Is there enough space for the log Message (if we take a message string with 100 chars)?
   if ( _rawEntry.config.vColumns_uI > 0 )
      testLogSize( _rawEntry, vMaxTypeStringLength_usI );

// ========= Generate The Time Entry ==============================================================================================================

   if ( _rawEntry.config.vTime_LPT != OFF ) {
#if UNIX
      if ( _rawEntry.config.vColor_LCT == FULL && _rawEntry.data.vBold_B == false )
         _rawEntry.temp.vTime_STR += eCMDColor::color( eCMDColor::OFF,  WinData.log.standardTimeColor );

      else if ( _rawEntry.config.vColor_LCT == FULL && _rawEntry.data.vBold_B == true )
         _rawEntry.temp.vTime_STR += eCMDColor::color( eCMDColor::BOLD, WinData.log.standardTimeColor );

      else if ( _rawEntry.config.vColor_LCT == REDUCED )
         _rawEntry.temp.vTime_STR += eCMDColor::RESET;
#endif

      struct tm *ltemp_TM;
      ltemp_TM = std::localtime( &_rawEntry.data.vTime_lI );

      if ( _rawEntry.config.vTime_LPT == LEFT_FULL || _rawEntry.config.vTime_LPT == RIGHT_FULL ) {
         _rawEntry.temp.vTime_STR += numToSizeString( ltemp_TM->tm_year + 1900, 4, L'0' ) + L'-' +
                                     numToSizeString( ltemp_TM->tm_mon  + 1,    2, L'0' ) + L'-' +
                                     numToSizeString( ltemp_TM->tm_mday,        2, L'0' ) + L' ';
      }

      _rawEntry.temp.vTime_STR += numToSizeString( ltemp_TM->tm_hour, 2, L'0' ) + L':' +
                                  numToSizeString( ltemp_TM->tm_min,  2, L'0' ) + L':' +
                                  numToSizeString( ltemp_TM->tm_sec,  2, L'0' ) + lResetColl_STR;
   }


// ========= Generate The File Entry ==============================================================================================================

   if ( _rawEntry.config.vFile_LPT != OFF ) {
#if defined UGLY_WINE_WORKAROUND
      boost::regex lReplace_EX( "^(/)?(.+/)*" );
      const char  *lReplaceChar = "";

      std::string  lTempFilename_STR = boost::to_upper_copy( boost::regex_replace(
                                          std::string( _rawEntry.data.vFilename_STR.begin(), _rawEntry.data.vFilename_STR.end() ),
                                          lReplace_EX,
                                          lReplaceChar
                                       ) );
      std::wstring lFilename_STR = std::wstring( lTempFilename_STR.begin(), lTempFilename_STR.end() );
#else
      boost::wregex lReplace_EX( L"^(/)?(.+/)*" );
      const wchar_t  *lReplaceChar = L"";

      std::wstring lFilename_STR = boost::to_upper_copy( boost::regex_replace( _rawEntry.data.vFilename_STR, lReplace_EX, lReplaceChar ) );
#endif

      if ( lFilename_STR.size() > WinData.log.maxFilenameSize )
         lFilename_STR.resize( WinData.log.maxFilenameSize );

#if UNIX
      if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.data.vBold_B == true && _rawEntry.config.vColor_LCT != DISABLED ) ) {
         _rawEntry.temp.vFile_STR += eCMDColor::color( _rawEntry.data.vBold_B ? eCMDColor::BOLD : eCMDColor::OFF, eCMDColor::RED );
      }
#endif

      _rawEntry.temp.vFile_STR += lFilename_STR +
                                  lResetColl_STR;

      // Make a (yellow) ':' ; add the function name; add a (yellow) ':'; add the line number
      if ( _rawEntry.config.vFile_LPT == LEFT_FULL || _rawEntry.config.vFile_LPT == RIGHT_FULL ) {
         if ( _rawEntry.data.vFunctionName_STR.size() > WinData.log.maxFunctionNameSize )
            _rawEntry.data.vFunctionName_STR.resize( WinData.log.maxFunctionNameSize );

#if UNIX
         if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.data.vBold_B == true && _rawEntry.config.vColor_LCT != DISABLED ) ) {
            _rawEntry.temp.vFile_STR += eCMDColor::color( eCMDColor::BOLD, eCMDColor::YELLOW ) +
                                        L':' +
                                        eCMDColor::color( _rawEntry.data.vBold_B ? eCMDColor::BOLD : eCMDColor::OFF, eCMDColor::CYAN ) +
                                        _rawEntry.data.vFunctionName_STR +
                                        eCMDColor::color( eCMDColor::BOLD, eCMDColor::YELLOW ) +
                                        L':' +
                                        eCMDColor::color( eCMDColor::BOLD, eCMDColor::GREEN ) +
                                        numToSizeString( _rawEntry.data.vLine_I, 4, L'0' ) +      // More than 9999 lines of code are very rare
                                        lResetColl_STR;
         } else {
            _rawEntry.temp.vFile_STR +=
               L':' +
               _rawEntry.data.vFunctionName_STR +
               L':' +
               numToSizeString( _rawEntry.data.vLine_I, 4, L'0' ); // More than 9999 lines of code are very rare
         }
#else
         _rawEntry.temp.vFile_STR +=
            L':' +
            _rawEntry.data.vFunctionName_STR +
            L':' +
            numToSizeString( _rawEntry.data.vLine_I, 4, L'0' ); // More than 9999 lines of code are very rare
#endif
      }


   }

// ========= Generate The Error Type Entry ========================================================================================================
   if ( _rawEntry.config.vErrorType_LPT != OFF ) {
      _rawEntry.temp.vErrorType_STR = lDefCol1_STR +
                                      boost::to_upper_copy( _rawEntry.data.vType_STR ) +
                                      lResetColl_STR;


   } else _rawEntry.temp.vErrorType_STR = L"";


// ========= Prepare Variables ====================================================================================================================

#if defined UGLY_WINE_WORKAROUND
   boost::regex lRmExcape_REGEX( "\x1b\\[[0-9;]+m" );
   const char  *lRegexReplace_CSTR = "";
#else
   boost::wregex lRmExcape_REGEX( L"\x1b\\[[0-9;]+m" );
   const wchar_t *lRegexReplace_CSTR = L"";
#endif

   std::wstring BR_OPEN  = L"[";
   std::wstring BR_CLOSE = L"]";

#if UNIX
   if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.config.vColor_LCT == REDUCED && _rawEntry.data.vBold_B ) ) {
      BR_OPEN  = eCMDColor::color( eCMDColor::BOLD, eCMDColor::CYAN ) + L'[';
      BR_CLOSE = eCMDColor::color( eCMDColor::BOLD, eCMDColor::CYAN ) + L']' + eCMDColor::RESET;
   }
#endif

   std::wstring lL_STR = L"";
   std::wstring lR_STR = L"";

   unsigned int lErrTypeL_uI;
   unsigned int lFileL_uI;

   unsigned int lLeftL_uI;
   unsigned int lRightL_uI;

   unsigned int lErrorTypeUpdatedStringLength_uI = vMaxTypeStringLength_usI;

   std::vector<unsigned int> lColorPossitions_uI;
   std::vector<std::wstring> lMessage_VEC;

   unsigned int lMaxFileSize  = WinData.log.maxFilenameSize +
                                ( ( _rawEntry.config.vFile_LPT == LEFT_FULL || _rawEntry.config.vFile_LPT == RIGHT_FULL )
                                  ? 6 + WinData.log.maxFunctionNameSize : 0 );

   // Directions: -1-Left ; 0-Off ; 1-Right
   int lErrTypeD_I = 0;
   int lTimeD_I    = 0;
   int lFileD_I    = 0;

   if ( _rawEntry.config.vColumns_uI < 0 ) {
      lErrTypeD_I = -1;
      lTimeD_I    = -1;
      lFileD_I    = -1;
   } else {
      lErrTypeD_I = _rawEntry.config.vErrorType_LPT == LEFT_FULL  || _rawEntry.config.vErrorType_LPT == LEFT_REDUCED   ? -1 : 0;
      lErrTypeD_I = _rawEntry.config.vErrorType_LPT == RIGHT_FULL || _rawEntry.config.vErrorType_LPT == RIGHT_REDUCED  ?  1 : lErrTypeD_I;

      lTimeD_I    = _rawEntry.config.vTime_LPT      == LEFT_FULL  || _rawEntry.config.vTime_LPT      == LEFT_REDUCED   ? -1 : 0;
      lTimeD_I    = _rawEntry.config.vTime_LPT      == RIGHT_FULL || _rawEntry.config.vTime_LPT      == RIGHT_REDUCED  ?  1 : lTimeD_I;

      lFileD_I    = _rawEntry.config.vFile_LPT      == LEFT_FULL  || _rawEntry.config.vFile_LPT      == LEFT_REDUCED   ? -1 : 0;
      lFileD_I    = _rawEntry.config.vFile_LPT      == RIGHT_FULL || _rawEntry.config.vFile_LPT      == RIGHT_REDUCED  ?  1 : lFileD_I;
   }

   if ( lFileD_I != 0 ) {
      _rawEntry.temp.vFile_STR.insert( 0, BR_OPEN );
      _rawEntry.temp.vFile_STR.append( BR_CLOSE );

      lMaxFileSize += 2;
   }

   if ( lErrTypeD_I == ( lFileD_I * -1 ) ) {
      _rawEntry.temp.vErrorType_STR.insert( 0, BR_OPEN );
      _rawEntry.temp.vErrorType_STR.append( BR_CLOSE );

      lErrorTypeUpdatedStringLength_uI += 2;
   }

   // Get Size information (without escape sequences)
   if ( _rawEntry.config.vColor_LCT != DISABLED ) {
#if defined UGLY_WINE_WORKAROUND
      lErrTypeL_uI = boost::regex_replace( std::string( _rawEntry.temp.vErrorType_STR.begin(), _rawEntry.temp.vErrorType_STR.end() ), lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lFileL_uI    = boost::regex_replace( std::string( _rawEntry.temp.vFile_STR.begin(),      _rawEntry.temp.vFile_STR.end() ),      lRmExcape_REGEX, lRegexReplace_CSTR ).size();
#else
      lErrTypeL_uI = boost::regex_replace( _rawEntry.temp.vErrorType_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lFileL_uI    = boost::regex_replace( _rawEntry.temp.vFile_STR,      lRmExcape_REGEX, lRegexReplace_CSTR ).size();
#endif
   } else {
      lErrTypeL_uI = _rawEntry.temp.vErrorType_STR.size();
      lFileL_uI    = _rawEntry.temp.vFile_STR.size();
   }



// ========= Generate Left String =================================================================================================================
   if ( lTimeD_I == -1 ) {
      lL_STR += _rawEntry.temp.vTime_STR;
   }

   if ( lErrTypeD_I == -1 ) {
      if ( lTimeD_I == -1 ) {
         lL_STR += L' ';
      }
      // Place the string as much in the center as possible
      unsigned int lFillBeforeString = ( lErrorTypeUpdatedStringLength_uI / 2 ) - ( lErrTypeL_uI / 2 );
      lL_STR.append( lFillBeforeString, ' ' );

      lL_STR += _rawEntry.temp.vErrorType_STR;
      lL_STR.append( ( unsigned int )( ( lErrorTypeUpdatedStringLength_uI + 2 ) - lErrTypeL_uI - lFillBeforeString ), ( char )' ' );
   }

   if ( lFileD_I == -1 ) {
      if ( lErrTypeD_I == -1 ) {
         lL_STR += L' ';
      } else if ( lTimeD_I == -1 ) {
         lL_STR += L' ';
      }
      lL_STR.append( ( ( lMaxFileSize / 2 ) - ( lFileL_uI / 2 ) ) , L' ' );
      lL_STR += _rawEntry.temp.vFile_STR;
      lL_STR.append( lMaxFileSize - ( ( ( lMaxFileSize / 2 ) - ( lFileL_uI / 2 ) ) + lFileL_uI ) , L' ' );
   }



// ========= Generate Right String ================================================================================================================
   if ( lTimeD_I == 1 ) {
      lR_STR += _rawEntry.temp.vTime_STR;
   }

   if ( lErrTypeD_I == 1 ) {
      if ( lTimeD_I == 1 ) {
         lR_STR += ' ';
      }
      // Place the string as much in the center as possible
      unsigned int lFillBeforeString = ( lErrorTypeUpdatedStringLength_uI / 2 ) - ( lErrTypeL_uI / 2 );
      lR_STR.append( lFillBeforeString, ' ' );

      lR_STR += _rawEntry.temp.vErrorType_STR;
      lR_STR.append( ( unsigned int )( ( lErrorTypeUpdatedStringLength_uI + 2 ) - lErrTypeL_uI - lFillBeforeString ), ( char )' ' );
   }

   if ( lFileD_I == 1 ) {
      if ( lErrTypeD_I == 1 ) {
         lR_STR += L' ';
      } else if ( lTimeD_I == 1 ) {
         lR_STR += L' ';
      }
      lR_STR.append( ( ( lMaxFileSize + 1 ) - lFileL_uI ) , L' ' );
      lR_STR += _rawEntry.temp.vFile_STR;
   }



// ========= Prepare Variables ====================================================================================================================
   unsigned int lMaxMessageSize_uI = 100000;

   if ( _rawEntry.config.vColor_LCT != DISABLED ) {
#if defined UGLY_WINE_WORKAROUND
      lLeftL_uI  = boost::regex_replace( std::string( lL_STR.begin(), lL_STR.end() ), lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lRightL_uI = boost::regex_replace( std::string( lR_STR.begin(), lR_STR.end() ), lRmExcape_REGEX, lRegexReplace_CSTR ).size();
#else
      lLeftL_uI  = boost::regex_replace( lL_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lRightL_uI = boost::regex_replace( lR_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
#endif
   } else {
      lLeftL_uI  = lL_STR.size();
      lRightL_uI = lR_STR.size();
   }

   if ( ( ( int )_rawEntry.config.vColumns_uI - ( int )lLeftL_uI - ( int )lRightL_uI ) < 0 &&
         !( _rawEntry.config.vColumns_uI < 0 ) ) {

      lL_STR.clear();
      lR_STR.clear();
      lLeftL_uI  = 0;
      lRightL_uI = 0;
      lMaxMessageSize_uI = _rawEntry.config.vColumns_uI;
   }

   if ( _rawEntry.config.vColumns_uI > 0 )
      lMaxMessageSize_uI = ( _rawEntry.config.vColumns_uI - lLeftL_uI - lRightL_uI );



// ========= Generate The Message Strings =========================================================================================================
   std::wstring  lTempMessageString_STR;
   unsigned int lOldSize_uI = 0;
   std::wstring  lTempColor_STR = lDefCol2_STR;

   for ( unsigned int i = 0; i < _rawEntry.data.vLogEntries_V_eLS.size(); ++i ) {
      std::wstring lTemp_STR = _rawEntry.data.vLogEntries_V_eLS[i].getString();
      std::wstring lColorTemp_STR = L"";

      if ( _rawEntry.data.vLogEntries_V_eLS[i].getType() == e_engine_internal::NEW_LINE ) {
         lMessage_VEC.push_back( ( lTempMessageString_STR + lResetColl_STR ) );
         lTempMessageString_STR.clear();
         lOldSize_uI = 0;
         lTempColor_STR = lDefCol2_STR;
         continue;
      }

      if ( _rawEntry.data.vLogEntries_V_eLS[i].getType() == e_engine_internal::NEW_POINT ) {
         lMessage_VEC.push_back( ( lTempMessageString_STR + lResetColl_STR ) );

#if UNIX
         if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.config.vColor_LCT == REDUCED && _rawEntry.data.vBold_B ) )
            lTempColor_STR = eCMDColor::color( eCMDColor::OFF, eCMDColor::CYAN );
#endif

         lTempMessageString_STR = L"  " +
                                  lTempColor_STR +
                                  L"- ";
         lOldSize_uI = 4;
         continue;
      }

      if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.config.vColor_LCT == REDUCED && _rawEntry.data.vBold_B ) ) {
         if ( _rawEntry.data.vLogEntries_V_eLS[i].hasColor() ) {
            lColorTemp_STR = _rawEntry.data.vLogEntries_V_eLS[i].getColorString();
         } else {
            lColorTemp_STR = lTempColor_STR;
         }
      }

      if ( ( lOldSize_uI + lTemp_STR.size() ) <= lMaxMessageSize_uI ) {

         lTempMessageString_STR += lColorTemp_STR +
                                   lTemp_STR +
                                   lTempColor_STR;

         lOldSize_uI += lTemp_STR.size();
      } else {
         unsigned int lCount_uI = 1;
         unsigned int lStart_uI = 0;
         for ( ; ( int )( ( lMaxMessageSize_uI * lCount_uI ) - lOldSize_uI - lTemp_STR.size() ) < 0; ++lCount_uI ) {
            lTempMessageString_STR += lColorTemp_STR;
            lTempMessageString_STR.append( lTemp_STR, ( lCount_uI - 1 ) * lMaxMessageSize_uI , lMaxMessageSize_uI - lOldSize_uI );
            lTempMessageString_STR += lResetColl_STR;

            lStart_uI += lMaxMessageSize_uI - lOldSize_uI;

            lMessage_VEC.push_back( lTempMessageString_STR );
            lTempColor_STR = lDefCol2_STR;

            lTempMessageString_STR.clear();
            lOldSize_uI = 0;
         }
         lTempMessageString_STR += lColorTemp_STR;
         lTempMessageString_STR.append( lTemp_STR, lStart_uI, lTemp_STR.size() );
         lTempMessageString_STR += lTempColor_STR;
         lOldSize_uI = lTemp_STR.size() - lStart_uI;
      }
   }

   if ( ! lTempMessageString_STR.empty() )
      lMessage_VEC.push_back( lTempMessageString_STR );



// ========= Put Everything Together ==============================================================================================================
   for ( unsigned int i = 0; i < lMessage_VEC.size(); ++i ) {
      unsigned int lTempMessageSize_uI;
      if ( _rawEntry.config.vColor_LCT != DISABLED )
#if defined UGLY_WINE_WORKAROUND
         lTempMessageSize_uI = boost::regex_replace( std::string( lMessage_VEC[i].begin(), lMessage_VEC[i].end() ), lRmExcape_REGEX, lRegexReplace_CSTR ).size();
#else
         lTempMessageSize_uI = boost::regex_replace( lMessage_VEC[i], lRmExcape_REGEX, lRegexReplace_CSTR ).size();
#endif
      else
         lTempMessageSize_uI = lMessage_VEC[i].size();

#if UNIX
      if ( isatty( fileno( stdout ) ) != 0 && _rawEntry.config.vColor_LCT != DISABLED ) {
         // Clear the current line
         _rawEntry.vResultStrin_STR += L"\x1B[2K\x1b[0G";
      }
#endif // __linux__

      _rawEntry.vResultStrin_STR += lL_STR + lMessage_VEC[i];
      if ( _rawEntry.config.vColumns_uI > 0 )
         _rawEntry.vResultStrin_STR.append( ( lMaxMessageSize_uI - lTempMessageSize_uI ) , L' ' );

      else
         _rawEntry.vResultStrin_STR += L"  ";

      _rawEntry.vResultStrin_STR += lR_STR + lResetColl_STR + L'\n';

      if ( i == 0 && lMessage_VEC.size() > 1 ) {
         /*
          * IMHO, an empty field is the best solution
          * /
         boost::trim( lL_STR );
         boost::trim( lR_STR );

         GLuint lLeftNewL_uI;
         GLuint lRightNewL_uI;

         if ( _rawEntry.config.vColor_LCT != DISABLED ) {
         #if defined UGLY_WINE_WORKAROUND
            lLeftNewL_uI  = boost::regex_replace( std::string( lL_STR.begin(), lL_STR.end() ), lRmExcape_REGEX, lRegexReplace_CSTR ).size();
            lRightNewL_uI = boost::regex_replace( std::string( lR_STR.begin(), lR_STR.end() ), lRmExcape_REGEX, lRegexReplace_CSTR ).size();
         #else
            lLeftNewL_uI  = boost::regex_replace( lL_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
            lRightNewL_uI = boost::regex_replace( lR_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
         #endif
         } else {
            lLeftNewL_uI  = lL_STR.size();
            lRightNewL_uI = lR_STR.size();
         }

         if ( lLeftNewL_uI > 2 ) {
            lL_STR = BR_OPEN;
            lL_STR.append( lLeftNewL_uI - 2, '-' );
            lL_STR += BR_CLOSE;
            lL_STR.append( lLeftL_uI - lLeftNewL_uI, ' ' );
         }

         if ( lRightNewL_uI > 2 ) {
            lR_STR.clear();
            lR_STR.append( lRightL_uI - lRightNewL_uI, ' ' );
            lR_STR += BR_OPEN;
            lR_STR.append( lRightNewL_uI - 2, '-' );
            lR_STR += BR_CLOSE;
         }
         // */

         lL_STR.clear();
         lR_STR.clear();

         lL_STR.append( lLeftL_uI,  L' ' );
         lR_STR.append( lRightL_uI, L' ' );
         // */


      }
   }
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
