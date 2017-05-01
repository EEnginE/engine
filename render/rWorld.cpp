/*!
 * \file rWorld.cpp
 * \brief \b Classes: \a rWorld
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

#include "rWorld.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"
#include "rScene.hpp"

#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

#define DEBUG_DISABLE_MULTIRENDER 0

namespace e_engine {

void rWorld::handleResize(iEventInfo const &) {
  bool lWasRunning = vRenderLoop.getIsRunning();

  init(); // Will stop the render loop

  if (lWasRunning) vRenderLoop.start();
}

/*!
 * \brief Constructor
 * \note The pointer _init must be valid over the lifetime of the object!
 */
rWorld::rWorld(iInit *_init) : vInitPtr(_init), vRenderLoop(_init, this), vResizeSlot(&rWorld::handleResize, this) {
  vDevice_vk  = vInitPtr->getDevice();
  vSurface_vk = vInitPtr->getVulkanSurface();

  vViewPort.vNeedUpdate_B = false;
  vViewPort.x             = 0;
  vViewPort.y             = 0;
  vViewPort.width         = 0;
  vViewPort.height        = 0;

  vClearColor.vNeedUpdate_B = false;
  vClearColor.r             = 0;
  vClearColor.g             = 0;
  vClearColor.b             = 0;
  vClearColor.a             = 1;
}

rWorld::~rWorld() { shutdown(); }


/*!
 * \brief initializes the swapchain and the depth and stencil buffer
 *
 * Frees old objects (if they exist) and then recreates them with the new settings.
 * Call this function manually if you want to update a global setting like multi sampling.
 *
 * \returns 0 on success
 * \note this function will be automatically called every time the window is resized
 */
int rWorld::init() {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  if (vRenderLoop.getIsRunning()) { vRenderLoop.stop(); }
  vRenderLoop.destroy();

  dVkLOG("  -- destroying old swapchain image views");
  for (auto &i : vSwapchainViews_vk) vkDestroyImageView(vDevice_vk, i, nullptr);

  dVkLOG("  -- destroying old swapchain");
  if (vSwapchain_vk) vkDestroySwapchainKHR(vDevice_vk, vSwapchain_vk, nullptr);

  vSwapchainViews_vk.clear();

  uint32_t        lQueueFamily;
  VkQueue         lQueue = vInitPtr->getQueue(VK_QUEUE_TRANSFER_BIT, 0.0, &lQueueFamily);
  VkCommandPool   lPool  = getCommandPool(lQueueFamily);
  VkCommandBuffer lBuf   = createCommandBuffer(lPool);
  VkFence         lFence = createFence();

  if (!lBuf) return -1;

  if (beginCommandBuffer(lBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT)) return -2;

  if (vInitPtr->handleResize()) return -3;
  vSurface_vk = vInitPtr->getVulkanSurface();

  if (recreateSwapchain()) return 1;

  if (recreateSwapchainImages(lBuf)) return 2;

  vkEndCommandBuffer(lBuf);

  VkSubmitInfo lInfo;
  lInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  lInfo.pNext                = nullptr;
  lInfo.waitSemaphoreCount   = 0;
  lInfo.pWaitSemaphores      = nullptr;
  lInfo.pWaitDstStageMask    = nullptr;
  lInfo.commandBufferCount   = 1;
  lInfo.pCommandBuffers      = &lBuf;
  lInfo.signalSemaphoreCount = 0;
  lInfo.pSignalSemaphores    = nullptr;

  {
    std::lock_guard<std::mutex> lGuard3(vInitPtr->getQueueMutex(lQueue));
    vkQueueSubmit(lQueue, 1, &lInfo, lFence);
  }

  auto lRes = vkWaitForFences(vDevice_vk, 1, &lFence, VK_TRUE, UINT64_MAX);
  if (lRes) {
    eLOG("'vkWaitForFences' returned ", uEnum2Str::toStr(lRes));
    return 3;
  }

  vkDestroyFence(vDevice_vk, lFence, nullptr);
  vkFreeCommandBuffers(vDevice_vk, lPool, 1, &lBuf);

  vRenderLoop.init();

  if (!vIsResizeSlotSetup) vInitPtr->addResizeSlot(&vResizeSlot);

  vIsResizeSlotSetup = true;
  vIsSetup           = true;
  return 0;
}

/*!
 * \brief Stops all render loops
 */
void rWorld::shutdown() {
  if (!vIsSetup) return;

  if (vRenderLoop.getIsRunning()) vRenderLoop.stop();


  dVkLOG("Vulkan cleanup [rWorld]:");
  vRenderLoop.destroy();

  dVkLOG("  -- Destroying command pools...");

  std::lock_guard<std::mutex> lLock(vCommandPoolsMutex);
  vkDeviceWaitIdle(vInitPtr->getDevice());
  for (auto &i : vCmdPools_vk) {
    dVkLOG("    -- tID: ", i.first.tID, "; queue family: ", i.first.qfIndex);
    if (i.second == nullptr) {
      wLOG("Command pool already destroyed");
      continue;
    }

    vkDestroyCommandPool(vInitPtr->getDevice(), i.second, nullptr);
  }

  dVkLOG("  -- destroying swapchain image views");
  for (auto &i : vSwapchainViews_vk) vkDestroyImageView(vDevice_vk, i, nullptr);

  dVkLOG("  -- destroying swapchain");
  if (vSwapchain_vk) vkDestroySwapchainKHR(vDevice_vk, vSwapchain_vk, nullptr);

  vIsSetup = false;
}

/*!
 * \brief Waits until a frame is rendered or a timeout occurs
 *
 * The timeout is 500ms
 *
 * \returns false on timeout
 */
bool rWorld::waitForFrame(std::mutex &_mutex) {
  std::unique_lock<std::mutex> lLock(_mutex);
  if (vRenderedFrameSignal.wait_for(lLock, std::chrono::milliseconds(500)) == std::cv_status::timeout)
    return false;
  else
    return true;
}


/*!
 * \brief Changes a Vulkan image layout
 *
 * \param _cmdBuffer The command buffer to record to
 * \param _img       The image to change
 * \param _imgSubres Subresource information (also required for crating an image)
 * \param _src       The current layout
 * \param _dst       The destination layout
 * \param _srcFlags  Pipeline src flags (for vkCmdPipelineBarrier)
 * \param _dstFlags  Pipeline dst flags (for vkCmdPipelineBarrier)
 *
 * \vkIntern
 */
void rWorld::cmdChangeImageLayout(VkCommandBuffer         _cmdBuffer,
                                  VkImage                 _img,
                                  VkImageSubresourceRange _imgSubres,
                                  VkImageLayout           _src,
                                  VkImageLayout           _dst,
                                  VkPipelineStageFlags    _srcFlags,
                                  VkPipelineStageFlags    _dstFlags) {
  VkImageMemoryBarrier lBarriar;
  lBarriar.sType               = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
  lBarriar.pNext               = nullptr;
  lBarriar.oldLayout           = _src;
  lBarriar.newLayout           = _dst;
  lBarriar.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  lBarriar.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
  lBarriar.image               = _img;
  lBarriar.subresourceRange    = _imgSubres;

  switch (_src) {
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      lBarriar.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: lBarriar.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      lBarriar.srcAccessMask =
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
      lBarriar.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: lBarriar.srcAccessMask = VK_ACCESS_SHADER_READ_BIT; break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      lBarriar.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      lBarriar.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR: lBarriar.srcAccessMask = VK_ACCESS_MEMORY_READ_BIT; break;
    default: lBarriar.srcAccessMask                              = 0; break;
  }

  switch (_dst) {
    case VK_IMAGE_LAYOUT_PREINITIALIZED:
      lBarriar.dstAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL: lBarriar.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
      lBarriar.dstAccessMask =
          VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
      lBarriar.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
      break;
    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL: lBarriar.dstAccessMask = VK_ACCESS_SHADER_READ_BIT; break;
    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
      lBarriar.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
      lBarriar.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
      break;
    default: lBarriar.dstAccessMask = 0; break;
  }

  vkCmdPipelineBarrier(_cmdBuffer, _srcFlags, _dstFlags, 0, 0, nullptr, 0, nullptr, 1, &lBarriar);
}


/*!
 * \brief Get a command pool
 * \vkIntern
 *
 * This function selects (or generates if needed) a command pool for the current thread (commad
 * pools are not thread safe ==> one command pool for every thread).
 *
 * \param _queueFamilyIndex The queue family index
 * \param __flags           command pool flags
 * \returns a command buffer (or nullptr)
 */
VkCommandPool rWorld::getCommandPool(uint32_t _queueFamilyIndex, VkCommandPoolCreateFlags _flags) {
  PoolInfo lTemp;
  lTemp.tID     = std::this_thread::get_id();
  lTemp.qfIndex = _queueFamilyIndex;
  lTemp.flags   = _flags;

  std::lock_guard<std::mutex> lLock(vCommandPoolsMutex);

  if (vCmdPools_vk.count(lTemp) > 0) return vCmdPools_vk[lTemp];

  // Command pool does not exists
  dVkLOG("Creating command pool for thread ", lTemp.tID, ", queue family ", lTemp.qfIndex);

  VkCommandPool           lPool;
  VkCommandPoolCreateInfo lInfo;
  lInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  lInfo.pNext            = nullptr;
  lInfo.flags            = _flags;
  lInfo.queueFamilyIndex = _queueFamilyIndex;

  auto lRes = vkCreateCommandPool(vInitPtr->getDevice(), &lInfo, nullptr, &lPool);
  if (lRes) { eLOG("vkCreateCommandPool returned ", uEnum2Str::toStr(lRes)); }

  vCmdPools_vk[lTemp] = lPool;

  return lPool;
}

/*!
 * \brief Creates a vulkan command buffer
 * \vkIntern
 *
 * \param _pool  The command pool to use
 * \param _level The command buffer level (primary/secondary)
 *
 * \returns the command buffer or nullptr on error
 */
VkCommandBuffer rWorld::createCommandBuffer(VkCommandPool _pool, VkCommandBufferLevel _level) {
  VkCommandBuffer             lBuf;
  VkCommandBufferAllocateInfo lInfo;
  lInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  lInfo.pNext              = nullptr;
  lInfo.commandPool        = _pool;
  lInfo.level              = _level;
  lInfo.commandBufferCount = 1;

  auto lRes = vkAllocateCommandBuffers(vDevice_vk, &lInfo, &lBuf);
  if (lRes) {
    eLOG("'vkAllocateCommandBuffers' returned ", uEnum2Str::toStr(lRes));
    return nullptr;
  }

  return lBuf;
}

/*!
 * \brief Starts recording a command buffer
 * \vkIntern
 *
 * \param _buf   The command buffer to start recording
 * \param _flags Vulkan flags to specify the command buffer usage
 * \param _info  Inheritance info for secondary command buffers
 *
 * \returns the result of vkBeginCommandBuffer
 */
VkResult rWorld::beginCommandBuffer(VkCommandBuffer                 _buf,
                                    VkCommandBufferUsageFlags       _flags,
                                    VkCommandBufferInheritanceInfo *_info) {
  VkCommandBufferBeginInfo lBegin;
  lBegin.sType            = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  lBegin.pNext            = nullptr;
  lBegin.flags            = _flags;
  lBegin.pInheritanceInfo = _info;

  VkResult lRes = vkBeginCommandBuffer(_buf, &lBegin);
  if (lRes) { eLOG("'vkBeginCommandBuffer' returned ", uEnum2Str::toStr(lRes)); }

  return lRes;
}

/*!
 * \brief Get a command pool
 * \vkIntern
 *
 * This function selects (or generates if needed) a command pool for the current thread (commad
 * pools are not thread safe ==> one command pool for every thread).
 *
 * \param _qFlags Flags the queue family MUST support
 * \param __flags command pool flags
 * \returns a command buffer (or nullptr)
 *
 * \note Wrapper for rWorld::getCommandPool
 */
VkCommandPool rWorld::getCommandPoolFlags(VkQueueFlags _qFlags, VkCommandPoolCreateFlags _flags) {
  uint32_t lFamilyIndex = vInitPtr->getQueueFamily(_qFlags);
  if (lFamilyIndex == UINT32_MAX) { return nullptr; }

  return getCommandPool(lFamilyIndex, _flags);
}

/*!
 * \brief Creates a Vulkan fence
 * \vkIntern
 *
 * \param _flags Some Vulkan flags (default value should be fine in most cases)
 *
 * \returns The fence or nullptr on error
 */
VkFence rWorld::createFence(VkFenceCreateFlags _flags) {
  VkFence lFence;

  VkFenceCreateInfo lInfo;
  lInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  lInfo.pNext = nullptr;
  lInfo.flags = _flags;

  auto lRes = vkCreateFence(vDevice_vk, &lInfo, nullptr, &lFence);
  if (lRes) {
    eLOG("'vkCreateFence' returned ", uEnum2Str::toStr(lRes));
    return nullptr;
  }

  return lFence;
}

/*!
 * \brief Creates a Vulkan semaphore
 * \vkIntern
 *
 * \returns The semaphore or nullptr on error
 */
VkSemaphore rWorld::createSemaphore() {
  VkSemaphore lTemp;

  VkSemaphoreCreateInfo lInfo = {};
  lInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  lInfo.pNext                 = nullptr;
  lInfo.flags                 = 0;

  auto lRes = vkCreateSemaphore(vDevice_vk, &lInfo, nullptr, &lTemp);
  if (lRes) {
    eLOG("'vkCreateSemaphore' returned ", uEnum2Str::toStr(lRes));
    return nullptr;
  }

  return lTemp;
}


VkSwapchainKHR     rWorld::getSwapchain() { return vSwapchain_vk; }
VkSurfaceFormatKHR rWorld::getSwapchainFormat() { return vSwapchainFormat; }

std::vector<rWorld::SwapChainImg> rWorld::getSwapchainImageViews() {
  std::vector<SwapChainImg> lTemp;

  for (uint32_t i = 0; i < vSwapchainViews_vk.size(); i++) {
    lTemp.emplace_back();
    lTemp.back() = {vSwapchainImages_vk[i], vSwapchainViews_vk[i]};
  }

  return lTemp;
}

void rWorld::updateViewPort(int _x, int _y, int _width, int _height) {
  vViewPort.vNeedUpdate_B = true;
  vViewPort.x             = _x;
  vViewPort.y             = _y;
  vViewPort.width         = _width;
  vViewPort.height        = _height;
}

/*!
 * \brief Updates the clear color
 * \note This function will only take effect once the render loop is restarted!
 */
void rWorld::updateClearColor(float _r, float _g, float _b, float _a) {
  VkClearColorValue lClear = {{_r, _g, _b, _a}};
  vRenderLoop.updateGlobalClearColor(lClear);
}

/*!
 * \returns A pointer to the integer counting the number of rendered frames
 */
uint64_t *rWorld::getRenderedFramesPtr() { return vRenderLoop.getRenderedFramesPtr(); }

/*!
 * \returns The nuber of framebuffers
 */
uint32_t rWorld::getNumFramebuffers() const { return vSwapchainImages_vk.size(); }

/*!
 * \returns The used vulkan device handle
 * \vkIntern
 */
VkDevice rWorld::getDevice() { return vDevice_vk; }

/*!
 * \returns the internaly used iInit pointer
 */
iInit *rWorld::getInitPtr() { return vInitPtr; }

/*!
 * \returns a reverence to the render loop
 */
rRenderLoop *rWorld::getRenderLoop() { return &vRenderLoop; }
}
