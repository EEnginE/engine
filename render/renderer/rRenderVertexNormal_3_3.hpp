/*!
 * \file rRenderVertexNormal_3_3.hpp
 */

#ifndef R_RENDER_VERTEX_NOTMAL_3_3_HPP
#define R_RENDER_VERTEX_NOTMAL_3_3_HPP

#include <GL/glew.h>
#include "rRenderBase.hpp"
#include "rObjectBase.hpp"
#include "rMatrixMath.hpp"

namespace e_engine {

class rRenderVertexNormal_3_3 : public rRenderBase {
   private:
      GLuint vVertexBufferObj_OGL      = 0;
      GLuint vIndexBufferObj_OGL       = 0;
      GLuint vNormalBufferObj_OGL      = 0;

      GLuint vShader_OGL               = 0;

      GLint  vInputVertexLocation_OGL  = -1;
      GLint  vInputNormalsLocation_OGL = -1;
      GLint  vUniformMVP_OGL           = -1;

      GLuint vDataSize_uI              = 0;

      rMat4f *vModelViewProjection     = nullptr;

   public:
      rRenderVertexNormal_3_3() {}
      virtual ~rRenderVertexNormal_3_3() {}

      virtual void        render();
      virtual RENDERER_ID getRendererID() const { return render_OGL_3_3_Normal_Basic_1S_1D; }
      virtual void        setDataFromShader( rShader *_s );
      virtual void        setDataFromObject( rObjectBase *_obj );

      virtual bool        canRender();

      static bool         testShader( rShader *_shader );
      static bool         testObject( rObjectBase *_obj );
};

}

#endif // R_RENDER_VERTEX_NOTMAL_3_3_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
