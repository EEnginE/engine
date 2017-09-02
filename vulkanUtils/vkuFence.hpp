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
 *
 */

#pragma once

#include "defines.hpp"
#include <vulkan/vulkan.h>

namespace e_engine {

/*!
 * \brief Vulkan fence management class
 * \warning This class does no parameter validation, to reduce overhead!
 */
template <uint32_t N>
class vkuFences     final {
  static_assert(N > 0, "Min for N is 1");

 private:
  VkFence  vFence[N];
  VkDevice vDevice = VK_NULL_HANDLE;

 public:
  vkuFences();
  vkuFences(VkDevice _device);
  vkuFences(VkDevice _device, VkFenceCreateInfo _info);
  ~vkuFences();

  vkuFences(vkuFences<N> const &) = delete;
  vkuFences &operator=(const vkuFences<N> &) = delete;

  vkuFences(vkuFences<N> &&);
  vkuFences &operator=(vkuFences<N> &&);

  VkResult reset(uint32_t _start = 0, uint32_t _num = N);
  VkResult wait(uint32_t _start = 0, uint32_t _num = N, uint64_t _timeout = UINT64_MAX);
  VkResult waitThenReset(uint32_t _start = 0, uint32_t _num = N, uint64_t _timeout = UINT64_MAX);
  VkResult status(uint32_t _n = 0);

  inline VkFence &at(uint32_t _n = 0) noexcept { return vFence[_n]; }
  inline VkFence &operator[](uint32_t _n) { return vFence[_n]; }
  inline VkResult operator()(uint32_t _start = 0, uint32_t _num = N, uint64_t _timeout = UINT64_MAX) {
    return waitThenReset(_start, _num, _timeout);
  }

  bool operator!() const;
  explicit operator bool() const;
};

using vkuFence_t = vkuFences<1>;

/*!
 * \brief Default constructor
 */
template <uint32_t N>
vkuFences<N>::vkuFences() {
  for (VkFence &i : vFence) {
    i = VK_NULL_HANDLE;
  }
}

/*!
 * \brief Creates a fence
 * \param _device The device to create the fence on
 * \param _info   The fence create info
 */
template <uint32_t N>
vkuFences<N>::vkuFences(VkDevice _device, VkFenceCreateInfo _info) : vDevice(_device) {
  if (vDevice != VK_NULL_HANDLE) {
    for (VkFence &i : vFence) {
      auto lRes = vkCreateFence(vDevice, &_info, nullptr, &i);
      if (lRes != VK_SUCCESS) {
        i = VK_NULL_HANDLE;
      }
    }
  }
}

/*!
 * \brief Creates a fence with default settings
 * \param _device The device to create the fence on
 */
template <uint32_t N>
vkuFences<N>::vkuFences(VkDevice _device) : vkuFences(_device, {VK_STRUCTURE_TYPE_FENCE_CREATE_INFO, nullptr, 0}) {}

template <uint32_t N>
vkuFences<N>::~vkuFences() {
  for (VkFence &i : vFence) {
    if (i != VK_NULL_HANDLE) {
      vkDestroyFence(vDevice, i, nullptr);
    }
  }
}

/*!
 * \brief Move constructor
 * \param _old old Fence
 */
template <uint32_t N>
vkuFences<N>::vkuFences(vkuFences<N> &&_old) {
  vDevice      = _old.vDevice;
  _old.vDevice = VK_NULL_HANDLE;

  for (uint32_t i = 0; i < N; ++i) {
    vFence[i]      = _old.vFence[i];
    _old.vFence[i] = VK_NULL_HANDLE;
  }
}

/*!
 * \brief Move assignment constructor
 * \param _old old Fence
 */
template <uint32_t N>
vkuFences<N> &vkuFences<N>::operator=(vkuFences<N> &&_old) {
  vDevice      = _old.vDevice;
  _old.vDevice = VK_NULL_HANDLE;

  for (uint32_t i = 0; i < N; ++i) {
    vFence[i]      = _old.vFence[i];
    _old.vFence[i] = VK_NULL_HANDLE;
  }
  return *this;
}

/*!
 * \brief Resets the fence(s)
 * \param _start First fence
 * \param _num   Number of fences (after the start fence)
 */
template <uint32_t N>
VkResult vkuFences<N>::reset(uint32_t _start, uint32_t _num) {
  return vkResetFences(vDevice, _num, &vFence[_start]);
}

/*!
 * \brief Waits for the fence(s)
 * \param _start First fence
 * \param _num   Number of fences (after the start fence)
 * \param _timeout The timeout
 */
template <uint32_t N>
VkResult vkuFences<N>::wait(uint32_t _start, uint32_t _num, uint64_t _timeout) {
  return vkWaitForFences(vDevice, _num, &vFence[_start], VK_TRUE, _timeout);
}

/*!
 * \brief Waits for the fence(s) and then resets them
 * \param _start First fence
 * \param _num   Number of fences (after the start fence)
 * \param _timeout The timeout
 */
template <uint32_t N>
VkResult vkuFences<N>::waitThenReset(uint32_t _start, uint32_t _num, uint64_t _timeout) {
  auto lRes = vkWaitForFences(vDevice, _num, &vFence[_start], VK_TRUE, _timeout);
  if (lRes != VK_SUCCESS) {
    return lRes;
  }

  return vkResetFences(vDevice, _num, &vFence[_start]);
}

/*!
 * \brief Returns the status of a fence
 * \param _n The fence to check
 */
template <uint32_t N>
VkResult vkuFences<N>::status(uint32_t _n) {
  return vkGetFenceStatus(vDevice, vFence[_n]);
}

template <uint32_t N>
bool               vkuFences<N>::operator!() const {
  if (vDevice == VK_NULL_HANDLE)
    return true;

  for (VkFence &i : vFence)
    if (i == VK_NULL_HANDLE)
      return true;

  return false;
}

template <uint32_t N>
vkuFences<N>::operator bool() const {
  if (vDevice == VK_NULL_HANDLE)
    return false;

  for (VkFence &i : vFence)
    if (i == VK_NULL_HANDLE)
      return false;

  return true;
}
}
