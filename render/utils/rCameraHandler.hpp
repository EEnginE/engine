/*!
 * \file rCameraHandler.hpp
 */

#ifndef R_CAMERA_HANDLER_HPP
#define R_CAMERA_HANDLER_HPP

#include "rMatrixWorldBase.hpp"
#include "iInit.hpp"
#include "uSignalSlot.hpp"
#include "uLog.hpp"

namespace e_engine {

template<class T>
class rCameraHandler {
      typedef uSlot<void, rCameraHandler<T>, iEventInfo>  _SLOT_;
   public:
      enum KEY_MOVEMENT {
         UP = 0,
         DOWN,
         LEFT,
         RIGHT,
         FORWARD,
         BACKWARD,
         __LAST__
      };
   private:
      rVec3<T> vPosition;
      rVec3<T> vDirection;
      rVec3<T> vUp;

      rMatrixWorldBase<T> *vWorld;
      iInit               *vInit;

      bool     vCameraMovementEnabled;

      _SLOT_   vMouseSlot;
      _SLOT_   vKeySlot;

      wchar_t  keys[KEY_MOVEMENT::__LAST__];

      void updateDirectionAndUp();

      void mouse( iEventInfo _event );
      void key( iEventInfo _event );

      rCameraHandler() {}
   public:
      rCameraHandler( rMatrixWorldBase<T> *_world, iInit *_init ) :
         vWorld( _world ),
         vInit( _init ),

         vCameraMovementEnabled( true ),

         vPosition( ( T )0.0, ( T )0.0, ( T )0.0 ),

         vMouseSlot( &rCameraHandler::mouse, this ),
         vKeySlot( &rCameraHandler::key, this ) {

         vInit->addMousuSlot( &vMouseSlot );
         vInit->addKeySlot( &vKeySlot );

         keys[UP]       = L'q';
         keys[DOWN]     = L'e';
         keys[LEFT]     = L'a';
         keys[RIGHT]    = L'd';
         keys[FORWARD]  = L'w';
         keys[BACKWARD] = L's';

         updateDirectionAndUp();

         iLOG( "Camera movement enabled" );
      }

      void setCameraKey( KEY_MOVEMENT _key, wchar_t _what );

      void enableCamera()  {vCameraMovementEnabled = true;  iLOG( "Camera movement enabled" );}
      void disableCamera() {vCameraMovementEnabled = false; iLOG( "Camera movement disabled" );}

      void updateCamera();

      bool getIsCameraEnabled()   const {return vCameraMovementEnabled;}
};

template<class T>
void rCameraHandler<T>::setCameraKey( KEY_MOVEMENT _key, wchar_t _what ) {
   if ( _key >= __LAST__ || _key < 0 )
      return;

   keys[_key] = _what;
}

template<class T>
void rCameraHandler<T>::updateCamera() {
   if ( vCameraMovementEnabled )
      vWorld->setCamera( vPosition, vPosition + vDirection, vUp );
}


template<class T>
void rCameraHandler<T>::key( iEventInfo _event ) {
   if ( !vCameraMovementEnabled || _event.eKey.state == E_RELEASED )
      return;

   T lSpeed = GlobConf.camera.movementSpeed;
   KEY_MOVEMENT _action = __LAST__;

   rVec3<T> lTempVector;

   for ( uint8_t i = 0; i < __LAST__; ++i ) {
      if ( _event.eKey.key == keys[i] ) {
         _action = ( KEY_MOVEMENT )i;
         break;
      }
   }

   switch ( _action ) {
      case DOWN: lSpeed *= -1;
      case UP:
         lTempVector = lSpeed * vUp;
         vPosition += lTempVector;
         break;

      case LEFT: lSpeed *= -1;
      case RIGHT:
         lTempVector = rVectorMath::crossProduct( vDirection, vUp );
         lTempVector.normalize();
         lTempVector *= lSpeed;
         vPosition += lTempVector;
         break;

      case BACKWARD: lSpeed *= -1;
      case FORWARD:
         lTempVector = lSpeed * vDirection;
         vPosition += lTempVector;
         break;

      default: return;
   }

   updateCamera();
}


template<class T>
void rCameraHandler<T>::updateDirectionAndUp() {
   vDirection.x = cos( GlobConf.camera.angleVertical ) * sin( GlobConf.camera.angleHorizontal );
   vDirection.y = sin( GlobConf.camera.angleVertical );
   vDirection.z = cos( GlobConf.camera.angleVertical ) * cos( GlobConf.camera.angleHorizontal );

   rVec3<T> lTempRight;
   lTempRight.y = 0;

#ifdef M_PIl
   lTempRight.x = sin( GlobConf.camera.angleHorizontal - ( T )( M_PIl / 2 ) );
   lTempRight.z = cos( GlobConf.camera.angleHorizontal - ( T )( M_PIl / 2 ) );
#else
   lTempRight.x = sin( GlobConf.camera.angleHorizontal - ( T )( M_PI / 2 ) );
   lTempRight.z = cos( GlobConf.camera.angleHorizontal - ( T )( M_PI / 2 ) );
#endif

   vUp = rVectorMath::crossProduct( lTempRight, vDirection );
}


template<class T>
void rCameraHandler<T>::mouse( iEventInfo _event ) {
   int lDifX = signed( GlobConf.win.width  / 2 ) - _event.iMouse.posX;
   int lDifY = signed( GlobConf.win.height / 2 ) - _event.iMouse.posY;
   if ( ( lDifX == 0 && lDifY == 0 ) || !vCameraMovementEnabled )
      return;

   GlobConf.camera.angleHorizontal += GlobConf.camera.mouseSensitivity * lDifX;
   GlobConf.camera.angleVertical   += GlobConf.camera.mouseSensitivity * lDifY;
   vInit->moveMouse( GlobConf.win.width / 2, GlobConf.win.height / 2 );

   updateDirectionAndUp();

   updateCamera();
}






}

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
