/*!
 * \file x11/RandR/randr.hpp
 * \brief \b Classes: \a eRandR
 */

#ifndef E_RANDR_HPP
#define E_RANDR_HPP

#include "randr_structs.hpp"
#include "displays.hpp"

namespace e_engine {


/*!
 * \class e_engine::eRandR
 * \brief Class for changing the screen size in X11
 *
 * This class uses XRandR for positioning the displays and
 * for changing their resolution.
 * 
 * This class "only" checks and loads the configuration and applies it. This class
 * generates eDisplays objects containing the display ID and some configurations.
 * 
 * If you want to change anything, you need to get those via getDisplayResolutions()
 * and change them. Then you can feed them back to this class. Some changes will only
 * be made if you call applyNewRandRSettings().
 *
 * \todo Add rotation support
 * \todo Add subpixel order support -- e_engine_internal::_output::subpixel_order
 * \todo Add mode flags support -- e_engine_internal::_mode::modeFlags
 */
class eRandR {

   private:
      std::vector<e_engine_internal::_crtc>   vCRTC_V_RandR;            //!< all current CRTC ( Data saved in \c e_engine_internal::_crtc )
      std::vector<e_engine_internal::_output> vOutput_V_RandR;          //!< all current possible outputs ( Data saved in \c e_engine_internal::_output )
      std::list<e_engine_internal::_mode>     vMode_V_RandR;            //!< all current available modes ( Data saved in \c e_engine_internal::_mode )

      e_engine_internal::_config              vDefaultConfig_RandR;     //!< the RandR config when init(); was called
      e_engine_internal::_config              vLatestConfig_RandR;      //!< the RandR config before <b>applyNewSettings();</b> was called

      std::vector<e_engine_internal::_crtc>   vChangeCRTC_V_RandR;

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
      bool  restore( e_engine_internal::_config _conf );
      int   changeCRTC( e_engine::e_engine_internal::_crtc _changeToThis );

      e_engine_internal::_crtc isOutputPossible( RROutput _id, RRCrtc _crtc );

   protected:
      void endRandR();


   public:
      eRandR();
      virtual ~eRandR();

      void printRandRStatus();

      bool setGamma( eDisplays const &_disp, float _r, float _g, float _b, float _brightness = 1 );

      void getMostLeftRightTopBottomCRTC( unsigned int &_left, unsigned int &_right, unsigned int &_top, unsigned int &_bottom );
      int  getIndexOfDisplay( eDisplays const &_disp );

      bool initRandR( Display *_display, Window _window, Window _root );

      std::vector<eDisplays> getDisplayResolutions();

      bool setDisplaySizes( eDisplays const &_disp );
      void setPrimary( eDisplays const &_disp );

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
 * \fn bool eRandR::restoreDefaults()
 * \brief Restores the RandR config before anything was changed.
 *
 * \returns \a true when everything went fine and \a false when there was an error
 */

/*!
 * \fn bool eRandR::restoreLatest()
 * \brief Restores the RandR config before the last change.
 *
 * \returns \a true when everything went fine and \a false when there was an error
 */

/*!
 * \fn void eRandR::getVersion( GLint &_vMajor, GLint &_vMinor )
 * \brief Get the RandR version
 *
 * \param[out] _vMajor the major version
 * \param[out] _vMinor the minor version
 *
 * \returns Nothing
 */

/*!
 * \fn void eRandR::getScreenResolution( GLuint &_width, GLuint &_height )
 * \brief Get the current screen resolution
 *
 * \param[out] _width  the current width
 * \param[out] _height the current height
 *
 * \returns Nothing
 */

/*!
 * \fn bool eRandR::isRandRSupported()
 * \brief Check if RandR is supported
 *
 * \returns \a true when RandR is support and \a false when not.
 */

}

#endif // E_RANDR_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 

