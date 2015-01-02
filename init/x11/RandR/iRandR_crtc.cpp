/*!
 * \file x11/RandR/iRandR_crtc.cpp
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
 * \brief Change one CRTC
 *
 * \returns -5 when the CRTC could not be found
 * \returns 1 when everything went fine
 */
int iRandR::changeCRTC( internal::_crtc _changeToThis ) {
   RRCrtc lCRTC_XRR = _changeToThis.id;
   bool lChangedCRTC_B = false;

   bool lCRTCInfoFound = false;

   XRRCrtcInfo *lTempCRTCInfo_XRR;
   RROutput *lTempOutputs_XRR;

   for ( unsigned int i = 0; i < (unsigned)vResources_XRR->ncrtc; ++i ) {
      if ( vResources_XRR->crtcs[i] == lCRTC_XRR ) {
         lCRTCInfoFound = true;
         break;
      }
   }

   if ( !lCRTCInfoFound ) {
      wLOG( "Cannot find CRTC ", lCRTC_XRR, " in the current CRTC --> return -5" );
      return -5;
   }

   lTempCRTCInfo_XRR = XRRGetCrtcInfo( vDisplay_X11, vResources_XRR, lCRTC_XRR );

   if ( lTempCRTCInfo_XRR->mode != _changeToThis.mode ||
        lTempCRTCInfo_XRR->noutput != (int)_changeToThis.outputs.size() ||
        lTempCRTCInfo_XRR->x != _changeToThis.posX || lTempCRTCInfo_XRR->y != _changeToThis.posY ||
        lTempCRTCInfo_XRR->rotation != _changeToThis.rotation ) {
      lChangedCRTC_B = true;
   } else {
      for ( unsigned int i = 0; i < _changeToThis.outputs.size(); ++i ) {
         if ( lTempCRTCInfo_XRR->outputs[i] != _changeToThis.outputs[i] ) {
            lChangedCRTC_B = true;
            break;
         }
      }
   }

   if ( lChangedCRTC_B ) {
      int lReturn_I;

      if ( _changeToThis.outputs.size() == 0 || _changeToThis.mode == None ) {
         // Disable output
         lReturn_I = XRRSetCrtcConfig( vDisplay_X11,
                                       vResources_XRR,
                                       lCRTC_XRR,
                                       CurrentTime,
                                       0,
                                       0,
                                       None,
                                       RR_Rotate_0,
                                       nullptr,
                                       0 );
         iLOG( "RandR: Disabled CRTC ", _changeToThis.id );
      } else {
         lTempOutputs_XRR = new RROutput[_changeToThis.outputs.size()];
         for ( unsigned int i = 0; i < _changeToThis.outputs.size(); ++i ) {
            lTempOutputs_XRR[i] = _changeToThis.outputs[i];
         }
         lReturn_I = XRRSetCrtcConfig( vDisplay_X11,
                                       vResources_XRR,
                                       lCRTC_XRR,
                                       CurrentTime,
                                       _changeToThis.posX,
                                       _changeToThis.posY,
                                       _changeToThis.mode,
                                       _changeToThis.rotation,
                                       lTempOutputs_XRR,
                                       _changeToThis.outputs.size() );
         delete[] lTempOutputs_XRR;
         iLOG( "RandR: Changed CRTC ", _changeToThis.id );
      }

      if ( lReturn_I != RRSetConfigSuccess ) {
         wLOG( "RabdR: Failed to set CRTC config ( XRRSetCrtcConfig(...) returns '",
               lReturn_I,
               "' )" );
         return lReturn_I;
      }

   } else { iLOG( "RandR: Changed CRTC ", _changeToThis.id, " -- nothing to do" ); }

   XRRFreeCrtcInfo( lTempCRTCInfo_XRR );

   return 1;
}

} // unix_x11

} // e_engine
