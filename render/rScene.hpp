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

#include "vkuCommandPoolManager.hpp"
#include "rMatrixSceneBase.hpp"
#include "rObjectBase.hpp"
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>
#include <vulkan.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>

namespace e_engine {

class rWorld;

class rSceneBase {
 public:
  struct MeshInfo {
    uint32_t    index;
    std::string name;
    MESH_TYPES  type;
  };

  template <typename T>
  using OBJECTS   = std::vector<std::shared_ptr<T>>;
  using BASE_OBJS = OBJECTS<rObjectBase>;

 private:
  rWorld *vWorldPtr;

  BASE_OBJS vObjects;

  std::vector<size_t> vLightSourcesIndex;

  std::string vName_str;

  std::mutex           vObjects_MUT;
  std::recursive_mutex vObjectsInit_MUT;

  bool            vInitializingObjects = false;
  vkuCommandPool *vInitPool_vk         = nullptr;
  VkCommandBuffer vInitBuff_vk;
  VkQueue         vInitQueue_vk;

  BASE_OBJS vInitObjects;

  Assimp::Importer vImporter_assimp;
  aiScene const *  vScene_assimp = nullptr;


 public:
  rSceneBase() = delete;
  rSceneBase(std::string _name, rWorld *_world);
  virtual ~rSceneBase();

  bool canRenderScene();

  unsigned addObject(std::shared_ptr<rObjectBase> _obj);
  BASE_OBJS getObjects();

  std::vector<MeshInfo> loadFile(std::string _file);
  aiMesh const *getAiMesh(uint32_t _objIndex);

  bool beginInitObject();
  bool initObject(std::shared_ptr<rObjectBase> _obj, uint32_t _objIndex);
  bool endInitObject();

  size_t getNumObjects() { return vObjects.size(); }
};

template <class T>
class rScene : public rSceneBase, public rMatrixSceneBase<float> {
 public:
  rScene(std::string _name, rWorld *_world) : rSceneBase(_name, _world), rMatrixSceneBase<float>(_world) {}
};
}


// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
