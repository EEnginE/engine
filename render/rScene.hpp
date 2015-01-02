/*!
 * \file rScene.hpp
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

#ifndef R_SCENE_HPP
#define R_SCENE_HPP

#include "defines.hpp"

#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include "rRenderBase.hpp"
#include "rShader.hpp"
#include <vector>
#include <string>
#include <thread>

namespace e_engine {

class rSceneBase {
 public:
   struct rObject {
      rObjectBase *vObjectPointer;
      rRenderBase *vRenderer;
      GLint vShaderIndex;

      rObject( rObjectBase *_obj, GLint _index )
          : vObjectPointer( _obj ), vRenderer( nullptr ), vShaderIndex( _index ) {}
      virtual ~rObject() {}
   };

   template <class... R>
   struct select {
      template <class... T>
      select( T... ) {}
   };

   template <class T, class... R>
   struct select<T, R...> : select<R...> {
      select( rRenderBase **_p, rShader *_s, rObjectBase *_o ) : select<R...>( _p, _s, _o ) {
         if ( !*_p && T::testShader( _s ) && T::testObject( _o ) ) {
            *_p = new T();
         }
      }
   };

 private:
   std::vector<rObject> vObjects;
   std::vector<rShader> vShaders;

   std::vector<size_t> vLightSourcesIndex;

   std::string vName_str;

   bool vReadyToRender_B;

   std::mutex vObjects_MUT;
   std::mutex vShaders_MUT;

   int assignObjectRenderer( GLuint _index, rRenderBase *_renderer );

 public:
   rSceneBase( std::string _name ) : vName_str( _name ), vReadyToRender_B( false ) {}
   virtual ~rSceneBase();
   void renderScene();

   bool canRenderScene();

   int addObject( rObjectBase *_obj, GLint _shaderIndex );

   int addShader( std::string _shader );
   int compileShaders();
   int parseShaders();

   GLuint getNumObjects() { return vObjects.size(); }

   template <class T, class... RENDERERS>
   int setObjectRenderer( GLuint _index );
};

/*!
 * \brief Assigns a renderer to an object
 *
 * \note You must pass the renderers in REVERSE ORDER
 *
 * The renderer will only be changed if this function returns 0. If the previous
 * renderer is is still valid, nothing will be changed.
 *
 * \returns 0 on success
 * \returns 1 if _index is out of range
 * \returns 2 if the object pointer is invalid
 * \returns 3 if the shader does not exist
 * \returns 4 if no matching renderer was found
 */
template <class T, class... RENDERERS>
int rSceneBase::setObjectRenderer( GLuint _index ) {
   std::lock_guard<std::mutex> lLockObjects( vObjects_MUT );
   std::lock_guard<std::mutex> lLockShaders( vShaders_MUT );

   if ( _index > vObjects.size() )
      return 1;


   if ( !vObjects[_index].vObjectPointer )
      return 2;

   if ( (size_t)vObjects[_index].vShaderIndex > vShaders.size() ||
        vObjects[_index].vShaderIndex < 0 )
      return 3;

   rRenderBase *lRenderer = nullptr;
   select<T, RENDERERS...> selecter(
         &lRenderer, &vShaders[vObjects[_index].vShaderIndex], vObjects[_index].vObjectPointer );

   if ( !lRenderer )
      return 4;

   return assignObjectRenderer( _index, lRenderer );
}





template <class T>
class rScene : public rSceneBase, public rMatrixSceneBase<float> {
 public:
   rScene( std::string _name ) : rSceneBase( _name ) {}
};
}

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
