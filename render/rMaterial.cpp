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

#include "defines.hpp"
#include "rMaterial.hpp"
#include "uLog.hpp"

using namespace e_engine;

rMaterial::~rMaterial() {}

rMaterial::rMaterial(rMaterial &&_old) {
  vDevice = _old.vDevice;
  vName   = _old.vName;

  _old.vDevice = nullptr;

  vTextures = std::move(_old.vTextures);
}

rMaterial &rMaterial::operator=(rMaterial &&_old) {
  vDevice = _old.vDevice;
  vName   = _old.vName;

  _old.vDevice = nullptr;

  vTextures = std::move(_old.vTextures);
  return *this;
}

VkResult rMaterial::addTexture(std::string    _path,
                               uint32_t       _UVIndex,
                               float          _blend,
                               TextureType    _type,
                               TextureOP      _blendOP,
                               TextureMapping _mapping,
                               TextureMapMode _mapMode) {
  vTextures.emplace_back(vDevice);
  rTexture &lTex = vTextures.back();
  lTex->UVIndex  = _UVIndex;
  lTex->blend    = _blend;
  lTex->type     = _type;
  lTex->blendOP  = _blendOP;
  lTex->mapping  = _mapping;
  lTex->mapMode  = _mapMode;

  return lTex.init(_path);
}
