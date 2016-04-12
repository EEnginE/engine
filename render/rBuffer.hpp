/*!
 * \file rBuffer.hpp
 * \brief \b Classes: \a rBuffer
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

#pragma once

#include "defines.hpp"
#include <vulkan.h>
#include <vector>


namespace e_engine {

class iInit;
class rWorld;

class rBuffer {
 private:
   VkDevice vDevice_vk;
   iInit *vInitPtr;

   VkBuffer vTempBuffer_vk    = nullptr;
   VkBuffer vBuffer_vk        = nullptr;
   VkDeviceMemory vMemTemp_vk = nullptr;
   VkDeviceMemory vMem_vk     = nullptr;

   bool vIsLoaded            = false;
   bool vSettingUpInProgress = false;

   uint32_t vSize = 0;

   bool errorCleanup();

 public:
   rBuffer() = delete;
   rBuffer( iInit *_init );
   rBuffer( rWorld *_tempWorld );
   rBuffer( const rBuffer &_obj ) = delete;
   rBuffer( rBuffer && ) = default;
   rBuffer &operator=( const rBuffer & ) = delete;
   rBuffer &operator=( rBuffer && ) = default;
   virtual ~rBuffer();

   template <class T>
   bool cmdInit( std::vector<T> const &_data, VkCommandBuffer _buff, VkBufferUsageFlags _flags );

   bool doneCopying();
   bool destroy();
   bool getIsSetup() const { return vIsLoaded; }

   VkBuffer getBuffer();
   uint32_t getSize() { return vSize; }
};
}
