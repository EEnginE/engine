/*!
 * \file x11/RandR/iRandR_reload.cpp
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

#include "defines.hpp"
#include "uLog.hpp"
#include "iRandR.hpp"
#include "eCMDColor.hpp"

using namespace e_engine;
using namespace unix_x11;

namespace {

/*!
 * \brief Fill a string with chars until it has a specific size.
 *
 * \param _str  [in] The string to resize
 * \param _size [in] The wished size
 * \param _fill [in] Fill it with these chars
 *
 * \returns The result
 */
std::string placeStringLeft(std::string _str, unsigned int _size, char _fill) {
  if (_str.size() == _size)
    return _str;

  std::string lResult_str = _str;
  if (lResult_str.size() > _size) {
    lResult_str.resize(_size);
    return lResult_str;
  }

  lResult_str.append(_size - lResult_str.size(), _fill);
  return lResult_str;
}
}

/*!
 * \brief Prints a (more or less) fancy table with all XRandR information
 */
void iRandR::printStatus() {
#if D_LOG_XRANDR

  if (!vIsRandRSupported_B)
    return;

  if (!vScreenResources_XCB || !vScreenInfo_XCB)
    return;

  reload(false);

  std::wstring lOFF_C = eCMDColor::color('O', 'W');
  std::wstring lBW_C  = eCMDColor::color('B', 'W');
  std::wstring lBR_C  = eCMDColor::color('B', 'R');
  std::wstring lBG_C  = eCMDColor::color('B', 'G');
  std::wstring lBB_C  = eCMDColor::color('B', 'B');
  std::wstring lBC_C  = eCMDColor::color('B', 'C');

  std::wstring lR_C = eCMDColor::color('O', 'R');
  std::wstring lG_C = eCMDColor::color('O', 'G');
  std::wstring lB_C = eCMDColor::color('O', 'B');
  std::wstring lC_C = eCMDColor::color('O', 'C');

  //
  //   -- HEADDER
  //
  //    |============|======|=========|===========|============|=========================|
  //    |   Output   | CRTC | Primary | Connected |  Position  |          MODE           |
  //    |            |      |         |           |            |  Resolutions  |  Rates  |
  //    |------------|------|---------|-----------|------------|---------------|---------|
  iLOG("RandR Info:");
  LOG(_hI, "  - Screen Size: ", lBG_C, vScreenWidth_uI, 'x', vScreenHeight_uI);
  LOG(_hI, "  - Num CRTCs:   ", lBG_C, vScreenResources_XCB->num_crtcs, "\n");

  LOG(_hD, "|============|======|=========|===========|============|=========================|");
  LOG(_hD,
      "|   ",
      lBW_C,
      "Output",
      lOFF_C,
      "   | ",
      lBW_C,
      "CRTC",
      lOFF_C,
      " | ",
      lBW_C,
      "Primary",
      lOFF_C,
      " | ",
      lBW_C,
      "Connected",
      lOFF_C,
      " |  ",
      lBW_C,
      "Position",
      lOFF_C,
      "  |          ",
      lBW_C,
      "MODE",
      lOFF_C,
      "           |");
  LOG(_hD,
      "|            |      |         |           |            |  ",
      lBW_C,
      "Resolutions",
      lOFF_C,
      "  |  ",
      lBW_C,
      "Rates",
      lOFF_C,
      "  |");
  LOG(_hD, "|------------|------|---------|-----------|------------|---------------|---------|");
  //
  //   -- Entries
  //
  for (internal::_output const &fOutput : vOutput_V_RandR) {
    internal::_crtc lCRTC_RandR;

    std::string lCRTC_str      = (fOutput.crtc == 0) ? "OFF" : std::to_string(fOutput.crtc);
    std::string lPrimary_str   = (vLatestConfig_RandR.primary == fOutput.id) ? "YES" : "NO";
    std::string lConnected_str = (fOutput.connection == 0) ? "YES" : (fOutput.connection == 2) ? "???" : "NO";
    std::string lPosition_str  = "   NONE";

    char lBold_C = (fOutput.connection == 0) ? 'B' : 'O';
    char lCRTC_C = (lCRTC_str != "OFF") ? 'G' : 'R';

    if (!(fOutput.crtc == 0)) {
      for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
        if (fCRTC.id == fOutput.crtc) {
          lCRTC_RandR   = fCRTC;
          lPosition_str = "";
          if (fCRTC.posX >= 0)
            lPosition_str += '+';

          lPosition_str += std::to_string(fCRTC.posX);

          if (fCRTC.posY >= 0)
            lPosition_str += '+';

          lPosition_str += std::to_string(fCRTC.posY);
          break;
        }
      }
    }

    std::wstring lCO_C = eCMDColor::color('O', 'W');
    std::wstring lC1_C = eCMDColor::color(lBold_C, 'W');
    std::wstring lC2_C = eCMDColor::color(lBold_C, lCRTC_C);

    if (lBold_C == 'B') {
      LOG(_hD,
          "|            |      |         |           |            |               |       "
          "  |");
    }


    if (lBold_C == 'O' || fOutput.modes.size() == 0) {
      LOG(_hD,
          "| ",
          lC1_C,
          placeStringLeft(fOutput.name, 11, ' '),
          lCO_C,
          "| ",
          lC2_C,
          placeStringLeft(lCRTC_str, 5, ' '),
          lCO_C,
          "|   ",
          lC1_C,
          placeStringLeft(lPrimary_str, 6, ' '),
          lCO_C,
          "|    ",
          lC1_C,
          placeStringLeft(lConnected_str, 7, ' '),
          lCO_C,
          "| ",
          lC1_C,
          placeStringLeft(lPosition_str, 11, ' '),
          lCO_C,
          "|    OFFLINE    |   OFF   |");
    }

    unsigned int lWidth_uI  = 0;
    unsigned int lHeight_uI = 0;

    std::string lModeSize_str;
    // Unfortunately std::to_string doesn't support precision, so we must use sprintf if we want
    // to avoid the slow stringstreams
    char        lModeFreq_CSTR[15];
    std::string lModeFreq_str;

    bool lIsFirstModePrinted_B = true;

    //
    //   -- Modes
    //
    for (internal::_mode const &fMode : vMode_V_RandR) {
      bool         lFoundMode_B    = false;
      bool         lModePrefered_B = false;
      char         lAtrib_C        = 'O';
      char         lColor_C        = 'W';
      unsigned int lModeCounter_uI = 0; //!< Needed for preferred check

      // Check if the mode is supported by the output
      for (xcb_randr_mode_t const &fTempMode : fOutput.modes) {
        ++lModeCounter_uI;
        if (fTempMode == fMode.id) {
          lFoundMode_B = true;

          if (lModeCounter_uI == static_cast<unsigned int>(fOutput.npreferred))
            lModePrefered_B = true;

          break;
        }
      }

      if (!lFoundMode_B)
        continue;

      if (fMode.width == lWidth_uI && fMode.height == lHeight_uI) {
        lModeSize_str.clear();
      } else {
        lWidth_uI     = fMode.width;
        lHeight_uI    = fMode.height;
        lModeSize_str = std::to_string(lWidth_uI) + 'x' + std::to_string(lHeight_uI);
      }

      snprintf(lModeFreq_CSTR, 15, "%.2f", fMode.refresh);
      lModeFreq_str = lModeFreq_CSTR;

      // This should never happen
      if (!lFoundMode_B) {
        lModeSize_str = "RandR";
        lModeFreq_str = "ERROR";
      }

      // This is a (!) mode with the current width and height of the CRTC
      if (lWidth_uI == lCRTC_RandR.width && lHeight_uI == lCRTC_RandR.height) {
        lAtrib_C = 'B';
        if (!lModeSize_str.empty())
          lModeSize_str += '*';
      }

      // This is the (!) mode of the CRTC
      if (fMode.id == lCRTC_RandR.mode) {
        lAtrib_C = 'B';
        lModeFreq_str += '*';
      }

      // This is the preferred mode
      if (lModePrefered_B) {
        lColor_C = 'G';
        lModeFreq_str += '+';
      }

      std::wstring lC3_C = eCMDColor::color(lAtrib_C, lColor_C);

      if (!lIsFirstModePrinted_B) {
        LOG(_hD,
            "|            |      |         |           |            |   ",
            lC3_C,
            placeStringLeft(lModeSize_str, 12, ' '),
            lCO_C,
            "| ",
            lC3_C,
            placeStringLeft(lModeFreq_str, 8, ' '),
            lCO_C,
            '|');
      } else {
        LOG(_hD,
            "| ",
            lC1_C,
            placeStringLeft(fOutput.name, 11, ' '),
            lCO_C,
            "| ",
            lC2_C,
            placeStringLeft(lCRTC_str, 5, ' '),
            lCO_C,
            "|   ",
            lC1_C,
            placeStringLeft(lPrimary_str, 6, ' '),
            lCO_C,
            "|    ",
            lC1_C,
            placeStringLeft(lConnected_str, 7, ' '),
            lCO_C,
            "| ",
            lC1_C,
            placeStringLeft(lPosition_str, 11, ' '),
            lCO_C,
            "|   ",
            lC3_C,
            placeStringLeft(lModeSize_str, 12, ' '),
            lCO_C,
            "| ",
            lC3_C,
            placeStringLeft(lModeFreq_str, 8, ' '),
            lCO_C,
            '|');
      }

      lIsFirstModePrinted_B = false;
    }
  }


  LOG(_hD, "|============|======|=========|===========|============|=========================|\n\n");
#endif // D_LOG_XRANDR
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
