/*!
 * \file rSimpleLightSource.hpp
 */

#ifndef R_SIMPLE_LIGHT_SOURCE_HPP
#define R_SIMPLE_LIGHT_SOURCE_HPP

#include "rObjectBase.hpp"
#include "rMatrixObjectBase.hpp"

namespace e_engine {

template<class T>
class rSimpleLightSource : public rObjectBase, public rMatrixObjectBase<T> {
   private:
      rVec3<T> vLightColor;

      void setFlags();


      virtual int clearOGLData__() {return -1;};
      virtual int setOGLData__()   {return -1;};

   public:
      using rMatrixObjectBase<T>::getPosition;

      rSimpleLightSource( rMatrixSceneBase<float> *_scene, std::string _name ) :
         rObjectBase( _name, "", SET_DATA_MANUALLY ),
         rMatrixObjectBase<T>( _scene ) {
         vLightColor.fill( 0 );
         setFlags();
      }

      rSimpleLightSource( rMatrixSceneBase<float> *_scene, std::string _name, rVec3<T> const &_collor ) :
         rObjectBase( _name, "", SET_DATA_MANUALLY ),
         rMatrixObjectBase<T>( _scene ) {
         vLightColor = _collor;
         setFlags();
      }
      rSimpleLightSource( rMatrixSceneBase<float> *_scene, std::string _name, rVec3<T> const && _collor ) :
         rObjectBase( _name, "", SET_DATA_MANUALLY ),
         rMatrixObjectBase<T>( _scene ) {
         vLightColor = _collor;
         setFlags();
      }

      void      setCollor( rVec3<T> const &_collor )  {vLightColor = _collor;}
      void      setCollor( rVec3<T> const && _collor ) {vLightColor = _collor;}
      rVec3<T> *getCollor()                           {return &vLightColor;}

      virtual uint32_t getVector( rVec3<T> **_vec, VECTOR_TYPES _type );
};


template<class T>
uint32_t rSimpleLightSource<T>::getVector( rVec3<T> **_vec, VECTOR_TYPES _type ) {
   *_vec = nullptr;

   switch( _type ) {
      case LIGHT_COLOR: *_vec = &vLightColor;  return ALL_OK;
      case POSITION:    *_vec = getPosition(); return ALL_OK;
      default: return UNSUPPORTED_TYPE;
   }
}

template<class T>
void rSimpleLightSource<T>::setFlags() {
   vObjectHints[FLAGS]         = LIGHT_SOURCE;
   vObjectHints[IS_DATA_READY] = GL_TRUE;
   vObjectHints[MATRICES]      = 0;
}

}

#endif // R_SIMPLE_LIGHT_SOURCE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
