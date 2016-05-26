/*!
 * \file iDisplayBasic.hpp
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

#pragma once

#include "defines.hpp"

#include <string>
#include <vector>

namespace e_engine {

class INIT_API iDisplayBasic {
 public:
   /*!
    * \brief Describes the position of one display relative to another
    */
   enum POSITON {
      LEFT_OF,  //!< Place the display in the \a argument LEFT OF the \a calling display
      RIGHT_OF, //!< Place the \a calling display RIGHT OF the display in the \a argument
      ABOVE,    //!< Place the display in the \a argument ABOVE the \a calling display
      BELOW     //!< Place the \a calling display ABOVE the display in the \a argument
   };


   //! \brief Simple structure for storing mode resolution.
   struct res {
      unsigned int width;  //!< the width of a mode
      unsigned int height; //!< the height of a mode
   };

 protected:
   //! \brief Basic mode class for storing mode information
   struct mode_basic {
      bool prefered; //!< Is this mode prefered (only ONE mode should have this set to true)

      unsigned int width;  //!< The width of this mode
      unsigned int height; //!< The height of this mode
      double       rate;   //!< The display redraw frequency of this mode
   };

   bool vEnabled_B;   //!< Is the display enabled?
   bool vIsPrimary_B; //!< Is this the primary display? Only one eDisplay should have this true
   bool vPositionChanged_B; //!< Has the user changed the positon of this display?

   std::string  vName_str;         //!< The name of this display
   unsigned int vCurrentWidth_uI;  //!< The current width, without the changes the user made
   unsigned int vCurrentHeight_uI; //!< The current height, without the changes the user made
   unsigned int vPosX_uI;          //!< The position (X) of the display WITH the userchanges
   unsigned int vPosY_uI;          //!< The position (Y) of the display WITH the userchanges

   double vCurrentRate_D; //!< The current display frequency, without the changes the user made

   void setCurrentSizeAndPosition( unsigned int _width,
                                   unsigned int _height,
                                   unsigned int _posX,
                                   unsigned int _posY,
                                   unsigned int _rate );

   iDisplayBasic() : vPositionChanged_B( false ) {}

 public:
   virtual ~iDisplayBasic();

   virtual void disable() = 0;
   virtual void enable()  = 0;

   void setPositionAbsolute( unsigned int _posX, unsigned int _posY );
   void setPositionRelative( e_engine::iDisplayBasic::POSITON _where,
                             e_engine::iDisplayBasic &        _disp );

   void getSelectedPosition( int &_posX, int &_posY ) const;
   void getCurrentResolution( unsigned int &_width, unsigned int &_height, double &_rate ) const;

   bool getIsPrimary() const { return vIsPrimary_B; }

   std::string getName() const { return vName_str; }
};

/*!
 * \fn std::string iDisplayBasic::getName()
 * \brief Get the name of the display
 *
 * \returns The name of the display
 */
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
