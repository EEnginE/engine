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
#include <string.h> // memcpy

using namespace e_engine;
using namespace unix_x11;

#define CODE(err) static_cast<int>(lError->err)
#define CHECK_ERROR(func, ret)                                                                                         \
  if (lError) {                                                                                                        \
    eLOG("RandR: ", #func, " returned ", CODE(error_code), " -- ", CODE(major_code), ", ", CODE(minor_code));          \
    lNumErrors++;                                                                                                      \
    free(lError);                                                                                                      \
    lError = nullptr;                                                                                                  \
    goto error;                                                                                                        \
  }                                                                                                                    \
  if (!ret) {                                                                                                          \
    eLOG(#func, " returned a NULL pointer");                                                                           \
    lNumErrors++;                                                                                                      \
    goto error;                                                                                                        \
  }

#define CLEANUP(P)                                                                                                     \
  if (P)                                                                                                               \
    free(P);                                                                                                           \
  P = nullptr;

bool iRandR::reload(bool _overwriteLatest, bool _overwriteDefaults) {
  if (!vIsRandRSupported_B)
    return false;

  unsigned int lNumErrors = 0;

  xcb_generic_error_t *                 lError      = nullptr;
  xcb_randr_screen_size_t *             lSizes      = nullptr;
  xcb_randr_get_output_primary_reply_t *lPrimary    = nullptr;
  xcb_randr_get_crtc_info_reply_t *     lCrtcInfo   = nullptr;
  xcb_randr_get_output_info_reply_t *   lOutputInfo = nullptr;

  std::vector<internal::_config *> lCFGs;


  if (_overwriteLatest)
    lCFGs.emplace_back(&vLatestConfig_RandR);

  if (_overwriteDefaults)
    lCFGs.emplace_back(&vDefaultConfig_RandR);


  CLEANUP(vScreenInfo_XCB);
  CLEANUP(vScreenResources_XCB);

  auto lSICookie  = xcb_randr_get_screen_info(vConnection_XCB, vRootWindow_XCB);
  auto lSRCoockie = xcb_randr_get_screen_resources(vConnection_XCB, vRootWindow_XCB);

  vScreenInfo_XCB = xcb_randr_get_screen_info_reply(vConnection_XCB, lSICookie, &lError);
  CHECK_ERROR(xcb_randr_get_screen_info, vScreenInfo_XCB);

  vScreenResources_XCB = xcb_randr_get_screen_resources_reply(vConnection_XCB, lSRCoockie, &lError);
  CHECK_ERROR(xcb_randr_get_screen_resources, vScreenResources_XCB);

  vCRTCs_XCB   = xcb_randr_get_screen_resources_crtcs(vScreenResources_XCB);
  vOutputs_XCB = xcb_randr_get_screen_resources_outputs(vScreenResources_XCB);
  vModes_XCB   = xcb_randr_get_screen_resources_modes(vScreenResources_XCB);

  lSizes = xcb_randr_get_screen_info_sizes(vScreenInfo_XCB);

  if (vScreenInfo_XCB->sizeID < vScreenInfo_XCB->nSizes) {
    vScreenWidth_uI  = static_cast<uint32_t>(lSizes[vScreenInfo_XCB->sizeID].width);
    vScreenHeight_uI = static_cast<uint32_t>(lSizes[vScreenInfo_XCB->sizeID].height);
  } else {
    wLOG("RandR: Unable to determine screen size: sizeID bigger than number of returned sizes");
    vScreenWidth_uI  = 0;
    vScreenHeight_uI = 0;
  }


  for (auto *cfg : lCFGs) {
    auto lPrimCookie = xcb_randr_get_output_primary(vConnection_XCB, vRootWindow_XCB);
    lPrimary         = xcb_randr_get_output_primary_reply(vConnection_XCB, lPrimCookie, &lError);
    CHECK_ERROR(xcb_randr_get_output_primary, lPrimary);

    cfg->primary = lPrimary->output;
    CLEANUP(lPrimary);

    cfg->gamma.clear();

    for (int i = 0; i < vScreenResources_XCB->num_crtcs; ++i) {
      auto  lGammaCookie = xcb_randr_get_crtc_gamma(vConnection_XCB, vCRTCs_XCB[i]);
      auto *lGamma       = xcb_randr_get_crtc_gamma_reply(vConnection_XCB, lGammaCookie, &lError);
      CHECK_ERROR(xcb_randr_get_crtc_gamma, lGamma);

      cfg->gamma.emplace_back(std::make_shared<internal::_gamma>(lGamma->size, vCRTCs_XCB[i]));
      memcpy(cfg->gamma.back()->red, xcb_randr_get_crtc_gamma_red(lGamma), lGamma->size);
      memcpy(cfg->gamma.back()->green, xcb_randr_get_crtc_gamma_green(lGamma), lGamma->size);
      memcpy(cfg->gamma.back()->blue, xcb_randr_get_crtc_gamma_blue(lGamma), lGamma->size);
      CLEANUP(lGamma);
    }
  }

  // Clear old data
  vCRTC_V_RandR.clear();
  vOutput_V_RandR.clear();
  vMode_V_RandR.clear();
  vLatestConfig_RandR.CRTCInfo.clear();


  // CRTC
  for (int i = 0; i < vScreenResources_XCB->num_crtcs; ++i) {
    internal::_crtc lTempCRTC_RandR;

    auto lCrtcCookie = xcb_randr_get_crtc_info(vConnection_XCB, vCRTCs_XCB[i], vScreenInfo_XCB->config_timestamp);
    lCrtcInfo        = xcb_randr_get_crtc_info_reply(vConnection_XCB, lCrtcCookie, &lError);
    CHECK_ERROR(xcb_randr_get_crtc_info, lCrtcInfo);

    lTempCRTC_RandR.id        = vCRTCs_XCB[i];
    lTempCRTC_RandR.timestamp = lCrtcInfo->timestamp;
    lTempCRTC_RandR.posX      = lCrtcInfo->x;
    lTempCRTC_RandR.posY      = lCrtcInfo->y;
    lTempCRTC_RandR.width     = lCrtcInfo->width;
    lTempCRTC_RandR.height    = lCrtcInfo->height;
    lTempCRTC_RandR.mode      = lCrtcInfo->mode;
    lTempCRTC_RandR.rotation  = lCrtcInfo->rotation;
    lTempCRTC_RandR.rotations = lCrtcInfo->rotations;

    xcb_randr_output_t *lOutputs  = xcb_randr_get_crtc_info_outputs(lCrtcInfo);
    xcb_randr_output_t *lPossible = xcb_randr_get_crtc_info_possible(lCrtcInfo);

    for (uint16_t j = 0; j < lCrtcInfo->num_outputs; ++j) {
      lTempCRTC_RandR.outputs.push_back(lOutputs[j]);
    }

    for (uint16_t j = 0; j < lCrtcInfo->num_possible_outputs; ++j) {
      lTempCRTC_RandR.possibleOutputs.push_back(lPossible[j]);
    }

    vCRTC_V_RandR.push_back(lTempCRTC_RandR);
    CLEANUP(lCrtcInfo);
  }


  // Output
  for (int i = 0; i < vScreenResources_XCB->num_outputs; ++i) {
    internal::_output lTempOutput_RandR;

    auto lOutCookie = xcb_randr_get_output_info(vConnection_XCB, vOutputs_XCB[i], vScreenInfo_XCB->config_timestamp);
    lOutputInfo     = xcb_randr_get_output_info_reply(vConnection_XCB, lOutCookie, &lError);
    CHECK_ERROR(xcb_randr_get_output_info, lOutputInfo);

    char *lName = reinterpret_cast<char *>(xcb_randr_get_output_info_name(lOutputInfo));

    lTempOutput_RandR.id             = vOutputs_XCB[i];
    lTempOutput_RandR.timestamp      = lOutputInfo->timestamp;
    lTempOutput_RandR.crtc           = lOutputInfo->crtc;
    lTempOutput_RandR.name           = std::string(lName, lOutputInfo->name_len);
    lTempOutput_RandR.mm_width       = lOutputInfo->mm_width;
    lTempOutput_RandR.mm_height      = lOutputInfo->mm_height;
    lTempOutput_RandR.connection     = static_cast<xcb_randr_connection_t>(lOutputInfo->connection);
    lTempOutput_RandR.subpixel_order = lOutputInfo->subpixel_order;
    lTempOutput_RandR.npreferred     = lOutputInfo->num_preferred;

    xcb_randr_crtc_t *  lOutCrtcs  = xcb_randr_get_output_info_crtcs(lOutputInfo);
    xcb_randr_output_t *lOutColnes = xcb_randr_get_output_info_clones(lOutputInfo);
    xcb_randr_mode_t *  lOutModes  = xcb_randr_get_output_info_modes(lOutputInfo);


    for (uint16_t j = 0; j < lOutputInfo->num_crtcs; ++j)
      lTempOutput_RandR.crtcs.emplace_back(lOutCrtcs[j]);

    for (uint16_t j = 0; j < lOutputInfo->num_clones; ++j)
      lTempOutput_RandR.clones.emplace_back(lOutColnes[j]);

    for (uint16_t j = 0; j < lOutputInfo->num_modes; ++j)
      lTempOutput_RandR.modes.emplace_back(lOutModes[j]);

    vOutput_V_RandR.emplace_back(lTempOutput_RandR);
    CLEANUP(lOutputInfo);
  }


  // Modes
  for (int i = 0; i < vScreenResources_XCB->num_modes; ++i) {
    internal::_mode lTempMode_RandR;

    xcb_randr_mode_info_t lTempModeInfo = vModes_XCB[i];

    lTempMode_RandR.id         = lTempModeInfo.id;
    lTempMode_RandR.width      = lTempModeInfo.width;
    lTempMode_RandR.height     = lTempModeInfo.height;
    lTempMode_RandR.dotClock   = lTempModeInfo.dot_clock;
    lTempMode_RandR.hSyncStart = lTempModeInfo.hsync_start;
    lTempMode_RandR.hSyncEnd   = lTempModeInfo.hsync_end;
    lTempMode_RandR.hTotal     = lTempModeInfo.htotal;
    lTempMode_RandR.hSkew      = lTempModeInfo.hskew;
    lTempMode_RandR.vSyncStart = lTempModeInfo.vsync_start;
    lTempMode_RandR.vSyncEnd   = lTempModeInfo.vsync_end;
    lTempMode_RandR.vTotal     = lTempModeInfo.vtotal;
    lTempMode_RandR.modeFlags  = lTempModeInfo.mode_flags;


    /* v refresh frequency in Hz */
    unsigned int lVTotalTemp = lTempMode_RandR.vTotal;

    if (lTempMode_RandR.modeFlags & XCB_RANDR_MODE_FLAG_DOUBLE_SCAN)
      lVTotalTemp *= 2;

    if (lTempMode_RandR.modeFlags & XCB_RANDR_MODE_FLAG_INTERLACE)
      lVTotalTemp /= 2;

    if (lTempMode_RandR.hTotal && lVTotalTemp)
      lTempMode_RandR.refresh = (static_cast<double>(lTempMode_RandR.dotClock) /
                                 (static_cast<double>(lTempMode_RandR.hTotal) * static_cast<double>(lVTotalTemp)));
    else
      lTempMode_RandR.refresh = 0;


    /* h sync frequency in Hz */
    if (lTempMode_RandR.hTotal)
      lTempMode_RandR.syncFreq = lTempMode_RandR.dotClock / lTempMode_RandR.hTotal;
    else
      lTempMode_RandR.syncFreq = 0;

    vMode_V_RandR.push_back(lTempMode_RandR);
  }


  vLatestConfig_RandR.CRTCInfo = vCRTC_V_RandR;

  if (_overwriteLatest)
    vDefaultConfig_RandR.CRTCInfo = vCRTC_V_RandR;

  vMode_V_RandR.sort();

  return true;

error:
  CLEANUP(vScreenInfo_XCB);
  CLEANUP(vScreenResources_XCB);
  CLEANUP(lPrimary);
  CLEANUP(lCrtcInfo);
  CLEANUP(lOutputInfo);

  return false;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
