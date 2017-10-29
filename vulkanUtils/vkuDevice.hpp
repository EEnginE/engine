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
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <vector>
#include <vulkan.h>

namespace e_engine {

class vkuDevice;
typedef std::shared_ptr<vkuDevice> vkuDevicePTR;

/*!
 * \brief Wrapper for a vulkan device (physical and logical)
 */
class vkuDevice {
 public:
  struct Queue {
    VkQueue      queue;
    float        priority;
    VkQueueFlags flags;
    u_int32_t    familyIndex;
    u_int32_t    index;
    bool         surfaceSupport;

    Queue(float _priority, VkQueueFlags _flags, u_int32_t _familyIndex, u_int32_t _index, bool _surfaceSupport)
        : priority(_priority),
          flags(_flags),
          familyIndex(_familyIndex),
          index(_index),
          surfaceSupport(_surfaceSupport) {}
  };

  struct SurfaceInfo {
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR>   presentModels;
    VkSurfaceCapabilitiesKHR        surfaceInfo;
  };

 private:
  VkDevice         vDevice         = VK_NULL_HANDLE;
  VkPhysicalDevice vPhysicalDevice = VK_NULL_HANDLE;

  std::vector<Queue> vQueues;

  VkPhysicalDeviceMemoryProperties     vMemoryProperties;
  std::vector<VkQueueFamilyProperties> vQueueFamilyProperties;
  VkPhysicalDeviceProperties           vProperties;
  VkFormatProperties                   vFormats[VK_FORMAT_RANGE_SIZE];
  VkPhysicalDeviceFeatures             vFeatures;

  std::vector<std::string>                vLayers;
  std::vector<std::string>                vExtensions;
  std::unordered_map<VkQueue, std::mutex> vQueueMutexMap;

 public:
  vkuDevice() = delete;
  vkuDevice(VkPhysicalDevice         _device,
            std::vector<std::string> _layers,
            std::vector<std::string> _extensions,
            VkSurfaceKHR             _surface);
  ~vkuDevice();

  vkuDevice(vkuDevice const &) = delete;
  vkuDevice(vkuDevice &&)      = delete;

  vkuDevice &operator=(const vkuDevice &) = delete;
  vkuDevice &operator=(vkuDevice &&) = delete;

  uint32_t getQueueFamily(VkQueueFlags _flags);

  VkQueue getQueue(VkQueueFlags _flags, float _priority, uint32_t *_queueFamily = nullptr, bool _presentSupport = false);
  std::mutex &getQueueMutex(VkQueue _queue);

  uint32_t getMemoryTypeIndexFromBitfield(uint32_t _bits, VkMemoryPropertyFlags _flags = 0);
  uint32_t getMemoryTypeIndex(VkMemoryRequirements _requirements, VkMemoryPropertyFlags _flags = 0);

  bool isFormatSupported(VkFormat _format);
  bool formatSupportsFeature(VkFormat _format, VkFormatFeatureFlagBits _flags, VkImageTiling _type);

  void getDepthFormat(VkFormat &_format, VkImageTiling &_tiling, VkImageAspectFlags &_aspect);

  SurfaceInfo getSurfaceInfo(VkSurfaceKHR _surface);

  inline VkDevice get() const noexcept { return vDevice; }

  inline bool isCreated() const noexcept { return vDevice != VK_NULL_HANDLE; }

  inline VkDevice operator*() const noexcept { return vDevice; }

  inline bool     operator!() const noexcept { return !isCreated(); }
  inline explicit operator bool() const noexcept { return isCreated(); }
};
} // namespace e_engine
