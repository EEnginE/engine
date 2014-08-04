/*!
 * \file rRenderNormalOBJBase.hpp
 * \brief \b Classes: \a rRenderNormalOBJBase
 */

#ifndef R_RENDER_NORMAL_OBJ_BASE_HPP
#define R_RENDER_NORMAL_OBJ_BASE_HPP

#include <GL/glew.h>
#include "rMatrixMath.hpp"

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
template<class T>
class rRenderNormalOBJBase {
   protected:
      bool vNeedUpdateUniforms_B;
      bool vAlwaysUpdateUniforms_B;
   public:
      rRenderNormalOBJBase() : vNeedUpdateUniforms_B( false ), vAlwaysUpdateUniforms_B( false ) {}
      virtual ~rRenderNormalOBJBase() {}

      virtual void          render()                                 = 0;
      virtual bool          setOGLInfo( std::vector<void *> &_data ) = 0;
      virtual RENDERER_ID   getRendererID() const                    = 0;

      void updateUniforms() {vNeedUpdateUniforms_B = true;}
      void updateUniformsAlways( bool _doit ) { vAlwaysUpdateUniforms_B = _doit; }
};

}

}

#endif // RRENDERNORMALOBJBASE_HPP
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
