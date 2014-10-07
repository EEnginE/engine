/*!
 * \file windows/iDisplays.hpp
 * \brief \b Classes: \a iDisplays
 */

#ifndef E_RANDR_DISPLAY_HPP
#define E_RANDR_DISPLAY_HPP

#include "iDisplayBasic.hpp"
#include <windows.h>

namespace e_engine {

namespace windows_win32 {

class iRandR_win32;

/*!
 * \brief Contains information about the current Display
 *
 * Changes in objects of this class wont impact the current display
 * configuration. See iRandR for more details
 *
 * The only way to get a object of this class is calling iRandR::getDisplay()
 *
 * \note To apply the changes made here, you must call iRandR::setDisplaySizes()
 *       first and then iRandR::applyNewSettings()
 */
class iDisplays : public iDisplayBasic {
   private:
      //! \brief internal structure for storing important mode information.
      std::vector<DEVMODEW>     vModes_V_win32;                 //!< all possible modes
      DEVMODEW                  vCurrentSettings_win32;         //!< The current display settings
      DEVMODEW                  vSelectedDisplaySettings_win32; //!< The selected display settings
      DISPLAY_DEVICEW           vDisplayDevice_win32;           //!< The Winapi display device (stores information about the display)

      iDisplays() {}
      iDisplays( std::wstring _name, bool _enabled, bool _isPrimary );

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
      virtual ~iDisplays() {}

      void autoSelectBest();

      void disable();
      void enable();

      std::vector<iDisplayBasic::res> getPossibleResolutions() const;
      bool isSizeSupported( unsigned int _width, unsigned int _height ) const;

      std::vector<double> getPossibleRates( unsigned int _width, unsigned int _height ) const;

      double autoSelectBySize( unsigned int _width, unsigned int _height, double _preferedRate = 0, double _maxDiff = 1 );
      bool   select( unsigned int _width, unsigned int _height, double _rate );

      void setNoClones() {}
      void setCloneOf( iDisplays const &_disp ) {}

      void getSelectedRes( unsigned int &_width, unsigned int &_height, double &_rate ) const;

      friend class iRandR;
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################

/*!
 * \fn void iDisplays::setNoClones()
 * \brief Does nothing because Cloning not supported with windows
 *
 * \returns Nothing
 */

/*!
 * \fn void iDisplays::setCloneOf()
 * \brief Does nothing because Cloning not supported with windows
 *
 * \returns Nothing
 */

} // windows_win32

} // e_engine

#endif // E_RANDR_DISPLAY_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
