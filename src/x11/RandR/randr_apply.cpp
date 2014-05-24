/*!
 * \file x11/RandR/randr_reload.cpp
 * \brief \b Classes: \a eRandR
 */

#include "randr.hpp"
#include "log.hpp"

namespace e_engine {


/*!
 * \brief Applies the new RandR config set
 *
 * \returns true when everything went fine
 * \returns false when there was an error
 */
bool eRandR::applyNewRandRSettings() {
   if ( ! isRandRSupported() )
      return false;

   std::vector<e_engine_internal::_crtc> lTempAllCRTC_V_RandR;
   int    lMinWidth_I,     lMinHeight_I;
   int    lMaxWidth_I,     lMaxHeight_I;

   double lDPI_D;
   int    lNewWidth_I,     lNewHeight_I;
   int    lCurrentWidth_I, lCurrentHeight_I;

   reload();

   for ( e_engine_internal::_crtc const & fCRTC : vChangeCRTC_V_RandR ) {
      bool lAppend_B = true;
      // Check for duplicates
      for ( e_engine_internal::_crtc const & fCRTCTemp : lTempAllCRTC_V_RandR ) {
         if ( fCRTC.id == fCRTCTemp.id ) {
            lAppend_B = false;
            wLOG "RandR: Duplicate of a CRTC id in vChangeCRTC_V_RandR --> Only change first" END
            break;
         }
      }
      if ( lAppend_B )
         lTempAllCRTC_V_RandR.push_back( fCRTC );
   }

   // We now work with lTempAllCRTC_V_RandR only
   vChangeCRTC_V_RandR.clear();

   // Add mising CRTCs
   for ( e_engine_internal::_crtc const & fCRTC1 : vCRTC_V_RandR ) {
      bool lFound_B = false;
      for ( e_engine_internal::_crtc const & fCRTC2 : lTempAllCRTC_V_RandR ) {
         if ( fCRTC1.id == fCRTC2.id ) {
            lFound_B = true;
            break;
         }
      }
      if ( ! lFound_B )
         lTempAllCRTC_V_RandR.push_back( fCRTC1 );
   }

   if ( lTempAllCRTC_V_RandR.empty() )
      return false;

   XRRGetScreenSizeRange(
      vDisplay_X11, vRootWindow_X11,
      &lMinWidth_I, &lMinHeight_I,
      &lMaxWidth_I, &lMaxHeight_I
   );

   lNewWidth_I  = 0;
   lNewHeight_I = 0;

   for ( e_engine_internal::_crtc const & fCRTC : lTempAllCRTC_V_RandR ) {
      e_engine_internal::_mode lTempMode_RandR;
      bool                     lModeFound_B = false;

      if ( fCRTC.mode == None ) {
         lModeFound_B = true;
         lTempMode_RandR.width  = 0;
         lTempMode_RandR.height = 0;
      } else {
         for ( e_engine_internal::_mode const & fMode : vMode_V_RandR ) {
            if ( fCRTC.mode == fMode.id ) {
               lTempMode_RandR = fMode;
               lModeFound_B    = true;
               break;
            }
         }
      }

      if ( ! lModeFound_B ) {
         wLOG "RandR: Unable to find mode ( fCRTC.mode = " ADD fCRTC.mode ADD ") in vMode_V_RandR --> Do not change Screen size" END
         return false;
      }


      int lTempWidth_I  = fCRTC.posX + lTempMode_RandR.width;
      int lTempHeight_I = fCRTC.posY + lTempMode_RandR.height;
      lNewWidth_I         = ( lTempWidth_I  > lNewWidth_I )  ? lTempWidth_I  : lNewWidth_I ;
      lNewHeight_I        = ( lTempHeight_I > lNewHeight_I ) ? lTempHeight_I : lNewHeight_I ;
   }

   // Get current size
   int      lCountSizes_I;
   SizeID   lCurrentSizePossition_suI;
   Rotation lUselessRotationInfo_XRR;
   XRRScreenSize *sizes = XRRSizes( vDisplay_X11, 0, &lCountSizes_I );
   lCurrentSizePossition_suI = XRRConfigCurrentConfiguration( vConfig_XRR, &lUselessRotationInfo_XRR );

   if ( !( lCurrentSizePossition_suI < lCountSizes_I ) ) {
      wLOG "XRandR ERROR: ! lCurrentSizePossition_suI < lCountSizes_I ( " ADD lCurrentSizePossition_suI ADD " < " ADD lCountSizes_I ADD " )" END
      return false;
   }

   lCurrentWidth_I  = sizes[lCurrentSizePossition_suI].width;
   lCurrentHeight_I = sizes[lCurrentSizePossition_suI].height;

   /* Only for debugging
   iLOG "RandR Screen Size Info:"
   POINT "MinWidth      - " ADD lMinWidth_I
   POINT "MinHeight     - " ADD lMinHeight_I
   POINT "MaxWidth      - " ADD lMaxWidth_I
   POINT "MaxHeight     - " ADD lMaxHeight_I
   POINT "NewWidth      - " ADD lNewWidth_I
   POINT "NewHeight     - " ADD lNewHeight_I
   POINT "CurrentWidth  - " ADD lCurrentWidth_I
   POINT "CurrentHeight - " ADD lCurrentHeight_I
   END */

   if (
      ( lNewWidth_I  >= lMinWidth_I     && lNewWidth_I  <= lMaxWidth_I        &&
        lNewHeight_I >= lMinHeight_I    && lNewHeight_I <= lMaxHeight_I )     &&
      ( lNewWidth_I  != lCurrentWidth_I || lNewHeight_I != lCurrentHeight_I )
   ) {
      lDPI_D = ( 25.4 * DefaultScreenOfDisplay( vDisplay_X11 )->height ) / DefaultScreenOfDisplay( vDisplay_X11 )->mheight;

      XRRSetScreenSize(
         vDisplay_X11, vRootWindow_X11,
         ( lNewWidth_I   > lCurrentWidth_I )  ? lNewWidth_I  : lCurrentWidth_I,
         ( lNewHeight_I  > lCurrentHeight_I ) ? lNewHeight_I : lCurrentHeight_I,
         ( 25.4 * ( ( lNewWidth_I   > lCurrentWidth_I )  ? lNewWidth_I  : lCurrentWidth_I ) )  / lDPI_D,
         ( 25.4 * ( ( lNewHeight_I  > lCurrentHeight_I ) ? lNewHeight_I : lCurrentHeight_I ) ) / lDPI_D
      );
   }

   for ( e_engine_internal::_crtc const & fCRTC : lTempAllCRTC_V_RandR )
      changeCRTC( fCRTC );

   if (
      ( lNewWidth_I  >= lMinWidth_I     && lNewWidth_I  <= lMaxWidth_I        &&
        lNewHeight_I >= lMinHeight_I    && lNewHeight_I <= lMaxHeight_I )     &&
      ( lNewWidth_I  != lCurrentWidth_I || lNewHeight_I != lCurrentHeight_I )
   ) {
      lDPI_D = ( 25.4 * DefaultScreenOfDisplay( vDisplay_X11 )->height ) / DefaultScreenOfDisplay( vDisplay_X11 )->mheight;

      XRRSetScreenSize(
         vDisplay_X11, vRootWindow_X11,
         lNewWidth_I,  lNewHeight_I,
         ( 25.4 * lNewWidth_I )  / lDPI_D,
         ( 25.4 * lNewHeight_I ) / lDPI_D
      );
   }
   
   // We now work with lTempAllCRTC_V_RandR only
   vChangeCRTC_V_RandR.clear();

   printRandRStatus();

   return true;
}


}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
