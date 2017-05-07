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

using namespace e_engine;
using namespace unix_x11;

internal::_crtc iRandR::isOutputPossible(xcb_randr_output_t _id, xcb_randr_crtc_t _crtc) {
  internal::_crtc lTempCRTC_RandR;
  lTempCRTC_RandR.id = XCB_NONE;
  for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
    if (fCRTC.id != _crtc)
      continue;

    for (auto const &fOut : fCRTC.possibleOutputs) {
      if (fOut == _id) {
        lTempCRTC_RandR = fCRTC;
        return lTempCRTC_RandR;
      }
    }
  }
  return lTempCRTC_RandR;
}

/*!
 * \brief Set the new display sizes
 *
 * \param _disp The display to change
 *
 * \returns \a true when everything went fine and \a false when it is impossible to set the display
 *like this.
 *
 * \note To apply the change you need to run applyNewSettings()!
 *
 * \note It is also possible to run setDisplaySizes() more than once without running
 *applyNewSettings().
 */
iRandR::ERROR_CODE iRandR::setDisplaySizes(iDisplayBasic *_disp) {
  if (!isProtocolSupported())
    return RANDR_NOT_SUPPORTED;

  iDisplayRandR *lDisp = dynamic_cast<iDisplayRandR *>(_disp);
  if (!lDisp)
    return INVALID_DISPLAY_CLASS;

  reload(false);

  xcb_randr_crtc_t              lSingleMatch_XRR = XCB_NONE;
  xcb_randr_crtc_t              lClonesMatch_XRR = XCB_NONE;
  std::vector<xcb_randr_crtc_t> lEmptyCRTC_V_XRR;

  // Check mode
  bool lModeSupported_B = false;
  if (lDisp->getMode() != XCB_NONE) {
    for (internal::_output const &fOutput : vOutput_V_RandR) {
      if (lDisp->getOutput() != fOutput.id)
        continue;

      for (xcb_randr_mode_t const &fMode : fOutput.modes) {
        if (fMode == lDisp->getMode()) {
          lModeSupported_B = true;
          break;
        }
      }
    }
  }

  if (!lModeSupported_B && lDisp->getMode() != XCB_NONE) {
    // This should generally never happen because of iRandRDisplay! Something went wrong!!!
    wLOG("RandR: Selected mode ( ",
         lDisp->getMode(),
         " ) is not supported by output ",
         lDisp->getOutput(),
         " --> Do Nothing");
    return RANDR_OTHER_ERROR;
  }

  // Find CRTCs
  internal::_crtc lTempCRTC_RandR;
  lTempCRTC_RandR.id = XCB_NONE;
  for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
    if (fCRTC.outputs.size() == 1 && fCRTC.outputs[0] == lDisp->getOutput()) {
      lSingleMatch_XRR = fCRTC.id;
      // A Output can not be in two CRTC's
      break;
    }

    if (fCRTC.outputs.empty()) {
      lEmptyCRTC_V_XRR.push_back(fCRTC.id);
      continue;
    }

    if (fCRTC.outputs.size() > 1) {
      for (xcb_randr_output_t const &fRROut : fCRTC.outputs) {
        if (fRROut == lDisp->getOutput()) {
          lClonesMatch_XRR = lDisp->getOutput();
          break;
        }
      }
      if (lClonesMatch_XRR != XCB_NONE)
        break;
    }
  }

  // ========== Disable something disabled
  // =============================================================================================
  if (!lDisp->getIsEnabled() && lSingleMatch_XRR == XCB_NONE && lClonesMatch_XRR == XCB_NONE) {
    // Nothing to do here ( we dont need a change from diabled to disabled )
    return OK;
  }

  // ========== Disable something enabled
  // =============================================================================================
  if (!lDisp->getIsEnabled() && lSingleMatch_XRR != XCB_NONE && lClonesMatch_XRR == XCB_NONE) {
    // We disable an enabled Output
    lTempCRTC_RandR.id   = lSingleMatch_XRR;
    lTempCRTC_RandR.mode = XCB_NONE;
    lTempCRTC_RandR.outputs.clear();
    lTempCRTC_RandR.posX     = 0;
    lTempCRTC_RandR.posY     = 0;
    lTempCRTC_RandR.rotation = XCB_RANDR_ROTATION_ROTATE_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return OK;
  }

  // ========== Disable something cloned
  // =============================================================================================
  if (!lDisp->getIsEnabled() && lSingleMatch_XRR == XCB_NONE && lClonesMatch_XRR != XCB_NONE) {
    // We disable an enabled Output but not the clone
    std::vector<xcb_randr_output_t> lTempOutputs_V_XRR;
    for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
      if (lClonesMatch_XRR == fCRTC.id) {
        lTempCRTC_RandR = fCRTC;
        break;
      }
    }

    // Copy all but the output we want to disable
    for (xcb_randr_output_t const &fRROut : lTempCRTC_RandR.outputs) {
      if (lDisp->getOutput() != fRROut) {
        lTempOutputs_V_XRR.push_back(fRROut);
      }
    }

    // We removed only the output we wanted to remove
    lTempCRTC_RandR.outputs  = lTempOutputs_V_XRR;
    lTempCRTC_RandR.rotation = XCB_RANDR_ROTATION_ROTATE_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return OK;
  }

  // ========== Enable something disabled
  // =============================================================================================
  if (lDisp->getIsEnabled() && lSingleMatch_XRR == XCB_NONE && lClonesMatch_XRR == XCB_NONE) {
    // We want to enable a disabled output
    if (!lEmptyCRTC_V_XRR.empty()) {
      // Test if it is possible to set output
      for (xcb_randr_crtc_t const &fRRCrtc : lEmptyCRTC_V_XRR) {
        lTempCRTC_RandR = isOutputPossible(lDisp->getOutput(), fRRCrtc);
        if (lTempCRTC_RandR.id != XCB_NONE)
          break;
      }

      if (lTempCRTC_RandR.id != XCB_NONE) {
        lTempCRTC_RandR.mode    = lDisp->getMode();
        lTempCRTC_RandR.outputs = lDisp->getClones();
        lTempCRTC_RandR.outputs.push_back(lDisp->getOutput());
        lDisp->getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
        lTempCRTC_RandR.rotation = XCB_RANDR_ROTATION_ROTATE_0;

        vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
        return OK;
      }
    }

    // We can not use a fresh CRTC
    if (lDisp->getClones().empty()) {
      wLOG("RandR: Impossible to enable the Output ",
           lDisp->getOutput(),
           " without cloning --> do nothing ( return false (iRandR::setDisplaySizes))");
      return RANDR_OTHER_ERROR;
    }

    for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
      for (xcb_randr_output_t const &fRROut : fCRTC.outputs) {
        for (xcb_randr_output_t const &fRRClone : lDisp->getClones()) {
          if (fRROut == fRRClone) {
            lTempCRTC_RandR = isOutputPossible(lDisp->getOutput(), fCRTC.id);
            if (lTempCRTC_RandR.id != XCB_NONE) {
              lTempCRTC_RandR.mode    = lDisp->getMode();
              lTempCRTC_RandR.outputs = lDisp->getClones();
              lTempCRTC_RandR.outputs.push_back(lDisp->getOutput());
              lDisp->getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
              lTempCRTC_RandR.rotation = XCB_RANDR_ROTATION_ROTATE_0;

              vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
              return OK;
            }
          }
        }
      }
    }

    // Also finding the crtc with the clone failed
    return RANDR_CRTC_NOT_FOUND;
  }

  // ========== Change something enabled
  // =============================================================================================
  if (lDisp->getIsEnabled() && lSingleMatch_XRR != XCB_NONE && lClonesMatch_XRR == XCB_NONE) {
    // We want to change a enabled output
    lTempCRTC_RandR.id      = lSingleMatch_XRR;
    lTempCRTC_RandR.mode    = lDisp->getMode();
    lTempCRTC_RandR.outputs = lDisp->getClones();
    lTempCRTC_RandR.outputs.push_back(lDisp->getOutput());
    lDisp->getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
    lTempCRTC_RandR.rotation = XCB_RANDR_ROTATION_ROTATE_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return OK;
  }

  // ========== Change something cloned
  // =============================================================================================
  if (lDisp->getIsEnabled() && lSingleMatch_XRR == XCB_NONE && lClonesMatch_XRR != XCB_NONE) {
    std::vector<xcb_randr_output_t> lTempOutputs_V_XRR;
    for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
      if (lClonesMatch_XRR == fCRTC.id) {
        lTempCRTC_RandR = fCRTC;
        break;
      }
    }

    lTempCRTC_RandR.mode = lDisp->getMode();
    lDisp->getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
    lTempCRTC_RandR.rotation = XCB_RANDR_ROTATION_ROTATE_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return OK;
  }

  wLOG("Congratulations! It should be theoretically impossible to reach the end of this function");
  return RANDR_OTHER_ERROR;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
