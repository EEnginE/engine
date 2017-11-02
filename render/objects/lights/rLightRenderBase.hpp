/*!
 * \file rLightRenderBase.hpp
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

#pragma once

#include "defines.hpp"

#include "rObjectBase.hpp"
#include <string>
#include <vulkan.h>

namespace e_engine {

class rLightRenderBase : public rObjectBase {
 private:
  bool isMesh() override { return false; }
  void recordLight(VkCommandBuffer _buf, vkuBuffer &_vertex, vkuBuffer &_index) override;
  bool checkIsCompatible(rPipeline *_pipe) override;
  void signalRenderReset(rRendererBase *_renderer) override;

 public:
  rLightRenderBase() = delete;
  rLightRenderBase(vkuDevicePTR _device, std::string _name) : rObjectBase(_device, _name) {}
};

} // namespace e_engine
