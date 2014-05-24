/*!
 * \file x11/RandR/randr_reload.cpp
 * \brief \b Classes: \a eRandR
 */

#include "randr.hpp"
#include "log.hpp"

namespace e_engine {


namespace {

/*!
 * \brief Fill a string with chars until it has a specific size.
 *
 * \param _str  [in] The string to resize
 * \param _size [in] The wished size
 * \param _fill [in] Fill it with these chars
 *
 * \returns The result
 */
std::string placeStringLeft( std::string _str, unsigned int _size, char _fill ) {
   if ( _str.size() == _size )
      return _str;

   std::string lResult_str = _str;
   if ( lResult_str.size() > _size ) {
      lResult_str.resize( _size );
      return lResult_str;
   }

   lResult_str.append( _size - lResult_str.size(), _fill );
   return lResult_str;
}

}

/*!
 * \brief Prints a (more or less) fancy table with all XRandR information
 *
 * \returns \a true when everything went fine or \a false when there was a RandR error
 */
void eRandR::printRandRStatus() {
   if ( ! vIsRandRSupported_B )
      return;

   reload( false );

   LOG_ENTRY lTempEntry =

      //
      //   -- HEADDER
      //
      //    |============|======|=========|===========|============|=========================|
      //    |   Output   | CRTC | Primary | Connected |  Position  |          MODE           |
      //    |            |      |         |           |            |  Resolutions  |  Rates  |
      //    |------------|------|---------|-----------|------------|---------------|---------|
      iLOG "" S_COLOR 'O', 'W' ADD 'O', 'G',  "RandR Info -- Screen Size: "
      ADD 'B', 'G', vScreenWidth_uI  ADD 'B', 'G', 'x'
      ADD 'B', 'G', vScreenHeight_uI NEWLINE NEWLINE

      ADD "|============|======|=========|===========|============|=========================|" NEWLINE

      ADD "|   "          ADD 'B', 'W', "Output"
      ADD "   | "         ADD 'B', 'W', "CRTC"
      ADD " | "           ADD 'B', 'W', "Primary"
      ADD " | "           ADD 'B', 'W', "Connected"
      ADD " |  "          ADD 'B', 'W', "Position"
      ADD "  |          " ADD 'B', 'W', "MODE"
      ADD "           |"  NEWLINE

      ADD "|            |      |         |           |            |  " ADD 'B', 'W', "Resolutions"
      ADD "  |  " ADD 'B', 'W', "Rates" ADD "  |" NEWLINE

      ADD "|------------|------|---------|-----------|------------|---------------|---------|" NEWLINE _END_

      //
      //   -- Entries
      //
   for ( e_engine_internal::_output const & fOutput : vOutput_V_RandR ) {
      e_engine_internal::_crtc lCRTC_RandR;

      std::string lCRTC_str      = ( fOutput.crtc == 0 )                         ? "OFF" : boost::lexical_cast<std::string>( fOutput.crtc ) ;
      std::string lPrimary_str   = ( vLatestConfig_RandR.primary == fOutput.id ) ? "YES" : "NO" ;
      std::string lConnected_str = ( fOutput.connection == 0 )                   ? "YES" : ( fOutput.connection == 2 ) ? "???" : "NO";
      std::string lPosition_str  = "   NONE";

      char        lBold_C      = ( fOutput.connection == 0 )                     ? 'B'   : 'O';
      char        lCRTC_C      = ( lCRTC_str != "OFF" )                          ? 'G'   : 'R';

      if ( !( fOutput.crtc == 0 ) ) {
         for ( e_engine_internal::_crtc const & fCRTC : vCRTC_V_RandR ) {
            if ( fCRTC.id == fOutput.crtc ) {
               lCRTC_RandR = fCRTC;
               lPosition_str = "";
               if ( fCRTC.posX >= 0 )
                  lPosition_str += '+';

               lPosition_str += boost::lexical_cast<std::string>( fCRTC.posX );

               if ( fCRTC.posY >= 0 )
                  lPosition_str += '+';

               lPosition_str += boost::lexical_cast<std::string>( fCRTC.posY );
               break;
            }
         }
      }

      if ( lBold_C == 'B' ) {
         lTempEntry _ADD "|            |      |         |           |            |               |         |" NEWLINE _END_
      }

      lTempEntry _ADD "| "
      ADD lBold_C,                                'W', placeStringLeft( fOutput.name,   11, ' ' )   ADD "| "
      ADD lBold_C, ( lBold_C == 'B' ) ? lCRTC_C : 'W', placeStringLeft( lCRTC_str,      5,  ' ' )   ADD "|   "
      ADD lBold_C,                                'W', placeStringLeft( lPrimary_str,   6,  ' ' )   ADD "|    "
      ADD lBold_C,                                'W', placeStringLeft( lConnected_str, 7,  ' ' )   ADD "| "
      ADD lBold_C,                                'W', placeStringLeft( lPosition_str,  11, ' ' )   ADD "|   " _END_

      if ( lBold_C == 'O' || fOutput.modes.size() == 0 ) {
         lTempEntry _ADD " OFFLINE    |   OFF   |" NEWLINE _END_
      }



      unsigned int lWidth_uI  = 0;
      unsigned int lHeight_uI = 0;

      std::string lModeSize_str;
      // Unfortunately boost::lexical_cast doesn't support precision, so we must use sprintf if we want to avoid the slow stringstreams
      char        lModeFreq_CSTR[15];
      std::string lModeFreq_str;

      bool        lIsFirstModePrinted_B = true;

      //
      //   -- Modes
      //
      for ( e_engine_internal::_mode const & fMode : vMode_V_RandR ) {
         bool         lFoundMode_B    = false;
         bool         lModePrefered_B = false;
         char         lAtrib_C        = 'O';
         char         lColor_C        = 'W';
         unsigned int lModeCounter_uI = 0;      //< Needed for preferred check

         // Check if the mode is supported by the output
         for ( RRMode const & fTempMode : fOutput.modes ) {
            ++lModeCounter_uI;
            if ( fTempMode == fMode.id ) {
               lFoundMode_B = true;

               if ( lModeCounter_uI == ( unsigned int )fOutput.npreferred )
                  lModePrefered_B = true;

               break;
            }
         }

         if ( ! lFoundMode_B )
            continue;

         if ( ! lIsFirstModePrinted_B ) {
            lTempEntry _ADD "|            |      |         |           |            |   " _END_
         }

         if ( fMode.width == lWidth_uI && fMode.height == lHeight_uI ) {
            lModeSize_str.clear();
         } else {
            lWidth_uI      = fMode.width;
            lHeight_uI     = fMode.height;
            lModeSize_str  = boost::lexical_cast<std::string>( lWidth_uI ) +
                             'x' +
                             boost::lexical_cast<std::string>( lHeight_uI );
         }

         snprintf( lModeFreq_CSTR, 15, "%.2f", fMode.refresh );
         lModeFreq_str = lModeFreq_CSTR;

         // This should never happen
         if ( ! lFoundMode_B ) {
            lModeSize_str = "RandR";
            lModeFreq_str = "ERROR";
         }

         // This is a (!) mode with the current width and height of the CRTC
         if ( lWidth_uI == lCRTC_RandR.width && lHeight_uI == lCRTC_RandR.height ) {
            lAtrib_C       = 'B';
            if ( !lModeSize_str.empty() )
               lModeSize_str += '*';
         }

         // This is the (!) mode of the CRTC
         if ( fMode.id == lCRTC_RandR.mode ) {
            lAtrib_C       = 'B';
            lModeFreq_str += '*';
         }

         // This is the preferred mode
         if ( lModePrefered_B ) {
            lColor_C       = 'G';
            lModeFreq_str += '+';
         }

         lTempEntry _ADD
         lAtrib_C, lColor_C, placeStringLeft( lModeSize_str, 12, ' ' ) ADD "| " ADD
         lAtrib_C, lColor_C, placeStringLeft( lModeFreq_str, 8,  ' ' ) ADD '|'   NEWLINE _END_

         lIsFirstModePrinted_B = false;
      }
   }


   lTempEntry _ADD "|============|======|=========|===========|============|=========================|"  NEWLINE NEWLINE END

}

}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
