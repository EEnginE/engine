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

template <uint32_t N>
class vkuSemaphores final {
  static_assert(N > 0, "Min for N is 1");

 private:
  VkSemaphore vSemaphores[N];
  VkDevice    vDevice = VK_NULL_HANDLE;

 public:
  vkuSemaphores();
  vkuSemaphores(VkDevice _device);
  vkuSemaphores(VkDevice _device, VkSemaphoreCreateInfo _info);
  ~vkuSemaphores();

  vkuSemaphores(vkuSemaphores<N> const &) = delete;
  vkuSemaphores &operator=(const vkuSemaphores<N> &) = delete;

  vkuSemaphores(vkuSemaphores<N> &&);
  vkuSemaphores &operator=(vkuSemaphores<N> &&);

  inline VkSemaphore &at(uint32_t _n = 0) noexcept { return vSemaphores[_n]; }
  inline VkSemaphore &operator[](uint32_t _n) { return vSemaphores[_n]; }

  bool operator!() const;
  explicit operator bool() const;
};

using vkSemaphore_t = vkuSemaphores<1>;

/*!
 * \brief Default constructor
 */
template <uint32_t N>
vkuSemaphores<N>::vkuSemaphores() {
  for (VkSemaphore &i : vSemaphores) {
    i = VK_NULL_HANDLE;
  }
}

/*!
 * \brief Creates a fence
 * \param _device The device to create the fence on
 * \param _info   The fence create info
 */
template <uint32_t N>
vkuSemaphores<N>::vkuSemaphores(VkDevice _device, VkSemaphoreCreateInfo _info) : vDevice(_device) {
  if (vDevice != VK_NULL_HANDLE) {
    for (VkSemaphore &i : vSemaphores) {
      auto lRes = vkCreateSemaphore(vDevice, &_info, nullptr, &i);
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
vkuSemaphores<N>::vkuSemaphores(VkDevice _device)
    : vkuSemaphores(_device, {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO, nullptr, 0}) {}

template <uint32_t N>
vkuSemaphores<N>::~vkuSemaphores() {
  for (VkSemaphore &i : vSemaphores) {
    if (i != VK_NULL_HANDLE) {
      vkDestroySemaphore(vDevice, i, nullptr);
    }
  }
}

/*!
 * \brief Move constructor
 * \param _old old Fence
 */
template <uint32_t N>
vkuSemaphores<N>::vkuSemaphores(vkuSemaphores<N> &&_old) {
  vDevice      = _old.vDevice;
  _old.vDevice = VK_NULL_HANDLE;

  for (uint32_t i = 0; i < N; ++i) {
    vSemaphores[i]      = _old.vSemaphores[i];
    _old.vSemaphores[i] = VK_NULL_HANDLE;
  }
}

/*!
 * \brief Move assignment constructor
 * \param _old old Fence
 */
template <uint32_t N>
vkuSemaphores<N> &vkuSemaphores<N>::operator=(vkuSemaphores<N> &&_old) {
  vDevice      = _old.vDevice;
  _old.vDevice = VK_NULL_HANDLE;

  for (uint32_t i = 0; i < N; ++i) {
    vSemaphores[i]      = _old.vSemaphores[i];
    _old.vSemaphores[i] = VK_NULL_HANDLE;
  }
  return *this;
}

template <uint32_t N>
bool               vkuSemaphores<N>::operator!() const {
  if (vDevice == VK_NULL_HANDLE)
    return true;

  for (VkFence &i : vSemaphores)
    if (i == VK_NULL_HANDLE)
      return true;

  return false;
}

template <uint32_t N>
vkuSemaphores<N>::operator bool() const {
  if (vDevice == VK_NULL_HANDLE)
    return false;

  for (VkFence &i : vSemaphores)
    if (i == VK_NULL_HANDLE)
      return false;

  return true;
}
}
