/*!
 * \file rMatrixObjectBase.hpp
 * \brief \b Classes: \a rMatrixObjectBase
 */

#ifndef R_MATRIX_OBJECT_BASE_HPP
#define R_MATRIX_OBJECT_BASE_HPP

#include "rMatrixMath.hpp"
#include "engine_render_Export.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>


namespace e_engine {

/*!
 * \brief Class for managing Camera space matrix
 *
 *
 */
template<class T>
class rMatrixObjectBase {
   private:
      rMat4<T>  vScaleMatrix_MAT;
      rMat4<T>  vRotationMatrix_MAT;
      rMat4<T>  vTranslationMatrix_MAT;

      rMat4<T> *vCameraSpaceMatrix_MAT;

      rMat4<T>  vFinalMatrix_MAT;

      bool      vNeedMatrixUpdate_B;

      glm::mat4 *vCameraSpace_GLM;
      
      glm::mat4 vScale_GLM;
      glm::mat4 vRotation_GLM;
      glm::mat4 vTranslation_GLM;
      glm::mat4 vFINAL_GLM;

   public:
      rMatrixObjectBase();

      inline void setPosition( const rVec3<T> &_pos );
      inline void getPosition( rVec3<T> &_pos );
      inline rVec3<T> getPosition() {rVec3<T> lPos; getPosition( lPos ); return lPos;}
      inline void addPositionDelta( const rVec3<T> &_pos );

      inline void setRotation( const rVec3<T> &_axis, T _angle );

      inline void setScale( T _scale );
      inline void setScale( const rVec3<T> &_scale );
      inline void getScale( rVec3<T> &_scale );
      inline void addScaleDelta( const rVec3<T> &_scale );


      inline rMat4<T> *getScaleMatrix()       { return &vScaleMatrix_MAT; }
      inline rMat4<T> *getRotationMatrix()    { return &vRotationMatrix_MAT; }
      inline rMat4<T> *getTranslationMatrix() { return &vTranslationMatrix_MAT; }
      inline rMat4<T> *getFinalMatrix()       { return &vFinalMatrix_MAT; }

      inline glm::mat4 *getFinalGLM() { return &vFINAL_GLM; }

      inline void setCmaraSpaceMatrix( rMat4<T> *_mat ) { vCameraSpaceMatrix_MAT = _mat; }
      inline void setCameraSpaceMAtrix_GLM( glm::mat4 *_mat ) { vCameraSpace_GLM = _mat; }

      inline void updateFinalMatrix( bool _forceUpdate );
};

template<class T>
rMatrixObjectBase<T>::rMatrixObjectBase() {
   vScaleMatrix_MAT.toIdentityMatrix();
   vRotationMatrix_MAT.toIdentityMatrix();
   vTranslationMatrix_MAT.toIdentityMatrix();

   vFinalMatrix_MAT.toIdentityMatrix();

   vCameraSpaceMatrix_MAT = nullptr;
   vCameraSpace_GLM = nullptr;

   vScale_GLM = glm::mat4( 1.0f );
   vRotation_GLM = glm::mat4( 1.0f );
   vTranslation_GLM = glm::mat4( 1.0f );
   vFINAL_GLM = glm::mat4( 1.0f );

   vNeedMatrixUpdate_B = true;
}

template<class T>
void rMatrixObjectBase<T>::setScale( T _scale ) {
   vScaleMatrix_MAT.setMat(
      _scale, 0     , 0     , 0,
      0     , _scale, 0     , 0,
      0     , 0     , _scale, 0,
      0     , 0     , 0     , 1
   );
   vNeedMatrixUpdate_B = true;
}

template<class T>
void rMatrixObjectBase<T>::setScale( const rVec3< T > &_scale ) {
   vScaleMatrix_MAT.setMat(
      _scale.x, 0       , 0       , 0,
      0       , _scale.y, 0       , 0,
      0       , 0       , _scale.z, 0,
      0       , 0       , 0       , 1
   );
   vNeedMatrixUpdate_B = true;
}

template<class T>
void rMatrixObjectBase<T>::getScale( rVec3< T > &_scale ) {
   _scale.x = vScaleMatrix_MAT.template get<0, 0>();
   _scale.y = vScaleMatrix_MAT.template get<1, 1>();
   _scale.z = vScaleMatrix_MAT.template get<2, 2>();
}

template<class T>
void rMatrixObjectBase<T>::addScaleDelta( const rVec3< T > &_scale ) {
   vScaleMatrix_MAT.setMat(
      vScaleMatrix_MAT( 0, 0 ) + _scale.x, 0                                   , 0                                  , 0,
      0                                  , vScaleMatrix_MAT( 1, 1 ) +  _scale.y, 0                                  , 0,
      0                                  , 0                                   , vScaleMatrix_MAT( 2, 2 ) + _scale.z, 0,
      0                                  , 0                                   , 0                                  , 1
   );
   vNeedMatrixUpdate_B = true;
}

template<class T>
void rMatrixObjectBase<T>::setRotation( const rVec3< T > &_axis, T _angle ) {
   rMatrixMath::rotate( _axis, _angle, vRotationMatrix_MAT );
   vNeedMatrixUpdate_B = true;
}


template<class T>
void rMatrixObjectBase<T>::setPosition( const rVec3< T > &_pos ) {
   vTranslationMatrix_MAT.setMat
      (
         1, 0, 0, _pos.x,
         0, 1, 0, _pos.y,
         0, 0, 1, _pos.z,
         0, 0, 0, 1
      );
   vNeedMatrixUpdate_B = true;
   vTranslation_GLM = glm::translate(glm::vec3(_pos.x, _pos.y, _pos.z));
}

template<class T>
void rMatrixObjectBase<T>::getPosition( rVec3< T > &_pos ) {
   _pos.x = vTranslationMatrix_MAT.template get<3, 0>();
   _pos.y = vTranslationMatrix_MAT.template get<3, 1>();
   _pos.z = vTranslationMatrix_MAT.template get<3, 2>();
}

template<class T>
void rMatrixObjectBase<T>::addPositionDelta( const rVec3< T > &_pos ) {
   vTranslationMatrix_MAT.setMat
      (
         1, 0, 0, vTranslationMatrix_MAT.template get<3, 0>() + _pos.x,
         0, 1, 0, vTranslationMatrix_MAT.template get<3, 1>() + _pos.y,
         0, 0, 1, vTranslationMatrix_MAT.template get<3, 2>() + _pos.z,
         0, 0, 0, 1
      );
   vNeedMatrixUpdate_B = true;
}


namespace {

inline void getRowSTR( float x, std::string &_str ) {
   static std::string lTempStr;
   lTempStr = boost::lexical_cast<std::string>( x );

   if( lTempStr.size() < 10 ) {
      lTempStr.insert( lTempStr.begin(), 10 - lTempStr.size(), ' ' );
   } else {
      lTempStr.resize( 10 );
   }

   _str += lTempStr + " ";
}


inline void printGLMMat( glm::mat4 &_mat, std::string _name, char _type ) {
   LOG( _type, false, __FILE__, __LINE__, LOG_FUNCTION_NAME, _name, ": " );

   std::string lRowStr;

   for( size_t row = 0; row < 4; ++row ) {
      lRowStr.clear();
      for( size_t collumn = 0; collumn < 4; ++collumn ) {
         getRowSTR( _mat[collumn][row], lRowStr );
      }
      LOG( _type, true, __FILE__, __LINE__, LOG_FUNCTION_NAME, "( ", lRowStr, " )" );
   }

   LOG( _type, true, __FILE__, __LINE__, LOG_FUNCTION_NAME, "" );
}

}

template<class T>
void rMatrixObjectBase<T>::updateFinalMatrix( bool _forceUpdate ) {
   if( vCameraSpaceMatrix_MAT == nullptr || ( !_forceUpdate && !vNeedMatrixUpdate_B ) )
      return;

   vFinalMatrix_MAT = *vCameraSpaceMatrix_MAT * vTranslationMatrix_MAT * vRotationMatrix_MAT * vScaleMatrix_MAT;
   vFINAL_GLM = *vCameraSpace_GLM * vTranslation_GLM * vRotation_GLM * vScale_GLM;
   vNeedMatrixUpdate_B = false;

#if E_DEBUG_LOGGING

   vScaleMatrix_MAT.print( "[MATRIX - ScaleMatrix] OUT", 'I' );
   vRotationMatrix_MAT.print( "[MATRIX - RotationMatrix] OUT", 'I' );
   vTranslationMatrix_MAT.print( "[MATRIX - TranslationMatrix] OUT", 'I' );
   vCameraSpaceMatrix_MAT->print( "[MATRIX - CameraSpaceMatrix] OUT", 'I' );
   vFinalMatrix_MAT.print( "[MATRIX - FinalMatrix_MAT] OUT", 'I' );

   printGLMMat(vFINAL_GLM, "[MATRIX - FINAL GLM] OUT", 'W');
#endif
}



}

#endif // R_MATRIX_OBJECT_BASE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
