/*
 * Copyright (C) 2017 EEnginE project
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

#include "defines.hpp"
#include "rTexture.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "vkuBuffer.hpp"
#include "vkuCommandPoolManager.hpp"
#include "vkuFence.hpp"
#include <gli/gli.hpp>

using namespace e_engine;

rTexture::~rTexture() { destroy(); }

rTexture::rTexture(rTexture &&_old) {
  vDevice  = _old.vDevice;
  vSampler = _old.vSampler;
  cfg      = _old.cfg;

  _old.vDevice  = nullptr;
  _old.vSampler = VK_NULL_HANDLE;

  vImg = std::move(_old.vImg);
}

rTexture &rTexture::operator=(rTexture &&_old) {
  destroy(); // destroy old texture

  vDevice  = _old.vDevice;
  vSampler = _old.vSampler;
  cfg      = _old.cfg;

  _old.vDevice  = nullptr;
  _old.vSampler = VK_NULL_HANDLE;

  vImg = std::move(_old.vImg);
  return *this;
}


/*!
 * \brief Loads a texture from a file
 *
 * \todo make this more generic and support more file types
 */
VkResult rTexture::init(std::string _filePath) {
  if (!vDevice) {
    eLOG(L"Invalid device");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  if (vImg.isCreated())
    vImg.destroy();

  // Load the image with GLI
  auto lTemp = gli::load(_filePath);
  if (lTemp.empty()) {
    eLOG(L"Failed to load texture ", _filePath);
    return VK_ERROR_FORMAT_NOT_SUPPORTED;
  }

  gli::texture2d lTexGLi(lTemp);
  uint32_t       lWidth     = static_cast<uint32_t>(lTexGLi[0].extent().x);
  uint32_t       lHeight    = static_cast<uint32_t>(lTexGLi[0].extent().y);
  uint32_t       lMipLevels = static_cast<uint32_t>(lTexGLi.levels());

  if (lTexGLi.format() != gli::FORMAT_RGBA8_UNORM_PACK8) {
    eLOG(L"Invalid texture format ", uEnum2Str::toStr(lTexGLi.format()));
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  // Copy the image into a buffer
  vkuBuffer lTempBuff(vDevice);
  lTempBuff->memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  lTempBuff->usage       = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  VkResult lRes          = lTempBuff.init(lTexGLi.size());
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to allocate buffer for texture: ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  {
    auto lBufferPTR = lTempBuff.getBufferAccess();
    if (!lBufferPTR) {
      eLOG(L"Failed to bind vulkan memory");
      return VK_ERROR_INITIALIZATION_FAILED;
    }
    memcpy(*lBufferPTR, lTexGLi.data(), lTexGLi.size());
  }
  lTempBuff.sync(); // Should do nothing because memory is host visible

  // Setup image buffer
  VkImageSubresourceRange lSubResRange;
  lSubResRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
  lSubResRange.baseMipLevel   = 0;
  lSubResRange.levelCount     = lMipLevels;
  lSubResRange.baseArrayLayer = 0;
  lSubResRange.layerCount     = 1;


  vImg->type             = VK_IMAGE_TYPE_2D;
  vImg->format           = VK_FORMAT_R8G8B8A8_UNORM;
  vImg->extent           = {lWidth, lHeight, 1};
  vImg->mipLevels        = lMipLevels;
  vImg->usage            = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
  vImg->subresourceRange = lSubResRange;

  lRes = vImg.init();
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to initialize image: ", uEnum2Str::toStr(lRes));
    return lRes;
  }


  // Copy vkuBuffer --> vkuImageBuffer
  std::vector<VkBufferImageCopy> lBufferCopyRegions;
  uint32_t                       offset = 0;

  for (uint32_t i = 0; i < lMipLevels; ++i) {
    VkBufferImageCopy lRegion               = {};
    lRegion.bufferOffset                    = offset;
    lRegion.bufferRowLength                 = 0;
    lRegion.bufferImageHeight               = 0;
    lRegion.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    lRegion.imageSubresource.mipLevel       = i;
    lRegion.imageSubresource.baseArrayLayer = 0;
    lRegion.imageSubresource.layerCount     = 1;
    lRegion.imageExtent.width               = static_cast<uint32_t>(lTexGLi[i].extent().x);
    lRegion.imageExtent.height              = static_cast<uint32_t>(lTexGLi[i].extent().y);
    lRegion.imageExtent.depth               = 1;
    lRegion.imageOffset                     = {0, 0, 0};

    lBufferCopyRegions.push_back(lRegion);

    offset += static_cast<uint32_t>(lTexGLi[i].size());
  }

  uint32_t         lQueueFamilyIndex = 0;
  VkQueue          lQueue            = vDevice->getQueue(VK_QUEUE_GRAPHICS_BIT, 0.5, &lQueueFamilyIndex);
  vkuCommandPool * lPool             = vkuCommandPoolManager::get(**vDevice, lQueueFamilyIndex);
  vkuCommandBuffer lBuff             = lPool->getBuffer();
  vkuFence_t       lFence(**vDevice);

  lBuff.begin();

  vImg.cmdChangeLayout(&lBuff, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
  vkCmdCopyBufferToImage(*lBuff,
                         *lTempBuff,
                         vImg.getImage(),
                         VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                         static_cast<uint32_t>(lBufferCopyRegions.size()),
                         lBufferCopyRegions.data());
  vImg.cmdChangeLayout(&lBuff, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

  lRes = lBuff.end();
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to record command buffer: ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  VkSubmitInfo lSubmitInfo;
  lSubmitInfo.sType                = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  lSubmitInfo.pNext                = nullptr;
  lSubmitInfo.waitSemaphoreCount   = 0;
  lSubmitInfo.pWaitSemaphores      = nullptr;
  lSubmitInfo.pWaitDstStageMask    = nullptr;
  lSubmitInfo.commandBufferCount   = 1;
  lSubmitInfo.pCommandBuffers      = &lBuff.get();
  lSubmitInfo.signalSemaphoreCount = 0;
  lSubmitInfo.pSignalSemaphores    = nullptr;

  vkQueueSubmit(lQueue, 1, &lSubmitInfo, lFence[0]);
  lFence();

  // Create the sampler
  VkSamplerCreateInfo lSamplerInfo;
  lSamplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
  lSamplerInfo.pNext                   = nullptr;
  lSamplerInfo.flags                   = 0;
  lSamplerInfo.magFilter               = VK_FILTER_LINEAR;
  lSamplerInfo.minFilter               = VK_FILTER_LINEAR;
  lSamplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
  lSamplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  lSamplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  lSamplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
  lSamplerInfo.mipLodBias              = 0.0f;
  lSamplerInfo.anisotropyEnable        = VK_FALSE;
  lSamplerInfo.maxAnisotropy           = 1.0f;
  lSamplerInfo.compareEnable           = VK_FALSE;
  lSamplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;
  lSamplerInfo.minLod                  = 0.0f;
  lSamplerInfo.maxLod                  = 0.0f; // static_cast<float>(lMipLevels);
  lSamplerInfo.borderColor             = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
  lSamplerInfo.unnormalizedCoordinates = VK_FALSE;

  lRes = vkCreateSampler(**vDevice, &lSamplerInfo, nullptr, &vSampler);
  if (lRes != VK_SUCCESS) {
    eLOG(L"Failed to create sampler: ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  return VK_SUCCESS;
}

void rTexture::destroy() {
  if (!vDevice)
    return; // Moved form

  if (vSampler)
    vkDestroySampler(**vDevice, vSampler, nullptr);

  vImg.destroy();
}
