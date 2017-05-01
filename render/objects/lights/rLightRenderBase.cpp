/*!
* \file rLightRenderBase.cpp
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
*/

#include "rLightRenderBase.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "rPipeline.hpp"
#include "rRendererBase.hpp"

namespace e_engine {
namespace internal {

void rLightRenderBase::recordLight(VkCommandBuffer _buf, rBuffer &_vertex, rBuffer &_index) {
  VkDeviceSize lOffsets[] = {0};
  VkBuffer     lVertex    = _vertex.getBuffer();

  vPipeline->getShader()->cmdBindDescriptorSets(_buf, VK_PIPELINE_BIND_POINT_GRAPHICS);
  vPipeline->cmdBindPipeline(_buf, VK_PIPELINE_BIND_POINT_GRAPHICS);
  vkCmdBindVertexBuffers(_buf, vPipeline->getVertexBindPoint(), 1, &lVertex, &lOffsets[0]);
  vkCmdBindIndexBuffer(_buf, _index.getBuffer(), 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(_buf, _index.getSize(), 1, 0, 0, 1);
}

void rLightRenderBase::signalRenderReset(internal::rRendererBase *_renderer) {
  rShaderBase *lShader = vPipeline->getShader();
  if (!lShader) {
    eLOG("Fatal error: Shader not set");
    return;
  }

  auto lUniforms = lShader->getUniforms();
  for (auto const &i : lUniforms) {
    VkImageView lAttachImgView = nullptr;

    switch (i.guessedRole) {
      case rShaderBase::POSITION_SUBPASS_DATA:
        lAttachImgView = _renderer->getAttachmentView(rRendererBase::DEFERRED_POSITION);
        break;
      case rShaderBase::NORMAL_SUBPASS_DATA:
        lAttachImgView = _renderer->getAttachmentView(rRendererBase::DEFERRED_NORMAL);
        break;
      case rShaderBase::ALBEDO_SUBPASS_DATA:
        lAttachImgView = _renderer->getAttachmentView(rRendererBase::DEFERRED_ALBEDO);
        break;
      default: wLOG("Unknown uniform role ", uEnum2Str::toStr(i.guessedRole)); break;
    }

    if (lAttachImgView == nullptr) {
      wLOG("Uniform role unknown or failed to get VkImageView from renderer");
      continue;
    }

    lShader->updateDescriptorSet(i, lAttachImgView);
  }
}

bool rLightRenderBase::checkIsCompatible(rPipeline *_pipe) {
  return _pipe->checkInputCompatible({{2, sizeof(float)}}) &&
         _pipe->checkUniformCompatible(
             {rShaderBase::POSITION_SUBPASS_DATA, rShaderBase::NORMAL_SUBPASS_DATA, rShaderBase::ALBEDO_SUBPASS_DATA});
}
}
}
