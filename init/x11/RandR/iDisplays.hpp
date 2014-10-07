/*!
 * \file x11/RandR/iDisplays.hpp
 * \brief \b Classes: \a iDisplays
 */

#ifndef E_RANDR_DISPLAY_HPP
#define E_RANDR_DISPLAY_HPP

#include <X11/extensions/Xrandr.h>
#include "iDisplayBasic.hpp"
#include "engine_init_Export.hpp"

namespace e_engine {

namespace unix_x11 {

class init_EXPORT iRandR;

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
class init_EXPORT iDisplays : public iDisplayBasic {
   private:
      //! \brief internal structure for storing important mode information.
      struct mode : mode_basic {
         RRMode       id;
      };

      std::vector<mode>     vModes_V_mode;      //!< all possible modes
      
      RROutput              vID_XRR;
      std::vector<RROutput> vClones_V_XRR;

      RRMode                vModeToUse_XRR;


      iDisplays() {}
      iDisplays( std::string _name, RROutput _id, bool _enabled ) : vID_XRR( _id ) {vModeToUse_XRR = None; vEnabled_B = _enabled; vName_str = _name;}

      void addClone( RROutput _clone );
      void addMode( RRMode _id, bool _prefered, unsigned int _width, unsigned int _height, double _rate );

      std::vector<RROutput> getClones()    const { return vClones_V_XRR; }
      RRMode                getMode()      const { return vModeToUse_XRR; }
      RROutput              getOutput()    const { return vID_XRR; }
      bool                  getIsEnabled() const { return vEnabled_B; }

      double                findNearestFreqTo( double _rate, unsigned int _width, unsigned int _height, RRMode &_mode, double &_diff ) const;
   public:
      virtual ~iDisplays() {}
      
      void autoSelectBest();

      void disable();
      void enable();

      std::vector<iDisplayBasic::res> getPossibleResolutions() const;
      bool isSizeSupported( unsigned int _width, unsigned int _height ) const;

      std::vector<double> getPossibleRates( unsigned int _width, unsigned int _height ) const;

      double autoSelectBySize( unsigned int _width, unsigned int _height, double _preferedRate = 0, double _maxDiff = 1 );
      bool   select( unsigned int _width, unsigned int _height, double _rate );

      void setNoClones() { vClones_V_XRR.clear(); }
      void setCloneOf( iDisplays const &_disp );

      void getSelectedRes( unsigned int &_width, unsigned int &_height, double &_rate ) const;
      
      friend class iRandR;
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################

/*!
 * \fn void iDisplays::setNoClones()
 * \brief Disable all clones of this display
 *
 * \returns Nothing
 */

} // unix_x11

} // e_engine

#endif // E_RANDR_DISPLAY_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
