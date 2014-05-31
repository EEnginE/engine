/*!
 * \file eDisplayBasic.cpp
 * \brief \b Classes: \a eDisplayBasic
 */

#include "eDisplayBasic.hpp"

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
   vPosX_uI = _posX;
   vPosY_uI = _posY;
}

//! \brief Set the position relative to another display
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


/*!
 * \brief Returns the selected position via references
 * 
 * \param[out] _posX Position X in Pixels
 * \param[out] _posY Position Y in Pixels
 * 
 * \returns Nothing
 */
void eDisplayBasic::getSelectedPosition( int &_posX, int &_posY ) const {
   _posX = vPosX_uI;
   _posY = vPosY_uI;
}

/*!
 * \brief Get the current width, height, and rate
 * 
 * \param[out] _width  The current width
 * \param[out] _height The current height
 * \param[out] _rate   The currnet rate
 * 
 * \returns Nothing
 */
void eDisplayBasic::getCurrentResolution( unsigned int &_width, unsigned int &_height, double &_rate ) const {
   _width  = vCurrentWidth_uI;
   _height = vCurrentHeight_uI;
   _rate   = vCurrentRate_D;
}



}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
