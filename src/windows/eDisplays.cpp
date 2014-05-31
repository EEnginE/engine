/*!
 * \file windows/eDisplays.cpp
 * \brief \b Classes: \a eDisplays
 */

#include "eDisplays.hpp"

namespace e_engine {

namespace windows_win32 {

void eDisplays::autoSelectBest() {

}

double eDisplays::autoSelectBySize( unsigned int _width, unsigned int _height, double _preferedRate, double _maxDiff ) {
   return 1.1;
}

void eDisplays::disable() {

}

void eDisplays::enable() {

}

double eDisplays::findNearestFreqTo( double _rate, unsigned int _width, unsigned int _height, double &_diff ) const {
   return 1.1;
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
