/*!
 * \file iDisplayBasic.cpp
 * \brief \b Classes: \a iDisplayBasic
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

#include "iDisplayBasic.hpp"

namespace e_engine {

iDisplayBasic::~iDisplayBasic() {}

// --- private ---
void iDisplayBasic::setCurrentSizeAndPosition( unsigned int _width,
                                               unsigned int _height,
                                               unsigned int _posX,
                                               unsigned int _posY,
                                               unsigned int _rate ) {
   vCurrentWidth_uI  = _width;
   vCurrentHeight_uI = _height;
   vPosX_uI          = _posX;
   vPosY_uI          = _posY;
   vCurrentRate_D    = _rate;
}

//! \brief Set the absolute position of this display
void iDisplayBasic::setPositionAbsolute( unsigned int _posX, unsigned int _posY ) {
   vPosX_uI           = _posX;
   vPosY_uI           = _posY;
   vPositionChanged_B = true;
}

//! \brief Set the position relative to another display
void iDisplayBasic::setPositionRelative( iDisplayBasic::POSITON _where, iDisplayBasic &_disp ) {
   switch ( _where ) {
      case LEFT_OF:
         _disp.vPosX_uI = vPosX_uI + vCurrentWidth_uI;
         _disp.vPosY_uI = vPosY_uI;
         break;
      case RIGHT_OF:
         vPosX_uI = _disp.vPosX_uI + _disp.vCurrentWidth_uI;
         vPosY_uI = _disp.vPosY_uI;
         break;
      case ABOVE:
         _disp.vPosX_uI = vPosX_uI;
         _disp.vPosY_uI = vPosY_uI + vCurrentHeight_uI;
         break;
      case BELOW:
         vPosX_uI = _disp.vPosX_uI;
         vPosY_uI = _disp.vPosY_uI + _disp.vCurrentHeight_uI;
         break;
   }
   vPositionChanged_B = true;
}


/*!
 * \brief Returns the selected position via references
 *
 * \param[out] _posX Position X in Pixels
 * \param[out] _posY Position Y in Pixels
 */
void iDisplayBasic::getSelectedPosition( int &_posX, int &_posY ) const {
   _posX = static_cast<int>( vPosX_uI );
   _posY = static_cast<int>( vPosY_uI );
}

/*!
 * \brief Get the current width, height, and rate
 *
 * \param[out] _width  The current width
 * \param[out] _height The current height
 * \param[out] _rate   The current rate
 */
void iDisplayBasic::getCurrentResolution( unsigned int &_width,
                                          unsigned int &_height,
                                          double &_rate ) const {
   _width  = vCurrentWidth_uI;
   _height = vCurrentHeight_uI;
   _rate   = vCurrentRate_D;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
