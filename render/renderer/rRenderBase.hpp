/*!
 * \file rRenderNormalOBJBase.hpp
 * \brief \b Classes: \a rRenderNormalOBJBase
 */

#ifndef R_RENDER_NORMAL_OBJ_BASE_HPP
#define R_RENDER_NORMAL_OBJ_BASE_HPP

#include "rShader.hpp"
#include "rObjectBase.hpp"


namespace e_engine {

// Usage: render_<RenderType>_<V. Majo>_<V. Minor>_<Other Stuff>_<Min num Shader>[_<Max num Shader (n for arabitary)>]S_<Min num Data>[_<Max num Data (n for arabitary)>]D
enum RENDERER_ID {
   render_NONE,
   render_OGL_3_3_Normal_Basic_1S_1D,
   ___RENDERER_ENGINE_LAST___
};

/*!
 * \brief Basic render to provide an interface for other render classes
 */
class rRenderBase {
   protected:
      bool vNeedUpdateUniforms_B;
      bool vAlwaysUpdateUniforms_B;
   public:
      rRenderBase() : vNeedUpdateUniforms_B( false ), vAlwaysUpdateUniforms_B( false ) {}
      virtual ~rRenderBase() {}

      virtual void        render()                              = 0;
      virtual RENDERER_ID getRendererID() const                 = 0;
      virtual void        setDataFromShader( rShader *_s )      = 0;
      virtual void        setDataFromObject( rObjectBase *_obj ) = 0;

      void updateUniforms() {vNeedUpdateUniforms_B = true;}
      void updateUniformsAlways( bool _doit ) { vAlwaysUpdateUniforms_B = _doit; }
};

/*!
 * \fn rRenderBase::setDataFromShader
 * \brief Sets neccessary OpenGL information for the rendere from the shader
 * 
 * \warning This function assumes that testShader (from the derived class) returned true
 */


}

#endif // R_RENDER_NORMAL_OBJ_BASE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
