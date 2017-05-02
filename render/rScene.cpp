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
#include "iInit.hpp"
#include "rWorld.hpp"

#include <assimp/postprocess.h>

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
          aiProcess_ImproveCacheLocality);

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
      case aiPrimitiveType_POINT: lTempInfo.type    = POINTS_3D; break;
      case aiPrimitiveType_LINE: lTempInfo.type     = LINES_3D; break;
      case aiPrimitiveType_TRIANGLE: lTempInfo.type = MESH_3D; break;
      case aiPrimitiveType_POLYGON: lTempInfo.type  = POLYGON_3D; break;
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

  vInitQueue_vk = vWorldPtr->getInitPtr()->getQueue(VK_QUEUE_TRANSFER_BIT, 0.25f, &lQueueFamily);
  vInitPool_vk  = vWorldPtr->getCommandPool(lQueueFamily);
  vInitBuff_vk  = vWorldPtr->createCommandBuffer(vInitPool_vk);

  if (vWorldPtr->beginCommandBuffer(vInitBuff_vk, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) {
    vkFreeCommandBuffers(vWorldPtr->getDevice(), vInitPool_vk, 1, &vInitBuff_vk);
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
  auto const *                          lMesh = getAiMesh(_objIndex);

  if (!lMesh)
    return false;

  _obj->setData(vInitBuff_vk, lMesh);
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

  vkEndCommandBuffer(vInitBuff_vk);

  VkSubmitInfo lInfo         = {};
  lInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  lInfo.pNext                = nullptr;
  lInfo.waitSemaphoreCount   = 0;
  lInfo.pWaitSemaphores      = nullptr;
  lInfo.pWaitDstStageMask    = nullptr;
  lInfo.commandBufferCount   = 1;
  lInfo.pCommandBuffers      = &vInitBuff_vk;
  lInfo.signalSemaphoreCount = 0;
  lInfo.pSignalSemaphores    = nullptr;

  auto lFence = vWorldPtr->createFence();

  VkResult lRes;

  {
    std::lock_guard<std::mutex> lLock(vWorldPtr->getInitPtr()->getQueueMutex(vInitQueue_vk));
    lRes = vkQueueSubmit(vInitQueue_vk, 1, &lInfo, lFence);
  }

  if (lRes) {
    eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
    vInitObjects.clear();
    vObjectsInit_MUT.unlock();
    return false;
  }

  lRes = vkWaitForFences(vWorldPtr->getDevice(), 1, &lFence, VK_TRUE, UINT64_MAX);
  if (lRes) {
    eLOG("'vkQueueSubmit' returned ", uEnum2Str::toStr(lRes));
  }

  for (auto i : vInitObjects)
    i->finishData();

  vkDestroyFence(vWorldPtr->getDevice(), lFence, nullptr);
  vkFreeCommandBuffers(vWorldPtr->getDevice(), vInitPool_vk, 1, &vInitBuff_vk);

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
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
