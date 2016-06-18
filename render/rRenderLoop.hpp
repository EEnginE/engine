/*!
 * \file rRenderLoop.hpp
 * \brief \b Classes: \a rRenderLoop
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
#include "rRendererBase.hpp"
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vulkan.h>

namespace e_engine {

class rWorld;
class iInit;

class rRenderLoop {
 public:
   typedef struct CommandBuffers {
      std::vector<VkCommandBuffer> pre;
      std::vector<VkCommandBuffer> render;
      std::vector<VkCommandBuffer> post;

      std::vector<std::vector<internal::rRendererBase::CommandBuffers>> pointers;
   } CommandBuffers;


 private:
   static uint64_t vRenderedFrames;

   rWorld *vWorldPtr;
   iInit * vInitPtr;

   VkDevice vDevice_vk;

   std::vector<std::shared_ptr<internal::rRendererBase>> vRenderers;

   bool vRunRenderLoop     = false;
   bool vRunRenderThread   = true;
   bool vFinishedRecording = false;

   std::thread vRenderThread;

   std::mutex vMutexStartRecording;
   std::mutex vMutexFinishedRecording;
   std::mutex vMutexStartLogLoop;
   std::mutex vMutexStopLogLoop;

   std::recursive_mutex vLoopAccessMutex;

   std::condition_variable vVarStartRecording;
   std::condition_variable vVarFinishedRecording;
   std::condition_variable vVarStartLogLoop;
   std::condition_variable vVarStopLogLoop;

   void rebuildCommandBuffersArray( CommandBuffers *_buffers, uint32_t _framebuffer );
   void renderLoop();

 public:
   rRenderLoop() = delete;
   rRenderLoop( iInit *_init, rWorld *_root );
   rRenderLoop( const rRenderLoop &_obj ) = delete;
   rRenderLoop( rRenderLoop && )          = delete;
   rRenderLoop &operator=( const rRenderLoop & ) = delete;
   rRenderLoop &operator=( rRenderLoop && ) = delete;
   virtual ~rRenderLoop();

   int  init();
   void destroy();

   bool start();
   bool stop();
   bool getIsRunning() const;

   void addRenderer( std::shared_ptr<internal::rRendererBase> _renderer );
   void removeRenderer( std::shared_ptr<internal::rRendererBase> _renderer );
   void clearRenderers();

   void updateGlobalClearColor( VkClearColorValue _clear );

   uint64_t *getRenderedFramesPtr();
};
}
