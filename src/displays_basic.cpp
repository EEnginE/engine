/*!
 * \file displays_basic.cpp
 * \brief \b Classes: \a eDisplayBasic
 */

#include "displays_basic.hpp"

namespace e_engine {

// --- private ---
void eDisplayBasic::setCurrentSizeAndPosition( unsigned int _width, unsigned int _height, unsigned int _posX, unsigned int _posY, unsigned int _rate ) {
   vCurrentWidth_uI  = _width;
   vCurrentHeight_uI = _height;
   vPosX_uI          = _posX;
   vPosY_uI          = _posY;
   vCurrentRate_D    = _rate;
}

//! \brief Set the absolute position of this display
void eDisplayBasic::setPositionAbsolute( unsigned int _posX, unsigned int _posY ) {
   vPosX_uI = ( _posX < 0 ) ? 0 : _posX;
   vPosY_uI = ( _posY < 0 ) ? 0 : _posY;
}

//! \brief Set the position relative to an other display
void eDisplayBasic::setPositionRelative( eDisplayBasic::POSITON _where, eDisplayBasic &_disp ) {
   switch ( _where ) {
      case LEFT_OFF:
         _disp.vPosX_uI = vPosX_uI + vCurrentWidth_uI;
         _disp.vPosY_uI = vPosY_uI;
         break;
      case RIGHT_OFF:
         vPosX_uI       = _disp.vPosX_uI + _disp.vCurrentWidth_uI;
         vPosY_uI       = _disp.vPosY_uI;
         break;
      case ABOVE:
         _disp.vPosX_uI = vPosX_uI;
         _disp.vPosY_uI = vPosY_uI + vCurrentHeight_uI;
         break;
      case BELOW:
         vPosX_uI       = _disp.vPosX_uI;
         vPosY_uI       = _disp.vPosY_uI + _disp.vCurrentHeight_uI;
         break;
   }
}


//! \brief Returns the selected position via references
void eDisplayBasic::getSelectedPosition( int &_posX, int &_posY ) const {
   _posX = vPosX_uI;
   _posY = vPosY_uI;
}


}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 