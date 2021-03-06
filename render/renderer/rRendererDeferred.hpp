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
// #include "rBuffer.hpp"
#include "vkuCommandBuffer.hpp"
#include "rRendererBase.hpp"

namespace e_engine {

/*!
 * \warning THIS RENDERER IS BROKEN
 *
 * \todo FIX THIS
 */
class rRendererDeferred : public rRendererBase {
  struct FB_DATA {
    std::vector<vkuCommandBuffer> objects;
    std::vector<vkuCommandBuffer> lights;

    vkuCommandBuffer layoutChange1;
    vkuCommandBuffer layoutChange2;
  };

 private:
  std::vector<FB_DATA> vFbData;

  OBJECTS vRenderObjects;
  OBJECTS vLightObjects;

  //   rBuffer vDeferredDataBuffer;
  //   rBuffer vDeferredIndexBuffer;

 protected:
  void recordCmdBuffers(uint32_t &_fbIndex, RECORD_TARGET _toRender) override;

  bool initRendererData() override;
  bool freeRendererData() override;

 public:
  static const uint32_t DEPTH_STENCIL_ATTACHMENT_INDEX   = FIRST_FREE_ATTACHMENT_INDEX + 0;
  static const uint32_t DEFERRED_POS_ATTACHMENT_INDEX    = FIRST_FREE_ATTACHMENT_INDEX + 1;
  static const uint32_t DEFERRED_NORMAL_ATTACHMENT_INDEX = FIRST_FREE_ATTACHMENT_INDEX + 2;
  static const uint32_t DEFERRED_ALBEDO_ATTACHMENT_INDEX = FIRST_FREE_ATTACHMENT_INDEX + 3;

  rRendererDeferred() = delete;
  rRendererDeferred(rWorld *_root, std::wstring _id);
};
} // namespace e_engine
