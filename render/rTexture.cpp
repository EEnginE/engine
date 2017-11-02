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
#include "rTexture.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include <gli/gli.hpp>

using namespace e_engine;

rTexture::~rTexture() { destroy(); }

rTexture::rTexture(rTexture &&_old) {
  vDevice = _old.vDevice;
  cfg     = _old.cfg;

  _old.vDevice = nullptr;

  vImg = std::move(_old.vImg);
}

rTexture &rTexture::operator=(rTexture &&_old) {
  vDevice = _old.vDevice;
  cfg     = _old.cfg;

  _old.vDevice = nullptr;

  vImg = std::move(_old.vImg);
  return *this;
}


VkResult rTexture::init(std::string _filePath) {
  if (!vDevice) {
    eLOG(L"Invalid device");
    return VK_ERROR_INITIALIZATION_FAILED;
  }

  auto lTemp = gli::load(_filePath);
  if (lTemp.empty()) {
    eLOG(L"Failed to load texture ", _filePath);
    return VK_ERROR_FORMAT_NOT_SUPPORTED;
  }

  gli::texture2d lTexGLi(lTemp);

  if (vImg.isCreated())
    vImg.destroy();

  (void)_filePath;

  return VK_SUCCESS;
}

void rTexture::destroy() {
  if (!vDevice)
    return; // Moved form

  vImg.destroy();
}
