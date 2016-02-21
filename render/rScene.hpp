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
#include <vector>
#include <string>
#include <thread>

namespace e_engine {

class RENDER_API rSceneBase {
 public:
   struct rObject final {
      rObjectBase *vObjectPointer;
      int vShaderIndex;

      rObject( rObjectBase *_obj, int _index )
          : vObjectPointer( _obj ), vShaderIndex( _index ) {}
   };

 private:
   std::vector<rObject> vObjects;

   std::vector<size_t> vLightSourcesIndex;

   std::string vName_str;

   std::mutex vObjects_MUT;
   std::mutex vShaders_MUT;

 public:
   rSceneBase( std::string _name ) : vName_str( _name ) {}
   virtual ~rSceneBase();
   void renderScene();

   bool canRenderScene();

   unsigned addObject( rObjectBase *_obj, int _shaderIndex );

   int addShader( std::string _shader );
   int compileShaders();
   int parseShaders();

   size_t getNumObjects() { return vObjects.size(); }
};

template <class T>
class rScene : public rSceneBase, public rMatrixSceneBase<float> {
 public:
   rScene( std::string _name ) : rSceneBase( _name ) {}
};
}

#endif

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
