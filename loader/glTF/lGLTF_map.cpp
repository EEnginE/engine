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

#include "lGLTF_map.hpp"

namespace e_engine {
namespace glTF {

lGLTF_map::~lGLTF_map() {}

lGLTF_map::lGLTF_map()
    : vMap( {{"extensions", EXTENSIONS},
             {"additionalProperties", ADDITIONALPROPERTIES},
             {"extras", EXTRAS},

             // Root
             {"extensionsUsed", EXTENSIONSUSED},
             {"accessors", ACCESSORS},
             {"animations", ANIMATIONS},
             {"asset", ASSET},
             {"buffers", BUFFERS},
             {"bufferViews", BUFFERVIEWS},
             {"cameras", CAMERAS},
             {"images", IMAGES},
             {"materials", MATERIALS},
             {"meshes", MESHES},
             {"nodes", NODES},
             {"programs", PROGRAMS},
             {"samplers", SAMPLERS},
             {"scene", SCENE},
             {"scenes", SCENES},
             {"shaders", SHADERS},
             {"skins", SKINS},
             {"techniques", TECHNIQUES},
             {"textures", TEXTURES},

             // -- ACCESSORS
             {"bufferView", BUFFERVIEW},
             {"byteOffset", BYTEOFFSET},
             {"byteStride", BYTESTRIDE},
             {"componentType", COMPONENTTYPE},
             {"count", COUNT},
             {"type", TYPE},
             {"max", MAX},
             {"min", MIN},
             {"SCALAR", SCALAR},
             {"VEC2", VEC2},
             {"VEC3", VEC3},
             {"VEC4", VEC4},
             {"MAT2", MAT2},
             {"MAT3", MAT3},
             {"MAT4", MAT4},

             // -- ANIMATIONS
             {"channels", CHANNELS},
             {"parameters", PARAMETERS},
             {"sampler", SAMPLER},
             {"target", TARGET},
             {"id", ID},
             {"path", PATH},
             {"translation", TRANSLATION},
             {"rotation", ROTATION},
             {"scale", SCALE},
             {"input", INPUT},
             {"interpolation", INTERPOLATION},
             {"LINEAR", LINEAR},
             {"output", OUTPUT},

             // -- ASSET
             {"copyright", COPYRIGHT},
             {"generator", GENERATOR},
             {"premultipliedAlpha", PREMULTIPLIEDALPHA},
             {"profile", PROFILE},
             {"version", VERSION},
             {"api", API},

             // -- BUFFERS
             {"uri", URI},
             {"byteLength", BYTELENGTH},
             {"arraybuffer", ARRAYBUFFER},
             {"text", TEXT},

             // -- BUFFERVIEWS
             {"buffer", BUFFER},
             {"34962", TG_ARRAY_BUFFER},
             {"34963", TG_ELEMENT_ARRAY_BUFFER},

             // -- CAMERAS
             {"orthographic", ORTHOGRAPHIC},
             {"perspective", PERSPECTIVE},
             {"xmag", XMAG},
             {"ymag", YMAG},
             {"zfar", ZFAR},
             {"znear", ZNEAR},
             {"aspectRatio", ASPECTRATIO},
             {"yfov", YFOV},

             // -- MATERIALS
             {"technique", TECHNIQUE},
             {"values", VALUES},

             // -- MESHES
             {"primitives", PRIMITIVES},
             {"attributes", ATTRIBUTES},
             {"indices", INDICES},
             {"material", MATERIAL},
             {"mode", MODE},
             {"0", P_POINTS},
             {"1", P_LINES},
             {"2", P_LINE_LOOP},
             {"3", P_LINE_STRIP},
             {"4", P_TRIANGLES},
             {"5", P_TRIANGLE_STRIP},
             {"7", P_TRIANGLE_FAN},

             /// -- NODES
             {"camera", CAMERA},
             {"children", CHILDREN},
             {"skeletons", SKELETONS},
             {"skin", SKIN},
             {"jointName", JOINTNAME},
             {"matrix", MATRIX},

             // -- PROGRAMS
             {"fragmentShader", FRAGMENTSHADER},
             {"vertexShader", VERTEXSHADER},

             // -- SAMPLERS
             {"magFilter", MAGFILTER},
             {"minFilter", MINFILTER},
             {"wrapS", WRAPS},
             {"wrapT", WRAPT},
             {"9728", M_NEAREST},
             {"9729", M_LINEAR},
             {"9984", M_NEAREST_MIPMAP_NEAREST},
             {"9985", M_LINEAR_MIPMAP_NEAREST},
             {"9986", M_NEAREST_MIPMAP_LINEAR},
             {"9987", M_LINEAR_MIPMAP_LINEAR},
             {"33071", M_CLAMP_TO_EDGE},
             {"33648", M_MIRRORED_REPEAT},
             {"10497", M_REPEAT},

             // -- SHADERS
             {"35632", S_FRAGMENT_SHADER},
             {"35633", S_VERTEX_SHADER},

             // -- SKINS
             {"bindShapeMatrix", BINDSHAPEMATRIX},
             {"inverseBindMatrices", INVERSEBINDMATRICES},
             {"jointNames", JOINTNAMES},

             // -- TECHNIQUES
             {"program", PROGRAM},
             {"uniforms", UNIFORMS},
             {"states", STATES},
             {"enable", ENABLE},
             {"functions", FUNCTIONS},
             {"node", NODE},
             {"semantic", SEMANTIC},
             {"value", VALUE},
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

             {"5120", TP_BYTE},
             {"5121", TP_UNSIGNED_BYTE},
             {"5122", TP_SHORT},
             {"5123", TP_UNSIGNED_SHORT},
             {"5124", TP_INT},
             {"5125", TP_UNSIGNED_INT},
             {"5126", TP_FLOAT},
             {"35664", TP_FLOAT_VEC2},
             {"35665", TP_FLOAT_VEC3},
             {"35666", TP_FLOAT_VEC4},
             {"35667", TP_INT_VEC2},
             {"35668", TP_INT_VEC3},
             {"35669", TP_INT_VEC4},
             {"35670", TP_BOOL},
             {"35671", TP_BOOL_VEC2},
             {"35672", TP_BOOL_VEC3},
             {"35673", TP_BOOL_VEC4},
             {"35674", TP_FLOAT_MAT2},
             {"35675", TP_FLOAT_MAT3},
             {"35676", TP_FLOAT_MAT4},
             {"35678", TP_SAMPLER_2D},

             {"LOCAL", SM_LOCAL},
             {"MODEL", SM_MODEL},
             {"VIEW", SM_VIEW},
             {"PROJECTION", SM_PROJECTION},
             {"MODELVIEW", SM_MODELVIEW},
             {"MODELVIEWPROJECTION", SM_MODELVIEWPROJECTION},
             {"MODELINVERSE", SM_MODELINVERSE},
             {"VIEWINVERSE", SM_VIEWINVERSE},
             {"PROJECTIONINVERSE", SM_PROJECTIONINVERSE},
             {"MODELVIEWINVERSE", SM_MODELVIEWINVERSE},
             {"MODELVIEWPROJECTIONINVERSE", SM_MODELVIEWPROJECTIONINVERSE},
             {"MODELINVERSETRANSPOSE", SM_MODELINVERSETRANSPOSE},
             {"MODELVIEWINVERSETRANSPOSE", SM_MODELVIEWINVERSETRANSPOSE},
             {"VIEWPORT", SM_VIEWPORT},

             {"POSITION", SM_POSITION},
             {"NORMAL", SM_NORMAL},
             {"TEXCOORD", SM_TEXCOORD},
             {"COLOR", SM_COLOR},
             {"JOINT", SM_JOINT},
             {"JOINTMATRIX", SM_JOINTMATRIX},
             {"WEIGHT", SM_WEIGHT},

             {"3042", TECH_BLEND},
             {"2884", TECH_CULL_FACE},
             {"2929", TECH_DEPTH_TEST},
             {"32823", TECH_POLYGON_OFFSET_FILL},
             {"32926", TECH_SAMPLE_ALPHA_TO_COVERAGE},
             {"3089", TECH_SCISSOR_TEST},
             {"32774", TECH_FUNC_ADD},
             {"32778", TECH_FUNC_SUBTRACT},
             {"32779", TECH_FUNC_REVERSE_SUBTRACT},
             {"0", TECH_ZERO},
             {"1", TECH_ONE},
             {"768", TECH_SRC_COLOR},
             {"769", TECH_ONE_MINUS_SRC_COLOR},
             {"774", TECH_DST_COLOR},
             {"775", TECH_ONE_MINUS_DST_COLOR},
             {"770", TECH_SRC_ALPHA},
             {"771", TECH_ONE_MINUS_SRC_ALPHA},
             {"772", TECH_DST_ALPHA},
             {"773", TECH_ONE_MINUS_DST_ALPHA},
             {"32769", TECH_CONSTANT_COLOR},
             {"32770", TECH_ONE_MINUS_CONSTANT_COLOR},
             {"32771", TECH_CONSTANT_ALPHA},
             {"32772", TECH_ONE_MINUS_CONSTANT_ALPHA},
             {"776", TECH_SRC_ALPHA_SATURATE},
             {"1028", TECH_FRONT},
             {"1029", TECH_BACK},
             {"1032", TECH_FRONT_AND_BACK},
             {"512", TECH_NEVER},
             {"513", TECH_LESS},
             {"515", TECH_LEQUAL},
             {"514", TECH_EQUAL},
             {"516", TECH_GREATER},
             {"517", TECH_NOTEQUAL},
             {"518", TECH_GEQUAL},
             {"519", TECH_ALWAYS},
             {"2304", TECH_CW},
             {"2305", TECH_CCW},

             // -- TEXTURES
             {"format", FORMAT},
             {"internalFormat", INTERNALFORMAT},
             {"source", SOURCE},
             {"6406", TT_ALPHA},
             {"6407", TT_RGB},
             {"6408", TT_RGBA},
             {"6409", TT_LUMINANCE},
             {"6410", TT_LUMINANCE_ALPHA},
             {"3553", TT_TEXTURE_2D},
             {"5121", TT_UNSIGNED_BYTE},
             {"33635", TT_UNSIGNED_SHORT_5_6_5},
             {"32819", TT_UNSIGNED_SHORT_4_4_4_4},
             {"32820", TT_UNSIGNED_SHORT_5_5_5_1},



             {"name", NAME}} ) {}
}
}
