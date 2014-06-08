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
      std::vector<DEVMODE>     vModes_V_win32;                 //!< all possible modes
      DEVMODE                  vCurrentSettings_win32;         //!< The current display settings
      DEVMODE                  vSelectedDisplaySettings_win32; //!< The selected display settings
      DISPLAY_DEVICE           vDisplayDevice_win32;           //!< The Winapi display device (stores information about the display)

      eDisplays() {}
      eDisplays( std::string _name, bool _enabled, bool _isPrimary ) {vEnabled_B = _enabled; vName_str = _name; vIsPrimary_B = _isPrimary;}

      void addMode( DEVMODE _mode ) { vModes_V_win32.push_back( _mode ); }

      bool   getIsEnabled() const { return vEnabled_B; }

      double findNearestFreqTo( double _rate, unsigned int _width, unsigned int _height, DEVMODE &_mode, double &_diff ) const;

      void   setPrimary() { vIsPrimary_B = true; }
      void   setCurrentSettings( DEVMODE _current ) { vSelectedDisplaySettings_win32 = vCurrentSettings_win32 = _current; }
      void   setDisplayDevice( DISPLAY_DEVICE _device ) {vDisplayDevice_win32 = _device;}
      void   getSelectedDevmodeOptions(
         DWORD &_width,
         DWORD &_height,
         DWORD &_rate,
         DWORD &_bitsPerPel,
         LONG  &_posX,
         LONG  &_posY,
         DWORD &_fields
      ) const;
      DEVMODE getSelectedDevmode() const;
      DISPLAY_DEVICE getDisplayDevice() const {return vDisplayDevice_win32;}
      
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
