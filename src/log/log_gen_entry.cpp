/*!
 * \file log.cpp
 * \brief \b Classes: \a eLog (only log entry generation)
 * \sa e_log.hpp e_log.cpp
 */

#include "log.hpp"

#define E_COLOR_NO_TERMTEST
#include "color.hpp"

#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>


namespace e_engine {


template<class T>
inline std::string numToSizeString( T _val, unsigned int _size, char _fill ) {
   std::string lResult_STR = boost::lexical_cast<std::string>( _val );
   if ( _size > lResult_STR.size() )
      lResult_STR.insert( 0, ( _size - lResult_STR.size() ), _fill );
   return lResult_STR;
}

void testLogSize( eLogEntry &_rawEntry, unsigned int _maxTypeStringLength ) {
   static const unsigned int lFullTimeSize_cuI    = 19; // 4 + ( 2 * 2 ) + ( 3 * 2 ) + 5
   static const unsigned int lReducedTimeSize_cuI = 8;  //                 ( 3 * 2 ) + 2

   static const unsigned int lFullFileSize_cuI    = 25; // Default max filename length + ':' + Line (4 chars)
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
   std::string lDefCol1_STR = "";    // Default color escape sequence string -- Shown  when REDUCED
   std::string lDefCol2_STR = "";    // Default color escape sequence string -- Hidden when REDUCED
   std::string lResetColl_STR = "";  // Default color reset escape sequence

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

   else if ( ( ! _rawEntry.config.vColor_LCT == DISABLED ) && _rawEntry.data.vBold_B == true ) {
      if ( _rawEntry.data.vNewColor_STR.empty() ) {
         lDefCol2_STR = lDefCol1_STR = eCMDColor::color( eCMDColor::BOLD, _rawEntry.data.vBasicColor_C );
      } else {
         lDefCol2_STR = _rawEntry.data.vNewColor_STR;
         lDefCol1_STR = eCMDColor::color( eCMDColor::OFF,  _rawEntry.data.vBasicColor_C );
      }
   }



   // Is there enough space for the log Message (if we take a message string with 100 chars)?
   if ( _rawEntry.config.vColumns_uI > 0 )
      testLogSize( _rawEntry, vMaxTypeStringLength_usI );

// ========= Generate The Time Entry ==============================================================================================================

   if ( _rawEntry.config.vTime_LPT != OFF ) {
      if ( _rawEntry.config.vColor_LCT == FULL && _rawEntry.data.vBold_B == false )
         _rawEntry.temp.vTime_STR += eCMDColor::color( eCMDColor::OFF,  WinData.log.standardTimeColor );

      else if ( _rawEntry.config.vColor_LCT == FULL && _rawEntry.data.vBold_B == true )
         _rawEntry.temp.vTime_STR += eCMDColor::color( eCMDColor::BOLD, WinData.log.standardTimeColor );

      else if ( _rawEntry.config.vColor_LCT == REDUCED )
         _rawEntry.temp.vTime_STR += eCMDColor::RESET;

      struct tm *ltemp_TM;
      ltemp_TM = std::localtime( &_rawEntry.data.vTime_lI );

      if ( _rawEntry.config.vTime_LPT == LEFT_FULL || _rawEntry.config.vTime_LPT == RIGHT_FULL ) {
         _rawEntry.temp.vTime_STR += numToSizeString( ltemp_TM->tm_year + 1900, 4, '0' ) + '-' +
                                     numToSizeString( ltemp_TM->tm_mon  + 1,    2, '0' ) + '-' +
                                     numToSizeString( ltemp_TM->tm_mday,        2, '0' ) + ' ';
      }

      _rawEntry.temp.vTime_STR += numToSizeString( ltemp_TM->tm_hour, 2, '0' ) + ':' +
                                  numToSizeString( ltemp_TM->tm_min,  2, '0' ) + ':' +
                                  numToSizeString( ltemp_TM->tm_sec,  2, '0' ) + lResetColl_STR;
   }


// ========= Generate The File Entry ==============================================================================================================

   if ( _rawEntry.config.vFile_LPT != OFF ) {
      boost::regex lReplace_EX( "^(/)?(.+/)*" );
      const char  *lReplaceChar = "";

      std::string lFilename_STR = boost::to_upper_copy( boost::regex_replace( _rawEntry.data.vFilename_STR, lReplace_EX, lReplaceChar ) );

      if ( lFilename_STR.size() > WinData.log.maxFilenameSize ) {
         lFilename_STR.resize( WinData.log.maxFilenameSize );
      }

      if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.data.vBold_B == true && _rawEntry.config.vColor_LCT != DISABLED ) ) {
         _rawEntry.temp.vFile_STR += eCMDColor::color( _rawEntry.data.vBold_B ? eCMDColor::BOLD : eCMDColor::OFF, eCMDColor::RED );
      }

      _rawEntry.temp.vFile_STR += lFilename_STR +
                                  lResetColl_STR;

      // Make a (yellow) ':' (and set a color)
      if ( _rawEntry.config.vFile_LPT == LEFT_FULL || _rawEntry.config.vFile_LPT == RIGHT_FULL ) {
         if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.data.vBold_B == true && _rawEntry.config.vColor_LCT != DISABLED ) ) {
            _rawEntry.temp.vFile_STR += eCMDColor::color( eCMDColor::BOLD, eCMDColor::MAGENTA ) +
                                        ':' +
                                        eCMDColor::color( eCMDColor::BOLD, eCMDColor::GREEN ) +
                                        numToSizeString( _rawEntry.data.vLine_I, 4, '0' ) +      // More than 9999 lines of code are very rare
                                        lResetColl_STR;
         } else {
            _rawEntry.temp.vFile_STR += ':' + numToSizeString( _rawEntry.data.vLine_I, 4, '0' ); // More than 9999 lines of code are very rare
         }
      }

   }

// ========= Generate The Error Type Entry ========================================================================================================
   if ( _rawEntry.config.vErrorType_LPT != OFF ) {
      _rawEntry.temp.vErrorType_STR = lDefCol1_STR +
                                      boost::to_upper_copy( _rawEntry.data.vType_STR ) +
                                      lResetColl_STR;


   } else _rawEntry.temp.vErrorType_STR = "";


// ========= Prepare Variables ====================================================================================================================
   boost::regex lRmExcape_REGEX( "\x1b\\[[0-9;]+m" );
   const GLchar *lRegexReplace_CSTR = "";

   std::string BR_OPEN  = "[";
   std::string BR_CLOSE = "]";

   if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.config.vColor_LCT == REDUCED && _rawEntry.data.vBold_B ) ) {
      BR_OPEN  = eCMDColor::color( eCMDColor::BOLD, eCMDColor::CYAN ) + '[';
      BR_CLOSE = eCMDColor::color( eCMDColor::BOLD, eCMDColor::CYAN ) + ']' + eCMDColor::RESET;
   }

   std::string lL_STR = "";
   std::string lR_STR = "";

   GLuint lErrTypeL_uI;
   GLuint lFileL_uI;

   GLuint lLeftL_uI;
   GLuint lRightL_uI;

   GLuint lErrorTypeUpdatedStringLength_uI = vMaxTypeStringLength_usI;

   std::vector<GLuint> lColorPossitions_uI;
   std::vector<std::string> lMessage_VEC;

   GLuint lMaxFileSize  = WinData.log.maxFilenameSize +
                          ( ( _rawEntry.config.vFile_LPT == LEFT_FULL || _rawEntry.config.vFile_LPT == RIGHT_FULL ) ? 5 : 0 );

   // Directions: -1-Left ; 0-Off ; 1-Right
   GLint lErrTypeD_I = 0;
   GLint lTimeD_I    = 0;
   GLint lFileD_I    = 0;

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
      lErrTypeL_uI = boost::regex_replace( _rawEntry.temp.vErrorType_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lFileL_uI    = boost::regex_replace( _rawEntry.temp.vFile_STR,      lRmExcape_REGEX, lRegexReplace_CSTR ).size();
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
         lL_STR += ' ';
      }
      // Place the string as much in the center as possible
      GLuint lFillBeforeString = ( lErrorTypeUpdatedStringLength_uI / 2 ) - ( lErrTypeL_uI / 2 );
      lL_STR.append( lFillBeforeString, ' ' );

      lL_STR += _rawEntry.temp.vErrorType_STR;
      lL_STR.append( ( unsigned int )( ( lErrorTypeUpdatedStringLength_uI + 2 ) - lErrTypeL_uI - lFillBeforeString ), ( char )' ' );
   }

   if ( lFileD_I == -1 ) {
      if ( lErrTypeD_I == -1 ) {
         lL_STR += ' ';
      } else if ( lTimeD_I == -1 ) {
         lL_STR += ' ';
      }
      lL_STR.append( ( ( lMaxFileSize / 2 ) - ( lFileL_uI / 2 ) ) , ' ' );
      lL_STR += _rawEntry.temp.vFile_STR;
      lL_STR.append( lMaxFileSize - ( ( ( lMaxFileSize / 2 ) - ( lFileL_uI / 2 ) ) + lFileL_uI ) , ' ' );
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
      GLuint lFillBeforeString = ( lErrorTypeUpdatedStringLength_uI / 2 ) - ( lErrTypeL_uI / 2 );
      lR_STR.append( lFillBeforeString, ' ' );

      lR_STR += _rawEntry.temp.vErrorType_STR;
      lR_STR.append( ( unsigned int )( ( lErrorTypeUpdatedStringLength_uI + 2 ) - lErrTypeL_uI - lFillBeforeString ), ( char )' ' );
   }

   if ( lFileD_I == 1 ) {
      if ( lErrTypeD_I == 1 ) {
         lR_STR += ' ';
      } else if ( lTimeD_I == 1 ) {
         lR_STR += ' ';
      }
      lR_STR.append( ( ( lMaxFileSize + 1 ) - lFileL_uI ) , ' ' );
      lR_STR += _rawEntry.temp.vFile_STR;
   }



// ========= Prepare Variables ====================================================================================================================
   GLuint lMaxMessageSize_uI = 100000;

   if ( _rawEntry.config.vColor_LCT != DISABLED ) {
      lLeftL_uI  = boost::regex_replace( lL_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      lRightL_uI = boost::regex_replace( lR_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
   } else {
      lLeftL_uI  = lL_STR.size();
      lRightL_uI = lR_STR.size();
   }

   if ( ( ( GLint )_rawEntry.config.vColumns_uI - ( GLint )lLeftL_uI - ( GLint )lRightL_uI ) < 0 &&
         ! _rawEntry.config.vColumns_uI < 0 ) {

      lL_STR.clear();
      lR_STR.clear();
      lLeftL_uI  = 0;
      lRightL_uI = 0;
      lMaxMessageSize_uI = _rawEntry.config.vColumns_uI;
   }

   if ( _rawEntry.config.vColumns_uI > 0 )
      lMaxMessageSize_uI = ( _rawEntry.config.vColumns_uI - lLeftL_uI - lRightL_uI );



// ========= Generate The Message Strings =========================================================================================================
   std::string lTempMessageString_STR;
   GLuint      lOldSize_uI = 0;
   std::string lTempColor_STR = lDefCol2_STR;

   for ( GLuint i = 0; i < _rawEntry.data.vLogEntries_V_eLS.size(); ++i ) {
      std::string lTemp_STR = _rawEntry.data.vLogEntries_V_eLS[i].getString();
      std::string lColorTemp_STR = "";

      if ( _rawEntry.data.vLogEntries_V_eLS[i].getType() == e_engine_internal::NEW_LINE ) {
         lMessage_VEC.push_back( ( lTempMessageString_STR + lResetColl_STR ) );
         lTempMessageString_STR.clear();
         lOldSize_uI = 0;
         lTempColor_STR = lDefCol2_STR;
         continue;
      }

      if ( _rawEntry.data.vLogEntries_V_eLS[i].getType() == e_engine_internal::NEW_POINT ) {
         lMessage_VEC.push_back( ( lTempMessageString_STR + lResetColl_STR ) );

         if ( _rawEntry.config.vColor_LCT == FULL || ( _rawEntry.config.vColor_LCT == REDUCED && _rawEntry.data.vBold_B ) )
            lTempColor_STR = eCMDColor::color( eCMDColor::OFF, eCMDColor::CYAN );

         lTempMessageString_STR = "  " +
                                  lTempColor_STR +
                                  "- ";
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
         GLuint lCount_uI = 1;
         GLuint lStart_uI = 0;
         for ( ; ( GLint )( ( lMaxMessageSize_uI * lCount_uI ) - lOldSize_uI - lTemp_STR.size() ) < 0; ++lCount_uI ) {
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
   for ( GLuint i = 0; i < lMessage_VEC.size(); ++i ) {
      GLuint lTempMessageSize_uI;
      if ( _rawEntry.config.vColor_LCT != DISABLED )
         lTempMessageSize_uI = boost::regex_replace( lMessage_VEC[i], lRmExcape_REGEX, lRegexReplace_CSTR ).size();
      else
         lTempMessageSize_uI = lMessage_VEC[i].size();

#ifdef __linux__
      if ( isatty( fileno( stdout ) ) != 0 && _rawEntry.config.vColor_LCT != DISABLED ) {
         // Clear the current line
         _rawEntry.vResultStrin_STR += "\x1B[2K\x1b[0G";
      }
#endif // __linux__

      _rawEntry.vResultStrin_STR += lL_STR + lMessage_VEC[i];
      if ( _rawEntry.config.vColumns_uI > 0 )
         _rawEntry.vResultStrin_STR.append( ( lMaxMessageSize_uI - lTempMessageSize_uI ) , ' ' );

      else
         _rawEntry.vResultStrin_STR += "  ";

      _rawEntry.vResultStrin_STR += lR_STR + lResetColl_STR + '\n';

      if ( i == 0 && lMessage_VEC.size() > 1 ) {
         /*
          * IMHO is a empty field the best solution
          * /
         boost::trim( lL_STR );
         boost::trim( lR_STR );

         GLuint lLeftNewL_uI;
         GLuint lRightNewL_uI;

         if ( _rawEntry.config.vColor_LCT != DISABLED ) {
            lLeftNewL_uI  = boost::regex_replace( lL_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
            lRightNewL_uI = boost::regex_replace( lR_STR, lRmExcape_REGEX, lRegexReplace_CSTR ).size();
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
         / * */

         lL_STR.clear();
         lR_STR.clear();

         lL_STR.append( lLeftL_uI,  ' ' );
         lR_STR.append( lRightL_uI, ' ' );
         /* */


      }
   }
}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
