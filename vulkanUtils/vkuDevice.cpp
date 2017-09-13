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
#include "vkuDevice.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"

using namespace e_engine;

#if D_LOG_VULKAN_UTILS
#define dVkLOG(...) dLOG(__VA_ARGS__)
#else
#define dVkLOG(...)
#endif

vkuDevice::vkuDevice(VkPhysicalDevice         _device,
                     std::vector<std::string> _layers,
                     std::vector<std::string> _extensions,
                     VkSurfaceKHR             _surface) {
  uint32_t lCount;

  vPhysicalDevice = _device;
  vLayers         = _layers;
  vExtensions     = _extensions;
  vSurface        = _surface;

  vkGetPhysicalDeviceProperties(vPhysicalDevice, &vProperties);
  vkGetPhysicalDeviceFeatures(vPhysicalDevice, &vFeatures);
  vkGetPhysicalDeviceMemoryProperties(vPhysicalDevice, &vMemoryProperties);

  vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &lCount, nullptr);
  vQueueFamilyProperties.resize(lCount);
  vkGetPhysicalDeviceQueueFamilyProperties(vPhysicalDevice, &lCount, vQueueFamilyProperties.data());

  for (uint32_t i = 0; i <= VK_FORMAT_END_RANGE; i++)
    vkGetPhysicalDeviceFormatProperties(vPhysicalDevice, static_cast<VkFormat>(i), &vFormats[i]);

  auto lRes = loadDeviceSurfaceInfo();
  if (lRes) {
    eLOG("'loadDeviceSurfaceInfo' returned ", uEnum2Str::toStr(lRes));
  }

  std::vector<VkDeviceQueueCreateInfo> lQueueCreateInfo;
  std::vector<float>                   lQueuePriorities;
  float                                lPriority;
  uint32_t                             lFamilyIndex;

  for (auto const &i : vQueueFamilyProperties) {
    lQueuePriorities.clear();
    lQueuePriorities.resize(i.queueCount);

    lFamilyIndex = static_cast<uint32_t>(lQueueCreateInfo.size());

    VkBool32 lSupported;
    lRes = vkGetPhysicalDeviceSurfaceSupportKHR(vPhysicalDevice, lFamilyIndex, _surface, &lSupported);

    if (lRes) {
      eLOG("'vkGetPhysicalDeviceSurfaceSupportKHR' returned ", uEnum2Str::toStr(lRes));
      lSupported = VK_FALSE;
    }

    // Setting priorities (1.0, 0.5, 0.25, ...)
    for (uint32_t j = 0; j < i.queueCount; j++) {
      lPriority           = 1.0f / static_cast<float>(j + 1);
      lQueuePriorities[j] = lPriority;

      // Also create the queue object (only setting priority, and indexes)
      // The actual queue will be stored after creating the device (==> and the queues),
      // using this data.
      vQueues.emplace_back(lPriority,                                       // priority
                           vQueueFamilyProperties[lFamilyIndex].queueFlags, // flags
                           lFamilyIndex,                                    // familyIndex
                           j,                                               // index
                           lSupported == VK_TRUE ? true : false             // surfaceSupport
                           );
    }

    lQueueCreateInfo.push_back({
        VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO, // sType
        nullptr,                                    // pNext
        0,                                          // flags
        lFamilyIndex,                               // queueFamilyIndex
        i.queueCount,                               // queueCount
        lQueuePriorities.data()                     // pQueuePriorities
    });
  }

  // Create C type string arrays for layers and extensions
  const char **lLayers     = new const char *[vLayers.size()];
  const char **lExtensions = new const char *[vExtensions.size()];
  for (uint32_t i = 0; i < vLayers.size(); i++) {
    lLayers[i] = vLayers[i].c_str();
  }

  for (uint32_t i = 0; i < vExtensions.size(); i++) {
    lExtensions[i] = vExtensions[i].c_str();
  }

  VkDeviceCreateInfo lCreateInfo;
  lCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  lCreateInfo.pNext                   = nullptr;
  lCreateInfo.flags                   = 0;
  lCreateInfo.queueCreateInfoCount    = static_cast<uint32_t>(lQueueCreateInfo.size());
  lCreateInfo.pQueueCreateInfos       = lQueueCreateInfo.data();
  lCreateInfo.enabledLayerCount       = static_cast<uint32_t>(vLayers.size());
  lCreateInfo.ppEnabledLayerNames     = lLayers;
  lCreateInfo.enabledExtensionCount   = static_cast<uint32_t>(vExtensions.size());
  lCreateInfo.ppEnabledExtensionNames = lExtensions;
  lCreateInfo.pEnabledFeatures        = &vFeatures;

  iLOG(L"Creating vulkan device on GPU '", vProperties.deviceName, L"'; id = ", vProperties.deviceID);
#if D_LOG_VULKAN_UTILS
  dLOG(L"  -- Device Layers:");
  for (auto &i : vLayers) {
    dLOG(L"    - ", i);
  }
  dLOG(L"  -- Device extensions:");
  for (auto &i : vExtensions) {
    dLOG(L"    - ", i);
  }
#endif

  lRes = vkCreateDevice(vPhysicalDevice, &lCreateInfo, nullptr, &vDevice);

  delete[] lLayers;
  delete[] lExtensions;

  if (lRes != VK_SUCCESS) {
    eLOG("'vkCreateDevice' returned ", uEnum2Str::toStr(lRes));
    vDevice = VK_NULL_HANDLE;
    return;
  }

  dVkLOG(L"  -- Created Queues:");
  for (auto &i : vQueues) {
    vkGetDeviceQueue(vDevice, i.familyIndex, i.index, &i.queue);
    dVkLOG(L"    - family: ", i.familyIndex, L"; index: ", i.index, L"; priority: ", i.priority);
  }
}

vkuDevice::~vkuDevice() {
  if (vDevice != VK_NULL_HANDLE) {
    vkDestroyDevice(vDevice, nullptr);
  }
}

VkResult vkuDevice::loadDeviceSurfaceInfo() {
  uint32_t lCount;

  auto lRes = vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &lCount, nullptr);
  if (lRes) {
    eLOG("'vkGetPhysicalDeviceSurfaceFormatsKHR' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  vSurfaceInfo.formats.resize(lCount);
  lRes = vkGetPhysicalDeviceSurfaceFormatsKHR(vPhysicalDevice, vSurface, &lCount, vSurfaceInfo.formats.data());
  if (lRes) {
    eLOG("'vkGetPhysicalDeviceSurfaceFormatsKHR' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  lRes = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vPhysicalDevice, vSurface, &vSurfaceInfo.surfaceInfo);
  if (lRes) {
    eLOG("'vkGetPhysicalDeviceSurfaceCapabilitiesKHR' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  lRes = vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &lCount, nullptr);
  if (lRes) {
    eLOG("'vkGetPhysicalDeviceSurfacePresentModesKHR' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  vSurfaceInfo.presentModels.resize(lCount);
  lRes =
      vkGetPhysicalDeviceSurfacePresentModesKHR(vPhysicalDevice, vSurface, &lCount, vSurfaceInfo.presentModels.data());
  if (lRes) {
    eLOG("'vkGetPhysicalDeviceSurfacePresentModesKHR' returned ", uEnum2Str::toStr(lRes));
    return lRes;
  }

  return VK_SUCCESS;
}


/*!
 * \brief Returns the queue family index, that supports _flags
 * \param _flags Flags the Queue family MUST support
 * \returns the queue family index, that supports _flags (UINT32_MAX on not found / error)
 *
 * \vkIntern
 */
uint32_t vkuDevice::getQueueFamily(VkQueueFlags _flags) {
  if (!isCreated())
    return UINT32_MAX;

  uint32_t lCounter = 0;
  for (auto const &i : vQueueFamilyProperties) {
    if (i.queueFlags & _flags)
      return lCounter;

    lCounter++;
  }
  return UINT32_MAX;
}

/*!
 * \brief Selects and returns a vulkan queue
 *
 * Selects and returns a vulkan queue.
 *
 * \param _flags    Flags the Queue MUST support
 * \param _priority Queue target priority
 * \returns the best matching queue; nullptr if none found
 *
 * \todo Solve this with RAII
 *
 * \vkIntern
 */
VkQueue vkuDevice::getQueue(VkQueueFlags _flags, float _priority, uint32_t *_queueFamily) {
  float   lMinDiff = 100.0f;
  VkQueue lQueue   = nullptr;

  for (auto const &i : vQueues) {
    if (!(i.flags & _flags))
      continue;

    auto lTemp = i.priority - _priority;
    lTemp      = lTemp < 0 ? -lTemp : lTemp; // Make positive
    if (lTemp < lMinDiff) {
      lMinDiff = lTemp;
      lQueue   = i.queue;
      if (_queueFamily)
        *_queueFamily = i.familyIndex;
    }
  }

  return lQueue;
}


std::mutex &vkuDevice::getQueueMutex(VkQueue _queue) { return vQueueMutexMap[_queue]; }


/*!
 * \brief Checks whether a format is supported on the device
 * \vkIntern
 */
bool vkuDevice::isFormatSupported(VkFormat _format) {
  if (!isCreated())
    return false;

  return vFormats[_format].linearTilingFeatures != 0 || vFormats[_format].optimalTilingFeatures != 0 ||
         vFormats[_format].bufferFeatures != 0;
}

/*!
 * \brief Checks whether a format supports a feature
 * \param _format The format to check
 * \param _flags  The flags the format must support
 * \param _type   The feature type
 *
 * Supported values for _type:
 *   - VK_IMAGE_TILING_LINEAR
 *   - VK_IMAGE_TILING_OPTIMAL
 *   - VK_IMAGE_TILING_MAX_ENUM == buffer
 *
 * \vkIntern
 */
bool vkuDevice::formatSupportsFeature(VkFormat _format, VkFormatFeatureFlagBits _flags, VkImageTiling _type) {
  if (!isCreated())
    return false;

  switch (_type) {
    case VK_IMAGE_TILING_LINEAR: return (vFormats[_format].linearTilingFeatures & _flags) != 0;
    case VK_IMAGE_TILING_OPTIMAL: return (vFormats[_format].optimalTilingFeatures & _flags) != 0;
    case VK_IMAGE_TILING_MAX_ENUM: return (vFormats[_format].bufferFeatures & _flags) != 0;
    default: return false;
  }
}

/*!
 * \brief returns the best supported memory index
 * \param _bits  The bitfield (see vkGetImageMemoryRequirements)
 * \param _flags Flags the memory type must support
 * \returns a memory index or UINT32_MAX on error
 * \vkIntern
 */
uint32_t vkuDevice::getMemoryTypeIndexFromBitfield(uint32_t _bits, VkMemoryHeapFlags _flags) {
  for (uint16_t i = 0; i < vMemoryProperties.memoryTypeCount; i++) {
    if ((_bits & 1)) {
      // Finds best match because of memory type ordering
      if (_flags == (vMemoryProperties.memoryTypes[i].propertyFlags & _flags)) {
        return i;
      }
    }
    _bits >>= 1;
  }

  return UINT32_MAX;
}

uint32_t vkuDevice::getMemoryTypeIndex(VkMemoryRequirements _requirements, VkMemoryHeapFlags _flags) {
  return getMemoryTypeIndexFromBitfield(_requirements.memoryTypeBits, _flags);
}
