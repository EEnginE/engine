/*!
 * \file rLoader_glTF_map.hpp
 * \brief \b Classes: \a rLoader_glTF_map
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

#ifndef R_LOADER_GLTF_MAP_HPP
#define R_LOADER_GLTF_MAP_HPP

#include <unordered_map>
#include <string>

namespace e_engine {
namespace glTF {

class rLoader_glTF_map {
 protected:
   rLoader_glTF_map();
   virtual ~rLoader_glTF_map();

   enum ELEMENTS {
      ALLEXTENSIONS,
      NAME,
      EXTENSIONS,
      EXTRAS,
      ACCESSORS,
      BUFFERVIEW,
      BYTEOFFSET,
      BYTESTRIDE,
      COMPONENTTYPE,
      COUNT,
      TYPE,
      MAX,
      MIN,
      ANIMATIONS,
      CHANNELS,
      PARAMETERS,
      SAMPLERS,
      SAMPLER,
      TARGET,
      ID,
      PATH,
      INPUT,
      INTERPOLATION,
      OUTPUT,
      ASSET,
      COPYRIGHT,
      GENERATOR,
      PREMULTIPLIEDALPHA,
      PROFILE,
      VERSION,
      BUFFERS,
      URI,
      BYTELENGTH,
      BUFFERVIEWS,
      BUFFER,
      CAMERAS,
      ORTHOGRAPHIC,
      PERSPECTIVE,
      XMAG,
      YMAG,
      ZFAR,
      ZNEAR,
      ASPECTRATIO,
      YFOV,
      IMAGES,
      LIGHTS,
      AMBIENT,
      DIRECTIONAL,
      POINT,
      SPOT,
      COLOR,
      CONSTANTATTENUATION,
      LINEARATTENUATION,
      QUADRATICATTENUATION,
      FALLOFFANGLE,
      FALLOFFEXPONENT,
      MATERIALS,
      INSTANCETECHNIQUE,
      TECHNIQUE,
      VALUES,
      MESHES,
      PRIMITIVES,
      ATTRIBUTES,
      INDICES,
      MATERIAL,
      PRIMITIVE,
      NODES,
      CAMERA,
      CHILDREN,
      INSTANCESKIN,
      JOINTNAME,
      LIGHT,
      MATRIX,
      ROTATION,
      SCALE,
      TRANSLATION,
      SKELETONS,
      SKIN,
      PROGRAMS,
      FRAGMENTSHADER,
      VERTEXSHADER,
      MAGFILTER,
      MINFILTER,
      WRAPS,
      WRAPT,
      SCENE,
      SCENES,
      SHADERS,
      SKINS,
      BINDSHAPEMATRIX,
      INVERSEBINDMATRICES,
      JOINTNAMES,
      TECHNIQUES,
      PASS,
      PASSES,
      SEMANTIC,
      NODE,
      VALUE,
      DETAILS,
      INSTANCEPROGRAM,
      STATES,
      COMMONPROFILE,
      LIGHTINGMODEL,
      TEXCOORDBINDINGS,
      PROGRAM,
      UNIFORMS,
      ENABLE,
      FUNCTIONS,
      BLENDCOLOR,
      BLENDEQUATIONSEPARATE,
      BLENDFUNCSEPARATE,
      COLORMASK,
      CULLFACE,
      DEPTHFUNC,
      DEPTHMASK,
      DEPTHRANGE,
      FRONTFACE,
      LINEWIDTH,
      POLYGONOFFSET,
      SCISSOR,
      TEXTURES,
      FORMAT,
      INTERNALFORMAT,
      SOURCE,

      // Component types
      COMP_BYTE,           //!< 5120
      COMP_UNSIGNED_BYTE,  //!< 5121
      COMP_SHORT,          //!< 5122
      COMP_UNSIGNED_SHORT, //!< 5123
      COMP_FLOAT,          //!< 5126

      // Types
      TP_SCALAR,
      TP_VEC2,
      TP_VEC3,
      TP_VEC4,
      TP_MAT2,
      TP_MAT3,
      TP_MAT4,

      TP_ARRAYBUFFER,
      TP_TEXT,

      TP_NORMAL,
      TP_POSITION,
      TP_TEXCOORD_0,

      // Primitives
      P_POINTS,         //!< 0
      P_LINES,          //!< 1
      P_LINE_LOOP,      //!< 2
      P_LINE_STRIP,     //!< 3
      P_TRIANGLES,      //!< 4
      P_TRIANGLE_STRIP, //!< 5
      P_TRIANGLE_FAN,   //!< 6

      // Targets
      TG_ARRAY_BUFFER,         //!< 34962
      TG_ELEMENT_ARRAY_BUFFER, //!< 34963
   };

   std::unordered_map<std::string, ELEMENTS> vMap;
};
}
}

#endif // R_LOADER_GLTF_MAP_HPP
