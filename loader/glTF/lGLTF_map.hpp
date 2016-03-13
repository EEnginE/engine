/*!
 * \file lGLTF_map.hpp
 * \brief \b Classes: \a lGLTF_map
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

#include "defines.hpp"
#include <unordered_map>
#include <string>

namespace e_engine {
namespace glTF {

class UTILS_API lGLTF_map {
 protected:
   lGLTF_map();
   virtual ~lGLTF_map();

 public:
   enum ELEMENTS {
      EXTENSIONS,
      ADDITIONALPROPERTIES,
      EXTRAS,

      // Root
      EXTENSIONSUSED,
      ACCESSORS,
      ANIMATIONS,
      ASSET,
      BUFFERS,
      BUFFERVIEWS,
      CAMERAS,
      IMAGES,
      MATERIALS,
      MESHES,
      NODES,
      PROGRAMS,
      SAMPLERS,
      SCENE,
      SCENES,
      SHADERS,
      SKINS,
      TECHNIQUES,
      TEXTURES,

      // -- ACCESSORS
      BUFFERVIEW,
      BYTEOFFSET,
      BYTESTRIDE,
      COMPONENTTYPE,
      COUNT,
      TYPE,
      MAX,
      MIN,
      SCALAR,
      VEC2,
      VEC3,
      VEC4,
      MAT2,
      MAT3,
      MAT4,

      // -- ANIMATIONS [SAMPLERS]
      CHANNELS,
      PARAMETERS,
      SAMPLER,
      TARGET,
      ID,
      PATH,
      TRANSLATION,
      ROTATION,
      SCALE,
      INPUT,
      INTERPOLATION,
      LINEAR,
      OUTPUT,

      // -- ASSET
      COPYRIGHT,
      GENERATOR,
      PREMULTIPLIEDALPHA,
      PROFILE,
      VERSION,
      API,

      // -- BUFFERS [TYPE]
      URI,
      BYTELENGTH,
      ARRAYBUFFER,
      TEXT,

      // -- BUFFERVIEWS [BYTEOFFSET, BYTELENGTH, TARGET]
      BUFFER,
      TG_ARRAY_BUFFER,         // 34962
      TG_ELEMENT_ARRAY_BUFFER, // 34963

      // -- CAMERAS [TYPE]
      ORTHOGRAPHIC,
      PERSPECTIVE,
      XMAG,
      YMAG,
      ZFAR,
      ZNEAR,
      ASPECTRATIO,
      YFOV,

      // -- MATERIALS
      TECHNIQUE,
      VALUES,

      // -- MESHES
      PRIMITIVES,
      ATTRIBUTES,
      INDICES,
      MATERIAL,
      MODE,
      P_POINTS,
      P_LINES,
      P_LINE_LOOP,
      P_LINE_STRIP,
      P_TRIANGLES,
      P_TRIANGLE_STRIP,
      P_TRIANGLE_FAN,

      // -- NODES [MESES; ROTATION, SCALE, TRANSLATION]
      CAMERA,
      CHILDREN,
      SKELETONS,
      SKIN,
      JOINTNAME,
      MATRIX,

      // -- PROGRAMS [ATTRIBUTES]
      FRAGMENTSHADER,
      VERTEXSHADER,

      // -- SAMPLERS
      MAGFILTER,
      MINFILTER,
      WRAPS,
      WRAPT,
      M_NEAREST,
      M_LINEAR,
      M_NEAREST_MIPMAP_NEAREST,
      M_LINEAR_MIPMAP_NEAREST,
      M_NEAREST_MIPMAP_LINEAR,
      M_LINEAR_MIPMAP_LINEAR,
      M_CLAMP_TO_EDGE,
      M_MIRRORED_REPEAT,
      M_REPEAT,

      // -- SHADERS [uri]
      S_FRAGMENT_SHADER,
      S_VERTEX_SHADER,

      // -- SKINS
      BINDSHAPEMATRIX,
      INVERSEBINDMATRICES,
      JOINTNAMES,

      // -- TECHNIQUES [PARAMETERS, ATTRIBUTES, COUNT]
      PROGRAM,
      UNIFORMS,
      STATES,
      ENABLE,
      FUNCTIONS,
      NODE,
      SEMANTIC,
      VALUE,
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

      TP_BYTE,
      TP_UNSIGNED_BYTE,
      TP_SHORT,
      TP_UNSIGNED_SHORT,
      TP_INT,
      TP_UNSIGNED_INT,
      TP_FLOAT,
      TP_FLOAT_VEC2,
      TP_FLOAT_VEC3,
      TP_FLOAT_VEC4,
      TP_INT_VEC2,
      TP_INT_VEC3,
      TP_INT_VEC4,
      TP_BOOL,
      TP_BOOL_VEC2,
      TP_BOOL_VEC3,
      TP_BOOL_VEC4,
      TP_FLOAT_MAT2,
      TP_FLOAT_MAT3,
      TP_FLOAT_MAT4,
      TP_SAMPLER_2D,

      SM_LOCAL,
      SM_MODEL,
      SM_VIEW,
      SM_PROJECTION,
      SM_MODELVIEW,
      SM_MODELVIEWPROJECTION,
      SM_MODELINVERSE,
      SM_VIEWINVERSE,
      SM_PROJECTIONINVERSE,
      SM_MODELVIEWINVERSE,
      SM_MODELVIEWPROJECTIONINVERSE,
      SM_MODELINVERSETRANSPOSE,
      SM_MODELVIEWINVERSETRANSPOSE,
      SM_VIEWPORT,

      SM_POSITION,
      SM_NORMAL,
      SM_TEXCOORD,
      SM_COLOR,
      SM_JOINT,
      SM_JOINTMATRIX,
      SM_WEIGHT,

      TECH_BLEND,
      TECH_CULL_FACE,
      TECH_DEPTH_TEST,
      TECH_POLYGON_OFFSET_FILL,
      TECH_SAMPLE_ALPHA_TO_COVERAGE,
      TECH_SCISSOR_TEST,
      TECH_FUNC_ADD,
      TECH_FUNC_SUBTRACT,
      TECH_FUNC_REVERSE_SUBTRACT,
      TECH_ZERO,
      TECH_ONE,
      TECH_SRC_COLOR,
      TECH_ONE_MINUS_SRC_COLOR,
      TECH_DST_COLOR,
      TECH_ONE_MINUS_DST_COLOR,
      TECH_SRC_ALPHA,
      TECH_ONE_MINUS_SRC_ALPHA,
      TECH_DST_ALPHA,
      TECH_ONE_MINUS_DST_ALPHA,
      TECH_CONSTANT_COLOR,
      TECH_ONE_MINUS_CONSTANT_COLOR,
      TECH_CONSTANT_ALPHA,
      TECH_ONE_MINUS_CONSTANT_ALPHA,
      TECH_SRC_ALPHA_SATURATE,
      TECH_FRONT,
      TECH_BACK,
      TECH_FRONT_AND_BACK,
      TECH_NEVER,
      TECH_LESS,
      TECH_LEQUAL,
      TECH_EQUAL,
      TECH_GREATER,
      TECH_NOTEQUAL,
      TECH_GEQUAL,
      TECH_ALWAYS,
      TECH_CW,
      TECH_CCW,

      // -- TEXTURES [SAMPLER, TARGET, TYPE]
      FORMAT,
      INTERNALFORMAT,
      SOURCE,
      TT_ALPHA,
      TT_RGB,
      TT_RGBA,
      TT_LUMINANCE,
      TT_LUMINANCE_ALPHA,
      TT_TEXTURE_2D,
      TT_UNSIGNED_BYTE,
      TT_UNSIGNED_SHORT_5_6_5,
      TT_UNSIGNED_SHORT_4_4_4_4,
      TT_UNSIGNED_SHORT_5_5_5_1,

      NAME,


   };

 protected:
   std::unordered_map<std::string, ELEMENTS> vMap;
};
}
}

#endif // R_LOADER_GLTF_MAP_HPP
