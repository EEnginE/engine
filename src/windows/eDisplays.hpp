/*!
 * \file windows/eDisplays.hpp
 * \brief \b Classes: \a eDisplays
 */

#ifndef E_RANDR_DISPLAY_HPP
#define E_RANDR_DISPLAY_HPP

#include "eDisplayBasic.hpp"
#include <windows.h>

namespace e_engine {

namespace windows_win32 {

class eRandR_win32;

/*!
 * \brief Contains information about the current Display
 *
 * Changes in objects of this class wont impact the current display
 * configuration. See eRandR for more details
 *
 * The only way to get a object of this class is calling eRandR::getDisplay()
 *
 * \note To apply the changes made here, you must call eRandR::setDisplaySizes()
 *       first and then eRandR::applyNewSettings()
 */
class eDisplays : public eDisplayBasic {
   private:
      //! \brief internal structure for storing important mode information.
      std::vector<DEVMODEW>     vModes_V_win32;                 //!< all possible modes
      DEVMODEW                  vCurrentSettings_win32;         //!< The current display settings
      DEVMODEW                  vSelectedDisplaySettings_win32; //!< The selected display settings
      DISPLAY_DEVICEW           vDisplayDevice_win32;           //!< The Winapi display device (stores information about the display)

      eDisplays() {}
      eDisplays( std::wstring _name, bool _enabled, bool _isPrimary );

      //! \brief Add a new DEVMODEW to the internal mode list
      void addMode( DEVMODEW _mode ) { vModes_V_win32.push_back( _mode ); }

      double findNearestFreqTo( double _rate, unsigned int _width, unsigned int _height, DEVMODEW &_mode, double &_diff ) const;

      //! \brief Sets the current settings (DEVMODEW)
      void   setCurrentSettings( DEVMODEW _current ) { vSelectedDisplaySettings_win32 = vCurrentSettings_win32 = _current; }
      //! \brief Set the display device (needed for the disply ID)
      void   setDisplayDevice( DISPLAY_DEVICEW _device ) {vDisplayDevice_win32 = _device;}
      DEVMODEW getSelectedDevmode() const;
      //! \brief Get the display device (needed for the disply ID)
      DISPLAY_DEVICEW getDisplayDevice() const {return vDisplayDevice_win32;}
      
      int getMaxBitsPerPelFromResolutionAndFreq( unsigned int _width, unsigned int _height, double _rate ) const;

   public:
      virtual ~eDisplays() {}

      void autoSelectBest();

      void disable();
      void enable();

      std::vector<eDisplayBasic::res> getPossibleResolutions() const;
      bool isSizeSupported( unsigned int _width, unsigned int _height ) const;

      std::vector<double> getPossibleRates( unsigned int _width, unsigned int _height ) const;

      double autoSelectBySize( unsigned int _width, unsigned int _height, double _preferedRate = 0, double _maxDiff = 1 );
      bool   select( unsigned int _width, unsigned int _height, double _rate );

      void setNoClones() {}
      void setCloneOf( eDisplays const &_disp ) {}

      void getSelectedRes( unsigned int &_width, unsigned int &_height, double &_rate ) const;

      friend class eRandR_win32;
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################

/*!
 * \fn void eDisplays::setNoClones()
 * \brief Does nothing because Cloning not supported with windows
 *
 * \returns Nothing
 */

/*!
 * \fn void eDisplays::setCloneOf()
 * \brief Does nothing because Cloning not supported with windows
 *
 * \returns Nothing
 */

} // windows_win32

} // e_engine

#endif // E_RANDR_DISPLAY_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
