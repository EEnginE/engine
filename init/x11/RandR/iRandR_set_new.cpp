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

namespace e_engine {

namespace unix_x11 {

internal::_crtc iRandR::isOutputPossible(RROutput _id, RRCrtc _crtc) {
  internal::_crtc lTempCRTC_RandR;
  lTempCRTC_RandR.id = None;
  for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
    if (fCRTC.id != _crtc)
      continue;

    for (RROutput const &fOut : fCRTC.possibleOutputs) {
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
bool iRandR::setDisplaySizes(iDisplays const &_disp) {
  if (!isRandRSupported())
    return false;

  reload(false);

  RRCrtc              lSingleMatch_XRR = None;
  RRCrtc              lClonesMatch_XRR = None;
  std::vector<RRCrtc> lEmptyCRTC_V_XRR;

  // Check mode
  bool lModeSupported_B = false;
  if (_disp.getMode() != None) {
    for (internal::_output const &fOutput : vOutput_V_RandR) {
      if (_disp.getOutput() != fOutput.id)
        continue;

      for (RRMode const &fMode : fOutput.modes) {
        if (fMode == _disp.getMode()) {
          lModeSupported_B = true;
          break;
        }
      }
    }
  }

  if (!lModeSupported_B && _disp.getMode() != None) {
    // This should generally never happen because of iRandRDisplay! Something went wrong!!!
    wLOG("RandR: Selected mode ( ",
         _disp.getMode(),
         " ) is not supported by output ",
         _disp.getOutput(),
         " --> Do Nothing");
    return false;
  }

  // Find CRTCs
  internal::_crtc lTempCRTC_RandR;
  lTempCRTC_RandR.id = None;
  for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
    if (fCRTC.outputs.size() == 1 && fCRTC.outputs[0] == _disp.getOutput()) {
      lSingleMatch_XRR = fCRTC.id;
      // A Output can not be in two CRTC's
      break;
    }

    if (fCRTC.outputs.empty()) {
      lEmptyCRTC_V_XRR.push_back(fCRTC.id);
      continue;
    }

    if (fCRTC.outputs.size() > 1) {
      for (RROutput const &fRROut : fCRTC.outputs) {
        if (fRROut == _disp.getOutput()) {
          lClonesMatch_XRR = _disp.getOutput();
          break;
        }
      }
      if (lClonesMatch_XRR != None)
        break;
    }
  }

  // ========== Disable something disabled
  // =============================================================================================
  if (!_disp.getIsEnabled() && lSingleMatch_XRR == None && lClonesMatch_XRR == None) {
    // Nothing to do here ( we dont need a change from diabled to disabled )
    return true;
  }

  // ========== Disable something enabled
  // =============================================================================================
  if (!_disp.getIsEnabled() && lSingleMatch_XRR != None && lClonesMatch_XRR == None) {
    // We disable an enabled Output
    lTempCRTC_RandR.id   = lSingleMatch_XRR;
    lTempCRTC_RandR.mode = None;
    lTempCRTC_RandR.outputs.clear();
    lTempCRTC_RandR.posX     = 0;
    lTempCRTC_RandR.posY     = 0;
    lTempCRTC_RandR.rotation = RR_Rotate_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return true;
  }

  // ========== Disable something cloned
  // =============================================================================================
  if (!_disp.getIsEnabled() && lSingleMatch_XRR == None && lClonesMatch_XRR != None) {
    // We disable an enabled Output but not the clone
    std::vector<RROutput> lTempOutputs_V_XRR;
    for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
      if (lClonesMatch_XRR == fCRTC.id) {
        lTempCRTC_RandR = fCRTC;
        break;
      }
    }

    // Copy all but the output we want to disable
    for (RROutput const &fRROut : lTempCRTC_RandR.outputs) {
      if (_disp.getOutput() != fRROut) {
        lTempOutputs_V_XRR.push_back(fRROut);
      }
    }

    // We removed only the output we wanted to remove
    lTempCRTC_RandR.outputs  = lTempOutputs_V_XRR;
    lTempCRTC_RandR.rotation = RR_Rotate_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return true;
  }

  // ========== Enable something disabled
  // =============================================================================================
  if (_disp.getIsEnabled() && lSingleMatch_XRR == None && lClonesMatch_XRR == None) {
    // We want to enable a disabled output
    if (!lEmptyCRTC_V_XRR.empty()) {
      // Test if it is possible to set output
      for (RRCrtc const &fRRCrtc : lEmptyCRTC_V_XRR) {
        lTempCRTC_RandR = isOutputPossible(_disp.getOutput(), fRRCrtc);
        if (lTempCRTC_RandR.id != None)
          break;
      }

      if (lTempCRTC_RandR.id != None) {
        lTempCRTC_RandR.mode    = _disp.getMode();
        lTempCRTC_RandR.outputs = _disp.getClones();
        lTempCRTC_RandR.outputs.push_back(_disp.getOutput());
        _disp.getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
        lTempCRTC_RandR.rotation = RR_Rotate_0;

        vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
        return true;
      }
    }

    // We can not use a fresh CRTC
    if (_disp.getClones().empty()) {
      wLOG("RandR: Impossible to enable the Output ",
           _disp.getOutput(),
           " without cloning --> do nothing ( return false (iRandR::setDisplaySizes))");
      return false;
    }

    for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
      for (RROutput const &fRROut : fCRTC.outputs) {
        for (RROutput const &fRRClone : _disp.getClones()) {
          if (fRROut == fRRClone) {
            lTempCRTC_RandR = isOutputPossible(_disp.getOutput(), fCRTC.id);
            if (lTempCRTC_RandR.id != None) {
              lTempCRTC_RandR.mode    = _disp.getMode();
              lTempCRTC_RandR.outputs = _disp.getClones();
              lTempCRTC_RandR.outputs.push_back(_disp.getOutput());
              _disp.getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
              lTempCRTC_RandR.rotation = RR_Rotate_0;

              vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
              return true;
            }
          }
        }
      }
    }

    // Also finding the crtc with the clone failed
    return false;
  }

  // ========== Change something enabled
  // =============================================================================================
  if (_disp.getIsEnabled() && lSingleMatch_XRR != None && lClonesMatch_XRR == None) {
    // We want to change a enabled output
    lTempCRTC_RandR.id      = lSingleMatch_XRR;
    lTempCRTC_RandR.mode    = _disp.getMode();
    lTempCRTC_RandR.outputs = _disp.getClones();
    lTempCRTC_RandR.outputs.push_back(_disp.getOutput());
    _disp.getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
    lTempCRTC_RandR.rotation = RR_Rotate_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return true;
  }

  // ========== Change something cloned
  // =============================================================================================
  if (_disp.getIsEnabled() && lSingleMatch_XRR == None && lClonesMatch_XRR != None) {
    std::vector<RROutput> lTempOutputs_V_XRR;
    for (internal::_crtc const &fCRTC : vCRTC_V_RandR) {
      if (lClonesMatch_XRR == fCRTC.id) {
        lTempCRTC_RandR = fCRTC;
        break;
      }
    }

    lTempCRTC_RandR.mode = _disp.getMode();
    _disp.getSelectedPosition(lTempCRTC_RandR.posX, lTempCRTC_RandR.posY);
    lTempCRTC_RandR.rotation = RR_Rotate_0;

    vChangeCRTC_V_RandR.push_back(lTempCRTC_RandR);
    return true;
  }

  wLOG(
      "Congratulations! It should be theoretically impossible to reach the end of this function "
      "[ iRandR::setDisplaySizes(...); ]");

  return false;
}

} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
