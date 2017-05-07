/*!
 * \file x11/RandR/iRandRBasic.hpp
 * \brief \b Classes: \a iRandRBasic
 */
/*
 * Copyright (C) 2017 EEnginE project
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

#pragma once

#include "defines.hpp"
#include "iDisplayBasic.hpp"
#include <memory>

namespace e_engine {

class iRandRBasic {
 public:
  enum ERROR_CODE {
    OK = 0,
    INVALID_DISPLAY_CLASS,
    INVALID_ARGUMENT,

    RANDR_XCB_ERROR = 1000,
    RANDR_CRTC_NOT_FOUND,
    RANDR_NOT_SUPPORTED,
    RANDR_OTHER_ERROR
  };

  iRandRBasic() = default;
  virtual ~iRandRBasic();

  iRandRBasic(const iRandRBasic &) = delete;
  iRandRBasic(iRandRBasic &&)      = delete;

  iRandRBasic &operator=(const iRandRBasic &) = delete;
  iRandRBasic &operator=(iRandRBasic &&) = delete;

  virtual void       printStatus() = 0;
  virtual ERROR_CODE setGamma(iDisplayBasic *_disp, double _r, double _g, double _b, double _brightness = 1) = 0;

  virtual ERROR_CODE getIndexOfDisplay(iDisplayBasic *_disp, uint32_t *index) = 0;
  virtual void getMostLeftRightTopBottomCRTC(unsigned int &_left,
                                             unsigned int &_right,
                                             unsigned int &_top,
                                             unsigned int &_bottom) = 0;

  virtual std::vector<std::shared_ptr<iDisplayBasic>> getDisplayResolutions() = 0;

  virtual ERROR_CODE setDisplaySizes(iDisplayBasic *_disp) = 0;
  virtual ERROR_CODE setPrimary(iDisplayBasic *_disp)      = 0;

  virtual ERROR_CODE applyNewRandRSettings() = 0;

  virtual ERROR_CODE restoreScreenDefaults() = 0;
  virtual ERROR_CODE restoreScreenLatest()   = 0;

  virtual void getRandRVersion(uint32_t &_vMajor, uint32_t &_vMinor)            = 0;
  virtual void getScreenResolution(unsigned int &_width, unsigned int &_height) = 0;

  virtual bool isProtocolSupported() = 0;
};
}
