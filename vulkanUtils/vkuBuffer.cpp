/*
 * Copyright (C) 2017 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this File except in compliance with the License.
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
#include "vkuBuffer.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"

using namespace e_engine;

vkuBuffer::MemoryAccess::MemoryAccess(vkuDevicePTR _dev, VkDeviceMemory _mem, VkDeviceSize _size) {
  vDevice = _dev;
  vMem    = _mem;
  vSize   = _size;

  if (vMem == VK_NULL_HANDLE)
    return;

  VkResult lRes = vkMapMemory(**vDevice, vMem, 0, vSize, 0, &vPtr);
  if (lRes != VK_SUCCESS) {
    vPtr = nullptr;
    vMem = VK_NULL_HANDLE;
    eLOG(L"Failed to map memory");
  }
}

vkuBuffer::MemoryAccess::~MemoryAccess() {
  if (vPtr)
    vkUnmapMemory(**vDevice, vMem);
}

vkuBuffer::MemoryAccess::MemoryAccess(vkuBuffer::MemoryAccess &&_old) {
  vDevice = _old.vDevice;
  vSize   = _old.vSize;
  vMem    = _old.vMem;
  vPtr    = _old.vPtr;

  // Invalidate old object
  _old.vDevice = nullptr;
  _old.vPtr    = nullptr;
  _old.vMem    = VK_NULL_HANDLE;
}

vkuBuffer::MemoryAccess &vkuBuffer::MemoryAccess::operator=(vkuBuffer::MemoryAccess &&_old) {
  if (vPtr)
    vkUnmapMemory(**vDevice, vMem);

  vDevice = _old.vDevice;
  vSize   = _old.vSize;
  vMem    = _old.vMem;
  vPtr    = _old.vPtr;

  // Invalidate old object
  _old.vDevice = nullptr;
  _old.vPtr    = nullptr;
  _old.vMem    = VK_NULL_HANDLE;

  return *this;
}


vkuBuffer::vkuBuffer(vkuDevicePTR _device) : vDevice(_device) {}
vkuBuffer::~vkuBuffer() { destroy(); }

vkuBuffer::vkuBuffer(vkuBuffer &&_old) {
  vDevice             = _old.vDevice;
  vSize               = _old.vSize;
  vMainBuffer         = _old.vMainBuffer;
  vStagingBuffer      = _old.vStagingBuffer;
  vMainMemory         = _old.vMainMemory;
  vStagingMemory      = _old.vStagingMemory;
  vMainMemoryIndex    = _old.vMainMemoryIndex;
  vStagingMemoryIndex = _old.vStagingMemoryIndex;
  cfg                 = _old.cfg;

  // Invalidate old object
  _old.vDevice        = nullptr;
  _old.vMainBuffer    = VK_NULL_HANDLE;
  _old.vStagingBuffer = VK_NULL_HANDLE;
  _old.vMainMemory    = VK_NULL_HANDLE;
  _old.vStagingMemory = VK_NULL_HANDLE;
}

vkuBuffer &vkuBuffer::operator=(vkuBuffer &&_old) {
  destroy(); // Destroy old buffer

  vDevice             = _old.vDevice;
  vSize               = _old.vSize;
  vMainBuffer         = _old.vMainBuffer;
  vStagingBuffer      = _old.vStagingBuffer;
  vMainMemory         = _old.vMainMemory;
  vStagingMemory      = _old.vStagingMemory;
  vMainMemoryIndex    = _old.vMainMemoryIndex;
  vStagingMemoryIndex = _old.vStagingMemoryIndex;
  cfg                 = _old.cfg;

  // Invalidate old object
  _old.vDevice        = nullptr;
  _old.vMainBuffer    = VK_NULL_HANDLE;
  _old.vStagingBuffer = VK_NULL_HANDLE;
  _old.vMainMemory    = VK_NULL_HANDLE;
  _old.vStagingMemory = VK_NULL_HANDLE;

  return *this;
}

VkResult vkuBuffer::init(VkDeviceSize _size) {
  if (!vDevice || !*vDevice) {
    eLOG(L"Invalid vulkan device");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (isCreated()) {
    eLOG(L"Buffer already created");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  vSize = _size;

  VkBufferCreateInfo lBuffInfo;
  lBuffInfo.sType                 = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  lBuffInfo.pNext                 = nullptr;
  lBuffInfo.flags                 = 0;
  lBuffInfo.size                  = vSize;
  lBuffInfo.usage                 = cfg.usage;
  lBuffInfo.sharingMode           = cfg.sharingMode;
  lBuffInfo.queueFamilyIndexCount = static_cast<uint32_t>(cfg.queueFamilyIndices.size());
  lBuffInfo.pQueueFamilyIndices   = cfg.queueFamilyIndices.data();

  VkResult lRes = vkCreateBuffer(**vDevice, &lBuffInfo, nullptr, &vMainBuffer);
  if (lRes) {
    eLOG("'vkCreateBuffer' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  lBuffInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  lRes            = vkCreateBuffer(**vDevice, &lBuffInfo, nullptr, &vStagingBuffer);
  if (lRes) {
    eLOG("'vkCreateBuffer' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  VkMemoryRequirements lMainMemReqs, lStagingMemReqs;
  vkGetBufferMemoryRequirements(**vDevice, vMainBuffer, &lMainMemReqs);
  vkGetBufferMemoryRequirements(**vDevice, vStagingBuffer, &lStagingMemReqs);

  vMainMemoryIndex    = vDevice->getMemoryTypeIndex(lMainMemReqs, cfg.memoryFlags);
  vStagingMemoryIndex = vDevice->getMemoryTypeIndex(lStagingMemReqs, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

  // Check if we need the staging buffer at all. This is not the case when the main buffer is host visible
  if ((cfg.memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) == VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)
    vMainMemoryIndex = vStagingMemoryIndex;

  if (vMainMemoryIndex == UINT32_MAX || vStagingMemoryIndex == UINT32_MAX) {
    eLOG("Unable to find memory type");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  // Check if we need the staging buffer at all
  if (vMainMemoryIndex == vStagingMemoryIndex) {
    vkDestroyBuffer(**vDevice, vStagingBuffer, nullptr);
    vStagingBuffer = VK_NULL_HANDLE;
  }

  VkMemoryAllocateInfo lAllocInfo = {};
  lAllocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  lAllocInfo.pNext                = nullptr;
  lAllocInfo.allocationSize       = lMainMemReqs.size;
  lAllocInfo.memoryTypeIndex      = vMainMemoryIndex;

  lRes = vkAllocateMemory(**vDevice, &lAllocInfo, nullptr, &vMainMemory);
  if (lRes) {
    eLOG("'vkAllocateMemory' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  lRes = vkBindBufferMemory(**vDevice, vMainBuffer, vMainMemory, 0);
  if (lRes) {
    eLOG("'vkBindBufferMemory' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  if (vMainMemoryIndex != vStagingMemoryIndex) {
    lAllocInfo.allocationSize  = lStagingMemReqs.size;
    lAllocInfo.memoryTypeIndex = vStagingMemoryIndex;

    lRes = vkAllocateMemory(**vDevice, &lAllocInfo, nullptr, &vStagingMemory);
    if (lRes) {
      eLOG("'vkAllocateMemory' returned ", uEnum2Str::toStr(lRes));
      return lRes;
    }

    lRes = vkBindBufferMemory(**vDevice, vStagingBuffer, vStagingMemory, 0);
    if (lRes) {
      eLOG("'vkBindBufferMemory' returned ", uEnum2Str::toStr(lRes));
      return lRes;
    }
  }

  return VK_SUCCESS;
}

void vkuBuffer::destroy() {
  if (!vDevice || !*vDevice)
    return;

  if (vStagingBuffer != VK_NULL_HANDLE)
    vkDestroyBuffer(**vDevice, vStagingBuffer, nullptr);

  if (vMainBuffer != VK_NULL_HANDLE)
    vkDestroyBuffer(**vDevice, vMainBuffer, nullptr);

  if (vStagingMemory != VK_NULL_HANDLE)
    vkFreeMemory(**vDevice, vStagingMemory, nullptr);

  if (vMainMemory != VK_NULL_HANDLE)
    vkFreeMemory(**vDevice, vMainMemory, nullptr);

  vMainBuffer    = VK_NULL_HANDLE;
  vStagingBuffer = VK_NULL_HANDLE;
  vMainMemory    = VK_NULL_HANDLE;
  vStagingMemory = VK_NULL_HANDLE;
}

/*!
 * \brief Frees the staging buffer memory (only if a staging buffer exists)
 *
 * \warning Do not call this function while the memory is mapped or a cmdSync / sync is in progress
 */
void vkuBuffer::destroyStagingBufferMemory() {
  if (!vDevice || !*vDevice || vStagingMemory == VK_NULL_HANDLE)
    return;

  if (vStagingBuffer != VK_NULL_HANDLE)
    vkDestroyBuffer(**vDevice, vStagingBuffer, nullptr);

  if (vStagingMemory != VK_NULL_HANDLE)
    vkFreeMemory(**vDevice, vStagingMemory, nullptr);

  vStagingBuffer = VK_NULL_HANDLE;
  vStagingMemory = VK_NULL_HANDLE;
}

/*!
 * \brief Returns a handle for writing into the device memory
 *
 * The void pointer stored in the returned object can be used to write to the vulkan memory.
 *
 * \note Check with requiresInternalCopying() whether you need to finalize the write process with cmdSync() / sync()
 */
vkuBuffer::MemoryAccess vkuBuffer::getBufferAccess() {
  if (!vDevice || !*vDevice)
    return MemoryAccess(nullptr, VK_NULL_HANDLE, 0);

  if (vMainMemoryIndex == vStagingMemoryIndex)
    return MemoryAccess(vDevice, vMainMemory, vSize);

  if (vStagingMemory == VK_NULL_HANDLE) {
    eLOG(L"Staging buffer was destroyed");
    return MemoryAccess(nullptr, VK_NULL_HANDLE, 0);
  }

  return MemoryAccess(vDevice, vStagingMemory, vSize);
}

void vkuBuffer::cmdSync(vkuCommandBuffer &_buff) {
  if (vMainMemoryIndex == vStagingMemoryIndex)
    return;

  if (vStagingBuffer == VK_NULL_HANDLE) {
    eLOG(L"Staging buffer was destroyed");
    return;
  }

  VkBufferCopy lRegion = {};
  lRegion.srcOffset    = 0;
  lRegion.dstOffset    = 0;
  lRegion.size         = vSize;

  vkCmdCopyBuffer(*_buff, vStagingBuffer, vMainBuffer, 1, &lRegion);
}

/*!
 * \brief Synchronizes the internal staging with the main buffer
 *
 * \note Does nothing if the staging buffer is not required (== the main buffer is host visible)
 */
VkResult vkuBuffer::sync() {
  if (vMainMemoryIndex == vStagingMemoryIndex)
    return VK_SUCCESS;

  if (!vDevice || !*vDevice)
    return VK_ERROR_DEVICE_LOST;

  VkResult lRes = VK_SUCCESS;

  uint32_t         lQueueFamily = 0;
  VkQueue          lQueue       = vDevice->getQueue(VK_QUEUE_TRANSFER_BIT, 0.25, &lQueueFamily);
  vkuCommandPool * lPool        = vkuCommandPoolManager::get(**vDevice, lQueueFamily);
  vkuCommandBuffer lBuff        = lPool->getBuffer();
  vkuFence_t       lFence(**vDevice);

  lBuff.begin();
  cmdSync(lBuff);
  lRes = lBuff.end();

  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to record command buffer: ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  VkSubmitInfo lSubmit;
  lSubmit.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  lSubmit.pNext                = nullptr;
  lSubmit.waitSemaphoreCount   = 0;
  lSubmit.pWaitSemaphores      = nullptr;
  lSubmit.pWaitDstStageMask    = nullptr;
  lSubmit.commandBufferCount   = 1;
  lSubmit.pCommandBuffers      = &lBuff.get();
  lSubmit.signalSemaphoreCount = 0;
  lSubmit.pSignalSemaphores    = nullptr;

  vkQueueSubmit(lQueue, 1, &lSubmit, lFence[0]);
  lFence();

  if (cfg.deleteStagingBufferAfterUse)
    destroyStagingBufferMemory();

  return lRes;
}
