/*!
 * \file rScene.cpp
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

#include "rScene.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"
#include "iInit.hpp"
#include "rWorld.hpp"
#include <assimp/postprocess.h>

#if __cplusplus <= 201402L || true //! \todo FIX THIS when C++17 is released
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;
#else
#include <filesystem>
namespace fs = std::filesystem;
#endif

namespace e_engine {


rSceneBase::~rSceneBase() {}

/*!
 * \brief Constructor
 * \note The pointer _world must be valid over the lifetime of the object!
 */
rSceneBase::rSceneBase(std::string _name, rWorld *_world) : vWorldPtr(_world), vName_str(_name) {}

/*!
 * \brief Tests if it is safe to render the scene
 * \returns true when it is safe
 */
bool rSceneBase::canRenderScene() {
  bool lCanRender = true;
  for (auto const &d : vObjects) {
    int64_t lIsObjectReady;

    if (!d) {
      wLOG("Invalid Object Pointer");
      lCanRender = false;
      continue;
    }

    lIsObjectReady = d->getIsDataLoaded();

    if (lIsObjectReady != true) {
      wLOG("Object data for '", d->getName(), "' is not completely loaded --> Do not render scene '", vName_str, "'");
      lCanRender = false;
      continue;
    }
  }

  //! \todo Add vulkan stuff

  if (lCanRender)
    iLOG("Scene '", vName_str, "' with ", vObjects.size(), " objects ready for rendering");

  return lCanRender;

  wLOG("No objects in scene");
  return false;
}

/*!
 * \brief Parses and loads Object data form a file using assimp
 * \param _file The file to load
 * \returns A vector of mesh names
 */
std::vector<rSceneBase::MeshInfo> rSceneBase::loadFile(std::string _file) {
  std::lock_guard<std::recursive_mutex> lGuard(vObjectsInit_MUT);

  vImporter_assimp.FreeScene();
  vScene_assimp = vImporter_assimp.ReadFile(
      _file,
      aiProcess_Triangulate | aiProcess_CalcTangentSpace | aiProcess_GenSmoothNormals |
          aiProcess_JoinIdenticalVertices | aiProcess_SortByPType | aiProcess_RemoveRedundantMaterials |
          aiProcess_GenUVCoords | aiProcess_FindDegenerates | aiProcess_FindInvalidData | aiProcess_FixInfacingNormals |
          aiProcess_ImproveCacheLocality | aiProcess_OptimizeMeshes | aiProcess_OptimizeGraph);

  fs::path lTempPath(_file);
  vLoadedFilePath = lTempPath.parent_path().string();

  if (!vScene_assimp) {
    eLOG("Loading ", _file, " failed!");
    eLOG(vImporter_assimp.GetErrorString());
    return {};
  }


  if (!vScene_assimp->HasMeshes()) {
    wLOG("Imported file ", _file, " does not contain meshes!");
    return {};
  }

  std::vector<MeshInfo> lInfos;
  MeshInfo              lTempInfo;
  for (uint32_t i = 0; i < vScene_assimp->mNumMeshes; i++) {
    const char *lTemp = vScene_assimp->mMeshes[i]->mName.C_Str();
    lTempInfo.index   = static_cast<uint32_t>(lInfos.size());
    lTempInfo.name    = vScene_assimp->mMeshes[i]->mName.length > 0 ? lTemp : "";

    switch (vScene_assimp->mMeshes[i]->mPrimitiveTypes) {
      case aiPrimitiveType_POINT: lTempInfo.type = POINTS_3D; break;
      case aiPrimitiveType_LINE: lTempInfo.type = LINES_3D; break;
      case aiPrimitiveType_TRIANGLE: lTempInfo.type = MESH_3D; break;
      case aiPrimitiveType_POLYGON: lTempInfo.type = POLYGON_3D; break;
      default:
        lTempInfo.type = UNDEFINED_3D;
        wLOG("Unknown primitive type ", vScene_assimp->mMeshes[i]->mPrimitiveTypes);
    }

    lInfos.emplace_back(lTempInfo);
  }

  return lInfos;
}

aiMesh const *rSceneBase::getAiMesh(uint32_t _objIndex) {
  std::lock_guard<std::recursive_mutex> lGuard(vObjectsInit_MUT);

  if (!vScene_assimp) {
    eLOG("File not loaded");
    return nullptr;
  }

  if (_objIndex >= vScene_assimp->mNumMeshes) {
    eLOG("Invalid object index ", _objIndex);
    return nullptr;
  }

  return vScene_assimp->mMeshes[_objIndex];
}

/*!
 * \brief Objects can be initialized after calling this function
 *
 * Sets up internal command buffers and queues, needed to initialize objects
 *
 * \note calling this function will lock initializing for other threads, because only one thread can
 * \note initialize objects per scene
 * \returns true on success
 */
bool rSceneBase::beginInitObject() {
  if (vInitializingObjects) {
    eLOG("beginInitObject already called on secene ", vName_str);
    return false;
  }
  vObjectsInit_MUT.lock();

  uint32_t lQueueFamily;

  auto lDevice = vWorldPtr->getDevice();

  vInitQueue_vk = lDevice->getQueue(VK_QUEUE_TRANSFER_BIT, 0.25f, &lQueueFamily);
  vInitBuff_vk  = vkuCommandPoolManager::getBuffer(**lDevice, lQueueFamily);

  if (vInitBuff_vk.begin() != VK_SUCCESS) {
    vInitBuff_vk.destroy();
    vObjectsInit_MUT.unlock();
    return false;
  }

  vInitializingObjects = true;
  return true;
}


/*!
 * \brief STARTS initializing an object
 *
 * The object will NOT be fully initialized until endInitObject() is called
 */
bool rSceneBase::initObject(std::shared_ptr<rObjectBase> _obj, uint32_t _objIndex) {
  if (!vInitializingObjects) {
    eLOG("beginInitObject was NOT called on scene ", vName_str);
    return false;
  }

  std::lock_guard<std::recursive_mutex> lGuard(vObjectsInit_MUT);

  _obj->setData(vInitBuff_vk, vScene_assimp, _objIndex, vLoadedFilePath);
  vInitObjects.emplace_back(_obj);
  return true;
}

/*!
 * \brief Finishes initializing all previously recorded objects
 *
 * Submits internal vulkan command buffer
 */
bool rSceneBase::endInitObject() {
  if (!vInitializingObjects) {
    eLOG("beginInitObject was NOT called on secene ", vName_str);
    return false;
  }

  vInitBuff_vk.end();

  VkSubmitInfo lInfo         = {};
  lInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  lInfo.pNext                = nullptr;
  lInfo.waitSemaphoreCount   = 0;
  lInfo.pWaitSemaphores      = nullptr;
  lInfo.pWaitDstStageMask    = nullptr;
  lInfo.commandBufferCount   = 1;
  lInfo.pCommandBuffers      = &vInitBuff_vk.get();
  lInfo.signalSemaphoreCount = 0;
  lInfo.pSignalSemaphores    = nullptr;

  auto lDevice = vWorldPtr->getDevice();

  vkuFence_t lFence(**lDevice);

  VkResult lRes;

  {
    std::lock_guard<std::mutex> lLock(lDevice->getQueueMutex(vInitQueue_vk));
    lRes = vkQueueSubmit(vInitQueue_vk, 1, &lInfo, lFence[0]);
  }

  if (lRes) {
    eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
    vInitObjects.clear();
    vObjectsInit_MUT.unlock();
    return false;
  }

  lRes = lFence();
  if (lRes) {
    eLOG("Failed to wait for fence: ", uEnum2Str::toStr(lRes));
  }

  for (auto i : vInitObjects)
    i->finishData();

  vInitBuff_vk.destroy();

  vInitObjects.clear();
  vInitializingObjects = false;
  vObjectsInit_MUT.unlock();
  return true;
}

/*!
 * \brief Adds an object to render
 *
 * \todo Implement functions to remove / disable / enable objects
 *
 * \param[in] _obj Pointer to an object
 *
 * \returns The Index of the object
 */
unsigned rSceneBase::addObject(std::shared_ptr<rObjectBase> _obj) {
  std::lock_guard<std::mutex> lLockObjects(vObjects_MUT);

  vObjects.emplace_back(_obj);

#if 0
   int64_t lFlags;

   _obj->getHints( rObjectBase::FLAGS, lFlags );
   if ( lFlags & LIGHT_SOURCE )
      vLightSourcesIndex.emplace_back( vObjects.size() - 1 );
#endif

  return static_cast<unsigned>(vObjects.size() - 1);
}

std::vector<std::shared_ptr<rObjectBase>> rSceneBase::getObjects() { return vObjects; }
} // namespace e_engine

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
