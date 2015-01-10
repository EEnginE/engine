/*!
 * \file x11/RandR/iRandR_reload.cpp
 * \brief \b Classes: \a iRandR
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

#include "iRandR.hpp"
#include "uLog.hpp"

namespace e_engine {

namespace unix_x11 {

/*!
 * \brief Applies the new RandR config set
 *
 * \returns true when everything went fine
 * \returns false when there was an error
 */
bool iRandR::applyNewRandRSettings() {
   if ( !isRandRSupported() )
      return false;

   std::vector<internal::_crtc> lTempAllCRTC_V_RandR;
   int lMinWidth_I, lMinHeight_I;
   int lMaxWidth_I, lMaxHeight_I;

   double lDPI_D;
   int lNewWidth_I, lNewHeight_I;
   int lCurrentWidth_I, lCurrentHeight_I;

   reload();

   for ( internal::_crtc const &fCRTC : vChangeCRTC_V_RandR ) {
      bool lAppend_B = true;
      // Check for duplicates
      for ( internal::_crtc const &fCRTCTemp : lTempAllCRTC_V_RandR ) {
         if ( fCRTC.id == fCRTCTemp.id ) {
            lAppend_B = false;
            wLOG( "RandR: Duplicate of a CRTC id in vChangeCRTC_V_RandR --> Only change first" );
            break;
         }
      }
      if ( lAppend_B )
         lTempAllCRTC_V_RandR.push_back( fCRTC );
   }

   // We now work with lTempAllCRTC_V_RandR only
   vChangeCRTC_V_RandR.clear();

   // Add mising CRTCs
   for ( internal::_crtc const &fCRTC1 : vCRTC_V_RandR ) {
      bool lFound_B = false;
      for ( internal::_crtc const &fCRTC2 : lTempAllCRTC_V_RandR ) {
         if ( fCRTC1.id == fCRTC2.id ) {
            lFound_B = true;
            break;
         }
      }
      if ( !lFound_B )
         lTempAllCRTC_V_RandR.push_back( fCRTC1 );
   }

   if ( lTempAllCRTC_V_RandR.empty() )
      return false;

   XRRGetScreenSizeRange(
         vDisplay_X11, vRootWindow_X11, &lMinWidth_I, &lMinHeight_I, &lMaxWidth_I, &lMaxHeight_I );

   lNewWidth_I = 0;
   lNewHeight_I = 0;

   for ( internal::_crtc const &fCRTC : lTempAllCRTC_V_RandR ) {
      internal::_mode lTempMode_RandR;
      bool lModeFound_B = false;

      if ( fCRTC.mode == None ) {
         lModeFound_B = true;
         lTempMode_RandR.width = 0;
         lTempMode_RandR.height = 0;
      } else {
         for ( internal::_mode const &fMode : vMode_V_RandR ) {
            if ( fCRTC.mode == fMode.id ) {
               lTempMode_RandR = fMode;
               lModeFound_B = true;
               break;
            }
         }
      }

      if ( !lModeFound_B ) {
         wLOG( "RandR: Unable to find mode ( fCRTC.mode = ",
               fCRTC.mode,
               ") in vMode_V_RandR --> Do not change Screen size" );
         return false;
      }


      int lTempWidth_I = fCRTC.posX + static_cast<int>( lTempMode_RandR.width );
      int lTempHeight_I = fCRTC.posY + static_cast<int>( lTempMode_RandR.height );
      lNewWidth_I = ( lTempWidth_I > lNewWidth_I ) ? lTempWidth_I : lNewWidth_I;
      lNewHeight_I = ( lTempHeight_I > lNewHeight_I ) ? lTempHeight_I : lNewHeight_I;
   }

   // Get current size
   int lCountSizes_I;
   SizeID lCurrentSizePossition_suI;
   Rotation lUselessRotationInfo_XRR;
   XRRScreenSize *sizes = XRRSizes( vDisplay_X11, 0, &lCountSizes_I );
   lCurrentSizePossition_suI =
         XRRConfigCurrentConfiguration( vConfig_XRR, &lUselessRotationInfo_XRR );

   if ( !( lCurrentSizePossition_suI < lCountSizes_I ) ) {
      wLOG( "XRandR ERROR: ! lCurrentSizePossition_suI < lCountSizes_I ( ",
            lCurrentSizePossition_suI,
            " < ",
            lCountSizes_I,
            " )" );
      return false;
   }

   lCurrentWidth_I = sizes[lCurrentSizePossition_suI].width;
   lCurrentHeight_I = sizes[lCurrentSizePossition_suI].height;

   /* Only for debugging
   iLOG( "RandR Screen Size Info:"
   POINT "MinWidth      - ", lMinWidth_I
   POINT "MinHeight     - ", lMinHeight_I
   POINT "MaxWidth      - ", lMaxWidth_I
   POINT "MaxHeight     - ", lMaxHeight_I
   POINT "NewWidth      - ", lNewWidth_I
   POINT "NewHeight     - ", lNewHeight_I
   POINT "CurrentWidth  - ", lCurrentWidth_I
   POINT "CurrentHeight - ", lCurrentHeight_I
   ); */

   if ( ( lNewWidth_I >= lMinWidth_I && lNewWidth_I <= lMaxWidth_I &&
          lNewHeight_I >= lMinHeight_I && lNewHeight_I <= lMaxHeight_I ) &&
        ( lNewWidth_I != lCurrentWidth_I || lNewHeight_I != lCurrentHeight_I ) ) {
      lDPI_D = ( 25.4 * DefaultScreenOfDisplay( vDisplay_X11 )->height ) /
               DefaultScreenOfDisplay( vDisplay_X11 )->mheight;

      XRRSetScreenSize(
            vDisplay_X11,
            vRootWindow_X11,
            ( lNewWidth_I > lCurrentWidth_I ) ? lNewWidth_I : lCurrentWidth_I,
            ( lNewHeight_I > lCurrentHeight_I ) ? lNewHeight_I : lCurrentHeight_I,
            static_cast<int>(
                  ( 25.4 * ( ( lNewWidth_I > lCurrentWidth_I ) ? lNewWidth_I : lCurrentWidth_I ) ) /
                  lDPI_D ),
            static_cast<int>(
                  ( 25.4 *
                    ( ( lNewHeight_I > lCurrentHeight_I ) ? lNewHeight_I : lCurrentHeight_I ) ) /
                  lDPI_D ) );
   }

   for ( internal::_crtc const &fCRTC : lTempAllCRTC_V_RandR )
      changeCRTC( fCRTC );

   if ( ( lNewWidth_I >= lMinWidth_I && lNewWidth_I <= lMaxWidth_I &&
          lNewHeight_I >= lMinHeight_I && lNewHeight_I <= lMaxHeight_I ) &&
        ( lNewWidth_I != lCurrentWidth_I || lNewHeight_I != lCurrentHeight_I ) ) {
      lDPI_D = ( 25.4 * DefaultScreenOfDisplay( vDisplay_X11 )->height ) /
               DefaultScreenOfDisplay( vDisplay_X11 )->mheight;

      XRRSetScreenSize( vDisplay_X11,
                        vRootWindow_X11,
                        lNewWidth_I,
                        lNewHeight_I,
                        static_cast<int>( ( 25.4 * lNewWidth_I ) / lDPI_D ),
                        static_cast<int>( ( 25.4 * lNewHeight_I ) / lDPI_D ) );
   }

   // We now work with lTempAllCRTC_V_RandR only
   vChangeCRTC_V_RandR.clear();

   printRandRStatus();

   return true;
}


} // unix_x11

} // e_engine


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
