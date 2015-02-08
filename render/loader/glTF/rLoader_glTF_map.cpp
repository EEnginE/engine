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

#include "rLoader_glTF_map.hpp"

namespace e_engine {
namespace glTF {

rLoader_glTF_map::~rLoader_glTF_map() {}

rLoader_glTF_map::rLoader_glTF_map()
    : vMap( {{"allExtensions", ALLEXTENSIONS},
             {"name", NAME},
             {"extensions", EXTENSIONS},
             {"extras", EXTRAS},

             {"accessors", ACCESSORS},
             {"bufferView", BUFFERVIEW},
             {"byteOffset", BYTEOFFSET},
             {"byteStride", BYTESTRIDE},
             {"componentType", COMPONENTTYPE},
             {"count", COUNT},
             {"type", TYPE},
             {"max", MAX},
             {"min", MIN},

             {"animations", ANIMATIONS},
             {"channels", CHANNELS},
             {"parameters", PARAMETERS},
             {"samplers", SAMPLERS},
             {"sampler", SAMPLER},
             {"target", TARGET},
             {"id", ID},
             {"path", PATH},
             {"input", INPUT},
             {"interpolation", INTERPOLATION},
             {"output", OUTPUT},

             {"asset", ASSET},
             {"copyright", COPYRIGHT},
             {"generator", GENERATOR},
             {"premultipliedAlpha", PREMULTIPLIEDALPHA},
             {"profile", PROFILE},
             {"version", VERSION},

             {"buffers", BUFFERS},
             {"uri", URI},
             {"byteLength", BYTELENGTH},
             {"type", TYPE},

             {"bufferViews", BUFFERVIEWS},
             {"buffer", BUFFER},
             {"byteOffset", BYTEOFFSET},
             {"byteLength", BYTELENGTH},
             {"target", TARGET},

             {"cameras", CAMERAS},
             {"orthographic", ORTHOGRAPHIC},
             {"perspective", PERSPECTIVE},
             {"type", TYPE},
             {"xmag", XMAG},
             {"ymag", YMAG},
             {"zfar", ZFAR},
             {"znear", ZNEAR},
             {"aspectRatio", ASPECTRATIO},
             {"yfov", YFOV},

             {"images", IMAGES},
             {"uri", URI},

             {"lights", LIGHTS},
             {"ambient", AMBIENT},
             {"directional", DIRECTIONAL},
             {"point", POINT},
             {"spot", SPOT},
             {"type", TYPE},
             {"color", COLOR},
             {"constantAttenuation", CONSTANTATTENUATION},
             {"linearAttenuation", LINEARATTENUATION},
             {"quadraticAttenuation", QUADRATICATTENUATION},
             {"fallOffAngle", FALLOFFANGLE},
             {"fallOffExponent", FALLOFFEXPONENT},

             {"materials", MATERIALS},
             {"instanceTechnique", INSTANCETECHNIQUE},
             {"technique", TECHNIQUE},
             {"values", VALUES},

             {"meshes", MESHES},
             {"primitives", PRIMITIVES},
             {"attributes", ATTRIBUTES},
             {"indices", INDICES},
             {"material", MATERIAL},
             {"primitive", PRIMITIVE},

             {"nodes", NODES},
             {"camera", CAMERA},
             {"children", CHILDREN},
             {"instanceSkin", INSTANCESKIN},
             {"jointName", JOINTNAME},
             {"light", LIGHT},
             {"matrix", MATRIX},
             {"meshes", MESHES},
             {"rotation", ROTATION},
             {"scale", SCALE},
             {"translation", TRANSLATION},
             {"skeletons", SKELETONS},
             {"skin", SKIN},
             {"meshes", MESHES},

             {"programs", PROGRAMS},
             {"attributes", ATTRIBUTES},
             {"fragmentShader", FRAGMENTSHADER},
             {"vertexShader", VERTEXSHADER},

             {"samplers", SAMPLERS},
             {"magFilter", MAGFILTER},
             {"minFilter", MINFILTER},
             {"wrapS", WRAPS},
             {"wrapT", WRAPT},

             {"scene", SCENE},
             {"nodes", NODES},
             {"scenes", SCENES},

             {"shaders", SHADERS},
             {"uri", URI},
             {"type", TYPE},

             {"skins", SKINS},
             {"bindShapeMatrix", BINDSHAPEMATRIX},
             {"inverseBindMatrices", INVERSEBINDMATRICES},
             {"jointNames", JOINTNAMES},

             {"techniques", TECHNIQUES},
             {"parameters", PARAMETERS},
             {"pass", PASS},
             {"passes", PASSES},
             {"count", COUNT},
             {"type", TYPE},
             {"semantic", SEMANTIC},
             {"node", NODE},
             {"value", VALUE},
             {"details", DETAILS},
             {"instanceProgram", INSTANCEPROGRAM},
             {"states", STATES},
             {"commonProfile", COMMONPROFILE},
             {"type", TYPE},
             {"lightingModel", LIGHTINGMODEL},
             {"parameters", PARAMETERS},
             {"texcoordBindings", TEXCOORDBINDINGS},
             {"attributes", ATTRIBUTES},
             {"program", PROGRAM},
             {"uniforms", UNIFORMS},
             {"enable", ENABLE},
             {"functions", FUNCTIONS},
             {"blendColor", BLENDCOLOR},
             {"blendEquationSeparate", BLENDEQUATIONSEPARATE},
             {"blendFuncSeparate", BLENDFUNCSEPARATE},
             {"colorMask", COLORMASK},
             {"cullFace", CULLFACE},
             {"depthFunc", DEPTHFUNC},
             {"depthMask", DEPTHMASK},
             {"depthRange", DEPTHRANGE},
             {"frontFace", FRONTFACE},
             {"lineWidth", LINEWIDTH},
             {"polygonOffset", POLYGONOFFSET},
             {"scissor", SCISSOR},

             {"textures", TEXTURES},
             {"format", FORMAT},
             {"internalFormat", INTERNALFORMAT},
             {"sampler", SAMPLER},
             {"source", SOURCE},
             {"target", TARGET},
             {"type", TYPE},



             {"5120", COMP_BYTE},
             {"5121", COMP_UNSIGNED_BYTE},
             {"5122", COMP_SHORT},
             {"5123", COMP_UNSIGNED_SHORT},
             {"5126", COMP_FLOAT},

             {"SCALAR", TP_SCALAR},
             {"VEC2", TP_VEC2},
             {"VEC3", TP_VEC3},
             {"VEC4", TP_VEC4},
             {"MAT2", TP_MAT2},
             {"MAT3", TP_MAT3},
             {"MAT4", TP_MAT4},

             {"arraybuffer", TP_ARRAYBUFFER},
             {"text", TP_TEXT},

             {"NORMAL", TP_NORMAL},
             {"POSITION", TP_POSITION},
             {"TEXCOORD_0", TP_TEXCOORD_0},

             {"0", P_POINTS},
             {"1", P_LINES},
             {"2", P_LINE_LOOP},
             {"3", P_LINE_STRIP},
             {"4", P_TRIANGLES},
             {"5", P_TRIANGLE_STRIP},
             {"6", P_TRIANGLE_FAN},

             {"34962", TG_ARRAY_BUFFER},
             {"34963", TG_ELEMENT_ARRAY_BUFFER}} ) {}
}
}
