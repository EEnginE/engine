/*!
 * \file eDisplayBasic.hpp
 * \brief \b Classes: \a eDisplayBasic
 */

#ifndef DISPLAYS_BASIC_HPP
#define DISPLAYS_BASIC_HPP

#include <vector>
#include <string>

namespace e_engine {

class eDisplayBasic {
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
      struct mode_basic {
         bool         prefered;

         unsigned int width;
         unsigned int height;
         double       rate;
      };
      
      bool                  vEnabled_B;
      bool                  vIsPrimary_B;

      std::string           vName_str;
      unsigned int          vCurrentWidth_uI;
      unsigned int          vCurrentHeight_uI;
      unsigned int          vPosX_uI;
      unsigned int          vPosY_uI;
      
      double                vCurrentRate_D;
      
      void setCurrentSizeAndPosition( unsigned int _width, unsigned int _height, unsigned int _posX, unsigned int _posY, unsigned int _rate );

   public:
      virtual ~eDisplayBasic() {}
      
      virtual void disable() = 0;
      virtual void enable()  = 0;
      
      void setPositionAbsolute( unsigned int _posX, unsigned int _posY );
      void setPositionRelative( e_engine::eDisplayBasic::POSITON _where, e_engine::eDisplayBasic &_disp );
      
      void getSelectedPosition( int &_posX, int &_posY ) const;
      void getCurrentResolution( unsigned int &_width, unsigned int &_height, double &_rate ) const;
      
      bool getIsPrimary() const { return vIsPrimary_B; }
      
      std::string getName() const { return vName_str; }
};

/*!
 * \fn std::string eDisplayBasic::getName()
 * \brief Get the name of the display
 *
 * \returns The name of the display
 */

}


#endif // DISPLAYS_BASIC_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
