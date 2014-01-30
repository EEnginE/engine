/// \file randr_display.hpp
/// \brief \b Classes: \a eRandRDisplay

#ifndef E_RANDR_DISPLAY_HPP
#define E_RANDR_DISPLAY_HPP

#include <X11/extensions/Xrandr.h>
#include <vector>
#include <string>

namespace e_engine {

class eRandR;

/*!
 * \brief Contains information about the current Display
 *
 * Changes in objects of this class wont effect the current display
 * configuration. Se eRandR for mor details
 *
 * The only way to get a object of this class is calling eRandR::getDisplay()
 *
 * \note To apply the changes made here, you must call eRandR::setDisplaySizes()
 *       first and then eRandR::applyNewSettings()
 */
class eRandRDisplay {
   public:
      /*!
       * \brief describes the position of one display relative to an other
       */
      enum POSITON {
         LEFT_OFF,   //!< Place the display int the \a argument LEFT OFF the \a calling display
         RIGHT_OFF,  //!< Place the \a calling display LEFT OFF the display int the \a argument
         ABOVE,      //!< Place the display int the \a argument ABOVE the \a calling display
         BELOW       //!< Place the \a calling display ABOVE the display int the \a argument
      };


      //! \brief simple structure for storing mode resolution.
      struct res {
         unsigned int width;  //!< the width of a mode
         unsigned int height; //!< the height of a mode
      };

   private:
      //! \brief internal structure for storing important mode information.
      struct mode {
         RRMode       id;

         bool         prefered;

         unsigned int width;
         unsigned int height;
         double       rate;
      };

      std::vector<mode>    vModes_V_mode;      //!< all possible modes

      bool                  vEnabled_B;

      std::string           vName_str;
      unsigned int          vCurrentWidth_uI;
      unsigned int          vCurrentHeight_uI;
      unsigned int          vPosX_uI;
      unsigned int          vPosY_uI;

      double                vCurrentRate_D;
      RROutput              vID_XRR;
      std::vector<RROutput> vClones_V_XRR;

      RRMode                vModeToUse_XRR;


      eRandRDisplay() {}
      eRandRDisplay( std::string _name, RROutput _id, bool _enabled ) : vEnabled_B( _enabled ), vName_str( _name ), vID_XRR( _id ) {vModeToUse_XRR = None;}

      void setCurrentSizeAndPosition( unsigned int _width, unsigned int _height, unsigned int _posX, unsigned int _posY, unsigned int _rate );
      void addClone( RROutput _clone );
      void addMode( RRMode _id, bool _prefered, unsigned int _width, unsigned int _height, double _rate );

      std::vector<RROutput> getClones()    { return vClones_V_XRR; }
      RRMode                getMode()      { return vModeToUse_XRR; }
      RROutput              getOutput()    { return vID_XRR; }
      bool                  getIsEnabled() { return vEnabled_B; }

      double                findNearestFreqTo( double _rate, unsigned int _width, unsigned int _height, RRMode &_mode, double &_diff );
   public:
      // Allow only eRandR to create and change this class
      friend class eRandR;

      void disable();
      void enable();

      void autoSelectBest();

      std::vector<eRandRDisplay::res> getPossibleResolutions();
      bool isSizeSupported( unsigned int _width, unsigned int _height );

      std::vector<double> getPossibleRates( unsigned int _width, unsigned int _height );

      double autoSelectBySize( unsigned int _width, unsigned int _height, double _preferedRate = 0, double _maxDiff = 1 );
      bool   select( unsigned int _width, unsigned int _height, double _rate );

      void setPositionAbsolute( unsigned int _posX, unsigned int _posY );
      void setPositionRelative( e_engine::eRandRDisplay::POSITON _where, e_engine::eRandRDisplay &_disp );

      void setNoClones() { vClones_V_XRR.clear(); }
      void setCloneOf( eRandRDisplay const &_disp );

      void getSelectedPosition( int &_posX, int &_posY );
      void getSelectedRes( unsigned int &_width, unsigned int &_height, double &_rate );
      std::string getName() { return vName_str; }
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################

/*!
 * \fn void eRandRDisplay::setNoClones()
 * \brief Disable all clones of this display
 *
 * \returns Nothing
 */

/*!
 * \fn std::string eRandRDisplay::getName()
 * \brief Get the name of the display
 *
 * \returns The name of the display
 */

}

#endif // E_RANDR_DISPLAY_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
