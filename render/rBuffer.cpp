/*!
 * \file rBuffer.cpp
 * \brief \b Classes: \a rBuffer
 */
/*
 * Copyright (C) 2016 EEnginE project
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
 *
 */

#include "defines.hpp"
#include "rBuffer.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"
#include "rWorld.hpp"
#include <string.h> // memcpy

namespace e_engine {

rBuffer::rBuffer(rWorld *_tempWorld) : rBuffer(_tempWorld->getInitPtr()) {}

rBuffer::rBuffer(iInit *_init) : vDevice_vk(_init->getDevice()), vInitPtr(_init) {}

rBuffer::~rBuffer() {
  if (vIsLoaded)
    destroy();
}

bool rBuffer::errorCleanup() {
  if (vTempBuffer_vk)
    vkDestroyBuffer(vDevice_vk, vTempBuffer_vk, nullptr);

  if (vBuffer_vk)
    vkDestroyBuffer(vDevice_vk, vBuffer_vk, nullptr);

  if (vMemTemp_vk)
    vkFreeMemory(vDevice_vk, vMemTemp_vk, nullptr);

  if (vMem_vk)
    vkFreeMemory(vDevice_vk, vMem_vk, nullptr);

  vTempBuffer_vk = nullptr;
  vBuffer_vk     = nullptr;
  vMemTemp_vk    = nullptr;
  vMem_vk        = nullptr;

  return false;
}

/*!
 * \brief Creates the buffer
 * \vkIntern
 *
 * This function creates the buffers and allocates the memory for the data. The command buffer must
 * be submitted to a queue supporting TRANSFER before the buffer can be marked ready with
 * doneCopying()
 *
 * \returns true on success
 * \todo concurrent sharing mode and sparse binding (both only if necessary)
 *
 * Available data types:
 *  - double
 *  - float
 *  - uint8_t
 *  - uint16_t
 *  - uint32_t
 *  - uint64_t
 *  - int8_t
 *  - int16_t
 *  - int32_t
 *  - int64_t
 */
template <class T>
bool rBuffer::cmdInit(std::vector<T> const &_data, VkCommandBuffer _buff, VkBufferUsageFlags _flags) {
  if (vIsLoaded) {
    eLOG("Data already loaded!");
    return false;
  }

  if (vSettingUpInProgress) {
    eLOG("Init not finished jet! This init stage is already complete");
    return false;
  }

  void *lData;

  uint32_t lIndex_temp  = 0;
  uint32_t lIndex_final = 0;

  // ================
  // Creating Buffers
  // ================

  VkBufferCreateInfo lBuffInfo    = {};
  lBuffInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  lBuffInfo.pNext                 = nullptr;
  lBuffInfo.flags                 = 0;
  lBuffInfo.size                  = _data.size() * sizeof(T);
  lBuffInfo.usage                 = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  lBuffInfo.sharingMode           = VK_SHARING_MODE_EXCLUSIVE;
  lBuffInfo.queueFamilyIndexCount = 0;
  lBuffInfo.pQueueFamilyIndices   = nullptr;

#if D_LOG_VULKAN
  dLOG("Creating buffer:");
  dLOG("  -- size:  ", _data.size() * sizeof(T), " = ", _data.size(), "*", sizeof(T));
  dLOG("  -- usage: ", uEnum2Str::toStr(static_cast<VkBufferUsageFlagBits>(_flags)));
#endif

  auto lRes = vkCreateBuffer(vDevice_vk, &lBuffInfo, nullptr, &vTempBuffer_vk);
  if (lRes) {
    eLOG("'vkCreateBuffer' returned ", uEnum2Str::toStr(lRes));
    return errorCleanup();
  }

  lBuffInfo.usage = _flags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  lRes            = vkCreateBuffer(vDevice_vk, &lBuffInfo, nullptr, &vBuffer_vk);
  if (lRes) {
    eLOG("'vkCreateBuffer' returned ", uEnum2Str::toStr(lRes));
    return errorCleanup();
  }

  // =================
  // Allocating Memory
  // =================

  VkMemoryRequirements lMemReqs_temp, lMemReqs_final;
  vkGetBufferMemoryRequirements(vDevice_vk, vTempBuffer_vk, &lMemReqs_temp);
  vkGetBufferMemoryRequirements(vDevice_vk, vBuffer_vk, &lMemReqs_final);

  lIndex_temp =
      vInitPtr->getMemoryTypeIndexFromBitfield(lMemReqs_temp.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  lIndex_final =
      vInitPtr->getMemoryTypeIndexFromBitfield(lMemReqs_final.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

  if (lIndex_temp == UINT32_MAX || lIndex_final == UINT32_MAX) {
    eLOG("Unable to find memory type");
    return errorCleanup();
  }

  VkMemoryAllocateInfo lAllocInfo = {};
  lAllocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  lAllocInfo.pNext                = nullptr;
  lAllocInfo.allocationSize       = lMemReqs_temp.size;
  lAllocInfo.memoryTypeIndex      = lIndex_temp;
  lRes                            = vkAllocateMemory(vDevice_vk, &lAllocInfo, nullptr, &vMemTemp_vk);
  if (lRes) {
    eLOG("'vkAllocateMemory' returned ", uEnum2Str::toStr(lRes));
    return errorCleanup();
  }

  lAllocInfo.allocationSize  = lMemReqs_final.size;
  lAllocInfo.memoryTypeIndex = lIndex_final;
  lRes                       = vkAllocateMemory(vDevice_vk, &lAllocInfo, nullptr, &vMem_vk);
  if (lRes) {
    eLOG("'vkAllocateMemory' returned ", uEnum2Str::toStr(lRes));
    return errorCleanup();
  }

  // ===========
  // Copy memory
  // ===========

  // Make device memory available
  lRes = vkMapMemory(vDevice_vk, vMemTemp_vk, 0, lAllocInfo.allocationSize, 0, &lData);
  if (lRes) {
    eLOG("'vkMapMemory' returned ", uEnum2Str::toStr(lRes));
    return errorCleanup();
  }

  // Copy the data into the device memory
  memcpy(lData, _data.data(), _data.size() * sizeof(T));

  // Done copying
  vkUnmapMemory(vDevice_vk, vMemTemp_vk);

  // ============
  // Bind Buffers
  // ============

  lRes = vkBindBufferMemory(vDevice_vk, vTempBuffer_vk, vMemTemp_vk, 0);
  if (lRes) {
    eLOG("'vkBindBufferMemory' returned ", uEnum2Str::toStr(lRes));
    return errorCleanup();
  }

  lRes = vkBindBufferMemory(vDevice_vk, vBuffer_vk, vMem_vk, 0);
  if (lRes) {
    eLOG("'vkBindBufferMemory' returned ", uEnum2Str::toStr(lRes));
    return errorCleanup();
  }


  // ============
  // Copy Buffers
  // ============

  VkBufferCopy lRegion = {};
  lRegion.srcOffset    = 0;
  lRegion.dstOffset    = 0;
  lRegion.size         = _data.size() * sizeof(T);

  vkCmdCopyBuffer(_buff, vTempBuffer_vk, vBuffer_vk, 1, &lRegion);

  vSize                = static_cast<uint32_t>(_data.size());
  vSettingUpInProgress = true;
  return true;
}


/*!
 * \brief Signals the buffer object that the command buffer has executed
 * \vkIntern
 *
 * Call this function when the command bufer used int cmdInit has finished executing
 */
bool rBuffer::doneCopying() {
  if (vIsLoaded) {
    eLOG("Data already loaded!");
    return false;
  }

  if (!vSettingUpInProgress) {
    eLOG("Init not finished jet! This init stage is already complete");
    return false;
  }

  vkDestroyBuffer(vDevice_vk, vTempBuffer_vk, nullptr);
  vkFreeMemory(vDevice_vk, vMemTemp_vk, nullptr);

  vTempBuffer_vk = nullptr;
  vMemTemp_vk    = nullptr;

  vSettingUpInProgress = false;
  vIsLoaded            = true;
  return true;
}

bool rBuffer::destroy() {
  if (!vIsLoaded) {
    eLOG("Data already destroyed!");
    return false;
  }

  if (vTempBuffer_vk)
    vkDestroyBuffer(vDevice_vk, vTempBuffer_vk, nullptr);

  if (vBuffer_vk)
    vkDestroyBuffer(vDevice_vk, vBuffer_vk, nullptr);

  if (vMemTemp_vk)
    vkFreeMemory(vDevice_vk, vMemTemp_vk, nullptr);

  if (vMem_vk)
    vkFreeMemory(vDevice_vk, vMem_vk, nullptr);

  vTempBuffer_vk = nullptr;
  vBuffer_vk     = nullptr;
  vMemTemp_vk    = nullptr;
  vMem_vk        = nullptr;

  vIsLoaded            = false;
  vSettingUpInProgress = false;

  return true;
}

/*!
 * \brief returns nullptr on error
 */
VkBuffer rBuffer::getBuffer() {
  if (!vIsLoaded)
    return nullptr;

  return vBuffer_vk;
}

// Explicit isntanciate rBuffer::cmdInit
template bool rBuffer::cmdInit<double>(std::vector<double> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<float>(std::vector<float> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<uint8_t>(std::vector<uint8_t> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<uint16_t>(std::vector<uint16_t> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<uint32_t>(std::vector<uint32_t> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<uint64_t>(std::vector<uint64_t> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<int8_t>(std::vector<int8_t> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<int16_t>(std::vector<int16_t> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<int32_t>(std::vector<int32_t> const &, VkCommandBuffer, VkBufferUsageFlags);
template bool rBuffer::cmdInit<int64_t>(std::vector<int64_t> const &, VkCommandBuffer, VkBufferUsageFlags);
}
