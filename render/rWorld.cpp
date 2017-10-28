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
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"
#include "iInit.hpp"
#include "rScene.hpp"

#if D_LOG_VULKAN
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

namespace e_engine {

void rWorld::handleResize(iEventInfo const &) {
  init(); // Will reset the swapchain and update the renderers
}

/*!
 * \brief Constructor
 * \note The pointer _init must be valid over the lifetime of the object!
 */
rWorld::rWorld(iInit *_init)
    : vInitPtr(_init),
      vDevice(_init->getDevicePTR()),
      vDevice_vk(**vDevice),
      vRenderLoop(this),
      vResizeSlot(&rWorld::handleResize, this) {
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
  if (!vInitPtr->getIsSetup())
    return -100;

  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();
  iLOG(L"(Re)initializing renderers");

  destroyRenderers();

  vSurface_vk = vInitPtr->getVulkanSurface();

  if (!vSwapChain.init(vDevice, vSurface_vk))
    return 1;

  initRenderers();
  rebuildRenderLoopCommandBuffers();

  if (!vIsResizeSlotSetup)
    vInitPtr->addResizeSlot(&vResizeSlot);

  vIsResizeSlotSetup = true;
  vIsSetup           = true;
  iLOG(L"Done (re)initializing renderers");
  return 0;
}

/*!
 * \brief Stops all render loops
 */
void rWorld::shutdown() {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  if (!vIsSetup || !vInitPtr->getIsSetup())
    return;

  if (vRenderLoop.getIsRunning())
    vRenderLoop.stop();

  dVkLOG("Vulkan cleanup [rWorld]:");
  destroyRenderers();
  vSwapChain.destroy();

  vIsSetup = false;
}

/*!
 * \brief Recrecords command buffers and writes them to the renderloop
 */
void rWorld::rebuildRenderers() {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();
  rebuildRenderLoopCommandBuffers();
}


/*!
 * \brief Non thread safe private implementation of rebuildRenderers
 * \note Requires external synchronisation with the Render Loop Lock
 */
void rWorld::rebuildRenderLoopCommandBuffers() {
  auto *lBufferRef = vRenderLoop.getCommandBufferReferences();

  for (auto const &i : vRenderers) {
    if (!i->getIsInit()) {
      continue;
    }

    i->updateRenderer();
  }

  lBufferRef->frames.resize(vSwapChain.getNumImages());

  for (uint32_t i = 0; i < vSwapChain.getNumImages(); ++i) {
    lBufferRef->frames[i].pre.clear();
    lBufferRef->frames[i].render.clear();
    lBufferRef->frames[i].post.clear();

    for (auto &j : vRenderers) {
      if (!j->getIsInit()) {
        continue;
      }

      if (!j->getIsRenderingEnabled())
        continue;

      auto lBuff = j->getCommandBuffers(i);

      lBufferRef->frames[i].pre.emplace_back(*lBuff.pre);
      lBufferRef->frames[i].render.emplace_back(*lBuff.render);
      lBufferRef->frames[i].post.emplace_back(*lBuff.post);
    }
  }
}


/**
 * \brief Initilizes all renderes (if neccessary)
 * \returns The sum of all error codes
 * \note Requires external synchronisation with the Render Loop Lock
 */
int rWorld::initRenderers() {
  vkuCommandPool *lPool     = vkuCommandPoolManager::get(vDevice_vk, vRenderLoop.getQueueFamilyIndex());
  int             errorCode = 0;
  for (auto const &i : vRenderers)
    if (!i->getIsInit())
      errorCode += i->init(lPool);

  rebuildRenderLoopCommandBuffers();

  return errorCode;
}

/**
 * \brief Destoyes all renderes (if neccessary)
 * \note Requires external synchronisation with the Render Loop Lock
 */
void rWorld::destroyRenderers() {
  for (auto const &i : vRenderers)
    if (i->getIsInit())
      i->destroy();

  rebuildRenderLoopCommandBuffers();
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
    default: lBarriar.srcAccessMask = 0; break;
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
 * \brief Ads a renderer to the render loop
 * \note This will NOT initialize the renderer
 */
void rWorld::addRenderer(std::shared_ptr<rRendererBase> _renderer) {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();

  vRenderers.push_back(_renderer);
  rebuildRenderLoopCommandBuffers();
}

/*!
 * \brief Removes a renderer to the render loop
 */
void rWorld::removeRenderer(std::shared_ptr<rRendererBase> _renderer) {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();

  vRenderers.erase(std::remove(vRenderers.begin(), vRenderers.end(), _renderer), vRenderers.end());
  rebuildRenderLoopCommandBuffers();
}

/*!
 * \brief Removes all renderers from the render loop
 */
void rWorld::clearRenderers() {
  std::lock_guard<std::mutex> lGuard(vRenderAccessMutex);
  auto                        lRenderLoopLock = vRenderLoop.getRenderLoopLock();

  vRenderers.clear();
  rebuildRenderLoopCommandBuffers();
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

  for (auto const &i : vRenderers) {
    i->setClearColor(lClear);
  }
}

/*!
 * \returns A pointer to the integer counting the number of rendered frames
 */
uint64_t *rWorld::getRenderedFramesPtr() { return vRenderLoop.getRenderedFramesPtr(); }

/*!
 * \returns A shared pointer to the vulkan device wrapper
 * \vkIntern
 */
vkuDevicePTR rWorld::getDevice() { return vDevice; }

/*!
 * \returns A pointer to the swapchain in use
 * \vkIntern
 */
vkuSwapChain *rWorld::getSwapChain() { return &vSwapChain; }

/*!
 * \returns the internaly used iInit pointer
 */
iInit *rWorld::getInitPtr() { return vInitPtr; }

/*!
 * \returns a reverence to the render loop
 */
rRenderLoop *rWorld::getRenderLoop() { return &vRenderLoop; }
} // namespace e_engine
