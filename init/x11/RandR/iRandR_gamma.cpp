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

#define CODE(err) static_cast<int>(lError->err)
#define CHECK_ERROR(func, ret)                                                                                \
  if (lError) {                                                                                               \
    eLOG("RandR: ", #func, " returned ", CODE(error_code), " -- ", CODE(major_code), ", ", CODE(minor_code)); \
    free(lError);                                                                                             \
    if (ret)                                                                                                  \
      free(ret);                                                                                              \
    return false;                                                                                             \
  }                                                                                                           \
  if (!ret) {                                                                                                 \
    eLOG(#func, " returned a NULL pointer");                                                                  \
    return false;                                                                                             \
  }

using namespace e_engine;
using namespace unix_x11;

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

  xcb_generic_error_t *                  lError          = nullptr;
  xcb_randr_crtc_t                       lCRTC           = XCB_NONE;
  xcb_randr_get_crtc_gamma_size_reply_t *lGammaSizeReply = nullptr;

  uint32_t  lSize_I       = lGammaSizeReply->length;
  int       lShift_I      = 0;
  double    lGammaRed_F   = 0.0;
  double    lGammaGreen_F = 0.0;
  double    lGammaBlue_F  = 0.0;
  uint16_t *lRed          = nullptr;
  uint16_t *lGreen        = nullptr;
  uint16_t *lBlue         = nullptr;

  for (internal::_output const &fOutout : vOutput_V_RandR) {
    if (fOutout.connection == 0) {
      if (_disp.getOutput() == fOutout.id) {
        lCRTC = fOutout.crtc;
        break;
      }
    }
  }

  if (lCRTC == XCB_NONE)
    return false;

  auto lGammaSizeCookie = xcb_randr_get_crtc_gamma_size(vConnection_XCB, lCRTC);
  lGammaSizeReply       = xcb_randr_get_crtc_gamma_size_reply(vConnection_XCB, lGammaSizeCookie, &lError);
  CHECK_ERROR(xcb_randr_get_crtc_gamma_size, lGammaSizeReply);

  lSize_I = lGammaSizeReply->length;
  free(lGammaSizeReply);

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

  // Allocate arays
  lRed   = new uint16_t[lSize_I];
  lGreen = new uint16_t[lSize_I];
  lBlue  = new uint16_t[lSize_I];

  /*
   * The hardware color lookup table has a number of significant
   * bits equal to ffs(size) - 1; compute all values so that
   * they are in the range [0,size) then shift the values so
   * that they occupy the MSBs of the 16-bit X Color.
   */
  lShift_I = 16 - (ffs(static_cast<int>(lSize_I)) - 1);

  _r = (std::abs(_r) < std::numeric_limits<double>::epsilon()) ? 1 : _r;
  _g = (std::abs(_g) < std::numeric_limits<double>::epsilon()) ? 1 : _g;
  _b = (std::abs(_b) < std::numeric_limits<double>::epsilon()) ? 1 : _b;

  lGammaRed_F   = 1 / _r;
  lGammaGreen_F = 1 / _g;
  lGammaBlue_F  = 1 / _b;

  for (uint32_t i = 0; i < lSize_I; ++i) {
    if (std::abs(lGammaRed_F - 1.0) < std::numeric_limits<double>::epsilon() &&
        std::abs(_brightness - 1.0) < std::numeric_limits<double>::epsilon()) {
      lRed[i] = static_cast<uint16_t>(i);
    } else {
      lRed[i] = static_cast<uint16_t>(
          fmin(pow(static_cast<double>(i) / static_cast<double>(lSize_I - 1), lGammaRed_F) * _brightness, 1.0) *
          static_cast<double>(lSize_I - 1));
    }

    lRed[i] <<= lShift_I;

    if (std::abs(lGammaGreen_F - 1.0) < std::numeric_limits<double>::epsilon() &&
        std::abs(_brightness - 1.0) < std::numeric_limits<double>::epsilon()) {
      lGreen[i] = static_cast<uint16_t>(i);
    } else {
      lGreen[i] = static_cast<uint16_t>(
          fmin(pow(static_cast<double>(i) / static_cast<double>(lSize_I - 1), lGammaGreen_F) * _brightness, 1.0) *
          static_cast<double>(lSize_I - 1));
    }

    lGreen[i] <<= lShift_I;

    if (std::abs(lGammaBlue_F - 1.0) < std::numeric_limits<double>::epsilon() &&
        std::abs(_brightness - 1.0) < std::numeric_limits<double>::epsilon()) {
      lBlue[i] = static_cast<uint16_t>(i);
    } else {
      lBlue[i] = static_cast<uint16_t>(
          fmin(pow(static_cast<double>(i) / static_cast<double>(lSize_I - 1), lGammaBlue_F) * _brightness, 1.0) *
          static_cast<double>(lSize_I - 1));
    }

    lBlue[i] <<= lShift_I;
  }

  xcb_randr_set_crtc_gamma(vConnection_XCB, lCRTC, static_cast<uint16_t>(lSize_I), lRed, lGreen, lBlue);

  delete[] lRed;
  delete[] lGreen;
  delete[] lBlue;

  iLOG("Successfully set Gamma to   R: ",
       _r,
       "  --  G: ",
       _g,
       "  --  B: ",
       _b,
       "  --  Brightness: ",
       _brightness,
       "  !!  ",
       lCRTC);

  return true;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
