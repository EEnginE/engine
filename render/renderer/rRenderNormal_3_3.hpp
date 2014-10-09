/*!
 * \file rRenderNormal_3_3.hpp
 * \brief \b Classes: \a rRenderNormal_3_3
 */

#ifndef RRENDERNORMAL_3_3_HPP
#define RRENDERNORMAL_3_3_HPP

#include <GL/glew.h>
#include "rRenderNormalOBJBase.hpp"
#include "engine_render_Export.hpp"



namespace e_engine {

/*!
 * \brief Verry basic OpenGL 3.3 renderer
 * 
 * ID: render_OGL_3_3_Normal_Basic_1S_1D
 */
class render_EXPORT rRenderNormal_3_3 : public internal::rRenderNormalOBJBase<float> {
   private:
      GLuint vVertexBufferObj_OGL;
      GLuint vIndexBufferObj_OGL;
      
      GLuint vShader_OGL;
      
      GLint  vInputLocation_OGL;
      GLint  vUniformLocation_OGL;
      
      GLuint vDataSize_uI;
      
      rMat4f *vMatrix;

      rRenderNormal_3_3() {}
   public:
      rRenderNormal_3_3( rMat4f *_mat );
      virtual ~rRenderNormal_3_3() {}
      
      virtual void          render();
      virtual bool          setOGLInfo( std::vector<void *> &_data );
      virtual RENDERER_ID   getRendererID() const { return render_OGL_3_3_Normal_Basic_1S_1D; }
};

}

#endif // RRENDERNORMAL_3_3_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
