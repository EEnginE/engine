/*!
 * \file x11/RandR/iRandR.cpp
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

#if D_LOG_XRANDR
#define dRandLog(...) dLOG(__VA_ARGS__)
#else
#define dRandLog(...)
#endif

using namespace e_engine;
using namespace unix_x11;


//   _____      _ _       _____          _    ______          _
//  |_   _|    (_) |     |  ___|        | |   | ___ \        | |
//    | | _ __  _| |_    | |__ _ __   __| |   | |_/ /___  ___| |_ ___  _ __ ___
//    | || '_ \| | __|   |  __| '_ \ / _` |   |    // _ \/ __| __/ _ \| '__/ _ \
//   _| || | | | | |_ _  | |__| | | | (_| |_  | |\ \  __/\__ \ || (_) | | |  __/
//   \___/_| |_|_|\__( ) \____/_| |_|\__,_( ) \_| \_\___||___/\__\___/|_|  \___|
//                   |/                   |/
//


/*!
 * \brief sets only some basic values.
 */
iRandR::iRandR(xcb_connection_t *_connection, xcb_window_t _rootWin) {
  vConnection_XCB = _connection;
  vRootWindow_XCB = _rootWin;

  auto *lExtReply = xcb_get_extension_data(vConnection_XCB, &xcb_randr_id);
  if (lExtReply->present) {
    dRandLog("RandR extension present");
  }

  if (lExtReply->present) {
    auto lCookie       = xcb_randr_query_version(vConnection_XCB, 100, 100);
    auto lVersionReply = xcb_randr_query_version_reply(vConnection_XCB, lCookie, nullptr);

    if (!lVersionReply) {
      eLOG("RandR: Failed to determine the RandR version: xcb_randr_query_version returned NULL.");
      vRandRVersionMajor_I = UINT32_MAX;
      vRandRVersionMinor_I = UINT32_MAX;
    } else {
      vRandRVersionMajor_I = lVersionReply->major_version;
      vRandRVersionMinor_I = lVersionReply->minor_version;
      free(lVersionReply);
    }

    vIsRandRSupported_B = true;
  } else {
    wLOG("X11 RandR standard not supported. Screen resolution wont be changed");
    vIsRandRSupported_B = false;
    return;
  }

  if (!reload(true, true))
    return;

  vIsRandRSupported_B = true;

#if D_LOG_XRANDR
  printStatus();
#endif
}

iRandR::~iRandR() {
  if (!vIsRandRSupported_B)
    return;

  if (GlobConf.win.restoreOldScreenRes && vWasScreenChanged_B)
    restore(vDefaultConfig_RandR);

  vDefaultConfig_RandR.gamma.clear();
  vDefaultConfig_RandR.CRTCInfo.clear();
  vLatestConfig_RandR.gamma.clear();

  if (vScreenInfo_XCB)
    free(vScreenInfo_XCB);

  if (vScreenResources_XCB)
    free(vScreenResources_XCB);

  vScreenInfo_XCB      = nullptr;
  vScreenResources_XCB = nullptr;

  vCRTC_V_RandR.clear();
  vOutput_V_RandR.clear();
  vMode_V_RandR.clear();

  vIsRandRSupported_B = false;
}

iRandR::ERROR_CODE iRandR::restore(internal::_config _conf) {
  if (!isProtocolSupported())
    return RANDR_NOT_SUPPORTED;

  vChangeCRTC_V_RandR.clear();
  vChangeCRTC_V_RandR = _conf.CRTCInfo;

  auto lError = applyNewRandRSettings();
  if (lError != OK)
    return lError;

  // Who is primary?
  xcb_randr_set_output_primary(vConnection_XCB, vRootWindow_XCB, _conf.primary);


  // Reset gamma
  if (_conf.gamma.size() != vCRTC_V_RandR.size()) {
    eLOG("RandR: Fatal internal ERROR:  _conf.gamma.size() != vCRTC_V_RandR.size()");
    return RANDR_OTHER_ERROR;
  }

  for (auto &i : _conf.CRTCInfo) {
    for (auto &j : _conf.gamma) {
      if (j->crtc == i.id) {
        xcb_randr_set_crtc_gamma(vConnection_XCB, j->crtc, j->size, j->red, j->green, j->blue);
        break;
      }
    }
  }

  return OK;
}


//   _____      _    ______ _           _               _____ _
//  |  __ \    | |   |  _  (_)         | |             /  ___(_)
//  | |  \/ ___| |_  | | | |_ ___ _ __ | | __ _ _   _  \ `--. _ _______  ___
//  | | __ / _ \ __| | | | | / __| '_ \| |/ _` | | | |  `--. \ |_  / _ \/ __|
//  | |_\ \  __/ |_  | |/ /| \__ \ |_) | | (_| | |_| | /\__/ / |/ /  __/\__ \
//   \____/\___|\__| |___/ |_|___/ .__/|_|\__,_|\__, | \____/|_/___\___||___/
//                               | |             __/ |
//                               |_|            |___/

/*!
 * \brief Get all display information in a STL vector
 *
 * \returns a STL vector, containing all important display data
 * \throws Nothing
 *
 * \sa iRandRDisplay
 */
std::vector<std::shared_ptr<iDisplayBasic>> iRandR::getDisplayResolutions() {
  std::vector<std::shared_ptr<iDisplayBasic>> lResult;
  if (!isProtocolSupported())
    return lResult;

  reload(false);

  vMode_V_RandR.sort();

  for (internal::_output const &fOutput : vOutput_V_RandR) {
    if (fOutput.connection != 0)
      continue;

    std::shared_ptr<iDisplayRandR> lDsiplay =
        std::make_shared<iDisplayRandR>(fOutput.name, fOutput.id, (fOutput.crtc != XCB_NONE) ? true : false);

    if (fOutput.crtc != XCB_NONE) {

      // Find the CRTC of the output
      for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
        if (fOutput.crtc == fCRTC.id) {

          for (internal::_mode const &fMode : vMode_V_RandR) {
            if (fCRTC.mode == fMode.id) {
              lDsiplay->setCurrentSizeAndPosition(fMode.width,
                                                  fMode.height,
                                                  static_cast<unsigned>(fCRTC.posX),
                                                  static_cast<unsigned>(fCRTC.posY),
                                                  static_cast<unsigned>(fMode.refresh));
              break;
            }
          }

          // Clones
          for (auto const &fClone : fCRTC.outputs) {
            if (fClone != fOutput.id)
              lDsiplay->addClone(fClone);
          }

          break;
        }
      }
    }

    for (internal::_mode const &fMode : vMode_V_RandR) {
      // Is mode supported and / or preferred ?
      bool lModeSupported_B = false;
      bool lModePrefered_B  = false;
      for (unsigned int j = 0; j < fOutput.modes.size(); ++j) {
        if (fOutput.modes[j] == fMode.id) {
          lModeSupported_B = true;
          if (j == static_cast<unsigned>(fOutput.npreferred) - 1) {
            lModePrefered_B = true;
          }
          break;
        }
      }

      if (!lModeSupported_B)
        continue;

      lDsiplay->addMode(fMode.id, lModePrefered_B, fMode.width, fMode.height, fMode.refresh);
    }

    lResult.emplace_back(lDsiplay);
  }

  return lResult;
}

//  ______     _
//  | ___ \   (_)
//  | |_/ / __ _ _ __ ___   __ _ _ __ _   _
//  |  __/ '__| | '_ ` _ \ / _` | '__| | | |
//  | |  | |  | | | | | | | (_| | |  | |_| |
//  \_|  |_|  |_|_| |_| |_|\__,_|_|   \__, |
//                                     __/ |
//                                    |___/

/*!
 * \brief Set _disp to the primary display
 *
 * \param _disp The display that is to be set to primary
 *
 * \returns true if everything went fine
 *
 * \note This function will change the primary display IMMEDIATELY; Calling applyNewSettings() will
 *have no effect to this.
 */
iRandR::ERROR_CODE iRandR::setPrimary(iDisplayBasic *_disp) {
  if (!isProtocolSupported())
    return RANDR_NOT_SUPPORTED;

  iDisplayRandR *lDisp = dynamic_cast<iDisplayRandR *>(_disp);
  if (!lDisp)
    return INVALID_DISPLAY_CLASS;

  xcb_randr_set_output_primary(vConnection_XCB, vRootWindow_XCB, lDisp->getOutput());
  return OK;
}



//   _____      _     _____ ______ _____ _____      __       _ _
//  |  __ \    | |   /  __ \| ___ \_   _/  __ \_   / _|     | | |
//  | |  \/ ___| |_  | /  \/| |_/ / | | | /  \(_) | |_ _   _| | |___  ___ _ __ ___  ___ _ __
//  | | __ / _ \ __| | |    |    /  | | | |       |  _| | | | | / __|/ __| '__/ _ \/ _ \ '_ \
//  | |_\ \  __/ |_  | \__/\| |\ \  | | | \__/\_  | | | |_| | | \__ \ (__| | |  __/  __/ | | |
//   \____/\___|\__|  \____/\_| \_| \_/  \____( ) |_|  \__,_|_|_|___/\___|_|  \___|\___|_| |_|
//                                            |/
//


/*!
 * \brief Find the most left, right, top, bottom CRTC
 *
 * \param[out] _left   Index of the most left CRTC
 * \param[out] _right  Index of the most right CRTC
 * \param[out] _top    Index of the most top CRTC
 * \param[out] _bottom Index of the most bottom CRTC
 */
void iRandR::getMostLeftRightTopBottomCRTC(unsigned int &_left,
                                           unsigned int &_right,
                                           unsigned int &_top,
                                           unsigned int &_bottom) {
  reload();

  int lMinX = -1;
  int lMaxX = -1;
  int lMinY = -1;
  int lMaxY = -1;

  _left = _right = _top = _bottom = 0;

  for (unsigned int index = 0; index < vCRTC_V_RandR.size(); ++index) {
    if (vCRTC_V_RandR[index].posX < lMinX || lMinX == -1) {
      _left = index;
      lMinX = vCRTC_V_RandR[index].posX;
    }

    if (vCRTC_V_RandR[index].posY < lMinY || lMinY == -1) {
      _top  = index;
      lMinY = vCRTC_V_RandR[index].posY;
    }

    if (vCRTC_V_RandR[index].posX > lMaxX || lMaxX == -1) {
      _right = index;
      lMaxX  = vCRTC_V_RandR[index].posX;
    }

    if (vCRTC_V_RandR[index].posY > lMaxY || lMaxY == -1) {
      _bottom = index;
      lMaxY   = vCRTC_V_RandR[index].posY;
    }
  }
}

/*!
 * \brief Get the index of a iRandRDisplay
 * \returns The index of a iRandRDisplay
 * \returns -1 When the display is disabled
 * \returns -2 When the iRandRDisplay is out of date
 * \returns -10 When an impossible error happened
 */
iRandR::ERROR_CODE iRandR::getIndexOfDisplay(iDisplayBasic *_disp, uint32_t *index) {
  iDisplayRandR *dispRandR = dynamic_cast<iDisplayRandR *>(_disp);
  if (!dispRandR)
    return INVALID_DISPLAY_CLASS;

  if (!index)
    return INVALID_ARGUMENT;

  reload();

  xcb_randr_crtc_t lCRTC_XCB    = XCB_NONE;
  bool             lOutputFound = false;

  for (internal::_output fOut : vOutput_V_RandR) {
    if (dispRandR->getOutput() == fOut.id) {
      lOutputFound = true;
      lCRTC_XCB    = fOut.crtc;
      break;
    }
  }

  // Invalid iRandRDisplay. (Should never happen)
  if (!lOutputFound)
    return RANDR_OTHER_ERROR;

  // The output is not connected or disabled
  if (lCRTC_XCB == XCB_NONE)
    return RANDR_CRTC_NOT_FOUND;


  for (uint32_t i = 0; i < vCRTC_V_RandR.size(); ++i) {
    if (vCRTC_V_RandR[i].id == lCRTC_XCB) {
      *index = i; // The index
      return OK;
    }
  }

  // Well... this should be impossible, because we have a reload() at top of this function!
  eLOG("Reached (theoretically) unreachable code");
  return RANDR_OTHER_ERROR;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
