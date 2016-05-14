/*!
 * \file rRendererBasic.hpp
 * \brief \b Classes: \a rRendererBasic
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

namespace e_engine {

class rRendererBasic : public internal::rRendererBase {
   struct FB_DATA {
      std::vector<VkCommandBuffer> buffers;
   };

 private:
    std::vector<FB_DATA> vFbData;

    OBJECTS vRenderObjects;
    OBJECTS vPuschConstObjects;

 protected:
   void setupSubpasses() override;
   std::vector<AttachmentInfo> getAttachmentInfos() override;
   void recordCmdBuffers( Framebuffer_vk &_fb, RECORD_TARGET _toRender ) override;

   void initCmdBuffers( VkCommandPool _pool, uint32_t _numFramebuffers ) override;
   void freeCmdBuffers( VkCommandPool _pool ) override;

 public:
   static const uint32_t DEPTH_STENCIL_ATTACHMENT_INDEX = FIRST_FREE_ATTACHMENT_INDEX + 0;

   VkImageView getAttachmentView( ATTACHMENT_ROLE _role ) override;

   rRendererBasic() = delete;
   rRendererBasic( iInit *_init, rWorld *_root, std::wstring _id )
       : internal::rRendererBase( _init, _root, _id ) {}
};
}
