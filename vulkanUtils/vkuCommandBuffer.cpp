/*
 * Copyright (C) 2017 EEnginE project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this fFile except in compliance with the License.
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
#include "vkuCommandBuffer.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuCommandPool.hpp"

using namespace e_engine;

vkuCommandBuffer::vkuCommandBuffer(vkuCommandPool *_pool, VkCommandBufferLevel _level) { init(_pool, _level); }
vkuCommandBuffer::~vkuCommandBuffer() { destroy(); }

vkuCommandBuffer::vkuCommandBuffer(vkuCommandBuffer &&_old) {
  vBuffer = _old.vBuffer;
  vDevice = _old.vDevice;
  vPool   = _old.vPool;

  _old.vBuffer = VK_NULL_HANDLE;
  _old.vDevice = VK_NULL_HANDLE;
  _old.vPool   = nullptr;
}

/*!
 * \brief Move assignment constructor
 * \param _old old Fence
 */
vkuCommandBuffer &vkuCommandBuffer::operator=(vkuCommandBuffer &&_old) {
  vBuffer = _old.vBuffer;
  vDevice = _old.vDevice;
  vPool   = _old.vPool;

  _old.vBuffer = VK_NULL_HANDLE;
  _old.vDevice = VK_NULL_HANDLE;
  _old.vPool   = nullptr;

  return *this;
}

VkResult vkuCommandBuffer::init(vkuCommandPool *_pool, VkCommandBufferLevel _level) {
  vPool = _pool;
  if (!vPool) {
    eLOG("_pool MUST NOT be nullptr!");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (!*vPool) {
    eLOG("Command pool not initialized.");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  vDevice = _pool->getDevice();

  VkCommandBufferAllocateInfo lInfo;
  lInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  lInfo.pNext              = nullptr;
  lInfo.commandPool        = vPool->get();
  lInfo.level              = _level;
  lInfo.commandBufferCount = 1;

  auto lRes = vkAllocateCommandBuffers(vDevice, &lInfo, &vBuffer);
  if (lRes) {
    eLOG("'vkAllocateCommandBuffers' returned ", uEnum2Str::toStr(lRes));
    vBuffer = VK_NULL_HANDLE;
    vDevice = VK_NULL_HANDLE;
    vPool   = nullptr;
  }

  return lRes;
}

void vkuCommandBuffer::destroy() {
  if (vDevice && vPool) {
    vkFreeCommandBuffers(vDevice, vPool->get(), 1, &vBuffer);
  }

  vBuffer = VK_NULL_HANDLE;
  vDevice = VK_NULL_HANDLE;
  vPool   = nullptr;
}


/*!
 * \brief Starts recording a command buffer
 *
 * \param _flags Vulkan flags to specify the command buffer usage
 * \param _info  Inheritance info for secondary command buffers
 *
 * \returns the result of vkBeginCommandBuffer
 */
VkResult vkuCommandBuffer::begin(VkCommandBufferUsageFlags _flags, VkCommandBufferInheritanceInfo *_info) {
  VkCommandBufferBeginInfo lBegin;
  lBegin.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  lBegin.pNext            = nullptr;
  lBegin.flags            = _flags;
  lBegin.pInheritanceInfo = _info;

  VkResult lRes = vkBeginCommandBuffer(vBuffer, &lBegin);
  if (lRes) {
    eLOG("'vkBeginCommandBuffer' returned ", uEnum2Str::toStr(lRes));
  }

  return lRes;
}

/*!
 * \brief Ends recording a command buffer
 *
 * must be called after begin() or vkBeginCommandBuffer
 */
VkResult vkuCommandBuffer::end() { return vkEndCommandBuffer(vBuffer); }
