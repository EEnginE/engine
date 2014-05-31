/*!
 * \file windows/eRandR_win32.hpp
 * \brief \b Classes: \a eRandR_win32
 */

#ifndef E_RANDR_WIN32_HPP
#define E_RANDR_WIN32_HPP

#include "eDisplays.hpp"

namespace e_engine {

namespace windows_win32 {

class eRandR_win32 {
   private:
      unsigned int vScreenWidth_uI;
      unsigned int vScreenHeight_uI;
      
      std::vector<eDisplays> vCurrentConfig_eD;
      std::vector<eDisplays> vPreviousConfig_eD;
      
      void reload();

   public:
      eRandR_win32();
      virtual ~eRandR_win32() {}

      void printRandRStatus();

      bool setGamma( eDisplays const &_disp, float _r, float _g, float _b, float _brightness = 1 );

      void getMostLeftRightTopBottomCRTC( unsigned int &_left, unsigned int &_right, unsigned int &_top, unsigned int &_bottom );
      int  getIndexOfDisplay( eDisplays const &_disp ) { return -1; }

      std::vector<eDisplays> getDisplayResolutions() { return vCurrentConfig_eD; }

      bool setDisplaySizes( eDisplays const &_disp ) {return false;}
      bool setPrimary( const e_engine::windows_win32::eDisplays &_disp );

      bool applyNewRandRSettings() {return false;}

      bool restoreScreenDefaults();
      bool restoreScreenLatest();

      bool isRandRSupported() { return true; }

      void getRandRVersion( int &_vMajor, int &_vMinor )                      {_vMajor = -1; _vMinor = -1;}
      void getScreenResolution( unsigned int &_width, unsigned int &_height ) {_width  = vScreenWidth_uI; _height = vScreenHeight_uI;}

};

/*!
 * \fn eRandR_win32::getRandRVersion
 * \brief Does nothing
 *
 * _vMajor and _vMinor will always be -1 because Windows has no
 * special version for window resolution stuff.
 *
 * \param[out] _vMajor Major version
 * \param[out] _vMinor Minor version
 */

} // windows_win32

} // e_engine

#endif
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
