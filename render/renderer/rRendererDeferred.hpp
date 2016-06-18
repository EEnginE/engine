/*!
 * \file rRendererDeferred.hpp
 * \brief \b Classes: \a rRendererDeferred
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
#include "rBuffer.hpp"
#include "rRendererBase.hpp"

namespace e_engine {

class rRendererDeferred : public internal::rRendererBase {
   struct FB_DATA {
      std::vector<VkCommandBuffer> objects;
      std::vector<VkCommandBuffer> lights;

      VkCommandBuffer layoutChange1;
      VkCommandBuffer layoutChange2;
   };

 private:
   std::vector<FB_DATA> vFbData;

   OBJECTS vRenderObjects;
   OBJECTS vLightObjects;

   rBuffer vDeferredDataBuffer;
   rBuffer vDeferredIndexBuffer;

   void initBuffers();

 protected:
   void                        setupSubpasses() override;
   std::vector<AttachmentInfo> getAttachmentInfos() override;
   void recordCmdBuffers( Framebuffer_vk &_fb, RECORD_TARGET _toRender ) override;

   void initCmdBuffers( VkCommandPool _pool ) override;
   void freeCmdBuffers( VkCommandPool _pool ) override;

 public:
   static const uint32_t DEPTH_STENCIL_ATTACHMENT_INDEX   = FIRST_FREE_ATTACHMENT_INDEX + 0;
   static const uint32_t DEFERRED_POS_ATTACHMENT_INDEX    = FIRST_FREE_ATTACHMENT_INDEX + 1;
   static const uint32_t DEFERRED_NORMAL_ATTACHMENT_INDEX = FIRST_FREE_ATTACHMENT_INDEX + 2;
   static const uint32_t DEFERRED_ALBEDO_ATTACHMENT_INDEX = FIRST_FREE_ATTACHMENT_INDEX + 3;

   VkImageView getAttachmentView( ATTACHMENT_ROLE _role ) override;

   rRendererDeferred() = delete;
   rRendererDeferred( iInit *_init, rWorld *_root, std::wstring _id )
       : internal::rRendererBase( _init, _root, _id ),
         vDeferredDataBuffer( _root ),
         vDeferredIndexBuffer( _root ) {
      initBuffers();
   }
};
}
