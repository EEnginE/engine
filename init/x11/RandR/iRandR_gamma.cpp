/*!
 * \file x11/RandR/iRandR_gamma.cpp
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

#include "uLog.hpp"
#include "iRandR.hpp"
#include <strings.h>

namespace e_engine {

namespace unix_x11 {

/*!
 * \brief Sets the gamma values
 *
 * \param  _disp       Where to change gamma
 * \param  _r          Gamma red value
 * \param  _g          Gamma green value
 * \param  _b          Gamma blue value
 * \param  _brightness The brightness
 *
 * \returns Whether it was successful (\a true) or not (\a false)
 *
 * \note This function will change gamma IMMEDIATELY; Calling applyNewSettings() will have no effect
 *to this.
 *
 * \note Most code is a simple copy and paste from the XRandR project
 *(http://www.x.org/wiki/Projects/XRandR/)
 */
bool iRandR::setGamma(iDisplays const &_disp, double _r, double _g, double _b, double _brightness) {
  if (_r < 0 || _g < 0 || _b < 0 || _brightness < 0 || !isRandRSupported())
    return false;

  reload();

  RRCrtc lTempCRTCId_XRR = None;

  for (internal::_output const &fOutout : vOutput_V_RandR) {
    if (fOutout.connection == 0) {
      if (_disp.getOutput() == fOutout.id) {
        lTempCRTCId_XRR = fOutout.crtc;
        break;
      }
    }
  }

  if (lTempCRTCId_XRR == None)
    return false;


  int           lSize_I = XRRGetCrtcGammaSize(vDisplay_X11, lTempCRTCId_XRR);
  int           lShift_I;
  XRRCrtcGamma *lCRTCGamma_XRR;
  double        lGammaRed_F;
  double        lGammaGreen_F;
  double        lGammaBlue_F;

  if (!lSize_I) {
    eLOG("RandR: Gamma size is 0 => Unable to set Gamma");
    return false;
  }

  /*
   * The gamma-correction lookup table managed through XRR[GS]etCrtcGamma
   * is 2^n in size, where 'n' is the number of significant bits in
   * the X Color.  Because an X Color is 16 bits, size cannot be larger
   * than 2^16.
   */
  if (lSize_I > 65536) {
    eLOG("RandR: Gamma correction table is impossibly large");
    return false;
  }

  /*
   * The hardware color lookup table has a number of significant
   * bits equal to ffs(size) - 1; compute all values so that
   * they are in the range [0,size) then shift the values so
   * that they occupy the MSBs of the 16-bit X Color.
   */
  lShift_I = 16 - (ffs(lSize_I) - 1);

  lCRTCGamma_XRR = XRRAllocGamma(lSize_I);
  if (!lCRTCGamma_XRR) {
    eLOG("RandR: Gamma allocation failed");
    return false;
  }

  _r = (std::abs(_r) < std::numeric_limits<double>::epsilon()) ? 1 : _r;
  _g = (std::abs(_g) < std::numeric_limits<double>::epsilon()) ? 1 : _g;
  _b = (std::abs(_b) < std::numeric_limits<double>::epsilon()) ? 1 : _b;

  lGammaRed_F   = 1 / _r;
  lGammaGreen_F = 1 / _g;
  lGammaBlue_F  = 1 / _b;

  for (int i = 0; i < lSize_I; ++i) {
    if (std::abs(lGammaRed_F - 1.0) < std::numeric_limits<double>::epsilon() &&
        std::abs(_brightness - 1.0) < std::numeric_limits<double>::epsilon()) {
      lCRTCGamma_XRR->red[i] = static_cast<unsigned short>(i);
    } else {
      lCRTCGamma_XRR->red[i] = static_cast<unsigned short>(
          fmin(pow(static_cast<double>(i) / static_cast<double>(lSize_I - 1), lGammaRed_F) * _brightness, 1.0) *
          static_cast<double>(lSize_I - 1));
    }

    lCRTCGamma_XRR->red[i] <<= lShift_I;

    if (std::abs(lGammaGreen_F - 1.0) < std::numeric_limits<double>::epsilon() &&
        std::abs(_brightness - 1.0) < std::numeric_limits<double>::epsilon()) {
      lCRTCGamma_XRR->green[i] = static_cast<unsigned short>(i);
    } else {
      lCRTCGamma_XRR->green[i] = static_cast<unsigned short>(
          fmin(pow(static_cast<double>(i) / static_cast<double>(lSize_I - 1), lGammaGreen_F) * _brightness, 1.0) *
          static_cast<double>(lSize_I - 1));
    }

    lCRTCGamma_XRR->green[i] <<= lShift_I;

    if (std::abs(lGammaBlue_F - 1.0) < std::numeric_limits<double>::epsilon() &&
        std::abs(_brightness - 1.0) < std::numeric_limits<double>::epsilon()) {
      lCRTCGamma_XRR->blue[i] = static_cast<unsigned short>(i);
    } else {
      lCRTCGamma_XRR->blue[i] = static_cast<unsigned short>(
          fmin(pow(static_cast<double>(i) / static_cast<double>(lSize_I - 1), lGammaBlue_F) * _brightness, 1.0) *
          static_cast<double>(lSize_I - 1));
    }

    lCRTCGamma_XRR->blue[i] <<= lShift_I;
  }

  XRRSetCrtcGamma(vDisplay_X11, lTempCRTCId_XRR, lCRTCGamma_XRR);

  XRRFreeGamma(lCRTCGamma_XRR);

  iLOG("Successfully set Gamma to   R: ",
       _r,
       "  --  G: ",
       _g,
       "  --  B: ",
       _b,
       "  --  Brightness: ",
       _brightness,
       "  !!  ",
       lTempCRTCId_XRR);

  return true;
}

} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
