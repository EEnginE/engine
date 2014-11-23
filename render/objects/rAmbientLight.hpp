/*!
 * \file rAmbientLight.hpp
 */

#ifndef R_AMBIENT_LIGHT_HPP
#define R_AMBIENT_LIGHT_HPP

#include "rObjectBase.hpp"
#include "rMatrixMath.hpp"

namespace e_engine {

template<class T>
class rAmbientLightTemplate : public rObjectBase {
   private:
      rVec4<T> vAmbientCollor;

      void setFlags();

   protected:
      uint32_t getVectorTemplate( rVec4<T> **_vec, VECTOR_TYPES _type );

   public:
      rAmbientLightTemplate() {
         vAmbientCollor.fill( 0 );
         setFlags();
      }
      rAmbientLightTemplate( rVec4<T> const &_collor ) {
         vAmbientCollor = _collor;
         setFlags();
      }

      void setCollor( rVec4<T> const &_collor ) {vAmbientCollor = _collor;}
};

template<class T>
uint32_t rAmbientLightTemplate<T>::getVectorTemplate( rVec4<T> **_vec, VECTOR_TYPES _type ) {
   *_vec = nullptr;
   if ( _type != AMBIENT_COLOR )
      return UNSUPPORTED_TYPE;

   *_vec = &vAmbientCollor;
   return ALL_OK;
}

template<class T>
void rAmbientLightTemplate<T>::setFlags() {
   vObjectHints[FLAGS]    = AMBIENT_LIGHT;
   vObjectHints[MATRICES] = 0;
}






class rAmbientLightF : public rAmbientLightTemplate<float> {
   public:
      using rAmbientLightTemplate::rAmbientLightTemplate;

      virtual uint32_t getVector( rVec4f **_vec, VECTOR_TYPES _type )
      {return getVectorTemplate( _vec, _type );}
};


class rAmbientLightD : public rAmbientLightTemplate<float> {
   public:
      using rAmbientLightTemplate::rAmbientLightTemplate;

      virtual uint32_t getVector( rVec4f **_vec, VECTOR_TYPES _type )
      {return getVectorTemplate( _vec, _type );}
};

}

#endif // R_AMBIENT_LIGHT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
