/*!
 * \file rRenderNormalOBJBase.hpp
 * \brief \b Classes: \a rRenderNormalOBJBase
 */
/*
 * Copyright (C) 2015 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef R_RENDER_NORMAL_OBJ_BASE_HPP
#define R_RENDER_NORMAL_OBJ_BASE_HPP

#include "defines.hpp"

#include "rShader.hpp"
#include "rObjectBase.hpp"

#if E_DEBUG_LOGGING
#include <sstream>
#endif


namespace e_engine {

// Usage: render_<RenderType>_<V. Major>_<V. Minor>_<Other Stuff>_<Min num Shader>[_<Max num Shader
// (n for arbitrary)>]S_<Min num Data>[_<Max num Data (n for arbitrary)>]D
enum RENDERER_ID { render_NONE, render_OGL_3_3_Normal_Basic_1S_1D, ___RENDERER_ENGINE_LAST___ };

/*!
 * \brief Basic renderer to provide an interface for other renderer classes
 */
class rRenderBase {
 protected:
   bool vNeedUpdateUniforms_B;
   bool vAlwaysUpdateUniforms_B;

 protected:
   template <class... ARGS>
   static inline bool require( rShader *_s, rShader::SHADER_INFORMATION _inf, ARGS &&... _args );
   static inline bool require( rShader *_s );

   template <class... ARGS>
   static inline bool testUnifrom( GLint _u, std::wstring &&_missing, ARGS &&... _args );
   static inline bool testUnifrom();

   template <class... ARGS, class T>
   static inline bool testPointer( T *_p, std::wstring &&_missing, ARGS &&... _args );
   static inline bool testPointer();

 public:
   rRenderBase() : vNeedUpdateUniforms_B( false ), vAlwaysUpdateUniforms_B( false ) {}
   virtual ~rRenderBase() {}

   virtual void render() = 0;
   virtual RENDERER_ID getRendererID() const = 0;
   virtual void setDataFromShader( rShader *_s ) = 0;
   virtual void setDataFromObject( rObjectBase *_obj ) = 0;

   virtual void setDataFromAdditionalObjects( rObjectBase *_obj ) {}

   virtual bool canRender() { return true; }

   void updateUniforms() { vNeedUpdateUniforms_B = true; }
   void updateUniformsAlways( bool _doit ) { vAlwaysUpdateUniforms_B = _doit; }
};

template <class... ARGS>
bool rRenderBase::require( rShader *_s, rShader::SHADER_INFORMATION _inf, ARGS &&... _args ) {
   if ( !( _s->getLocation( _inf ) >= 0 ) )
      return false;

   return require( _s, _args... );
}

bool rRenderBase::require( rShader *_s ) { return true; }


template <class... ARGS>
bool rRenderBase::testUnifrom( GLint _u, std::wstring &&_missing, ARGS &&... _args ) {
#if E_DEBUG_LOGGING
   dLOG( L"Uniform: ", _missing, L": ", _u );
#endif

   if ( _u < 0 ) {
      eLOG( L"MISSING Uniform: ", _missing );
#if E_DEBUG_LOGGING
      testUnifrom( _args... );
#endif
      return false;
   }

   return testUnifrom( _args... );
}

bool rRenderBase::testUnifrom() { return true; }


template <class... ARGS, class T>
bool rRenderBase::testPointer( T *_p, std::wstring &&_missing, ARGS &&... _args ) {
#if E_DEBUG_LOGGING
   std::wstringstream lConverter;
   lConverter << _p;
   dLOG( L"Pointer: ", _missing, L": ", lConverter.str() );
#endif

   if ( !_p ) {
      eLOG( L"MISSING Pointer: ", _missing );
#if E_DEBUG_LOGGING
      testPointer( _args... );
#endif
      return false;
   }

   return testPointer( _args... );
}


bool rRenderBase::testPointer() { return true; }


/*!
 * \fn rRenderBase::setDataFromShader
 * \brief Sets neccessary OpenGL information for the renderer from the shader
 *
 * \warning This function assumes that testShader (from the derived class) returned true
 */

/*!
* \fn rRenderBase::setDataFromObject
* \brief Sets neccessary OpenGL information for the renderer from the object
*
* \warning This function assumes that testObject (from the derived class) returned true
*/
}

#endif // R_RENDER_NORMAL_OBJ_BASE_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
