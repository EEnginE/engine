/*!
 * \file windows/iDisplays.cpp
 * \brief \b Classes: \a iDisplays
 */

#include "iDisplays.hpp"
#include "uLog.hpp"

namespace e_engine {

namespace windows_win32 {


iDisplays::iDisplays( std::wstring _name, bool _enabled, bool _isPrimary ) {
   vEnabled_B = _enabled;
   vName_str = std::string( _name.begin(), _name.end() );
   vIsPrimary_B = _isPrimary;
}



/*!
 * \brief Selects the best mode for this display
 *
 * If it is impossible to find any matching mode, the display will be
 * disabled
 *
 * \note This function will be automatically run if you enable() a display
 *       which was disabled before.
 *
 * \returns Nothing
 */
void iDisplays::autoSelectBest() {
   //! \todo Check if there is a preferred mode - is this possible with windows?

   unsigned int lMaxWidth_uI = 0;  //!< Max currently found width
   unsigned int lMaxHeight_uI = 0; //!< Max currently found height

   for ( DEVMODEW const &fMode : vModes_V_win32 ) {
      if ( ( lMaxWidth_uI < fMode.dmPelsWidth ) && ( lMaxHeight_uI < fMode.dmPelsHeight ) ) {
         vCurrentWidth_uI = lMaxWidth_uI = fMode.dmPelsWidth;
         vCurrentHeight_uI = lMaxHeight_uI = fMode.dmPelsHeight;
      }
   }

   if ( !autoSelectBySize( lMaxWidth_uI, lMaxHeight_uI ) ) {
      // There are no modes which can be used
      vEnabled_B = false;
   }
}

/*!
 * \brief Selects the best mode for the resolution _width x _height
 *
 * If _preferedRate == 0:
 *
 * If the function can find a preferred mode with the resolution, it will choose this mode,
 * else the function will look for the mode, nearest to 60 Hz, 120 Hz, 240 Hz and 480 Hz.
 * Then it compares the differences between all 4 modes.
 * The mode with the least difference will be chosen.
 *
 *
 * If _preferedRate > 0:
 *
 * The function will look for the mode, nearest to _preferedRate. If the difference
 * between _preferedRate and the real value is greater than _maxDiff, the function
 * will fall back to the behavior from above.
 *
 * \warning If _preferedRate and/or _maxDiff are < 0 they will be multiplied with -1!!
 *
 * \param _width        The width  the mode must have.
 * \param _height       The height the mode must have.
 * \param _preferedRate Choose the mode nearest to \a _preferedRate.
 * \param _maxDiff      The difference between \a _preferedRate and the best chosen rate must be
 *less than \a _maxDiff
 *
 * \returns The rate of the chosen mode or 0 if no mode could be found.
 *          If _preferedRate failed but a mode with the "normal" behavior
 *          was found, the rate will be negative.
 */
double iDisplays::autoSelectBySize( unsigned int _width,
                                    unsigned int _height,
                                    double _preferedRate,
                                    double _maxDiff ) {
   double lMinDiffTo60Hz_D;
   double lMinDiffTo120Hz_D;
   double lMinDiffTo240Hz_D;
   double lMinDiffTo480Hz_D;
   double lMinDiffToPref_D;

   double lRate60Hz_D;
   double lRate120Hz_D;
   double lRate240Hz_D;
   double lRate480Hz_D;
   double lRatePref_D;

   DEVMODEW l60Hz_win32;
   DEVMODEW l120Hz_win32;
   DEVMODEW l240Hz_win32;
   DEVMODEW l480Hz_win32;
   DEVMODEW lPref_win32;

   bool lFoundOneSizeMatch = false;
   bool lFindPreferedRateFailed = false;

   if ( _preferedRate < 0 )
      _preferedRate *= -1;

   if ( _maxDiff < 0 )
      _maxDiff *= -1;

   if ( _preferedRate != 0 ) {
      lRatePref_D =
            findNearestFreqTo( _preferedRate, _width, _height, lPref_win32, lMinDiffToPref_D );

      // No mode for this size
      if ( lRatePref_D < 0 )
         return 0;

      if ( lMinDiffToPref_D > _maxDiff ) {
         lFindPreferedRateFailed = true;
      } else {
         vSelectedDisplaySettings_win32 = lPref_win32;
         return lRatePref_D;
      }
   }

   //! \todo Check if there is a preferred mode - is this possible with windows?

   if ( !lFoundOneSizeMatch )
      return false;

   vCurrentWidth_uI = _width;
   vCurrentHeight_uI = _height;

   lRate60Hz_D = findNearestFreqTo( (double)60, _width, _height, l60Hz_win32, lMinDiffTo60Hz_D );
   lRate120Hz_D =
         findNearestFreqTo( (double)120, _width, _height, l120Hz_win32, lMinDiffTo120Hz_D );
   lRate240Hz_D =
         findNearestFreqTo( (double)240, _width, _height, l240Hz_win32, lMinDiffTo240Hz_D );
   lRate480Hz_D =
         findNearestFreqTo( (double)480, _width, _height, l480Hz_win32, lMinDiffTo480Hz_D );

   // No mode for this size
   if ( lRate60Hz_D < 0 || lRate120Hz_D < 0 || lRate240Hz_D < 0 || lRate480Hz_D < 0 )
      return 0;

   if ( lMinDiffTo60Hz_D == (double)0 ||
        ( lMinDiffTo60Hz_D < lMinDiffTo120Hz_D && lMinDiffTo60Hz_D < lMinDiffTo240Hz_D &&
          lMinDiffTo60Hz_D < lMinDiffTo480Hz_D ) ) {
      vSelectedDisplaySettings_win32 = l60Hz_win32;
      return lFindPreferedRateFailed ? lRate60Hz_D * -1 : lRate60Hz_D;
   }

   if ( lMinDiffTo120Hz_D == (double)0 ||
        ( lMinDiffTo120Hz_D < lMinDiffTo60Hz_D && lMinDiffTo120Hz_D < lMinDiffTo240Hz_D &&
          lMinDiffTo120Hz_D < lMinDiffTo480Hz_D ) ) {
      vSelectedDisplaySettings_win32 = l120Hz_win32;
      return lFindPreferedRateFailed ? lRate120Hz_D * -1 : lRate120Hz_D;
   }

   if ( lMinDiffTo240Hz_D == (double)0 ||
        ( lMinDiffTo240Hz_D < lMinDiffTo60Hz_D && lMinDiffTo240Hz_D < lMinDiffTo120Hz_D &&
          lMinDiffTo240Hz_D < lMinDiffTo480Hz_D ) ) {
      vSelectedDisplaySettings_win32 = l240Hz_win32;
      return lFindPreferedRateFailed ? lRate240Hz_D * -1 : lRate240Hz_D;
   }

   if ( lMinDiffTo480Hz_D == (double)0 ||
        ( lMinDiffTo480Hz_D < lMinDiffTo60Hz_D && lMinDiffTo480Hz_D < lMinDiffTo120Hz_D &&
          lMinDiffTo480Hz_D < lMinDiffTo240Hz_D ) ) {
      vSelectedDisplaySettings_win32 = l480Hz_win32;
      return lFindPreferedRateFailed ? lRate480Hz_D * -1 : lRate480Hz_D;
   }

   return 0;
}

/*!
 * \brief disable this display
 */
void iDisplays::disable() { vEnabled_B = false; }

/*!
 * \brief enable this display
 */
void iDisplays::enable() { vEnabled_B = true; }

/*!
 * \brief Finds the display frequency most close to _rate with the _width and _height
 *
 * [private]
 *
 * \param[in]  _rate   Find the most closest display frequency to this rate
 * \param[in]  _width  The display mut have this width
 * \param[in]  _height The display mut have this height
 * \param[out] _mode   The found mode
 * \param[out] _diff   The difference between _rate and the final display frequency
 *
 * \returns the display frequency closest to _rate
 * \returns -1 when there was no mode found for this resolution
 */
double iDisplays::findNearestFreqTo( double _rate,
                                     unsigned int _width,
                                     unsigned int _height,
                                     DEVMODEW &_mode,
                                     double &_diff ) const {
   _diff = 1000000;
   double lRate_D = -1;
   for ( DEVMODEW const &fMode : vModes_V_win32 ) {
      if ( _width == fMode.dmPelsWidth && _height == fMode.dmPelsHeight ) {
         if ( _rate == fMode.dmDisplayFrequency ) {
            _diff = 0;
            _mode = fMode;
            return fMode.dmDisplayFrequency;
         } else if ( ( _rate - fMode.dmDisplayFrequency ) > 0 ) {
            if ( ( _rate - fMode.dmDisplayFrequency ) < _diff ) {
               _diff = ( _rate - fMode.dmDisplayFrequency );
               _mode = fMode;
               lRate_D = fMode.dmDisplayFrequency;
            }
         } else if ( ( fMode.dmDisplayFrequency - _rate ) < _diff ) {
            _diff = ( fMode.dmDisplayFrequency - _rate );
            _mode = fMode;
            lRate_D = fMode.dmDisplayFrequency;
         }
      }
   }
   return lRate_D;
}

/*!
 * \brief Returns the supported resolutions of a display
 *
 * \param _width The width of the resolution to check
 * \param _height The height of the resolution to check
 *
 * \return A vector (double) of the possible rates
 */
std::vector<double> iDisplays::getPossibleRates( unsigned int _width, unsigned int _height ) const {
   std::vector<double> lRates_V_D;

   for ( DEVMODEW const &d : vModes_V_win32 ) {
      if ( d.dmPelsWidth == _width && d.dmPelsHeight == _height ) {
         bool lRateAlreadyFound_B = false;
         for ( double const &rate : lRates_V_D ) {
            if ( rate == d.dmDisplayFrequency ) {
               lRateAlreadyFound_B = true;
               break;
            }
         }

         if ( lRateAlreadyFound_B )
            continue;

         lRates_V_D.push_back( d.dmDisplayFrequency );
      }
   }

   return lRates_V_D;
}

/*!
 * \brief Get the possible resolutions supported by this diplay
 *
 * \returns A vector of the supported resolutions
 */
std::vector<iDisplayBasic::res> iDisplays::getPossibleResolutions() const {
   std::vector<iDisplayBasic::res> lTempRes;
   for ( DEVMODEW const &d : vModes_V_win32 ) {
      bool lHasAlreadyResFound = false;

      // Check if already res found
      for ( iDisplayBasic::res const &e : lTempRes ) {
         if ( e.width == d.dmPelsWidth && e.height == d.dmPelsHeight ) {
            lHasAlreadyResFound = true;
            break;
         }
      }

      if ( lHasAlreadyResFound )
         continue;

      iDisplayBasic::res lNewRes;
      lNewRes.width = d.dmPelsWidth;
      lNewRes.height = d.dmPelsHeight;
      lTempRes.push_back( lNewRes );
   }
   return lTempRes;
}


/*!
 * \brief Get information about the currently selected settings
 *
 * \param[out] _width  The selected width
 * \param[out] _height The selected height
 * \param[out] _rate   The selected rate
 *
 * \returns Nothing
 */
void iDisplays::getSelectedRes( unsigned int &_width, unsigned int &_height, double &_rate ) const {
   _width = vSelectedDisplaySettings_win32.dmPelsWidth;
   _height = vSelectedDisplaySettings_win32.dmPelsHeight;
   _rate = vSelectedDisplaySettings_win32.dmDisplayFrequency;
}

/*!
 * \brief Check if the resolution is supported by the monitor
 *
 * \param[in] _width  The width of the resolution to check
 * \param[in] _height The height of the resolution to check
 *
 * \returns If the resolution is supported
 */
bool iDisplays::isSizeSupported( unsigned int _width, unsigned int _height ) const {
   bool lFoundResolution_B = false;

   for ( DEVMODEW const &d : vModes_V_win32 ) {
      if ( _width == d.dmPelsWidth && _height == d.dmPelsHeight ) {
         lFoundResolution_B = true;
         break;
      }
   }

   return lFoundResolution_B;
}

/*!
 * \brief Select the mode with this resolution and rate
 *
 * \param[in] _width  The width for the new mode
 * \param[in] _height The height for the new mode
 * \param[in] _rate   The rate for the new mode
 *
 * \returns true if the mode was found and false if not
 */
bool iDisplays::select( unsigned int _width, unsigned int _height, double _rate ) {
   if ( !isSizeSupported( _width, _height ) )
      return false;

   for ( DEVMODEW const &dMode : vModes_V_win32 ) {
      if ( dMode.dmPelsWidth == _width && dMode.dmPelsHeight == _height &&
           dMode.dmDisplayFrequency == _rate ) {
         // Found
         vSelectedDisplaySettings_win32 = dMode;
         return true;
      }
   }

   // _rate not found
   return false;
}

/*!
 * \brief Get the biggest possible BitsPerPel for this resolution and rate
 *
 * \param[in] _width  The width of the resolution
 * \param[in] _height The height of the resolution
 * \param[in] _rate   The selected rate
 *
 * \returns -1 if the resolution is not supported
 * \returns the biggest possible BitsPerPel for this resolution and rate
 */
int iDisplays::getMaxBitsPerPelFromResolutionAndFreq( unsigned int _width,
                                                      unsigned int _height,
                                                      double _rate ) const {
   if ( !isSizeSupported( _width, _height ) )
      return -1;

   unsigned int lMaxBitsPerPel_uI = 0;
   for ( DEVMODEW const &dMode : vModes_V_win32 ) {
      if ( dMode.dmPelsWidth != _width || dMode.dmPelsHeight != _height ||
           dMode.dmDisplayFrequency != _rate )
         continue;

      if ( lMaxBitsPerPel_uI < dMode.dmBitsPerPel )
         lMaxBitsPerPel_uI = dMode.dmBitsPerPel;
   }
   return lMaxBitsPerPel_uI;
}

/*!
 * \brief Get the selected display mode
 *
 * Also preperes the DEVMODEW for usage
 *
 * \returns The selected display mode
 */
DEVMODEW iDisplays::getSelectedDevmode() const {
   DEVMODEW lTempDevmode_win32 = vSelectedDisplaySettings_win32;

   // Get the mximum
   lTempDevmode_win32.dmBitsPerPel =
         getMaxBitsPerPelFromResolutionAndFreq( vSelectedDisplaySettings_win32.dmPelsWidth,
                                                vSelectedDisplaySettings_win32.dmPelsHeight,
                                                vSelectedDisplaySettings_win32.dmDisplayFrequency );

   if ( vPositionChanged_B ) {
      lTempDevmode_win32.dmFields |= DM_POSITION;
      lTempDevmode_win32.dmPosition.x = vPosX_uI;
      lTempDevmode_win32.dmPosition.y = vPosY_uI;
   }

   if ( !vEnabled_B )
      memset( &lTempDevmode_win32, 0, sizeof( lTempDevmode_win32 ) );

   lTempDevmode_win32.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFREQUENCY;
   lTempDevmode_win32.dmSize = sizeof( lTempDevmode_win32 );

   return lTempDevmode_win32;
}




} // windows_win32

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
