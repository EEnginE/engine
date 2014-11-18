
#ifndef R_LOADER_BASE_HPP
#define R_LOADER_BASE_HPP

#include <GL/glew.h>
#include <vector>
#include <boost/fusion/include/adapt_struct.hpp>
#include "engine_render_Export.hpp"


namespace e_engine {

namespace internal {

template<class T>
struct _3D_Data {
   std::vector<T>      vVertexData;
   std::vector<T>      vTextureData;
   std::vector<T>      vNormalesData;

   std::vector<GLuint> vIndexVertexData;
   std::vector<GLuint> vIndexTextureData;
   std::vector<GLuint> vIndexNormalData;
};

typedef _3D_Data<GLfloat>  _3D_DataF;
typedef _3D_Data<GLdouble> _3D_DataD;

template<class T>
class rLoader_3D_Base {
   protected:
      _3D_Data<T> vData;

   public:
      std::vector<GLfloat> *getRawVertexData();
      std::vector<GLfloat> *getRawTextureData();
      std::vector<GLfloat> *getRawNormalData();
      
      std::vector<GLuint>  *getRawIndexVertexData();
      std::vector<GLuint>  *getRawIndexTextureData();
      std::vector<GLuint>  *getRawIndexNormalData();
};

/*!
 * \brief Gets the raw vertex data pointer
 * \returns The raw vertex data pointer
 */
template<class T>
std::vector< GLfloat > *rLoader_3D_Base<T>::getRawVertexData() {
   return &vData.vVertexData;
}

/*!
 * \brief Gets the raw vertex data pointer
 * \returns The raw vertex data pointer
 */
template<class T>
std::vector< GLfloat > *rLoader_3D_Base<T>::getRawTextureData() {
   return &vData.vTextureData;
}


/*!
 * \brief Gets the raw vertex data pointer
 * \returns The raw vertex data pointer
 */
template<class T>
std::vector< GLfloat > *rLoader_3D_Base<T>::getRawNormalData() {
   return &vData.vNormalesData;
}



/*!
 * \brief Gets the raw vertex index pointer
 * \returns The raw vertex index pointer
 */
template<class T>
std::vector< GLuint > *rLoader_3D_Base<T>::getRawIndexVertexData() {
   return &vData.vIndexVertexData;
}

/*!
 * \brief Gets the raw vertex index pointer
 * \returns The raw vertex index pointer
 */
template<class T>
std::vector< GLuint > *rLoader_3D_Base<T>::getRawIndexTextureData() {
   return &vData.vIndexTextureData;
}

/*!
 * \brief Gets the raw normal index pointer
 * \returns The raw normal index pointer
 */
template<class T>
std::vector< GLuint > *rLoader_3D_Base<T>::getRawIndexNormalData() {
   return &vData.vIndexNormalData;
}


}

}

BOOST_FUSION_ADAPT_STRUCT(
      e_engine::internal::_3D_DataF,
      ( std::vector<GLfloat>, vVertexData )
      ( std::vector<GLfloat>, vTextureData )
      ( std::vector<GLfloat>, vNormalesData )
      
      ( std::vector<GLuint>,  vIndexVertexData )
      ( std::vector<GLuint>,  vIndexTextureData )
      ( std::vector<GLuint>,  vIndexNormalData )
)

#endif // R_LOADER_BASE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
