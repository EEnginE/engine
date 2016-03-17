/*!
 * \file windows/iRandR.hpp
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

#pragma once

#include "defines.hpp"

#include "iDisplays.hpp"

namespace e_engine {

namespace windows_win32 {

class INIT_API iRandR {
 private:
   unsigned int vScreenWidth_uI;
   unsigned int vScreenHeight_uI;

   std::vector<iDisplays> vCurrentConfig_eD;
   std::vector<iDisplays> vPreviousConfig_eD;
   std::vector<iDisplays> vDefaultConfig_eD;
   std::vector<iDisplays> vDisplaysToChange_eD;

   void reload();

 public:
   iRandR();
   virtual ~iRandR();

   void printRandRStatus();

   bool setGamma( iDisplays const &_disp, float _r, float _g, float _b, float _brightness = 1 );

   void getMostLeftRightTopBottomCRTC( unsigned int &_left,
                                       unsigned int &_right,
                                       unsigned int &_top,
                                       unsigned int &_bottom );
   int getIndexOfDisplay( iDisplays const & ) { return -1; }

   std::vector<iDisplays> getDisplayResolutions() { return vCurrentConfig_eD; }

   bool setDisplaySizes( iDisplays const &_disp );
   bool setPrimary( const e_engine::windows_win32::iDisplays &_disp );

   bool applyNewRandRSettings();

   bool restoreScreenDefaults();
   bool restoreScreenLatest();

   bool isRandRSupported() { return true; }

   void getRandRVersion( int &_vMajor, int &_vMinor ) {
      _vMajor = -1;
      _vMinor = -1;
   }
   void getScreenResolution( unsigned int &_width, unsigned int &_height ) {
      _width  = vScreenWidth_uI;
      _height = vScreenHeight_uI;
   }
};

/*!
 * \fn iRandR::getRandRVersion
 * \brief Does nothing
 *
 * _vMajor and _vMinor will always be -1 because Windows has no
 * special version for window resolution stuff.
 *
 * \param[out] _vMajor Major version
 * \param[out] _vMinor Minor version
 */

/*!
 * \fn iRandR::getScreenResolution
 * \brief Get the current screen resolution
 *
 * \param[out] _width  The current width
 * \param[out] _height The current height
 */

} // windows_win32

} // e_engine


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
