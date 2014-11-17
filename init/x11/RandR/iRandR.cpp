/*!
 * \file x11/RandR/iRandR.cpp
 * \brief \b Classes: \a iRandR
 */

#include "iRandR.hpp"
#include "uLog.hpp"


namespace e_engine {

namespace unix_x11 {

/*!
 * \brief sets only some basic values.
 */
iRandR::iRandR() {
   vIsRandRSupported_B  = false;
   
   vRandRVersionMajor_I = -1;
   vRandRVersionMinor_I = -1;
   
   vScreenHeight_uI     = 0;
   vScreenWidth_uI      = 0;
   
   vDisplay_X11         = NULL;
   vWasScreenChanged_B  = false;
}

iRandR::~iRandR() {
   endRandR();
}



// =========================================================================================================================
// ==============================================================================================================================================
// =========                      ===========================================================================================================================
// =======   Init End and Restore   ==============================================================================================================================
// =========                      ===========================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================


/*!
 * \brief Sets the standard values and checks if RandR is supported
 * \param _display The connection to the X-Server
 * \param _window  The Window
 * \warning !!! DO NOT CALL THIS METHOD !!!
 *
 * This method is designed to be called from iContext::createDisplay().
 * It is possible to call this manually with a own display and window
 * but it is NOT recommended
 */
bool iRandR::initRandR( Display *_display, Window _window, Window _root ) {
   if ( vIsRandRSupported_B ) {
      return false;
   }

   int lTempVersion_I;

   vDisplay_X11    = _display;
   vWindow_X11     = _window;
   vRootWindow_X11 = _root;

   if ( XQueryExtension( vDisplay_X11, "RANDR", &lTempVersion_I, &lTempVersion_I, &lTempVersion_I ) ) {
      XRRQueryVersion( vDisplay_X11 , &vRandRVersionMajor_I, &vRandRVersionMinor_I );
      vIsRandRSupported_B = true;
   } else {
      wLOG( "X11 RandR standard not supported. Screen resolution wont be changed" );
      return vIsRandRSupported_B = false;
   }

   reload( true, true );

   vIsRandRSupported_B = true;

   printRandRStatus();

   return true;
}

/*!
 * \brief Ends RandR support
 * \warning !!! DO NOT CALL THIS METHOD !!!
 *
 * This method is designed to be called from iContext::destroy() or
 * from the destructor of this class. Calling this method will end
 * the RandR support.
 */
void iRandR::endRandR() {
   if ( ! vIsRandRSupported_B )
      return;

   if ( GlobConf.win.restoreOldScreenRes && vWasScreenChanged_B )
      restore( vDefaultConfig_RandR );


   for ( unsigned int i = 0; i < vDefaultConfig_RandR.gamma.size(); ++i )
      XRRFreeGamma( vDefaultConfig_RandR.gamma[i] );

   vDefaultConfig_RandR.gamma.clear();
   vDefaultConfig_RandR.CRTCInfo.clear();


   for ( unsigned int i = 0; i < vLatestConfig_RandR.gamma.size(); ++i )
      XRRFreeGamma( vLatestConfig_RandR.gamma[i] );


   vLatestConfig_RandR.gamma.clear();


   XRRFreeScreenConfigInfo( vConfig_XRR );
   XRRFreeScreenResources( vResources_XRR );

   vCRTC_V_RandR.clear();
   vOutput_V_RandR.clear();
   vMode_V_RandR.clear();

   vIsRandRSupported_B = false;
}

bool iRandR::restore( internal::_config _conf ) {
   if ( ! isRandRSupported() )
      return false;

   vChangeCRTC_V_RandR.clear();
   vChangeCRTC_V_RandR = _conf.CRTCInfo;

   if ( ! applyNewRandRSettings() )
      return false;

   // Who is primary?
   XRRSetOutputPrimary( vDisplay_X11, vRootWindow_X11, _conf.primary );


   // Reset gamma
   if ( _conf.gamma.size() != vCRTC_V_RandR.size() ) {
      eLOG( "RandR: Fatal internal ERROR:  _conf.gamma.size() != vCRTC_V_RandR.size()" );
      return false;
   }

   for ( unsigned int i = 0; i < _conf.CRTCInfo.size(); ++i ) {
      for ( unsigned int j = 0; j < _conf.gamma.size(); ++j ) {
         XRRSetCrtcGamma( vDisplay_X11, vCRTC_V_RandR[i].id, _conf.gamma[j] );
      }
   }

   return true;
}

// =========================================================================================================================
// ==============================================================================================================================================
// =========                   ==============================================================================================================================
// =======   GET Display Sizes   =================================================================================================================================
// =========                   ==============================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief Get all display information in a STL vector
 *
 * \returns a STL vector, containing all important display data
 * \throws Nothing
 *
 * \sa iRandRDisplay
 */
std::vector< iDisplays > iRandR::getDisplayResolutions() {
   std::vector<iDisplays> lTempSizes_eWS;
   if ( ! isRandRSupported() )
      return lTempSizes_eWS;
   reload( false );

   vMode_V_RandR.sort();

   for ( internal::_output const & fOutput : vOutput_V_RandR ) {
      if ( fOutput.connection != 0 )
         continue;

      iDisplays lTempDisplay_eRRD(
         fOutput.name,
         fOutput.id,
         ( fOutput.crtc != None ) ? true : false
      );

      if ( fOutput.crtc != None ) {

         // Find the CRTC of the output
         for ( internal::_crtc const & fCRTC : vCRTC_V_RandR ) {
            if ( fOutput.crtc == fCRTC.id ) {

               for ( internal::_mode const & fMode : vMode_V_RandR ) {
                  if ( fCRTC.mode == fMode.id ) {
                     lTempDisplay_eRRD.setCurrentSizeAndPosition(
                        fMode.width,
                        fMode.height,
                        fCRTC.posX,
                        fCRTC.posY,
                        fMode.refresh
                     );
                     break;
                  }
               }

               // Clones
               for ( RROutput const & fClone : fCRTC.outputs ) {
                  if ( fClone != fOutput.id )
                     lTempDisplay_eRRD.addClone( fClone );
               }

               break;
            }
         }
      }

      for ( internal::_mode const & fMode : vMode_V_RandR ) {
         // Is mode supported and / or preferred ?
         bool lModeSupported_B = false;
         bool lModePrefered_B  = false;
         for ( unsigned int j = 0; j < fOutput.modes.size(); ++j ) {
            if ( fOutput.modes[j] == fMode.id ) {
               lModeSupported_B = true;
               if ( j == ( unsigned int )fOutput.npreferred - 1 ) {
                  lModePrefered_B = true;
               }
               break;
            }
         }

         if ( ! lModeSupported_B )
            continue;

         lTempDisplay_eRRD.addMode(
            fMode.id,
            lModePrefered_B,
            fMode.width,
            fMode.height,
            fMode.refresh
         );

      }

      lTempSizes_eWS.push_back( lTempDisplay_eRRD );
   }

   return lTempSizes_eWS;
}


// =========================================================================================================================
// ==============================================================================================================================================
// =========         ========================================================================================================================================
// =======   Primary   ===========================================================================================================================================
// =========         ========================================================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief Set _disp to the primary display
 *
 * \param _disp The display that is to be set to primary
 *
 * \returns true if everything went fine
 *
 * \note This function will change the primary display IMMEDIATELY; Calling applyNewSettings() will have no effect to this.
 */
bool iRandR::setPrimary( iDisplays const &_disp ) {
   if ( ! isRandRSupported() )
      return false;
   XRRSetOutputPrimary( vDisplay_X11, vRootWindow_X11, _disp.getOutput() );
   return true;
}



// =========================================================================================================================
// ==============================================================================================================================================
// =========                                  ===============================================================================================================
// =======   Get CRTC position for fullscreen   ==================================================================================================================
// =========                                  ===============================================================================================================
// ==============================================================================================================================================
// =========================================================================================================================

/*!
 * \brief Find the most left, right, top, bottom CRTC
 * 
 * \param[out] _left   Index of the most left CRTC
 * \param[out] _right  Index of the most right CRTC
 * \param[out] _top    Index of the most top CRTC
 * \param[out] _bottom Index of the most bottom CRTC
 */
void iRandR::getMostLeftRightTopBottomCRTC( unsigned int &_left, unsigned int &_right, unsigned int &_top, unsigned int &_bottom ) {
   reload();
   
   int lMinX = -1;
   int lMaxX = -1;
   int lMinY = -1;
   int lMaxY = -1;
   
   _left = _right = _top = _bottom = 0;
   
   for( unsigned int index = 0; index < vCRTC_V_RandR.size(); ++index ) {
      if( vCRTC_V_RandR[index].posX < lMinX || lMinX == -1 ) {
         _left = index;
         lMinX = vCRTC_V_RandR[index].posX;
      }
      
      if( vCRTC_V_RandR[index].posY < lMinY || lMinY == -1 ) {
         _top    = index;
         lMinY   = vCRTC_V_RandR[index].posY;
      }
      
      if( vCRTC_V_RandR[index].posX > lMaxX || lMaxX == -1 ) {
         _right  = index;
         lMaxX   = vCRTC_V_RandR[index].posX;
      }
      
      if( vCRTC_V_RandR[index].posY > lMaxY || lMaxY == -1 ) {
         _bottom = index;
         lMaxY   = vCRTC_V_RandR[index].posY;
      }
   }
}

/*!
 * \brief Get the index of a iRandRDisplay
 * \returns The index of a iRandRDisplay
 * \returns -1 When the display is disabled
 * \returns -2 When the iRandRDisplay is out of date
 * \returns -10 When an impossible error happened
 */
int iRandR::getIndexOfDisplay( iDisplays const &_disp ) {
   reload();
   
   RRCrtc lCRTC_XRR    = None;
   bool   lOutputFound = false;
   
   for( internal::_output fOut : vOutput_V_RandR ) {
      if( _disp.getOutput() == fOut.id ) {
         lOutputFound = true;
         lCRTC_XRR    = fOut.crtc;
         break;
      }
   }
   
   // Invalid iRandRDisplay. (Should never happen)
   if( ! lOutputFound )
      return -2;
   
   // The output is not connected or disabled
   if( lCRTC_XRR == None )
      return -1;
   
   
   for( unsigned int i = 0; i < vCRTC_V_RandR.size(); ++i ) {
      if( vCRTC_V_RandR[i].id == lCRTC_XRR )
         return i; // The index
   }
   
   // Well... this should be impossible, because we have a reload() at top of this function!
   return -10;
}



} // unix_x11

} // e_engine
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;








