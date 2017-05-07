/*!
 * \file x11/RandR/iDisplayRandR.hpp
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

#pragma once

#include "defines.hpp"

#include "iDisplayBasic.hpp"
#include <xcb/randr.h>

namespace e_engine {

namespace unix_x11 {

class iRandR;

/*!
 * \brief Contains information about the current Display
 *
 * Changes in objects of this class wont impact the current display
 * configuration. See iRandR for more details
 *
 * The only way to get a object of this class is calling iRandR::getDisplay()
 *
 * \note To apply the changes made here, you must call iRandR::setDisplaySizes()
 *       first and then iRandR::applyNewSettings()
 */
class iDisplayRandR final : public iDisplayBasic {
 private:
  //! \brief internal structure for storing important mode information.
  struct mode : mode_basic {
    xcb_randr_output_t id;
  };

  std::vector<mode> vModes_V_mode; //!< all possible modes

  xcb_randr_output_t              vID_XRR;
  std::vector<xcb_randr_output_t> vClones_V_XRR;

  xcb_randr_mode_t vModeToUse_XRR;


  iDisplayRandR() {}

  void addClone(xcb_randr_output_t _clone);
  void addMode(xcb_randr_mode_t _id, bool _prefered, unsigned int _width, unsigned int _height, double _rate);

  std::vector<xcb_randr_output_t> getClones() const { return vClones_V_XRR; }
  xcb_randr_mode_t                getMode() const { return vModeToUse_XRR; }
  xcb_randr_output_t              getOutput() const { return vID_XRR; }
  bool                            getIsEnabled() const { return vEnabled_B; }

  double findNearestFreqTo(
      double _rate, unsigned int _width, unsigned int _height, xcb_randr_mode_t &_mode, double &_diff) const;

 public:
  virtual ~iDisplayRandR();

  iDisplayRandR(std::string _name, xcb_randr_output_t _id, bool _enabled) : vID_XRR(_id) {
    vModeToUse_XRR = XCB_NONE;
    vEnabled_B     = _enabled;
    vName_str      = _name;
  }

  iDisplayRandR(const iDisplayRandR &_e) = delete;
  iDisplayRandR(const iDisplayRandR &&_e);

  iDisplayRandR &operator=(const iDisplayRandR &_e) = delete;
  iDisplayRandR &operator                           =(const iDisplayRandR &&_e);

  void autoSelectBest();

  void disable();
  void enable();

  std::vector<iDisplayBasic::res> getPossibleResolutions() const;
  bool isSizeSupported(unsigned int _width, unsigned int _height) const;

  std::vector<double> getPossibleRates(unsigned int _width, unsigned int _height) const;

  double autoSelectBySize(unsigned int _width, unsigned int _height, double _preferedRate = 0, double _maxDiff = 1);
  bool select(unsigned int _width, unsigned int _height, double _rate);

  void setNoClones() { vClones_V_XRR.clear(); }
  void setCloneOf(iDisplayRandR const &_disp);

  void getSelectedRes(unsigned int &_width, unsigned int &_height, double &_rate) const;

  friend class iRandR;
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################

/*!
 * \fn void iDisplayRandR::setNoClones()
 * \brief Disable all clones of this display
 *
 * \returns Nothing
 */

} // unix_x11

} // e_engine


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
