/*!
 * \file x11/RandR/iDisplayRandR.cpp
 * \brief \b Classes: \a iDisplayRandR
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

#include "iDisplayRandR.hpp"
#include <limits>

namespace e_engine {

namespace unix_x11 {

iDisplayRandR::~iDisplayRandR() {}

iDisplayRandR::iDisplayRandR(const iDisplayRandR &&_e) {
  vModes_V_mode = std::move(_e.vModes_V_mode);
  vClones_V_XRR = std::move(_e.vClones_V_XRR);

  vID_XRR        = _e.vID_XRR;
  vModeToUse_XRR = _e.vModeToUse_XRR;
}

iDisplayRandR &iDisplayRandR::operator=(const iDisplayRandR &&_e) {
  vModes_V_mode = std::move(_e.vModes_V_mode);
  vClones_V_XRR = std::move(_e.vClones_V_XRR);

  vID_XRR        = _e.vID_XRR;
  vModeToUse_XRR = _e.vModeToUse_XRR;
  return *this;
}

// --- private ---
void iDisplayRandR::addClone(xcb_randr_output_t _clone) { vClones_V_XRR.push_back(_clone); }

// --- private ---
void iDisplayRandR::addMode(
    xcb_randr_mode_t _id, bool _prefered, unsigned int _width, unsigned int _height, double _rate) {
  mode lTempMode_mode;

  lTempMode_mode.id = _id;

  lTempMode_mode.prefered = _prefered;

  lTempMode_mode.width  = _width;
  lTempMode_mode.height = _height;
  lTempMode_mode.rate   = _rate;

  vModes_V_mode.push_back(lTempMode_mode);
}

/*!
 * \brief Selects the best mode for this display
 *
 * If it is impossible to find any matching mode, the display will be
 * disabled
 *
 * \note This function will be automatically run if you enable() a display
 *       which was disabled before.
 */
void iDisplayRandR::autoSelectBest() {
  // Check if there is a preferred mode
  for (iDisplayRandR::mode const &fMode : vModes_V_mode) {
    if (fMode.prefered) {
      vModeToUse_XRR = fMode.id;
      return; // We have everything we need
    }
  }

  unsigned int lMaxWidth_uI  = 0; //!< Max currently found width
  unsigned int lMaxHeight_uI = 0; //!< Max currently found height

  for (iDisplayRandR::mode const &fMode : vModes_V_mode) {
    if ((lMaxWidth_uI < fMode.width) && (lMaxHeight_uI < fMode.height)) {
      vCurrentWidth_uI = lMaxWidth_uI = fMode.width;
      vCurrentHeight_uI = lMaxHeight_uI = fMode.height;
    }
  }

  if (std::abs(autoSelectBySize(lMaxWidth_uI, lMaxHeight_uI)) < std::numeric_limits<double>::epsilon()) {
    // There are no modes which can be used
    vEnabled_B     = false;
    vModeToUse_XRR = XCB_NONE;
  }
}



/*!
 * \brief Finds the display frequency most close to _rate with the _width and _height
 *
 * [private]
 *
 * \param[in]  _rate   Find the most closest display frequency to this rate
 * \param[in]  _width  The display mut have this width
 * \param[in]  _height The display mut have this height
 * \param[out] _mode   The found mode
 * \param[out] _diff   The difference between _rate and the final display frequency
 *
 * \returns the display frequency closest to _rate
 */
double iDisplayRandR::findNearestFreqTo(
    double _rate, unsigned int _width, unsigned int _height, xcb_randr_mode_t &_mode, double &_diff) const {
  _diff          = 1000000;
  double lRate_D = -1;
  for (iDisplayRandR::mode const &fMode : vModes_V_mode) {
    if (_width == fMode.width && _height == fMode.height) {
      if (std::abs(_rate - fMode.rate) < std::numeric_limits<double>::epsilon()) {
        _diff = 0;
        _mode = fMode.id;
        return fMode.rate;
      } else if ((_rate - fMode.rate) > 0) {
        if ((_rate - fMode.rate) < _diff) {
          _diff   = (_rate - fMode.rate);
          _mode   = fMode.id;
          lRate_D = fMode.rate;
        }
      } else if ((fMode.rate - _rate) < _diff) {
        _diff   = (fMode.rate - _rate);
        _mode   = fMode.id;
        lRate_D = fMode.rate;
      }
    }
  }
  return lRate_D;
}

/*!
 * \brief Selects the best mode for the resolution _width x _height
 *
 * If _preferedRate == 0:
 *
 * If the function can find a preferred mode with the resolution, it will choose this mode,
 * else the function will look for the mode, nearest to 60 Hz, 120 Hz, 240 Hz and 480 Hz.
 * Then it compares the differences between all 4 modes.
 * The mode with the least difference will be chosen.
 *
 *
 * If _preferedRate > 0:
 *
 * The function will look for the mode, nearest to _preferedRate. If the difference
 * between _preferedRate and the real value is greater than _maxDiff, the function
 * will fall back to the behavior from above.
 *
 * \warning If _preferedRate and/or _maxDiff are < 0 they will be multiplied with -1!!
 *
 * \param[in] _width        The width  the mode must have.
 * \param[in] _height       The height the mode must have.
 * \param[in] _preferedRate Choose the mode nearest to \a _preferedRate.
 * \param[in] _maxDiff      The difference between \a _preferedRate and the best chosen rate must be
 *less than \a _maxDiff
 *
 * \returns The rate of the chosen mode or 0 if no mode could be found.
 *          If _preferedRate failed but a mode with the "normal" behavior
 *          was found, the rate will be negative.
 */
double iDisplayRandR::autoSelectBySize(unsigned int _width,
                                       unsigned int _height,
                                       double       _preferedRate,
                                       double       _maxDiff) {
  double lMinDiffTo60Hz_D;
  double lMinDiffTo120Hz_D;
  double lMinDiffTo240Hz_D;
  double lMinDiffTo480Hz_D;
  double lMinDiffToPref_D;

  double lRate60Hz_D;
  double lRate120Hz_D;
  double lRate240Hz_D;
  double lRate480Hz_D;
  double lRatePref_D;

  xcb_randr_mode_t l60Hz_XRR  = XCB_NONE;
  xcb_randr_mode_t l120Hz_XRR = XCB_NONE;
  xcb_randr_mode_t l240Hz_XRR = XCB_NONE;
  xcb_randr_mode_t l480Hz_XRR = XCB_NONE;
  xcb_randr_mode_t lPref_XRR  = XCB_NONE;

  bool lFoundOneSizeMatch      = false;
  bool lFindPreferedRateFailed = false;

  if (_preferedRate < 0)
    _preferedRate *= -1;

  if (_maxDiff < 0)
    _maxDiff *= -1;

  if (std::abs(_preferedRate) > std::numeric_limits<double>::epsilon()) {
    lRatePref_D = findNearestFreqTo(_preferedRate, _width, _height, lPref_XRR, lMinDiffToPref_D);

    // No mode for this size
    if (lRatePref_D < 0)
      return 0;

    if (lMinDiffToPref_D > _maxDiff) {
      lFindPreferedRateFailed = true;
    } else {
      vModeToUse_XRR = lPref_XRR;
      return lRatePref_D;
    }
  }

  for (iDisplayRandR::mode const &fMode : vModes_V_mode) {
    if (_width == fMode.width && _height == fMode.height) {
      lFoundOneSizeMatch = true;
      if (fMode.prefered) {
        vModeToUse_XRR = fMode.id;
        return fMode.rate;
      }
    }
  }

  if (!lFoundOneSizeMatch)
    return false;

  vCurrentWidth_uI  = _width;
  vCurrentHeight_uI = _height;

  lRate60Hz_D  = findNearestFreqTo(60, _width, _height, l60Hz_XRR, lMinDiffTo60Hz_D);
  lRate120Hz_D = findNearestFreqTo(120, _width, _height, l120Hz_XRR, lMinDiffTo120Hz_D);
  lRate240Hz_D = findNearestFreqTo(240, _width, _height, l240Hz_XRR, lMinDiffTo240Hz_D);
  lRate480Hz_D = findNearestFreqTo(480, _width, _height, l480Hz_XRR, lMinDiffTo480Hz_D);

  // No mode for this size
  if (lRate60Hz_D < 0 || lRate120Hz_D < 0 || lRate240Hz_D < 0 || lRate480Hz_D < 0)
    return 0;

  if (std::abs(lMinDiffTo60Hz_D) < std::numeric_limits<double>::epsilon() ||
      (lMinDiffTo60Hz_D < lMinDiffTo120Hz_D && lMinDiffTo60Hz_D < lMinDiffTo240Hz_D &&
       lMinDiffTo60Hz_D < lMinDiffTo480Hz_D)) {
    vModeToUse_XRR = l60Hz_XRR;
    return lFindPreferedRateFailed ? lRate60Hz_D * -1 : lRate60Hz_D;
  }

  if (std::abs(lMinDiffTo120Hz_D) < std::numeric_limits<double>::epsilon() ||
      (lMinDiffTo120Hz_D < lMinDiffTo60Hz_D && lMinDiffTo120Hz_D < lMinDiffTo240Hz_D &&
       lMinDiffTo120Hz_D < lMinDiffTo480Hz_D)) {
    vModeToUse_XRR = l120Hz_XRR;
    return lFindPreferedRateFailed ? lRate120Hz_D * -1 : lRate120Hz_D;
  }

  if (std::abs(lMinDiffTo240Hz_D) < std::numeric_limits<double>::epsilon() ||
      (lMinDiffTo240Hz_D < lMinDiffTo60Hz_D && lMinDiffTo240Hz_D < lMinDiffTo120Hz_D &&
       lMinDiffTo240Hz_D < lMinDiffTo480Hz_D)) {
    vModeToUse_XRR = l240Hz_XRR;
    return lFindPreferedRateFailed ? lRate240Hz_D * -1 : lRate240Hz_D;
  }

  if (std::abs(lMinDiffTo480Hz_D) < std::numeric_limits<double>::epsilon() ||
      (lMinDiffTo480Hz_D < lMinDiffTo60Hz_D && lMinDiffTo480Hz_D < lMinDiffTo120Hz_D &&
       lMinDiffTo480Hz_D < lMinDiffTo240Hz_D)) {
    vModeToUse_XRR = l480Hz_XRR;
    return lFindPreferedRateFailed ? lRate480Hz_D * -1 : lRate480Hz_D;
  }

  return 0;
}

/*!
 * \brief Disables the display
 */
void iDisplayRandR::disable() {
  vEnabled_B        = false;
  vCurrentWidth_uI  = 0;
  vCurrentHeight_uI = 0;
  vCurrentRate_D    = 0;
  vModeToUse_XRR    = XCB_NONE;
}

/*!
 * \brief Enables the display and runs autoSelectBest() if necessary.
 */
void iDisplayRandR::enable() {
  vEnabled_B = true;
  if (vModeToUse_XRR == XCB_NONE)
    autoSelectBest();
}

/*!
 * \brief Returns a vector with all possible rates for the resolution
 *
 * \param[in] _width  The width of the resolution
 * \param[in] _height The height of the resolution
 *
 * \returns A vector with all possible rates for the resolution
 */
std::vector<double> iDisplayRandR::getPossibleRates(unsigned int _width, unsigned int _height) const {
  std::vector<double> lTempRates;

  for (auto &elem : vModes_V_mode) {
    if (_width == elem.width && _height == elem.height) {
      lTempRates.push_back(elem.rate);
    }
  }

  return lTempRates;
}

/*!
 * \brief Returns a vector with all possible resolutions
 * \returns a vector with all possible resolutions
 */
std::vector<iDisplayBasic::res> iDisplayRandR::getPossibleResolutions() const {
  std::vector<iDisplayBasic::res> lTempSizes_V;
  for (auto &elem : vModes_V_mode) {
    bool lSizeExistsAlready_B = false;

    for (auto &lTempSizes_V_j : lTempSizes_V) {
      if (lTempSizes_V_j.width == elem.width && lTempSizes_V_j.height == elem.height) {
        lSizeExistsAlready_B = true;
        break;
      }
    }

    if (lSizeExistsAlready_B)
      continue;

    iDisplayBasic::res lTempRes;

    lTempRes.width  = elem.width;
    lTempRes.height = elem.height;

    lTempSizes_V.push_back(lTempRes);
  }

  return lTempSizes_V;
}


/*!
 * \brief Returns the resolution and rate via references
 *
 * \param[out] _width  The width of the selected resolution
 * \param[out] _height The height of the selected resolution
 * \param[out] _rate   The rate of the selected mode
 */
void iDisplayRandR::getSelectedRes(unsigned int &_width, unsigned int &_height, double &_rate) const {
  if (vModeToUse_XRR == XCB_NONE) {
    _width  = 0;
    _height = 0;
    _rate   = 0;
    return;
  }

  for (auto &elem : vModes_V_mode) {
    if (vModeToUse_XRR == elem.id) {
      _width  = elem.width;
      _height = elem.height;
      _rate   = elem.rate;
      return;
    }
  }
}

/*!
 * \brief Checks if the resolution is supported
 *
 * \param[in] _width  The width to check
 * \param[in] _height The height to check
 *
 * \returns true if the resolution is supported and false if not
 */
bool iDisplayRandR::isSizeSupported(unsigned int _width, unsigned int _height) const {
  for (auto &elem : vModes_V_mode) {
    if (_width == elem.width && _height == elem.height) {
      return true;
    }
  }
  return false;
}

/*!
 * \brief Select the mode with this resolution and rate
 *
 * \param[in] _width  The width for the new mode
 * \param[in] _height The height for the new mode
 * \param[in] _rate   The rate for the new mode
 *
 * \returns true if the mode was found and false if not
 */
bool iDisplayRandR::select(unsigned int _width, unsigned int _height, double _rate) {
  for (unsigned int i = 0; vModes_V_mode.size(); ++i) {
    if (_width == vModes_V_mode[i].width && _height == vModes_V_mode[i].height &&
        std::abs(_rate - vModes_V_mode[i].rate) < std::numeric_limits<double>::epsilon()) {
      vModeToUse_XRR    = vModes_V_mode[i].id;
      vCurrentWidth_uI  = _width;
      vCurrentHeight_uI = _height;
      vCurrentRate_D    = _rate;
      return true;
    }
  }

  return false;
}

/*!
 * \brief Set this display as a clone of another display
 *
 * \param[in] _disp The display to set clone off
 */
void iDisplayRandR::setCloneOf(const iDisplayRandR &_disp) {
  for (auto &elem : vClones_V_XRR) {
    if (elem == _disp.vID_XRR) {
      return;
    }
  }

  vClones_V_XRR.push_back(_disp.vID_XRR);
}




} // namespace unix_x11

} // namespace e_engine

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
