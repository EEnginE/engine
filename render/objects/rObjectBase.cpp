/*!
 * \file rObjectBase.cpp
 */
/*
 * Copyright (C) 2015 EEnginE project
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

#include "rObjectBase.hpp"
#include "uEnum2Str.hpp"
#include "uLog.hpp"
#include "iInit.hpp"
#include "rPipeline.hpp"
#include <regex>

using namespace e_engine;
using namespace glm;

/*!
 * \brief Destructor
 * \warning NEVER CALL A VIRTUAL FUNCTION HERE!!
 */
rObjectBase::~rObjectBase() {}

bool rObjectBase::setPipeline(rPipeline *_pipe) {
  if (!checkIsCompatible(_pipe)) {
    eLOG("Pipeline not compatible with object ", vName_str);
    return false;
  }

  vPipeline = _pipe;
  return true;
}

bool rObjectBase::setData(VkCommandBuffer _buf, aiScene const *_scene, uint32_t _meshIndex) {
  if (vIsLoaded_B || vPartialLoaded_B) {
    eLOG("Data already loaded! Object ", vName_str);
    return false;
  }

  if (!_scene || _scene->mNumMeshes <= _meshIndex) {
    eLOG(L"Invalid mesh parameters");
    return false;
  }

  aiMesh *lMesh = _scene->mMeshes[_meshIndex];

  if (!lMesh) {
    eLOG("Invalid mesh object!");
    return false;
  }

  uint32_t lIndexSize = 3;

  switch (getMeshType()) {
    case POINTS_3D:
      if (lMesh->mPrimitiveTypes != aiPrimitiveType_POINT) {
        eLOG("Invalid primitive type ", lMesh->mPrimitiveTypes, " expected ", aiPrimitiveType_POINT, " (point)");
        return false;
      }

      lIndexSize = 1;
      break;
    case LINES_3D:
      if (lMesh->mPrimitiveTypes != aiPrimitiveType_LINE) {
        eLOG("Invalid primitive type ", lMesh->mPrimitiveTypes, " expected ", aiPrimitiveType_LINE, " (line)");
        return false;
      }

      lIndexSize = 2;
      break;
    case MESH_3D:
      if (lMesh->mPrimitiveTypes != aiPrimitiveType_TRIANGLE) {
        eLOG("Invalid primitive type ", lMesh->mPrimitiveTypes, " expected ", aiPrimitiveType_TRIANGLE, " (triangle)");
        return false;
      }

      lIndexSize = 3;
      break;
    case POLYGON_3D:
      if (lMesh->mPrimitiveTypes != aiPrimitiveType_POLYGON) {
        eLOG("Invalid primitive type ", lMesh->mPrimitiveTypes, " expected ", aiPrimitiveType_POLYGON, " (polygon)");
        return false;
      }

      eLOG("Polygon type currently not supported!");
      return false;
    default: eLOG("This object type does not support mesh data!"); return false;
  }

  std::vector<uint32_t> lIndex;
  std::vector<float>    lData;

  lIndex.resize(lIndexSize * lMesh->mNumFaces);
  for (uint32_t i = 0; i < lMesh->mNumFaces; i++)
    for (uint32_t j = 0; j < lIndexSize; j++)
      lIndex[i * lIndexSize + j] = lMesh->mFaces[i].mIndices[j];

  switch (getDataLayout()) {
    case POS_NORM: setupVertexData_PN(lMesh, lData); break;
    case POS_NORM_UV: setupVertexData_PNUV(lMesh, lData); break;
    default: eLOG("Data layout ", uEnum2Str::toStr(getDataLayout())); return false;
  }

  for (uint32_t i = 0; i < _scene->mNumMaterials; ++i) {
    aiString name;
    float    opacity;
    float    shininess;
    float    shininess_strength;
    float    refracti;
    int      blend_func;

    aiMaterial *lMat = _scene->mMaterials[i];
    lMat->Get(AI_MATKEY_NAME, &name, nullptr);
    lMat->Get(AI_MATKEY_OPACITY, &opacity, nullptr);
    lMat->Get(AI_MATKEY_SHININESS, &shininess, nullptr);
    lMat->Get(AI_MATKEY_SHININESS_STRENGTH, &shininess_strength, nullptr);
    lMat->Get(AI_MATKEY_REFRACTI, &refracti, nullptr);
    lMat->Get(AI_MATKEY_BLEND_FUNC, &blend_func, nullptr);

    iLOG(L"Material ", i);
    iLOG(L"  -- name:               ", name.C_Str());
    iLOG(L"  -- opacity:            ", opacity);
    iLOG(L"  -- shininess:          ", shininess);
    iLOG(L"  -- shininess_strength: ", shininess_strength);
    iLOG(L"  -- refracti:           ", refracti);
    iLOG(L"  -- blend_func:         ", blend_func);

    for (aiTextureType j : {aiTextureType_NONE,
                            aiTextureType_DIFFUSE,
                            aiTextureType_SPECULAR,
                            aiTextureType_AMBIENT,
                            aiTextureType_EMISSIVE,
                            aiTextureType_HEIGHT,
                            aiTextureType_NORMALS,
                            aiTextureType_SHININESS,
                            aiTextureType_OPACITY,
                            aiTextureType_DISPLACEMENT,
                            aiTextureType_LIGHTMAP,
                            aiTextureType_REFLECTION,
                            aiTextureType_UNKNOWN}) {
      uint32_t lCount = lMat->GetTextureCount(j);

      iLOG(L"  -- Texture type: [", lCount, L"]: ", uEnum2Str::toStr(j));

      for (uint32_t k = 0; k < lCount; ++k) {
        aiString         path;
        aiTextureMapping mapping;
        uint32_t         uvIndex;
        ai_real          blend;
        aiTextureOp      textureOp;
        aiTextureMapMode mapMode;
        lMat->GetTexture(j, k, &path, &mapping, &uvIndex, &blend, &textureOp, &mapMode);
        iLOG(L"    - path:      ", path.C_Str());
        iLOG(L"    - mapping:   ", uEnum2Str::toStr(mapping));
        iLOG(L"    - uvIndex:   ", uvIndex);
        iLOG(L"    - blend:     ", blend);
        iLOG(L"    - textureOp: ", uEnum2Str::toStr(textureOp));
        iLOG(L"    - mapMode:   ", uEnum2Str::toStr(mapMode));
      }
    }
  }

  vLoadBuffers = setData_IMPL(_buf, lIndex, lData);

  vPartialLoaded_B = true;
  return true;
}

bool rObjectBase::finishData() {
  if (vIsLoaded_B) {
    eLOG("Data already loaded! Object ", vName_str);
    return false;
  }

  if (!vPartialLoaded_B) {
    eLOG("setData not called yet! Object ", vName_str);
    return false;
  }

  for (auto const &i : vLoadBuffers) {
    if (!i->doneCopying()) {
      eLOG("Failed to init data for object ", vName_str);
      return false;
    }
  }

  vLoadBuffers.clear();
  vIsLoaded_B = true;
  return true;
}

void rObjectBase::destroy() { destroy_IMPL(); }

bool rObjectBase::setupVertexData_PN(aiMesh const *_mesh, std::vector<float> &_out) {
  if (!_mesh->HasNormals()) {
    eLOG("Invalid data! Object ", vName_str);
    return false;
  }

  _out.resize(_mesh->mNumVertices * 3 * 2);
  for (uint32_t i = 0; i < _mesh->mNumVertices; i++) {
    _out[6 * i + 0] = _mesh->mVertices[i].x;
    _out[6 * i + 1] = _mesh->mVertices[i].y;
    _out[6 * i + 2] = _mesh->mVertices[i].z;
    _out[6 * i + 3] = _mesh->mNormals[i].x;
    _out[6 * i + 4] = _mesh->mNormals[i].y;
    _out[6 * i + 5] = _mesh->mNormals[i].z;
  }

  return true;
}

bool rObjectBase::setupVertexData_PNUV(aiMesh const *_mesh, std::vector<float> &_out) {
  if (!_mesh->HasNormals()) {
    eLOG("Invalid data! (Missing Normals) Object ", vName_str);
    return false;
  }

  if (!_mesh->HasTextureCoords(0)) {
    eLOG("Invalid data! (Missing UV) Object ", vName_str);
    return false;
  }

  if (_mesh->mNumUVComponents[0] != 2) {
    eLOG("Invalid data! (Invalid number texcords UV) Object ", vName_str);
    return false;
  }

  _out.resize(_mesh->mNumVertices * (3 + 3 + 2));
  for (uint32_t i = 0; i < _mesh->mNumVertices; i++) {
    _out[8 * i + 0] = _mesh->mVertices[i].x;
    _out[8 * i + 1] = _mesh->mVertices[i].y;
    _out[8 * i + 2] = _mesh->mVertices[i].z;
    _out[8 * i + 3] = _mesh->mNormals[i].x;
    _out[8 * i + 4] = _mesh->mNormals[i].y;
    _out[8 * i + 5] = _mesh->mNormals[i].z;
    _out[8 * i + 6] = _mesh->mTextureCoords[0][i].x;
    _out[8 * i + 7] = _mesh->mTextureCoords[0][i].y;
  }

  return true;
}

/*!
 * \brief returns the shader object
 * \returns the shader or nullptr on error
 */
rShaderBase *rObjectBase::getShader() {
  if (!vPipeline)
    return nullptr;

  return vPipeline->getShader();
}


/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix(mat4 **_mat, rObjectBase::MATRIX_TYPES _type) {
  (void)_type;
  *_mat = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat  The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix(dmat4 **_mat, rObjectBase::MATRIX_TYPES _type) {
  (void)_type;
  *_mat = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix(mat3 **_mat, rObjectBase::MATRIX_TYPES _type) {
  (void)_type;
  *_mat = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Matrix
 *
 * \param[out] _mat The Matrix pointer
 * \param[in]  _type The Matrix type
 * \returns 0 if the requested Matrix exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getMatrix(dmat3 **_mat, rObjectBase::MATRIX_TYPES _type) {
  (void)_type;
  *_mat = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}



/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector(vec4 **_vec, rObjectBase::VECTOR_TYPES _type) {
  (void)_type;
  *_vec = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector(dvec4 **_vec, rObjectBase::VECTOR_TYPES _type) {
  (void)_type;
  *_vec = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}


/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector(vec3 **_vec, rObjectBase::VECTOR_TYPES _type) {
  (void)_type;
  *_vec = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

/*!
 * \brief Get the _type Vector
 *
 * \param[out] _vec The Vector pointer
 * \param[in]  _type The Vector type
 * \returns 0 if the requested Vector exists and ERROR_FLAGS flags if not
 */
uint32_t rObjectBase::getVector(dvec3 **_vec, rObjectBase::VECTOR_TYPES _type) {
  (void)_type;
  *_vec = nullptr;
  return FUNCTION_NOT_VALID_FOR_THIS_OBJECT;
}

// kate: indent-mode cstyle; indent-width 2; replace-tabs on; line-numbers on;
