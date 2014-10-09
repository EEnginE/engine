/*!
 * \file x11/RandR/iRandR.hpp
 * \brief \b Classes: \a iRandR
 */

#ifndef E_RANDR_HPP
#define E_RANDR_HPP

#include "iRandR_structs.hpp"
#include "iDisplays.hpp"
#include "engine_init_Export.hpp"

namespace e_engine {

namespace unix_x11 {

/*!
 * \brief Class for changing the screen size in X11
 *
 * This class uses XRandR for positioning the displays and
 * for changing their resolution.
 * 
 * This class "only" checks and loads the configuration and applies it. This class
 * generates iDisplays objects containing the display ID and some configurations.
 * 
 * If you want to change anything, you need to get those via getDisplayResolutions()
 * and change them. Then you can feed them back to this class. Some changes will only
 * be made if you call applyNewRandRSettings().
 *
 * \todo Add rotation support
 * \todo Add subpixel order support -- internal::_output::subpixel_order
 * \todo Add mode flags support -- internal::_mode::modeFlags
 */
class init_EXPORT iRandR {

   private:
      std::vector<internal::_crtc>   vCRTC_V_RandR;            //!< all current CRTC ( Data saved in \c internal::_crtc )
      std::vector<internal::_output> vOutput_V_RandR;          //!< all current possible outputs ( Data saved in \c internal::_output )
      std::list<internal::_mode>     vMode_V_RandR;            //!< all current available modes ( Data saved in \c internal::_mode )

      internal::_config              vDefaultConfig_RandR;     //!< the RandR config when init(); was called
      internal::_config              vLatestConfig_RandR;      //!< the RandR config before <b>applyNewSettings();</b> was called

      std::vector<internal::_crtc>   vChangeCRTC_V_RandR;

      Display                                *vDisplay_X11;             //!< The X11 display      -- set in init(...);
      Window                                  vWindow_X11;              //!< The X11 window       -- set in init(...);
      Window                                  vRootWindow_X11;          //!< The X11 root window  -- set in init(...);

      XRRScreenResources                     *vResources_XRR;
      XRRScreenConfiguration                 *vConfig_XRR;

      unsigned int                            vScreenWidth_uI;
      unsigned int                            vScreenHeight_uI;

      int                                     vRandRVersionMajor_I;
      int                                     vRandRVersionMinor_I;

      bool                                    vIsRandRSupported_B;

      bool  reload( bool _overwriteLatest = true, bool _overwriteDefaults = false );
      bool  restore( internal::_config _conf );
      int   changeCRTC( e_engine::internal::_crtc _changeToThis );

      internal::_crtc isOutputPossible( RROutput _id, RRCrtc _crtc );

   protected:
      void endRandR();
      
      bool initRandR( Display *_display, Window _window, Window _root );


   public:
      iRandR();
      virtual ~iRandR();

      void printRandRStatus();

      bool setGamma( iDisplays const &_disp, float _r, float _g, float _b, float _brightness = 1 );

      void getMostLeftRightTopBottomCRTC( unsigned int &_left, unsigned int &_right, unsigned int &_top, unsigned int &_bottom );
      int  getIndexOfDisplay( iDisplays const &_disp );

      std::vector<iDisplays> getDisplayResolutions();

      bool setDisplaySizes( iDisplays const &_disp );
      bool setPrimary( iDisplays const &_disp );

      bool applyNewRandRSettings();

      bool restoreScreenDefaults()  {return restore( vDefaultConfig_RandR );}
      bool restoreScreenLatest()    {return restore( vLatestConfig_RandR );}

      bool isRandRSupported() { return vIsRandRSupported_B; }

      void getRandRVersion( int &_vMajor, int &_vMinor )                      {_vMajor = vRandRVersionMajor_I; _vMinor = vRandRVersionMinor_I;}
      void getScreenResolution( unsigned int &_width, unsigned int &_height ) {_width  = vScreenWidth_uI; _height = vScreenHeight_uI;}
};

//    #########################
// ######## BEGIN DOXYGEN ########
//    #########################

/*!
 * \fn bool iRandR::restoreDefaults()
 * \brief Restores the RandR config before anything was changed.
 *
 * \returns \a true when everything went fine and \a false when there was an error
 */

/*!
 * \fn bool iRandR::restoreLatest()
 * \brief Restores the RandR config before the last change.
 *
 * \returns \a true when everything went fine and \a false when there was an error
 */

/*!
 * \fn void iRandR::getVersion( GLint &_vMajor, GLint &_vMinor )
 * \brief Get the RandR version
 *
 * \param[out] _vMajor the major version
 * \param[out] _vMinor the minor version
 *
 * \returns Nothing
 */

/*!
 * \fn void iRandR::getScreenResolution( GLuint &_width, GLuint &_height )
 * \brief Get the current screen resolution
 *
 * \param[out] _width  the current width
 * \param[out] _height the current height
 *
 * \returns Nothing
 */

/*!
 * \fn bool iRandR::isRandRSupported()
 * \brief Check if RandR is supported
 *
 * \returns \a true when RandR is support and \a false when not.
 */

} // unix_x11

} // e_engine

#endif // E_RANDR_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;

