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

#pragma once

#include "defines.hpp"
#include "vkuDevice.hpp"
#include <mutex>

namespace e_engine {

class vkuSwapChain final {
  struct Config {
    bool                    preferMailBoxPresetMode      = true;
    bool                    prefereNonTearingPresentMode = true;
    VkFormat                preferedSurfaceFormat        = VK_FORMAT_B8G8R8A8_UNORM;
    VkImageSubresourceRange subResRange                  = {
        VK_IMAGE_ASPECT_COLOR_BIT, // aspectMask
        0,                         // baseMipLevel
        1,                         // levelCount
        0,                         // baseArrayLayer
        1                          // layerCount
    };
  };

  struct SwapChainImg {
    VkImage     img;
    VkImageView iv;
  };

  class LockAndResult {
    typedef std::unique_lock<std::mutex> LOCK;

   private:
    LOCK     vLock;
    uint32_t vImg;
    VkResult vRes;

   public:
    LockAndResult() = delete;
    LockAndResult(LOCK _l, uint32_t _img) : vLock(std::move(_l)), vImg(_img), vRes(VK_SUCCESS) {}
    LockAndResult(LOCK _l, VkResult _res) : vLock(std::move(_l)), vImg(UINT32_MAX), vRes(_res) {}

    LockAndResult(LockAndResult const &) = delete;
    LockAndResult(LockAndResult &&)      = default;
    LockAndResult &operator=(const LockAndResult &) = delete;
    LockAndResult &operator=(LockAndResult &&) = default;

    inline uint32_t getNextImage() const noexcept { return vImg; }
    inline VkResult getError() const noexcept { return vRes; }
    inline uint32_t operator*() const noexcept { return vImg; }

    inline bool     operator!() const noexcept { return vRes != VK_SUCCESS; }
    inline explicit operator bool() const noexcept { return vRes == VK_SUCCESS; }
  };

 private:
  VkSwapchainKHR vSwapChain = VK_NULL_HANDLE;
  VkSurfaceKHR   vSurface   = VK_NULL_HANDLE;
  vkuDevicePTR   vDevice    = nullptr;

  std::vector<VkImage>     vSwapchainImages;
  std::vector<VkImageView> vSwapchainViews;

  VkSurfaceFormatKHR vSwapchainFormat;

  std::mutex vSwapChainCreateMutex;

  Config cfg;

 public:
  vkuSwapChain() = default;
  ~vkuSwapChain();

  vkuSwapChain(vkuSwapChain const &) = delete;
  vkuSwapChain(vkuSwapChain &&)      = delete;
  vkuSwapChain &operator=(const vkuSwapChain &) = delete;
  vkuSwapChain &operator=(vkuSwapChain &&) = delete;

  LockAndResult init(vkuDevicePTR _device, VkSurfaceKHR _surface);
  void          destroy();

  LockAndResult acquireNextImage(VkSemaphore _semaphore = VK_NULL_HANDLE,
                                 VkFence     _fence     = VK_NULL_HANDLE,
                                 uint64_t    _timeout   = UINT64_MAX);

  inline VkSwapchainKHR     get() const noexcept { return vSwapChain; }
  inline VkSurfaceFormatKHR getFormat() const noexcept { return vSwapchainFormat; }
  inline vkuDevicePTR       getDevice() const noexcept { return vDevice; }
  inline Config             getConfig() const noexcept { return cfg; }
  inline bool               isCreated() const noexcept { return vSwapChain != VK_NULL_HANDLE; }

  inline uint32_t           getNumImages() const noexcept { return static_cast<uint32_t>(vSwapchainImages.size()); }
  inline SwapChainImg       getImage(uint32_t i) const noexcept { return {vSwapchainImages[i], vSwapchainViews[i]}; }
  inline SwapChainImg       operator[](uint32_t i) const noexcept { return {vSwapchainImages[i], vSwapchainViews[i]}; }
  std::vector<SwapChainImg> getImages() const noexcept;

  inline VkSwapchainKHR operator*() const noexcept { return vSwapChain; }

  inline bool     operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};
} // namespace e_engine
