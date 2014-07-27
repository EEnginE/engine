/*!
 * \file rRenderNormalOBJBase.hpp
 * \brief \b Classes: \a rRenderNormalOBJBase
 */

#ifndef R_RENDER_NORMAL_OBJ_BASE_HPP
#define R_RENDER_NORMAL_OBJ_BASE_HPP

#include <GL/glew.h>

namespace e_engine {

// Usage: render_<RenderType>_<V. Majo>_<V. Minor>_<Other Stuff>_<Min num Shader>[_<Max num Shader (n for arabitary)>]S_<Min num Data>[_<Max num Data (n for arabitary)>]D
enum RENDERER_ID {
   render_NONE,
   render_OGL_3_3_Normal_Basic_1S_1D
};

namespace e_engine_internal {

/*!
 * \brief Basic render to provide an interface for other render classes
 */
class rRenderNormalOBJBase {
   public:
      rRenderNormalOBJBase() {}
      virtual ~rRenderNormalOBJBase() {}
      
      virtual void render() = 0;
      virtual bool setOGLInfo( std::vector<void *> &_data ) = 0;
      virtual RENDERER_ID getRendererID() const = 0;
};

}

}

#endif // RRENDERNORMALOBJBASE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
