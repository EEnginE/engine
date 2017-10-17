/*!
 * \file x11/RandR/iRandR_crtc.cpp
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
#define CHECK_ERROR(func, ret)                                                                                         \
  if (lError) {                                                                                                        \
    eLOG("RandR: ", #func, " returned ", CODE(error_code), " -- ", CODE(major_code), ", ", CODE(minor_code));          \
    free(lError);                                                                                                      \
    lError = nullptr;                                                                                                  \
    lNumErrors++;                                                                                                      \
  }                                                                                                                    \
  if (!ret) {                                                                                                          \
    eLOG(#func, " returned a NULL pointer");                                                                           \
    lNumErrors++;                                                                                                      \
  }

using namespace e_engine;
using namespace unix_x11;

/*!
 * \brief Change one CRTC
 *
 * \returns -5 when the CRTC could not be found
 * \returns 1 when everything went fine
 */
int iRandR::changeCRTC(internal::_crtc _changeToThis) {
  xcb_randr_crtc_t lCRTC_XCB      = _changeToThis.id;
  bool             lChangedCRTC_B = false;

  xcb_generic_error_t *lError;

  bool lCRTCInfoFound = false;
  int  lNumErrors     = 0;

  for (int i = 0; i < vScreenResources_XCB->num_crtcs; ++i) {
    if (vCRTCs_XCB[i] == lCRTC_XCB) {
      lCRTCInfoFound = true;
      break;
    }
  }

  if (!lCRTCInfoFound) {
    wLOG("RandR: Cannot find CRTC ", lCRTC_XCB, " in the current CRTC --> return -5");
    return -5;
  }

  auto  lCrtcCookie = xcb_randr_get_crtc_info(vConnection_XCB, lCRTC_XCB, vScreenInfo_XCB->config_timestamp);
  auto *lCrtcInfo   = xcb_randr_get_crtc_info_reply(vConnection_XCB, lCrtcCookie, &lError);
  CHECK_ERROR(xcb_randr_get_crtc_info, lCrtcInfo);
  if (lNumErrors != 0) {
    eLOG("RandR: Failed to get CRTC info of CRTC ", lCRTC_XCB);
    if (lCrtcInfo)
      free(lCrtcInfo);

    return -5;
  }

  xcb_randr_output_t *lOutputs = xcb_randr_get_crtc_info_outputs(lCrtcInfo);

  if (lCrtcInfo->mode != _changeToThis.mode ||
      lCrtcInfo->num_outputs != static_cast<int>(_changeToThis.outputs.size()) || lCrtcInfo->x != _changeToThis.posX ||
      lCrtcInfo->y != _changeToThis.posY || lCrtcInfo->rotation != _changeToThis.rotation) {
    lChangedCRTC_B = true;
  } else {
    for (unsigned int i = 0; i < _changeToThis.outputs.size(); ++i) {
      if (lOutputs[i] != _changeToThis.outputs[i]) {
        lChangedCRTC_B = true;
        break;
      }
    }
  }

  if (lChangedCRTC_B) {
    if (_changeToThis.outputs.size() == 0 || _changeToThis.mode == XCB_NONE) {
      // Disable output
      auto lConfigCookie = xcb_randr_set_crtc_config(vConnection_XCB,
                                                     lCRTC_XCB,
                                                     XCB_CURRENT_TIME,
                                                     XCB_CURRENT_TIME,
                                                     0,
                                                     0,
                                                     XCB_NONE,
                                                     XCB_RANDR_ROTATION_ROTATE_0,
                                                     0,
                                                     nullptr);

      auto *lConfigResult = xcb_randr_set_crtc_config_reply(vConnection_XCB, lConfigCookie, &lError);
      CHECK_ERROR(xcb_randr_set_crtc_config, lConfigResult);
      if (lNumErrors == 0) {
        iLOG("RandR: Disabled CRTC ", _changeToThis.id);
      } else {
        eLOG("RandR: Failed to disable CRTC ", _changeToThis.id);
      }
    } else {
      xcb_randr_output_t *lTempOutputs = new xcb_randr_output_t[_changeToThis.outputs.size()];
      for (unsigned int i = 0; i < _changeToThis.outputs.size(); ++i) {
        lTempOutputs[i] = _changeToThis.outputs[i];
      }

      auto lConfigCookie = xcb_randr_set_crtc_config(vConnection_XCB,
                                                     lCRTC_XCB,
                                                     XCB_CURRENT_TIME,
                                                     XCB_CURRENT_TIME,
                                                     static_cast<int16_t>(_changeToThis.posX),
                                                     static_cast<int16_t>(_changeToThis.posY),
                                                     _changeToThis.mode,
                                                     _changeToThis.rotation,
                                                     static_cast<uint32_t>(_changeToThis.outputs.size()),
                                                     lTempOutputs);
      delete[] lTempOutputs;

      auto *lConfigResult = xcb_randr_set_crtc_config_reply(vConnection_XCB, lConfigCookie, &lError);
      CHECK_ERROR(xcb_randr_set_crtc_config, lConfigResult);
      if (lNumErrors == 0) {
        iLOG("RandR: Changed CRTC ", _changeToThis.id);
      } else {
        eLOG("RandR: Failed to change CRTC ", _changeToThis.id);
      }
    }
  } else {
    iLOG("RandR: Changed CRTC ", _changeToThis.id, " -- nothing to do");
  }

  if (lCrtcInfo)
    free(lCrtcInfo);

  return (lNumErrors == 0) ? 1 : -10;
}
