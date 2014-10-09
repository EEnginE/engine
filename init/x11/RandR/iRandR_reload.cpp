/*!
 * \file x11/RandR/iRandR_reload.cpp
 * \brief \b Classes: \a iRandR
 */

#include "iRandR.hpp"

namespace e_engine {

namespace unix_x11 {


bool iRandR::reload( bool _overwriteLatest, bool _overwriteDefaults ) {
   if ( ! vIsRandRSupported_B )
      return false;

   SizeID lTemoID_suI;
   int    lTempSizes_I;

   Rotation lTempRotate_XRR; // Exists only for XRRConfigCurrentConfiguration(...); to make it happy

   vConfig_XRR    = XRRGetScreenInfo( vDisplay_X11, vRootWindow_X11 );
   vResources_XRR = XRRGetScreenResources( vDisplay_X11, vRootWindow_X11 );

   lTemoID_suI                     = XRRConfigCurrentConfiguration( vConfig_XRR, &lTempRotate_XRR );
   XRRScreenSize *lTempSizes_XRR   = XRRSizes( vDisplay_X11, 0, &lTempSizes_I );

   if ( lTemoID_suI < lTempSizes_I ) {
      vScreenWidth_uI  = lTempSizes_XRR[lTemoID_suI].width;
      vScreenHeight_uI = lTempSizes_XRR[lTemoID_suI].height;
   }

   if ( _overwriteLatest ) {
      vLatestConfig_RandR.primary     = XRRGetOutputPrimary( vDisplay_X11, vWindow_X11 );

      for ( unsigned int i = 0; i < vLatestConfig_RandR.gamma.size(); ++i )
         XRRFreeGamma( vLatestConfig_RandR.gamma[i] );

      vLatestConfig_RandR.gamma.clear();

      for ( int i = 0; i < vResources_XRR->ncrtc; ++i ) {
         vLatestConfig_RandR.gamma.push_back( XRRGetCrtcGamma( vDisplay_X11, vResources_XRR->crtcs[i] ) );
      }
   }

   if ( _overwriteDefaults ) {
      vDefaultConfig_RandR.primary        = XRRGetOutputPrimary( vDisplay_X11, vWindow_X11 );

      for ( unsigned int i = 0; i < vDefaultConfig_RandR.gamma.size(); ++i )
         XRRFreeGamma( vDefaultConfig_RandR.gamma[i] );

      vDefaultConfig_RandR.gamma.clear();

      for ( int i = 0; i < vResources_XRR->ncrtc; ++i ) {
         vDefaultConfig_RandR.gamma.push_back( XRRGetCrtcGamma( vDisplay_X11, vResources_XRR->crtcs[i] ) );
      }

   }

   // Clear old data
   vCRTC_V_RandR.clear();
   vOutput_V_RandR.clear();
   vMode_V_RandR.clear();
   vLatestConfig_RandR.CRTCInfo.clear();


   // CRTC
   for ( int i = 0; i < vResources_XRR->ncrtc ; ++i ) {
      internal::_crtc lTempCRTC_RandR;

      XRRCrtcInfo *lTempCRTCInfo_XRR     = XRRGetCrtcInfo( vDisplay_X11, vResources_XRR, vResources_XRR->crtcs[i] );

      lTempCRTC_RandR.id                 = vResources_XRR->crtcs[i];
      lTempCRTC_RandR.timestamp          = lTempCRTCInfo_XRR->timestamp;
      lTempCRTC_RandR.posX               = lTempCRTCInfo_XRR->x;
      lTempCRTC_RandR.posY               = lTempCRTCInfo_XRR->y;
      lTempCRTC_RandR.width              = lTempCRTCInfo_XRR->width;
      lTempCRTC_RandR.height             = lTempCRTCInfo_XRR->height;
      lTempCRTC_RandR.mode               = lTempCRTCInfo_XRR->mode;
      lTempCRTC_RandR.rotation           = lTempCRTCInfo_XRR->rotation;
      lTempCRTC_RandR.rotations          = lTempCRTCInfo_XRR->rotations;

      for ( int j = 0; j < lTempCRTCInfo_XRR->noutput; ++j ) {
         lTempCRTC_RandR.outputs.push_back( lTempCRTCInfo_XRR->outputs[j] );
      }

      for ( int j = 0; j < lTempCRTCInfo_XRR->npossible; ++j ) {
         lTempCRTC_RandR.possibleOutputs.push_back( lTempCRTCInfo_XRR->possible[j] );
      }

      vCRTC_V_RandR.push_back( lTempCRTC_RandR );
      XRRFreeCrtcInfo( lTempCRTCInfo_XRR );
   }


   // Output
   for ( int i = 0; i < vResources_XRR->noutput ; ++i ) {
      internal::_output lTempOutput_RandR;

      XRROutputInfo *lTempOutputInfo_XRR = XRRGetOutputInfo( vDisplay_X11, vResources_XRR, vResources_XRR->outputs[i] );

      lTempOutput_RandR.id               = vResources_XRR->outputs[i];
      lTempOutput_RandR.timestamp        = lTempOutputInfo_XRR->timestamp;
      lTempOutput_RandR.crtc             = lTempOutputInfo_XRR->crtc;
      lTempOutput_RandR.name             = lTempOutputInfo_XRR->name;
      lTempOutput_RandR.mm_width         = lTempOutputInfo_XRR->mm_width;
      lTempOutput_RandR.mm_height        = lTempOutputInfo_XRR->mm_height;
      lTempOutput_RandR.connection       = lTempOutputInfo_XRR->connection;
      lTempOutput_RandR.subpixel_order   = lTempOutputInfo_XRR->subpixel_order;
      lTempOutput_RandR.npreferred       = lTempOutputInfo_XRR->npreferred;

      for ( int j = 0; j < lTempOutputInfo_XRR->ncrtc; ++j ) {
         lTempOutput_RandR.crtcs.push_back( lTempOutputInfo_XRR->crtcs[j] );
      }

      for ( int j = 0; j < lTempOutputInfo_XRR->nclone; ++j ) {
         lTempOutput_RandR.clones.push_back( lTempOutputInfo_XRR->clones[j] );
      }

      for ( int j = 0; j < lTempOutputInfo_XRR->nmode; ++j ) {
         lTempOutput_RandR.modes.push_back( lTempOutputInfo_XRR->modes[j] );
      }

      vOutput_V_RandR.push_back( lTempOutput_RandR );
      XRRFreeOutputInfo( lTempOutputInfo_XRR );
   }


   // Modes
   for ( int i = 0; i < vResources_XRR->nmode ; ++i ) {
      internal::_mode lTempMode_RandR;

      XRRModeInfo lTempModeInfo_XRR     = vResources_XRR->modes[i];

      lTempMode_RandR.id                = lTempModeInfo_XRR.id;
      lTempMode_RandR.width             = lTempModeInfo_XRR.width;
      lTempMode_RandR.height            = lTempModeInfo_XRR.height;
      lTempMode_RandR.dotClock          = lTempModeInfo_XRR.dotClock;
      lTempMode_RandR.hSyncStart        = lTempModeInfo_XRR.hSyncStart;
      lTempMode_RandR.hSyncEnd          = lTempModeInfo_XRR.hSyncEnd;
      lTempMode_RandR.hTotal            = lTempModeInfo_XRR.hTotal;
      lTempMode_RandR.hSkew             = lTempModeInfo_XRR.hSkew;
      lTempMode_RandR.vSyncStart        = lTempModeInfo_XRR.vSyncStart;
      lTempMode_RandR.vSyncEnd          = lTempModeInfo_XRR.vSyncEnd;
      lTempMode_RandR.vTotal            = lTempModeInfo_XRR.vTotal;
      lTempMode_RandR.name              = lTempModeInfo_XRR.name;
      lTempMode_RandR.modeFlags         = lTempModeInfo_XRR.modeFlags;


      /* v refresh frequency in Hz */
      unsigned int lVTotalTemp = lTempMode_RandR.vTotal;

      if ( lTempMode_RandR.modeFlags & RR_DoubleScan )
         lVTotalTemp *= 2;

      if ( lTempMode_RandR.modeFlags & RR_Interlace )
         lVTotalTemp /= 2;

      if ( lTempMode_RandR.hTotal && lVTotalTemp )
         lTempMode_RandR.refresh = ( ( double )lTempMode_RandR.dotClock / ( ( double )lTempMode_RandR.hTotal * ( double )lVTotalTemp ) );
      else
         lTempMode_RandR.refresh = 0;


      /* h sync frequency in Hz */
      if ( lTempMode_RandR.hTotal )
         lTempMode_RandR.syncFreq = lTempMode_RandR.dotClock / lTempMode_RandR.hTotal;
      else
         lTempMode_RandR.syncFreq = 0;

      vMode_V_RandR.push_back( lTempMode_RandR );
   }


   vLatestConfig_RandR.CRTCInfo = vCRTC_V_RandR;

   if ( _overwriteLatest )
      vDefaultConfig_RandR.CRTCInfo = vCRTC_V_RandR;

   vMode_V_RandR.sort();

   return true;
}

} // unix_x11

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
