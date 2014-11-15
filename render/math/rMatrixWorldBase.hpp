/*!
 * \file rMatrixWorldBase.hpp
 * \brief \b Classes: \a rMatrixWorldBase
 */

#ifndef R_MATRIX_WORLD_BASE_HPP
#define R_MATRIX_WORLD_BASE_HPP

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
class rMatrixWorldBase {
   private:
      rMat4<T>  vProjectionMatrix_MAT;
      rMat4<T>  vCameraMatrix_MAT;
      rMat4<T>  vCameraSpaceMatrix_MAT;

      glm::mat4 vGLM_final;
      glm::mat4 vGLM_camera;
      glm::mat4 vGLM_projection;

      bool      vNeedCamaraSpaceMatrixUpdate_B;
      bool      vAlwaysUpdateMatrix_B;

   public:
      rMatrixWorldBase();

      inline void calculateProjectionPerspective( T _width, T _height, T _nearZ, T _farZ, T _fofy );
      inline void calculateProjectionPerspective( T _aspectRatio, T _nearZ, T _farZ, T _fofy );

      inline void setCamera( const rVec3< T > &_position, const rVec3< T > &_lookAt, const rVec3< T > &_upVector );

      inline void alwaysUpdateMatrix( bool _doItAlways = true ) { vAlwaysUpdateMatrix_B = _doItAlways; }

      inline rMat4<T> *getProjectionMatrix()  { return &vProjectionMatrix_MAT; }
      inline rMat4<T> *getCameraMatrix()      { return &vCameraMatrix_MAT; }
      inline rMat4<T> *getCameraSpaceMatrix() { return &vCameraSpaceMatrix_MAT; }

      inline bool updateCameraSpaceMatrix();
};

namespace internal {

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
rMatrixWorldBase<T>::rMatrixWorldBase() {
   vProjectionMatrix_MAT.toIdentityMatrix();
   vCameraMatrix_MAT.toIdentityMatrix();
   vCameraSpaceMatrix_MAT.toIdentityMatrix();

   vNeedCamaraSpaceMatrixUpdate_B = true;
   vAlwaysUpdateMatrix_B          = false;
}

/*!
 * \brief calculates the projection matrix (perspective)
 *
 * \param[in] _aspectRatio The aspect ratio of the viewport to render
 * \param[in] _nearZ       The near clipping plane
 * \param[in] _farZ        The far clipping plane
 * \param[in] _fofy        The field of view angle
 */
template<class T>
void rMatrixWorldBase<T>::calculateProjectionPerspective( T _aspectRatio, T _nearZ, T _farZ, T _fofy ) {
   rMatrixMath::perspective( _aspectRatio, _nearZ, _farZ, _fofy, vProjectionMatrix_MAT );
   vGLM_projection = glm::perspective( _fofy, _aspectRatio, _nearZ, _farZ );
   vNeedCamaraSpaceMatrixUpdate_B = true;
}

/*!
 * \brief calculates the projection matrix (perspective)
 *
 * \param[in] _width       The width of the viewport to render
 * \param[in] _height      The height of the viewport to render
 * \param[in] _nearZ       The near clipping plane
 * \param[in] _farZ        The far clipping plane
 * \param[in] _fofy        The field of view angle
 */
template<class T>
void rMatrixWorldBase<T>::calculateProjectionPerspective( T _width, T _height, T _nearZ, T _farZ, T _fofy ) {
   rMatrixMath::perspective( _width / _height, _nearZ, _farZ, _fofy, vProjectionMatrix_MAT );
   vGLM_projection = glm::perspective( _fofy, _width / _height, _nearZ, _farZ );
   vNeedCamaraSpaceMatrixUpdate_B = true;
}

/*!
 * \brief calculates the camera matrix
 *
 * \param[in] _position The position of the camera
 * \param[in] _lookAt   The direction of the camera
 * \param[in] _upVector The up direction of the camera ( mostly rVec3( 0, 1, 0 ) )
 */
template<class T>
void rMatrixWorldBase<T>::setCamera( const rVec3< T > &_position, const rVec3< T > &_lookAt, const rVec3< T > &_upVector ) {
   rMatrixMath::camera( _position, _lookAt, _upVector, vCameraMatrix_MAT );
   vGLM_camera = glm::lookAt(
         glm::vec3( _position.x, _position.y, _position.z ),
         glm::vec3( _lookAt.x, _lookAt.y, _lookAt.z ),
         glm::vec3( _upVector.x, _upVector.y, _upVector.z )
         );
   vNeedCamaraSpaceMatrixUpdate_B = true;
}



/*!
 * \brief Updates the camera-space-matrix
 *
 * This should be done once before rendering a frame
 *
 * \returns true - if matrix was updated and false if not
 */
template<class T>
bool rMatrixWorldBase<T>::updateCameraSpaceMatrix() {
   if( !vNeedCamaraSpaceMatrixUpdate_B && !vAlwaysUpdateMatrix_B )
      return false;

   vCameraSpaceMatrix_MAT = vProjectionMatrix_MAT * vCameraMatrix_MAT;
   vNeedCamaraSpaceMatrixUpdate_B = false;

#if E_DEBUG_LOGGING

   vCameraMatrix_MAT.print( "[MATRIX - CameraMatrix] OUT", 'I' );
   internal::printGLMMat( vGLM_camera, "GLM Camera", 'E' );

   vProjectionMatrix_MAT.print( "[MATRIX - ProjectionMatrix] OUT", 'I' );
   internal::printGLMMat( vGLM_projection, "GLM Projection", 'E' );

   vGLM_final = vGLM_projection * vGLM_camera;

   vCameraSpaceMatrix_MAT.print( "[MATRIX - CameraSoace] OUT", 'I' );
   internal::printGLMMat( vGLM_final, "GLM CameraSoace", 'E' );

#endif
   return true;
}




}

#endif // R_MATRIX_WORLD_BASE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
