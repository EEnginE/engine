/*!
 * \file rCameraHandler.hpp
 */

#ifndef R_CAMERA_HANDLER_HPP
#define R_CAMERA_HANDLER_HPP

#include "rMatrixWorldBase.hpp"
#include "iInit.hpp"
#include "uSignalSlot.hpp"

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

      T        vSpeed;
      T        vMouseSenseitvity;

      _SLOT_   vMouseSlot;
      _SLOT_   vKeySlot;

      wchar_t  keys[KEY_MOVEMENT::__LAST__];

      void mouse( iEventInfo _event );
      void key( iEventInfo _event );

      rCameraHandler() {}
   public:
      rCameraHandler( rMatrixWorldBase<T> *_world, iInit *_init, T _mouseSenseitvity = ( T )0.5, T _speed = ( T )0.25 ) :
         vWorld( _world ),
         vInit( _init ),

         vCameraMovementEnabled( true ),

         vSpeed( _speed ),
         vMouseSenseitvity( _mouseSenseitvity ),
         
         vPosition( (T)0.0, (T)0.0, (T)0.0 ),
         vDirection( (T)0.0, (T)0.0, (T)-1.0 ),
         vUp( (T)0.0, (T)1.0, (T)0.0 ),

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
      }

      void setKey( KEY_MOVEMENT _key, wchar_t _what );
      void setSpeedAndMouse( T _mouseSenseitvity = ( T )0.5, T _speed = ( T )0.25 );

};

template<class T>
void rCameraHandler<T>::setKey( KEY_MOVEMENT _key, wchar_t _what ) {
   if ( _key >= __LAST__ || _key < 0 )
      return;

   keys[_key] = _what;
}

template<class T>
void rCameraHandler<T>::setSpeedAndMouse( T _mouseSenseitvity, T _speed ) {
   vMouseSenseitvity = _mouseSenseitvity;
   vSpeed = _speed;
}

template<class T>
void rCameraHandler<T>::key( iEventInfo _event ) {
   if ( !vCameraMovementEnabled || _event.eKey.state == E_RELEASED )
      return;
   
   T lSpeed = vSpeed;
   KEY_MOVEMENT _action = __LAST__;
   
   rVec3<T> lTempVector;
   
   for( uint8_t i = 0; i < __LAST__; ++i ) {
      if( _event.eKey.key == keys[i] ) {
         _action = (KEY_MOVEMENT)i;
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
   
   vWorld->setCamera( vPosition, vPosition + vDirection, vUp );
}

template<class T>
void rCameraHandler<T>::mouse( iEventInfo _event ) {

}






}

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
