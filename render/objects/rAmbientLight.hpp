/*!
 * \file rAmbientLight.hpp
 */

#ifndef R_AMBIENT_LIGHT_HPP
#define R_AMBIENT_LIGHT_HPP

#include "defines.hpp"

#include "rObjectBase.hpp"
#include "rMatrixMath.hpp"

namespace e_engine {

template<class T>
class rAmbientLight : public rObjectBase {
   private:
      rVec3<T> vAmbientCollor;

      void setFlags();

      virtual int clearOGLData__() {return -1;};
      virtual int setOGLData__()   {return -1;};

   public:
      rAmbientLight( std::string _name ) : rObjectBase( _name, "", SET_DATA_MANUALLY ) {
         vAmbientCollor.fill( 0 );
         setFlags();
      }
      rAmbientLight( std::string _name, rVec3<T> const &_collor ) : rObjectBase( _name, "", SET_DATA_MANUALLY ) {
         vAmbientCollor = _collor;
         setFlags();
      }
      rAmbientLight( std::string _name, rVec3<T> const && _collor ) : rObjectBase( _name, "", SET_DATA_MANUALLY ) {
         vAmbientCollor = _collor;
         setFlags();
      }

      void      setCollor( rVec3<T> const &_collor )   {vAmbientCollor = _collor;}
      void      setCollor( rVec3<T> const && _collor ) {vAmbientCollor = _collor;}
      rVec4<T> *getCollor()                            {return &vAmbientCollor;}

      virtual uint32_t getVector( rVec3<T> **_vec, VECTOR_TYPES _type );
};

template<class T>
uint32_t rAmbientLight<T>::getVector( rVec3<T> **_vec, VECTOR_TYPES _type ) {
   *_vec = nullptr;

   switch( _type ) {
      case LIGHT_COLOR:     *_vec = &vAmbientCollor; return ALL_OK;
      default: return UNSUPPORTED_TYPE;
   }

   return ALL_OK;
}

template<class T>
void rAmbientLight<T>::setFlags() {
   vObjectHints[FLAGS]         = AMBIENT_LIGHT;
   vObjectHints[IS_DATA_READY] = GL_TRUE;
   vObjectHints[MATRICES]      = 0;
}

}

#endif // R_AMBIENT_LIGHT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
