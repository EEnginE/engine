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
#include "rTexture.hpp"
#include "vkuDevice.hpp"
#include <string>
#include <vector>

namespace e_engine {

enum class BlendFunc { UNDEFINED, DEFAULT, ADAPTIVE };
enum class ShadingMode {
  UNDEFINED,
  FLAT,
  GOURAUD,
  PHONG,
  BLINN,
  TOON,
  ORENNAYAR,
  MINNAERT,
  COOKTORRANCE,
  NOSHADING,
  FRESNEL
};

/*!
 * \brief Class for material description
 * \todo Implement this dummy class
 */
class rMaterial final {
  struct Config {
    float       opacity           = 1.0;
    float       shininess         = 0.0;
    float       shininessStrength = 1.0;
    float       refracti          = 1.0;
    BlendFunc   blendFunc         = BlendFunc::DEFAULT;
    ShadingMode shadingMode       = ShadingMode::FLAT;
  };

 private:
  vkuDevicePTR vDevice;
  std::string  vName = "";

  Config cfg;

  std::vector<rTexture> vTextures;

 public:
  rMaterial() = delete;
  rMaterial(vkuDevicePTR _device, std::string _name) : vDevice(_device), vName(_name) {}
  ~rMaterial();

  rMaterial(rMaterial const &) = delete;
  rMaterial &operator=(const rMaterial &) = delete;

  rMaterial(rMaterial &&);
  rMaterial &operator=(rMaterial &&);

  VkResult addTexture(std::string    _path,
                      uint32_t       _UVIndex,
                      float          _blend,
                      TextureType    _type,
                      TextureOP      _blendOP,
                      TextureMapping _mapping,
                      TextureMapMode _mapMode);

  inline std::vector<rTexture> &getTextures() noexcept { return vTextures; }
  inline std::string            getName() const noexcept { return vName; }
  inline Config                 getConfig() const noexcept { return cfg; }
  inline Config *               getConfigPTR() noexcept { return &cfg; }

  inline Config *operator->() noexcept { return &cfg; } //! \brief Allow config access via buffer->cfgField = 1;
};
} // namespace e_engine
