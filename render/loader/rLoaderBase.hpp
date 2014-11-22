
#ifndef R_LOADER_BASE_HPP
#define R_LOADER_BASE_HPP

#include <GL/glew.h>
#include <vector>
#include <string>
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
class rLoaderBase {
   protected:
      _3D_Data<T> vData;

      bool        vIsDataLoaded_B;
      std::string vFilePath_str;

   public:
      virtual ~rLoaderBase() {}

      std::vector<GLfloat> *getRawVertexData();
      std::vector<GLfloat> *getRawTextureData();
      std::vector<GLfloat> *getRawNormalData();

      std::vector<GLuint>  *getRawIndexVertexData();
      std::vector<GLuint>  *getRawIndexTextureData();
      std::vector<GLuint>  *getRawIndexNormalData();

      void setFile( std::string _file );

      bool        getIsLoaded() const;
      std::string getFilePath() const;

      virtual int load() = 0;
      void        unLoad();
};


/*!
 * \brief Clears the memory
 * \returns Nothing
 */
template<class T>
void rLoaderBase<T>::unLoad() {
   vIsDataLoaded_B = false;
   vData.vVertexData.clear();
   vData.vVertexData.resize( 0 );
   vData.vNormalesData.clear();
   vData.vNormalesData.resize( 0 );
   vData.vIndexVertexData.clear();
   vData.vIndexVertexData.resize( 0 );
   vData.vIndexNormalData.clear();
   vData.vIndexNormalData.resize( 0 );
}

/*!
 * \brief Gets wether or not the file is loaded and parsed
 * \returns The state of the file being loaded and parsed
 */
template<class T>
bool rLoaderBase<T>::getIsLoaded() const {
   return vIsDataLoaded_B;
}

/*!
 * \brief Gets the path of the file to parse
 * \returns The path of the file to parse
 */
template<class T>
std::string rLoaderBase<T>::getFilePath() const {
   return vFilePath_str;
}


/*!
 * \brief Sets the file to load
 * \param[in] _file The file to load
 *
 * \note This will NOT load the file! You have to manually load it with load()
 *
 * \returns Nothing
 */
template<class T>
void rLoaderBase<T>::setFile( std::string _file ) {
   vFilePath_str = _file;
}

/*!
 * \brief Gets the raw vertex data pointer
 * \returns The raw vertex data pointer
 */
template<class T>
std::vector< GLfloat > *rLoaderBase<T>::getRawVertexData() {
   return &vData.vVertexData;
}

/*!
 * \brief Gets the raw vertex data pointer
 * \returns The raw vertex data pointer
 */
template<class T>
std::vector< GLfloat > *rLoaderBase<T>::getRawTextureData() {
   return &vData.vTextureData;
}


/*!
 * \brief Gets the raw vertex data pointer
 * \returns The raw vertex data pointer
 */
template<class T>
std::vector< GLfloat > *rLoaderBase<T>::getRawNormalData() {
   return &vData.vNormalesData;
}



/*!
 * \brief Gets the raw vertex index pointer
 * \returns The raw vertex index pointer
 */
template<class T>
std::vector< GLuint > *rLoaderBase<T>::getRawIndexVertexData() {
   return &vData.vIndexVertexData;
}

/*!
 * \brief Gets the raw vertex index pointer
 * \returns The raw vertex index pointer
 */
template<class T>
std::vector< GLuint > *rLoaderBase<T>::getRawIndexTextureData() {
   return &vData.vIndexTextureData;
}

/*!
 * \brief Gets the raw normal index pointer
 * \returns The raw normal index pointer
 */
template<class T>
std::vector< GLuint > *rLoaderBase<T>::getRawIndexNormalData() {
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

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
