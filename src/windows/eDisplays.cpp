/*!
 * \file windows/eDisplays.cpp
 * \brief \b Classes: \a eDisplays
 */

#include "eDisplays.hpp"

namespace e_engine {

namespace windows_win32 {

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
void eDisplays::autoSelectBest() {
   //! \todo Check if there is a preferred mode - is this possible with windows?

   unsigned int lMaxWidth_uI  = 0; //!< Max currently found width
   unsigned int lMaxHeight_uI = 0; //!< Max currently found height
   
   for ( DEVMODE const & fMode : vModes_V_win32 ) {
      if ( ( lMaxWidth_uI < fMode.dmPelsWidth ) && ( lMaxHeight_uI < fMode.dmPelsHeight ) ) {
         vCurrentWidth_uI  = lMaxWidth_uI  = fMode.dmPelsWidth;
         vCurrentHeight_uI = lMaxHeight_uI = fMode.dmPelsHeight;
      }
   }

   if ( ! autoSelectBySize( lMaxWidth_uI, lMaxHeight_uI ) ) {
      // There are no modes which can be used
      vEnabled_B     = false;
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
 * \param _maxDiff      The difference between \a _preferedRate and the best chosen rate must be less than \a _maxDiff
 *
 * \returns The rate of the chosen mode or 0 if no mode could be found.
 *          If _preferedRate failed but a mode with the "normal" behavior
 *          was found, the rate will be negative.
 */
double eDisplays::autoSelectBySize( unsigned int _width, unsigned int _height, double _preferedRate, double _maxDiff ) {
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

   DEVMODE l60Hz_XRR;
   DEVMODE l120Hz_XRR;
   DEVMODE l240Hz_XRR;
   DEVMODE l480Hz_XRR;
   DEVMODE lPref_XRR;

   bool     lFoundOneSizeMatch      = false;
   bool     lFindPreferedRateFailed = false;

   if ( _preferedRate < 0 )
      _preferedRate *= -1;

   if ( _maxDiff < 0 )
      _maxDiff *= -1;

   if ( _preferedRate != 0 ) {
      lRatePref_D = findNearestFreqTo( _preferedRate, _width, _height, lPref_XRR, lMinDiffToPref_D );

      // No mode for this size
      if ( lRatePref_D < 0 )
         return 0;

      if ( lMinDiffToPref_D > _maxDiff ) {
         lFindPreferedRateFailed = true;
      } else {
         vSelectedDisplaySettings_win32 = lPref_XRR;
         return lRatePref_D;
      }
   }
   
   //! \todo Check if there is a preferred mode - is this possible with windows?

   if ( ! lFoundOneSizeMatch )
      return false;

   vCurrentWidth_uI  = _width;
   vCurrentHeight_uI = _height;

   lRate60Hz_D  = findNearestFreqTo( ( double )60,  _width, _height, l60Hz_XRR,  lMinDiffTo60Hz_D );
   lRate120Hz_D = findNearestFreqTo( ( double )120, _width, _height, l120Hz_XRR, lMinDiffTo120Hz_D );
   lRate240Hz_D = findNearestFreqTo( ( double )240, _width, _height, l240Hz_XRR, lMinDiffTo240Hz_D );
   lRate480Hz_D = findNearestFreqTo( ( double )480, _width, _height, l480Hz_XRR, lMinDiffTo480Hz_D );

   // No mode for this size
   if ( lRate60Hz_D < 0 || lRate120Hz_D < 0 || lRate240Hz_D < 0 || lRate480Hz_D < 0 )
      return 0;

   if ( lMinDiffTo60Hz_D == ( double )0       ||
         ( lMinDiffTo60Hz_D < lMinDiffTo120Hz_D &&
           lMinDiffTo60Hz_D < lMinDiffTo240Hz_D &&
           lMinDiffTo60Hz_D < lMinDiffTo480Hz_D
         )
      ) {
      vSelectedDisplaySettings_win32 = l60Hz_XRR;
      return lFindPreferedRateFailed ? lRate60Hz_D * -1 : lRate60Hz_D;
   }

   if ( lMinDiffTo120Hz_D == ( double )0       ||
         ( lMinDiffTo120Hz_D < lMinDiffTo60Hz_D  &&
           lMinDiffTo120Hz_D < lMinDiffTo240Hz_D &&
           lMinDiffTo120Hz_D < lMinDiffTo480Hz_D
         )
      ) {
      vSelectedDisplaySettings_win32 = l120Hz_XRR;
      return lFindPreferedRateFailed ? lRate120Hz_D * -1 : lRate120Hz_D;
   }

   if ( lMinDiffTo240Hz_D == ( double )0       ||
         ( lMinDiffTo240Hz_D < lMinDiffTo60Hz_D  &&
           lMinDiffTo240Hz_D < lMinDiffTo120Hz_D &&
           lMinDiffTo240Hz_D < lMinDiffTo480Hz_D
         )
      ) {
      vSelectedDisplaySettings_win32 = l240Hz_XRR;
      return lFindPreferedRateFailed ? lRate240Hz_D * -1 : lRate240Hz_D;
   }

   if ( lMinDiffTo480Hz_D == ( double )0       ||
         ( lMinDiffTo480Hz_D < lMinDiffTo60Hz_D  &&
           lMinDiffTo480Hz_D < lMinDiffTo120Hz_D &&
           lMinDiffTo480Hz_D < lMinDiffTo240Hz_D
         )
      ) {
      vSelectedDisplaySettings_win32 = l480Hz_XRR;
      return lFindPreferedRateFailed ? lRate480Hz_D * -1 : lRate480Hz_D;
   }

   return 0;
}

/*!
 * \brief disable this display
 */
void eDisplays::disable() {
   vEnabled_B = false;
}

/*!
 * \brief enable this display
 */
void eDisplays::enable() {
   vEnabled_B = true;
}

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
double eDisplays::findNearestFreqTo( double _rate, unsigned int _width, unsigned int _height, DEVMODE &_mode, double &_diff ) const {
   _diff = 1000000;
   double lRate_D = -1;
   for ( DEVMODE const & fMode : vModes_V_win32 ) {
      if ( _width == fMode.dmPelsWidth && _height == fMode.dmPelsHeight ) {
         if ( _rate == fMode.dmDisplayFrequency ) {
            _diff = 0;
            _mode = fMode;
            return fMode.dmDisplayFrequency;
         } else if ( ( _rate - fMode.dmDisplayFrequency ) > 0 ) {
            if ( ( _rate - fMode.dmDisplayFrequency ) < _diff ) {
               _diff   = ( _rate - fMode.dmDisplayFrequency );
               _mode   = fMode;
               lRate_D = fMode.dmDisplayFrequency;
            }
         } else if ( ( fMode.dmDisplayFrequency - _rate ) < _diff ) {
            _diff   = ( fMode.dmDisplayFrequency - _rate );
            _mode   = fMode;
            lRate_D = fMode.dmDisplayFrequency;
         }
      }
   }
   return lRate_D;
}

/*!
 * \brief Returns the supported resolutions of a display
 * *
 * \param _width The width of the resolution to check
 * \param _height The height of the resolution to check
 *
 * \return A vector (double) of the possible rates
 */
std::vector< double > eDisplays::getPossibleRates( unsigned int _width, unsigned int _height ) const {
   std::vector<double> lRates_V_D;

   for ( DEVMODE const & d : vModes_V_win32 ) {
      if ( d.dmPelsWidth == _width && d.dmPelsHeight == _height ) {
         bool lRateAlreadyFound_B = false;
         for ( double const & rate : lRates_V_D ) {
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
std::vector< eDisplayBasic::res > eDisplays::getPossibleResolutions() const {
   std::vector<eDisplayBasic::res> lTempRes;
   for ( DEVMODE const & d : vModes_V_win32 ) {
      bool lHasAlreadyResFound = false;

      // Check if already res found
      for ( eDisplayBasic::res const & e : lTempRes ) {
         if ( e.width == d.dmPelsWidth && e.height == d.dmPelsHeight ) {
            lHasAlreadyResFound = true;
            break;
         }
      }

      if ( lHasAlreadyResFound )
         continue;

      eDisplayBasic::res lNewRes;
      lNewRes.width  = d.dmPelsWidth;
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
void eDisplays::getSelectedRes( unsigned int &_width, unsigned int &_height, double &_rate ) const {
   _width  = vSelectedDisplaySettings_win32.dmPelsWidth;
   _height = vSelectedDisplaySettings_win32.dmPelsHeight;
   _rate   = vSelectedDisplaySettings_win32.dmDisplayFrequency;
}

/*!
 * \brief Check if the resolution is supported by the monitor
 *
 * \param[in] _width  The width of the resolution to check
 * \param[in] _height The height of the resolution to check
 *
 * \returns If the resolution is supported
 */
bool eDisplays::isSizeSupported( unsigned int _width, unsigned int _height ) const {
   bool lFoundResolution_B = false;

   for ( DEVMODE const & d : vModes_V_win32 ) {
      if ( _width == d.dmPelsWidth && _height == d.dmPelsHeight ) {
         lFoundResolution_B = true;
         break;
      }
   }

   return lFoundResolution_B;
}

bool eDisplays::select( unsigned int _width, unsigned int _height, double _rate ) {
   return false;
}




} // windows_win32

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
