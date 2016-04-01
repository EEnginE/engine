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

#pragma once

#include "defines.hpp"

#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <vulkan.h>

namespace e_engine {

class rWorld;

class RENDER_API rSceneBase {
 private:
   rWorld *vWorldPtr;

   std::vector<rObjectBase *> vObjects;

   std::vector<size_t> vLightSourcesIndex;

   std::string vName_str;

   std::mutex vObjects_MUT;
   std::recursive_mutex vObjectsInit_MUT;

   bool vInitializingObjects = false;
   VkCommandPool vInitPool_vk;
   VkCommandBuffer vInitBuff_vk;
   VkQueue vInitQueue_vk;

   std::vector<rObjectBase *> vInitObjs;


 public:
   rSceneBase() = delete;
   rSceneBase( std::string _name, rWorld *_world ) : vWorldPtr( _world ), vName_str( _name ) {}
   virtual ~rSceneBase();

   bool canRenderScene();

   unsigned addObject( rObjectBase *_obj );
   std::vector<rObjectBase *> getObjects();

   bool beginInitObject();
   bool initObject( rObjectBase *_obj,
                    std::vector<uint32_t> const &_index,
                    std::vector<float> const &_pos,
                    std::vector<float> const &_norm,
                    std::vector<float> const &_uv );
   bool endInitObject();

   size_t getNumObjects() { return vObjects.size(); }
};

template <class T>
class RENDER_API rScene : public rSceneBase, public rMatrixSceneBase<float> {
 public:
   rScene( std::string _name, rWorld *_world )
       : rSceneBase( _name, _world ), rMatrixSceneBase<float>( _world ) {}
};
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
