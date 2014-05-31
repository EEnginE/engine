/*!
 * \file windows/eRandR_win32.cpp
 * \brief \b Classes: \a eRandR_win32
 */

#include "eRandR_win32.hpp"
#include "log.hpp"
#include <windows.h>

namespace e_engine {

namespace windows_win32 {

eRandR_win32::eRandR_win32() {
   reload();
}

/*!
 * \brief get the current settins
 */
void eRandR_win32::reload() {
   HWND lDesktopHWND_win32 = GetDesktopWindow();
   RECT lDesktopRect_win32;


   DISPLAY_DEVICE lDisplayDevice_win32;
   lDisplayDevice_win32.cb = sizeof( DISPLAY_DEVICE );

   GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );

   vScreenWidth_uI  = lDesktopRect_win32.right  - lDesktopRect_win32.left;
   vScreenHeight_uI = lDesktopRect_win32.bottom - lDesktopRect_win32.top;

   vCurrentConfig_eD.clear();

   DEVMODE lSettings_win32 = {0};
   lSettings_win32.dmSize = sizeof( DEVMODE );
   
   iLOG "Begin reload" END

   for ( DWORD lDeviceNum_win32 = 0; EnumDisplayDevices( NULL, lDeviceNum_win32, &lDisplayDevice_win32, 0 ); ++lDeviceNum_win32 ) {
      
      iLOG "DEV: " ADD lDeviceNum_win32 END

      DISPLAY_DEVICE lDisplayDeviceTemp_win32;
      lDisplayDeviceTemp_win32.cb = sizeof( DISPLAY_DEVICE );

      for (
         DWORD lDisplayNum_win32 = 0;
         EnumDisplayDevices( lDisplayDevice_win32.DeviceName, lDisplayNum_win32, &lDisplayDeviceTemp_win32, 0 );
         ++lDisplayNum_win32
      ) {
         iLOG "DISP: " ADD lDisplayNum_win32 END
         bool lIsEnabled_B = false;
         bool lIsPrimary_B = false;

         if ( lDisplayDeviceTemp_win32.StateFlags & DISPLAY_DEVICE_ACTIVE )
            lIsEnabled_B = true;

         if ( lDisplayDeviceTemp_win32.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
            lIsPrimary_B = true;

         eDisplays lTempDisplay( lDisplayDevice_win32.DeviceName, lIsEnabled_B, lIsPrimary_B );

         lTempDisplay.setDisplayDevice( lDisplayDeviceTemp_win32 );
         
         iLOG "lDisplayDevice_win32 " ADD lDisplayDevice_win32.DeviceName END
         iLOG "lDisplayDeviceTemp_win32" ADD lDisplayDeviceTemp_win32.DeviceName END

         EnumDisplaySettings( lDisplayDevice_win32.DeviceName, ENUM_CURRENT_SETTINGS, &lSettings_win32 );
         lTempDisplay.setCurrentSettings( lSettings_win32 );
         lTempDisplay.setCurrentSizeAndPosition(
            lSettings_win32.dmPelsWidth,
            lSettings_win32.dmPelsHeight,
            lSettings_win32.dmPosition.x,
            lSettings_win32.dmPosition.y,
            lSettings_win32.dmDisplayFrequency
         );


         for ( DWORD lModeNum_win32 = 0; EnumDisplaySettings( lDisplayDevice_win32.DeviceName, lModeNum_win32, &lSettings_win32 ); lModeNum_win32++ ) {
            lTempDisplay.addMode( lSettings_win32 );
            iLOG "MODE: " ADD lModeNum_win32 END
         }

         vCurrentConfig_eD.push_back( lTempDisplay );
      }
   }
   printRandRStatus();
}

/*!
 * \brief Only usefull for Linux RandR.
 *
 * Does nothing in Windows: all outputs are 0
 *
 * \param[out] _left   = 0
 * \param[out] _right  = 0
 * \param[out] _top    = 0
 * \param[out] _bottom = 0
 */
void eRandR_win32::getMostLeftRightTopBottomCRTC( unsigned int &_left, unsigned int &_right, unsigned int &_top, unsigned int &_bottom ) {
   _left   = 0;
   _right  = 0;
   _top    = 0;
   _bottom = 0;
}


void eRandR_win32::printRandRStatus() {
   iLOG "PRINT: NUM: " ADD vCurrentConfig_eD.size() END
   for ( eDisplays const & d : vCurrentConfig_eD ) {

      unsigned int lWidth_uI, lHeight_uI;
      double       lRate_D;

      d.getCurrentResolution( lWidth_uI, lHeight_uI, lRate_D );

      iLOG "Display: " ADD d.getName()
      POINT "Primary: " ADD d.getIsPrimary()
      POINT "Current: " ADD lWidth_uI ADD "x" ADD lHeight_uI ADD ":" ADD lRate_D
      END

      for ( DEVMODE const & D : d.vModes_V_win32 ) {
         iLOG    "Resolution: " ADD D.dmPelsWidth ADD "x" ADD D.dmPelsHeight ADD ":" ADD D.dmDisplayFrequency END
         iLOG    "DM_POSITION           " ADD D.dmFields &DM_POSITION           ? 1 : 0 ADD ": " ADD D.dmPosition.x ADD "x" ADD D.dmPosition.y
         NEWLINE "DM_BITSPERPEL         " ADD D.dmFields &DM_BITSPERPEL         ? 1 : 0 ADD ": " ADD D.dmBitsPerPel
         NEWLINE "DM_DISPLAYORIENTATION " ADD D.dmFields &DM_DISPLAYORIENTATION ? 1 : 0 ADD ": " ADD D.dmDisplayOrientation
         NEWLINE "DM_DISPLAYFIXEDOUTPUT " ADD D.dmFields &DM_DISPLAYFIXEDOUTPUT ? 1 : 0 ADD ": " ADD D.dmDisplayFixedOutput
         NEWLINE "DM_PELSWIDTH          " ADD D.dmFields &DM_PELSWIDTH          ? 1 : 0 ADD ": " ADD D.dmPelsWidth
         NEWLINE "DM_PELSHEIGHT         " ADD D.dmFields &DM_PELSHEIGHT         ? 1 : 0 ADD ": " ADD D.dmPelsHeight
         NEWLINE "DM_DISPLAYFREQUENCY   " ADD D.dmFields &DM_DISPLAYFREQUENCY   ? 1 : 0 ADD ": " ADD D.dmDisplayFrequency
         NEWLINE "DM_COLOR              " ADD D.dmFields &DM_COLOR              ? 1 : 0 ADD ": " ADD D.dmColor
         NEWLINE "DM_DUPLEX             " ADD D.dmFields &DM_DUPLEX             ? 1 : 0 ADD ": " ADD D.dmDuplex
         NEWLINE "DM_ORIENTATION        " ADD D.dmFields &DM_ORIENTATION        ? 1 : 0 ADD ": " ADD D.dmOrientation
         NEWLINE " "
         END
      }
   }
}

/*!
 * \brief Set the primary display to _disp
 *
 * \param[in] _disp The display to set primary
 *
 * \note This function will change the primary display IMMEDIATELY; Calling applyNewSettings() will have no effect to this.
 *
 * \returns true if everything went fine
 */
bool eRandR_win32::setPrimary( const eDisplays &_disp ) {
   DEVMODE lNotReallyNeeded  = {0};
   lNotReallyNeeded.dmSize   = sizeof( DEVMODE );
   lNotReallyNeeded.dmFields = DM_POSITION; // Set the primary position
   lNotReallyNeeded.dmPosition.x = 0;
   lNotReallyNeeded.dmPosition.y = 0;

   switch ( ChangeDisplaySettingsEx( _disp.getDisplayDevice().DeviceName, &lNotReallyNeeded, NULL, CDS_SET_PRIMARY, NULL ) ) {
      case DISP_CHANGE_SUCCESSFUL :
         iLOG "Successfully set the primary display" END
         return true;

      case DISP_CHANGE_BADDUALVIEW :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW [setPrimary]" END
         return false;

      case DISP_CHANGE_BADFLAGS :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS [setPrimary]" END
         return false;

      case DISP_CHANGE_BADMODE :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE [setPrimary]" END
         return false;

      case DISP_CHANGE_BADPARAM :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM [setPrimary]" END
         return false;

      case DISP_CHANGE_FAILED :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [setPrimary]" END
         return false;

      case DISP_CHANGE_NOTUPDATED :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED [setPrimary]" END
         return false;

      case DISP_CHANGE_RESTART :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [setPrimary] (You need to restart yout PC because you have Windows)" END
         return false;

      default:
         eLOG "ChangeDisplaySettingsEx returned a unknown error [setPrimary]" END
         return false;
   }
}


bool eRandR_win32::restoreScreenDefaults() {
   ChangeDisplaySettings( NULL, 0 );
   
   reload();

   return true;
}

bool eRandR_win32::restoreScreenLatest() {
   return false;
}

bool eRandR_win32::setGamma( const eDisplays &_disp, float _r, float _g, float _b, float _brightness ) {
   return false;
}




} // windows_win32

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

