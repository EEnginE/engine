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

#include "uLog.hpp"
#include "iRandR.hpp"

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
 * \brief Applies the new RandR config set
 *
 * \returns true when everything went fine
 * \returns false when there was an error
 */
bool iRandR::applyNewRandRSettings() {
  if (!isRandRSupported())
    return false;

  std::vector<internal::_crtc> lTempAllCRTC_V_RandR;
  int                          lMinWidth_I, lMinHeight_I;
  int                          lMaxWidth_I, lMaxHeight_I;

  uint16_t lNewWidth_I, lNewHeight_I;
  uint16_t lCurrentWidth_I, lCurrentHeight_I;

  xcb_randr_get_screen_size_range_reply_t *lSizeRangeReply = nullptr;
  xcb_generic_error_t *                    lError          = nullptr;

  reload();

  for (internal::_crtc const &fCRTC : vChangeCRTC_V_RandR) {
    bool lAppend_B = true;
    // Check for duplicates
    for (internal::_crtc const &fCRTCTemp : lTempAllCRTC_V_RandR) {
      if (fCRTC.id == fCRTCTemp.id) {
        lAppend_B = false;
        wLOG("RandR: Duplicate of a CRTC id in vChangeCRTC_V_RandR --> Only change first");
        break;
      }
    }
    if (lAppend_B)
      lTempAllCRTC_V_RandR.push_back(fCRTC);
  }

  // We now work with lTempAllCRTC_V_RandR only
  vChangeCRTC_V_RandR.clear();

  // Add mising CRTCs
  for (internal::_crtc const &fCRTC1 : vCRTC_V_RandR) {
    bool lFound_B = false;
    for (internal::_crtc const &fCRTC2 : lTempAllCRTC_V_RandR) {
      if (fCRTC1.id == fCRTC2.id) {
        lFound_B = true;
        break;
      }
    }
    if (!lFound_B)
      lTempAllCRTC_V_RandR.push_back(fCRTC1);
  }

  if (lTempAllCRTC_V_RandR.empty())
    return false;

  auto lScreensSizeCookie = xcb_randr_get_screen_size_range(vConnection_XCB, vRootWindow_XCB);
  lSizeRangeReply         = xcb_randr_get_screen_size_range_reply(vConnection_XCB, lScreensSizeCookie, &lError);
  CHECK_ERROR(xcb_randr_get_screen_size_range, lSizeRangeReply);

  lMinWidth_I  = lSizeRangeReply->min_width;
  lMinHeight_I = lSizeRangeReply->min_height;
  lMaxWidth_I  = lSizeRangeReply->max_width;
  lMaxHeight_I = lSizeRangeReply->max_height;

  free(lSizeRangeReply);

  lNewWidth_I  = 0;
  lNewHeight_I = 0;

  for (internal::_crtc const &fCRTC : lTempAllCRTC_V_RandR) {
    internal::_mode lTempMode_RandR;
    bool            lModeFound_B = false;

    if (fCRTC.mode == XCB_NONE) {
      lModeFound_B           = true;
      lTempMode_RandR.width  = 0;
      lTempMode_RandR.height = 0;
    } else {
      for (internal::_mode const &fMode : vMode_V_RandR) {
        if (fCRTC.mode == fMode.id) {
          lTempMode_RandR = fMode;
          lModeFound_B    = true;
          break;
        }
      }
    }

    if (!lModeFound_B) {
      wLOG("RandR: Unable to find mode (Mode ID = ", fCRTC.mode, ") in vMode_V_RandR --> Do not change Screen size");
      return false;
    }


    uint16_t lTempWidth_I  = static_cast<uint16_t>(fCRTC.posX) + static_cast<uint16_t>(lTempMode_RandR.width);
    uint16_t lTempHeight_I = static_cast<uint16_t>(fCRTC.posY) + static_cast<uint16_t>(lTempMode_RandR.height);
    lNewWidth_I            = (lTempWidth_I > lNewWidth_I) ? lTempWidth_I : lNewWidth_I;
    lNewHeight_I           = (lTempHeight_I > lNewHeight_I) ? lTempHeight_I : lNewHeight_I;
  }

  // Get current size
  xcb_randr_screen_size_t *sizes = xcb_randr_get_screen_info_sizes(vScreenInfo_XCB);

  if (!(vScreenInfo_XCB->sizeID < vScreenInfo_XCB->nSizes)) {
    wLOG("XRandR ERROR: ! vScreenInfo_XCB->sizeID < vScreenInfo_XCB->nSizes ( ",
         vScreenInfo_XCB->sizeID,
         " < ",
         vScreenInfo_XCB->nSizes,
         " )");
    return false;
  }

  lCurrentWidth_I  = sizes[vScreenInfo_XCB->sizeID].width;
  lCurrentHeight_I = sizes[vScreenInfo_XCB->sizeID].height;

  if ((lNewWidth_I >= lMinWidth_I && lNewWidth_I <= lMaxWidth_I && lNewHeight_I >= lMinHeight_I &&
       lNewHeight_I <= lMaxHeight_I) &&
      (lNewWidth_I != lCurrentWidth_I || lNewHeight_I != lCurrentHeight_I)) {

    xcb_randr_set_screen_size(vConnection_XCB,
                              vRootWindow_XCB,
                              (lNewWidth_I > lCurrentWidth_I) ? lNewWidth_I : lCurrentWidth_I,
                              (lNewHeight_I > lCurrentHeight_I) ? lNewHeight_I : lCurrentHeight_I,
                              0,
                              0);
  }

  for (internal::_crtc const &fCRTC : lTempAllCRTC_V_RandR)
    changeCRTC(fCRTC);

  if ((lNewWidth_I >= lMinWidth_I && lNewWidth_I <= lMaxWidth_I && lNewHeight_I >= lMinHeight_I &&
       lNewHeight_I <= lMaxHeight_I) &&
      (lNewWidth_I != lCurrentWidth_I || lNewHeight_I != lCurrentHeight_I)) {

    xcb_randr_set_screen_size(vConnection_XCB, vRootWindow_XCB, lNewWidth_I, lNewHeight_I, 0, 0);
  }

  // We now work with lTempAllCRTC_V_RandR only
  vChangeCRTC_V_RandR.clear();

#if D_LOG_XRANDR
  printRandRStatus();
#endif

  return true;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
