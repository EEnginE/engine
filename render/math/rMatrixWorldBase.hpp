/*!
 * \file rMatrixWorldBase.hpp
 * \brief \b Classes: \a rMatrixWorldBase
 */

#ifndef R_MATRIX_WORLD_BASE_HPP
#define R_MATRIX_WORLD_BASE_HPP

#include "rMatrixMath.hpp"
#include "engine_render_Export.hpp"


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
void rMatrixWorldBase<T>::setCamera(const rVec3< T > &_position, const rVec3< T > &_lookAt, const rVec3< T > &_upVector) {
	rMatrixMath::camera(_position, _lookAt, _upVector, vCameraMatrix_MAT);
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
   vProjectionMatrix_MAT.print( "[MATRIX - ProjectionMatrix] OUT", 'I' );

#endif
   return true;
}




}

#endif // R_MATRIX_WORLD_BASE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
