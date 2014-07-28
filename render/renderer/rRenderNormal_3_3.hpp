/*!
 * \file rRenderNormal_3_3.hpp
 * \brief \b Classes: \a rRenderNormal_3_3
 */

#ifndef RRENDERNORMAL_3_3_HPP
#define RRENDERNORMAL_3_3_HPP

#include <GL/glew.h>
#include "rRenderNormalOBJBase.hpp"


namespace e_engine {

/*!
 * \brief Verry basic OpenGL 3.3 renderer
 * 
 * ID: render_OGL_3_3_Normal_Basic_1S_1D
 */
class rRenderNormal_3_3 : public e_engine_internal::rRenderNormalOBJBase {
   private:
      GLuint vVertexBufferObj_OGL;
      GLuint vIndexBufferObj_OGL;
      
      GLuint vShader_OGL;
      
      GLint  vInputLocation_OGL;
      GLint  vUniformLocation_OGL;
      
      GLuint vDataSize_uI;
      
      rMatrix<4,4> vMatrix;
      bool         vNeedUpdateUniforms_B;

   public:
      rRenderNormal_3_3();
      virtual ~rRenderNormal_3_3() {}
      
      virtual void          render();
      virtual bool          setOGLInfo( std::vector<void *> &_data );
      virtual RENDERER_ID   getRendererID() const { return render_OGL_3_3_Normal_Basic_1S_1D; }
      virtual rMatrix<4,4> *getMatrix() {return &vMatrix;}
      virtual void          updateUniforms() {vNeedUpdateUniforms_B = true;}
};

}

#endif // RRENDERNORMAL_3_3_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
