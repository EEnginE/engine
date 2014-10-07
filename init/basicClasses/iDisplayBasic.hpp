/*!
 * \file iDisplayBasic.hpp
 * \brief \b Classes: \a iDisplayBasic
 */

#ifndef DISPLAYS_BASIC_HPP
#define DISPLAYS_BASIC_HPP

#include <vector>
#include <string>
#include "engine_init_Export.hpp"

namespace e_engine {

class init_EXPORT iDisplayBasic {
   public:
      /*!
       * \brief describes the position of one display relative to an other
       */
      enum POSITON {
         LEFT_OFF,   //!< Place the display in the \a argument LEFT OFF the \a calling display
         RIGHT_OFF,  //!< Place the \a calling display LEFT OFF the display in the \a argument
         ABOVE,      //!< Place the display in the \a argument ABOVE the \a calling display
         BELOW       //!< Place the \a calling display ABOVE the display in the \a argument
      };


      //! \brief simple structure for storing mode resolution.
      struct res {
         unsigned int width;  //!< the width of a mode
         unsigned int height; //!< the height of a mode
      };

   protected:
      //! \brief Basic mode class for storing mode information
      struct mode_basic {
         bool         prefered; //!< Is this mode prefered (only ONE mode schould have this true)

         unsigned int width;    //!< The Width of this mode
         unsigned int height;   //!< The height of this mode
         double       rate;     //!< The display redraw frequency of this mode
      };
      
      bool            vEnabled_B;          //!< Is the display enabbled
      bool            vIsPrimary_B;        //!< Is this the primary display? Onl one eDisplay schould have this true
      bool            vPositionChanged_B;  //!< Has the user changed the positon of this display

      std::string     vName_str;           //!< The name of this display
      unsigned int    vCurrentWidth_uI;    //!< The current width, without the changes the user made
      unsigned int    vCurrentHeight_uI;   //!< The current height, without the changes the user made
      unsigned int    vPosX_uI;            //!< The position (X) of the display WITH the userchanges
      unsigned int    vPosY_uI;            //!< The position (Y) of the display WITH the userchanges
      
      double          vCurrentRate_D;      //!< The current display frequency, without the changes the user made
      
      void setCurrentSizeAndPosition( unsigned int _width, unsigned int _height, unsigned int _posX, unsigned int _posY, unsigned int _rate );
      
      iDisplayBasic() : vPositionChanged_B(false) {}

   public:
      virtual ~iDisplayBasic() {}
      
      virtual void disable() = 0;
      virtual void enable()  = 0;
      
      void setPositionAbsolute( unsigned int _posX, unsigned int _posY );
      void setPositionRelative( e_engine::iDisplayBasic::POSITON _where, e_engine::iDisplayBasic &_disp );
      
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


#endif // DISPLAYS_BASIC_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
