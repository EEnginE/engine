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
 */

#pragma once

#include "defines.hpp"
#include "vkuDevice.hpp"
#include "vkuImageBuffer.hpp"

namespace e_engine {

enum class TextureType {
  NONE,
  DIFFUSE,
  SPECULAR,
  AMBIENT,
  EMISSIVE,
  HEIGHT,
  NORMALS,
  SHININESS,
  OPACITY,
  DISPLACEMENT,
  LIGHTMAP,
  REFLECTION,
  UNKNOWN
};

enum class TextureOP { MULTIPLY, ADD, SUBTRACT, DIVIDE, SMOOTH_ADD, SIGNED_ADD };
enum class TextureMapping { UV, SPHERE, CYLINDER, BOX, PLANE, OTHER };
enum class TextureMapMode { WRAP, CLAMP, DECAL, MIRROR };

class rTexture final {
  struct Config {
    uint32_t       UVIndex = UINT32_MAX;
    float          blend   = 1.0;
    TextureType    type    = TextureType::DIFFUSE;
    TextureOP      blendOP = TextureOP::MULTIPLY;
    TextureMapping mapping = TextureMapping::UV;
    TextureMapMode mapMode = TextureMapMode::WRAP;
  };

 private:
  vkuDevicePTR   vDevice;
  vkuImageBuffer vImg;

  Config cfg;

 public:
  rTexture() = delete;
  rTexture(vkuDevicePTR _device) : vDevice(_device), vImg(_device) {}
  ~rTexture();

  rTexture(rTexture const &) = delete;
  rTexture &operator=(const rTexture &) = delete;

  rTexture(rTexture &&);
  rTexture &operator=(rTexture &&);

  VkResult init(std::string _filePath);
  void     destroy();

  inline Config  getConfig() const noexcept { return cfg; }
  inline Config *getConfigPTR() noexcept { return &cfg; }

  inline Config *operator->() noexcept { return &cfg; } //! \brief Allow config access via buffer->cfgField = 1;
};

} // namespace e_engine
