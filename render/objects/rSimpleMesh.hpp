/*!
 * \file rSimpleMesh.hpp
 * \brief \b Classes: \a rSimpleMesh
 */

#ifndef R_NORMAL_OBJECT_HPP
#define R_NORMAL_OBJECT_HPP

#include <string>
#include "rRenderBase.hpp"
#include "rMatrixObjectBase.hpp"
#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include "engine_render_Export.hpp"


namespace e_engine {

class render_EXPORT rSimpleMesh final : public rMatrixObjectBase<float>, public internal::rObjectBase {
   private:
      GLuint vVertexBufferObject;
      GLuint vIndexBufferObject;

      void setFlags();

      rSimpleMesh();
   public:
      rSimpleMesh( rMatrixSceneBase<float> *_scene, std::string _name, std::string _file, DATA_FILE_TYPE _type = AUTODETECT ) :
         rMatrixObjectBase( _scene ),
         rObjectBase( _name, _file, _type ) {setFlags();}

      virtual ~rSimpleMesh();

      int clearOGLData__();
      int setOGLData__();

      virtual uint32_t getVBO( uint32_t &_n );
      virtual uint32_t getIBO( uint32_t &_n );
      virtual uint32_t getMatrix( e_engine::rMat4f **_mat, internal::rObjectBase::MATRIX_TYPES _type );
};


}

#endif // R_NORMAL_OBJECT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
