/*!
 * \file windows/iRandR.cpp
 * \brief \b Classes: \a iRandR
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "iRandR.hpp"
#include "uLog.hpp"
#include "uConfig.hpp"
#include <windows.h>

namespace e_engine {

namespace windows_win32 {

iRandR::iRandR() {
   vDisplaysToChange_eD.clear();
   reload();
   vDefaultConfig_eD = vPreviousConfig_eD = vCurrentConfig_eD;
}

/*!
 * \brief get the current settins
 * \returns Nothing
 */
void iRandR::reload() {
   HWND lDesktopHWND_win32 = GetDesktopWindow();
   RECT lDesktopRect_win32;


   DISPLAY_DEVICEW lDisplayDevice_win32;
   lDisplayDevice_win32.cb = sizeof( DISPLAY_DEVICEW );

   GetWindowRect( lDesktopHWND_win32, &lDesktopRect_win32 );

   vScreenWidth_uI  = lDesktopRect_win32.right - lDesktopRect_win32.left;
   vScreenHeight_uI = lDesktopRect_win32.bottom - lDesktopRect_win32.top;

   vCurrentConfig_eD.clear();

   DEVMODEW lSettings_win32;
   lSettings_win32.dmSize = sizeof( DEVMODEW );

   for ( DWORD lDeviceNum_win32 = 0;
         EnumDisplayDevicesW( NULL, lDeviceNum_win32, &lDisplayDevice_win32, 0 );
         ++lDeviceNum_win32 ) {

      DISPLAY_DEVICEW lDisplayDeviceTemp_win32;
      lDisplayDeviceTemp_win32.cb = sizeof( DISPLAY_DEVICEW );

      for ( DWORD lDisplayNum_win32 = 0; EnumDisplayDevicesW(
                  lDisplayDevice_win32.DeviceName, lDisplayNum_win32, &lDisplayDeviceTemp_win32, 0 );
            ++lDisplayNum_win32 ) {
         bool lIsEnabled_B = false;
         bool lIsPrimary_B = false;

         if ( lDisplayDeviceTemp_win32.StateFlags & DISPLAY_DEVICE_ACTIVE )
            lIsEnabled_B = true;

         if ( lDisplayDeviceTemp_win32.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE )
            lIsPrimary_B = true;

         iDisplays lTempDisplay( lDisplayDevice_win32.DeviceName, lIsEnabled_B, lIsPrimary_B );

         lTempDisplay.setDisplayDevice( lDisplayDevice_win32 );

         EnumDisplaySettingsW(
               lDisplayDevice_win32.DeviceName, ENUM_CURRENT_SETTINGS, &lSettings_win32 );
         lTempDisplay.setCurrentSettings( lSettings_win32 );
         lTempDisplay.setCurrentSizeAndPosition( lSettings_win32.dmPelsWidth,
                                                 lSettings_win32.dmPelsHeight,
                                                 lSettings_win32.dmPosition.x,
                                                 lSettings_win32.dmPosition.y,
                                                 lSettings_win32.dmDisplayFrequency );


         for ( DWORD lModeNum_win32 = 0; EnumDisplaySettingsW(
                     lDisplayDevice_win32.DeviceName, lModeNum_win32, &lSettings_win32 );
               lModeNum_win32++ )
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
void iRandR::getMostLeftRightTopBottomCRTC( unsigned int &_left,
                                            unsigned int &_right,
                                            unsigned int &_top,
                                            unsigned int &_bottom ) {
   _left   = 0;
   _right  = 0;
   _top    = 0;
   _bottom = 0;
}

/*!
 * \brief Prints the possible RandR resolutions
 * \returns Nothing
 */
void iRandR::printRandRStatus() {
#if D_LOG_WINDOWS_RANDR
   iLOG( "PRINT: NUM: ", vCurrentConfig_eD.size() );
   for ( iDisplays const &d : vCurrentConfig_eD ) {

      unsigned int lWidth_uI, lHeight_uI;
      double lRate_D;

      d.getCurrentResolution( lWidth_uI, lHeight_uI, lRate_D );

      dLOG( "Display: ",
            d.getName(),
            "\n  - Primary: ",
            d.getIsPrimary(),
            "\n  - Current: ",
            lWidth_uI,
            "x",
            lHeight_uI,
            ":",
            lRate_D );

      for ( DEVMODEW const &D : d.vModes_V_win32 ) {
         dLOG( "Resolution: ",
               (int32_t)D.dmPelsWidth,
               "x",
               (int32_t)D.dmPelsHeight,
               ":",
               (double)D.dmDisplayFrequency );
         dLOG( "\n  - DM_POSITION           ",
               D.dmFields & DM_POSITION ? 1 : 0,
               ": ",
               (int32_t)D.dmPosition.x,
               "x",
               (int32_t)D.dmPosition.y,
               "\n  - DM_BITSPERPEL         ",
               D.dmFields & DM_BITSPERPEL ? 1 : 0,
               ": ",
               (int32_t)D.dmBitsPerPel,
               "\n  - DM_PELSWIDTH          ",
               D.dmFields & DM_PELSWIDTH ? 1 : 0,
               ": ",
               (int32_t)D.dmPelsWidth,
               "\n  - DM_PELSHEIGHT         ",
               D.dmFields & DM_PELSHEIGHT ? 1 : 0,
               ": ",
               (int32_t)D.dmPelsHeight,
               "\n  - DM_DISPLAYFREQUENCY   ",
               D.dmFields & DM_DISPLAYFREQUENCY ? 1 : 0,
               ": ",
               (double)D.dmDisplayFrequency,
               "\n" );
      }
   }
#endif
}

/*!
 * \brief Set the primary display to _disp
 *
 * \param[in] _disp The display to set primary
 *
 * \note This function will change the primary display IMMEDIATELY; Calling applyNewSettings() will
 *have no effect to this.
 *
 * \returns true if everything went fine
 */
bool iRandR::setPrimary( const iDisplays &_disp ) {
   DEVMODEW lNotReallyNeeded;
   lNotReallyNeeded.dmSize       = sizeof( DEVMODEW );
   lNotReallyNeeded.dmFields     = DM_POSITION; // Set the primary position
   lNotReallyNeeded.dmPosition.x = 0;
   lNotReallyNeeded.dmPosition.y = 0;

   switch ( ChangeDisplaySettingsExW(
         _disp.getDisplayDevice().DeviceName, &lNotReallyNeeded, NULL, CDS_SET_PRIMARY, NULL ) ) {
      case DISP_CHANGE_SUCCESSFUL:
         iLOG( "Successfully set the primary display to ", _disp.getDisplayDevice().DeviceName );
         return true;

      case DISP_CHANGE_BADDUALVIEW:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW [setPrimary]" );
         return false;

      case DISP_CHANGE_BADFLAGS:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS [setPrimary]" );
         return false;

      case DISP_CHANGE_BADMODE:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE [setPrimary]" );
         return false;

      case DISP_CHANGE_BADPARAM:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM [setPrimary]" );
         return false;

      case DISP_CHANGE_FAILED:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [setPrimary]" );
         return false;

      case DISP_CHANGE_NOTUPDATED:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED [setPrimary]" );
         return false;

      case DISP_CHANGE_RESTART:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [setPrimary] (You need to "
               "restart yout PC because you have Windows)" );
         return false;

      default:
         eLOG( "ChangeDisplaySettingsEx returned a unknown error [setPrimary]" );
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
bool iRandR::setDisplaySizes( const iDisplays &_disp ) {
   vDisplaysToChange_eD.push_back( _disp );
   return true;
}

/*!
 * \brief Applies the settings set with setDisplaySizes
 * \returns true is everything went fine and false if not
 */
bool iRandR::applyNewRandRSettings() {
   vPreviousConfig_eD = vCurrentConfig_eD;

   bool lRetrunValue_B = false;

   for ( iDisplays const &disp : vDisplaysToChange_eD ) {
      DEVMODEW lDevmodeToChange_win32 = disp.getSelectedDevmode();

      iLOG( "Change ",
            disp.getDisplayDevice().DeviceName,
            " to: ",
            (int32_t)lDevmodeToChange_win32.dmPelsWidth,
            "x",
            (int32_t)lDevmodeToChange_win32.dmPelsHeight );
      switch ( ChangeDisplaySettingsExW(
            disp.getDisplayDevice().DeviceName,
            &lDevmodeToChange_win32,
            NULL,
            ( CDS_UPDATEREGISTRY | CDS_NORESET ), // We will apply the settings later
            NULL ) ) {
         case DISP_CHANGE_SUCCESSFUL:
            iLOG( "Successfully changed display ", disp.getDisplayDevice().DeviceName, "  [set]" );
            lRetrunValue_B = true;
            break;

         case DISP_CHANGE_BADDUALVIEW:
            wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW [set]" );
            break;

         case DISP_CHANGE_BADFLAGS:
            wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS [set]" );
            break;

         case DISP_CHANGE_BADMODE:
            wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE [set]" );
            break;

         case DISP_CHANGE_BADPARAM:
            wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM [set]" );
            break;

         case DISP_CHANGE_FAILED:
            wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [set]" );
            break;

         case DISP_CHANGE_NOTUPDATED:
            wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED [set]" );
            break;

         case DISP_CHANGE_RESTART:
            wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [set] (You need to restart "
                  "yout PC because you have Windows)" );
            break;

         default: eLOG( "ChangeDisplaySettingsEx returned a unknown error [set]" ); break;
      }
   }

   // Now really apply the settings
   switch ( ChangeDisplaySettingsExW( NULL, NULL, NULL, 0, NULL ) ) {
      case DISP_CHANGE_SUCCESSFUL:
         iLOG( "Successfully applyed display changes [apply]" );
         lRetrunValue_B = true;
         break;

      case DISP_CHANGE_BADDUALVIEW:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADDUALVIEW [apply]" );
         break;

      case DISP_CHANGE_BADFLAGS:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADFLAGS [apply]" );
         break;

      case DISP_CHANGE_BADMODE:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADMODE [apply]" );
         break;

      case DISP_CHANGE_BADPARAM:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_BADPARAM [apply]" );
         break;

      case DISP_CHANGE_FAILED:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [apply]" );
         break;

      case DISP_CHANGE_NOTUPDATED:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_NOTUPDATED [apply]" );
         break;

      case DISP_CHANGE_RESTART:
         wLOG( "ChangeDisplaySettingsEx returned DISP_CHANGE_FAILED [apply] (You need to restart "
               "yout PC because you have Windows)" );
         break;

      default: eLOG( "ChangeDisplaySettingsEx returned a unknown error [apply]" ); break;
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
bool iRandR::restoreScreenDefaults() {
   vDisplaysToChange_eD = vDefaultConfig_eD;
   applyNewRandRSettings();
   return true;
}

/*!
 * \brief Restore the latest screen config
 *
 * \returns true
 */
bool iRandR::restoreScreenLatest() {
   vDisplaysToChange_eD = vPreviousConfig_eD;
   applyNewRandRSettings();
   return true;
}

/*!
 * \brief Gamma is under Windows not supported
 * \todo Find a solution with gamma
 * \returns false
 */
bool iRandR::setGamma( const iDisplays &, float, float, float, float ) { return false; }

iRandR::~iRandR() {
   if ( GlobConf.win.restoreOldScreenRes )
      restoreScreenDefaults();
}



} // windows_win32

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
