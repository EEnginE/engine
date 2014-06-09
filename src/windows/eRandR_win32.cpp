/*!
 * \file windows/eRandR_win32.cpp
 * \brief \b Classes: \a eRandR_win32
 */

#include "eRandR_win32.hpp"
#include "log.hpp"
#include "eWindowData.hpp"
#include <windows.h>

namespace e_engine {

namespace windows_win32 {

eRandR_win32::eRandR_win32() {
   vDisplaysToChange_eD.clear();
   reload();
   vDefaultConfig_eD = vPreviousConfig_eD = vCurrentConfig_eD;
}

/*!
 * \brief get the current settins
 * \returns Nothing
 */
void eRandR_win32::reload() {
   HWND lDesktopHWND_win32 = GetDesktopWindow();
   RECT lDesktopRect_win32;


   DISPLAY_DEVICEW lDisplayDevice_win32;
   lDisplayDevice_win32.cb = sizeof( DISPLAY_DEVICEW );

   GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );

   vScreenWidth_uI  = lDesktopRect_win32.right  - lDesktopRect_win32.left;
   vScreenHeight_uI = lDesktopRect_win32.bottom - lDesktopRect_win32.top;

   vCurrentConfig_eD.clear();

   DEVMODEW lSettings_win32 = {0};
   lSettings_win32.dmSize = sizeof( DEVMODEW );

   for ( DWORD lDeviceNum_win32 = 0; EnumDisplayDevicesW( NULL, lDeviceNum_win32, &lDisplayDevice_win32, 0 ); ++lDeviceNum_win32 ) {

      DISPLAY_DEVICEW lDisplayDeviceTemp_win32;
      lDisplayDeviceTemp_win32.cb = sizeof( DISPLAY_DEVICEW );

      for (
         DWORD lDisplayNum_win32 = 0;
         EnumDisplayDevicesW( lDisplayDevice_win32.DeviceName, lDisplayNum_win32, &lDisplayDeviceTemp_win32, 0 );
         ++lDisplayNum_win32
      ) {
         bool lIsEnabled_B = false;
         bool lIsPrimary_B = false;

         if ( lDisplayDeviceTemp_win32.StateFlags & DISPLAY_DEVICE_ACTIVE )
            lIsEnabled_B = true;

         if ( lDisplayDeviceTemp_win32.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
            lIsPrimary_B = true;

         eDisplays lTempDisplay( lDisplayDevice_win32.DeviceName, lIsEnabled_B, lIsPrimary_B );

         lTempDisplay.setDisplayDevice( lDisplayDevice_win32 );

         EnumDisplaySettingsW( lDisplayDevice_win32.DeviceName, ENUM_CURRENT_SETTINGS, &lSettings_win32 );
         lTempDisplay.setCurrentSettings( lSettings_win32 );
         lTempDisplay.setCurrentSizeAndPosition(
            lSettings_win32.dmPelsWidth,
            lSettings_win32.dmPelsHeight,
            lSettings_win32.dmPosition.x,
            lSettings_win32.dmPosition.y,
            lSettings_win32.dmDisplayFrequency
         );


         for ( DWORD lModeNum_win32 = 0; EnumDisplaySettingsW( lDisplayDevice_win32.DeviceName, lModeNum_win32, &lSettings_win32 ); lModeNum_win32++ )
            lTempDisplay.addMode( lSettings_win32 );

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

/*!
 * \brief Prints the possible RandR resolutions
 * \returns Nothing
 */
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

      for ( DEVMODEW const & D : d.vModes_V_win32 ) {
         iLOG    "Resolution: " ADD D.dmPelsWidth ADD "x" ADD D.dmPelsHeight ADD ":" ADD D.dmDisplayFrequency END
         iLOG    "DM_POSITION           " ADD D.dmFields &DM_POSITION           ? 1 : 0 ADD ": " ADD D.dmPosition.x ADD "x" ADD D.dmPosition.y
         NEWLINE "DM_BITSPERPEL         " ADD D.dmFields &DM_BITSPERPEL         ? 1 : 0 ADD ": " ADD D.dmBitsPerPel
         NEWLINE "DM_PELSWIDTH          " ADD D.dmFields &DM_PELSWIDTH          ? 1 : 0 ADD ": " ADD D.dmPelsWidth
         NEWLINE "DM_PELSHEIGHT         " ADD D.dmFields &DM_PELSHEIGHT         ? 1 : 0 ADD ": " ADD D.dmPelsHeight
         NEWLINE "DM_DISPLAYFREQUENCY   " ADD D.dmFields &DM_DISPLAYFREQUENCY   ? 1 : 0 ADD ": " ADD D.dmDisplayFrequency
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
   DEVMODEW lNotReallyNeeded  = {0};
   lNotReallyNeeded.dmSize   = sizeof( DEVMODEW );
   lNotReallyNeeded.dmFields = DM_POSITION; // Set the primary position
   lNotReallyNeeded.dmPosition.x = 0;
   lNotReallyNeeded.dmPosition.y = 0;

   switch ( ChangeDisplaySettingsExW( _disp.getDisplayDevice().DeviceName, &lNotReallyNeeded, NULL, CDS_SET_PRIMARY, NULL ) ) {
      case DISP_CHANGE_SUCCESSFUL :
         iLOG "Successfully set the primary display to " ADD _disp.getDisplayDevice().DeviceName END
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
   return false;
}

/*!
 * \brief Sets the Display config
 *
 * \param[in] _disp The new display config
 * \returns true
 */
bool eRandR_win32::setDisplaySizes( const eDisplays &_disp ) {
   vDisplaysToChange_eD.push_back( _disp );
   return true;
}

/*!
 * \brief Applies the settings set with setDisplaySizes
 * \returns true is everything went fine and false if not
 */
bool eRandR_win32::applyNewRandRSettings() {
   vPreviousConfig_eD = vCurrentConfig_eD;

   bool lRetrunValue_B = false;

   for ( eDisplays const & disp : vDisplaysToChange_eD ) {
      DEVMODEW lDevmodeToChange_win32 = disp.getSelectedDevmode();

      iLOG "Change " ADD disp.getDisplayDevice().DeviceName ADD " to: " ADD lDevmodeToChange_win32.dmPelsWidth ADD "x" ADD lDevmodeToChange_win32.dmPelsHeight END
      switch (
         ChangeDisplaySettingsExW(
            disp.getDisplayDevice().DeviceName,
            &lDevmodeToChange_win32,
            NULL,
            ( CDS_UPDATEREGISTRY | CDS_NORESET ), // We will apply the settings later
            NULL )
      ) {
         case DISP_CHANGE_SUCCESSFUL :
            iLOG "Successfully changed display " ADD disp.getDisplayDevice().DeviceName ADD "  [set]" END
            lRetrunValue_B = true;
            break;

         case DISP_CHANGE_BADDUALVIEW :
            wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW [set]" END
            break;

         case DISP_CHANGE_BADFLAGS :
            wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS [set]" END
            break;

         case DISP_CHANGE_BADMODE :
            wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE [set]" END
            break;

         case DISP_CHANGE_BADPARAM :
            wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM [set]" END
            break;

         case DISP_CHANGE_FAILED :
            wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [set]" END
            break;

         case DISP_CHANGE_NOTUPDATED :
            wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED [set]" END
            break;

         case DISP_CHANGE_RESTART :
            wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [set] (You need to restart yout PC because you have Windows)" END
            break;

         default:
            eLOG "ChangeDisplaySettingsEx returned a unknown error [set]" END
            break;
      }
   }

   // Now really apply the settings
   switch ( ChangeDisplaySettingsExW( NULL, NULL, NULL, 0, NULL ) ) {
      case DISP_CHANGE_SUCCESSFUL :
         iLOG "Successfully applyed display changes [apply]" END
         lRetrunValue_B = true;
         break;

      case DISP_CHANGE_BADDUALVIEW :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW [apply]" END
         break;

      case DISP_CHANGE_BADFLAGS :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS [apply]" END
         break;

      case DISP_CHANGE_BADMODE :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE [apply]" END
         break;

      case DISP_CHANGE_BADPARAM :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM [apply]" END
         break;

      case DISP_CHANGE_FAILED :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [apply]" END
         break;

      case DISP_CHANGE_NOTUPDATED :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED [apply]" END
         break;

      case DISP_CHANGE_RESTART :
         wLOG "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [apply] (You need to restart yout PC because you have Windows)" END
         break;

      default:
         eLOG "ChangeDisplaySettingsEx returned a unknown error [apply]" END
         break;
   }

   vDisplaysToChange_eD.clear();

   reload();
   return lRetrunValue_B;
}


/*!
 * \brief Resets every display to its defaults
 *
 * \returns true
 */
bool eRandR_win32::restoreScreenDefaults() {
   vDisplaysToChange_eD = vDefaultConfig_eD;
   applyNewRandRSettings();
   return true;
}

/*!
 * \brief Restore the latest screen config
 *
 * \returns true
 */
bool eRandR_win32::restoreScreenLatest() {
   vDisplaysToChange_eD = vPreviousConfig_eD;
   applyNewRandRSettings();
   return true;
}

/*!
 * \brief Gamma is under Windows not supported
 * \todo Find a solution with gamma
 * \returns false
 */
bool eRandR_win32::setGamma( const eDisplays &_disp, float _r, float _g, float _b, float _brightness ) {
   return false;
}

eRandR_win32::~eRandR_win32() {
   if ( WinData.win.restoreOldScreenRes )
      restoreScreenDefaults();
}



} // windows_win32

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 


