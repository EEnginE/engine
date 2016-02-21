/*!
 * \file rWorld.cpp
 * \brief \b Classes: \a rWorld
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

#include "rWorld.hpp"
#include "uLog.hpp"
#include "math.h"

namespace e_engine {

rWorld::rWorld( iInit *_init ) {
   vInitObjSet_B = false; // Will be set true in setInitObj

   vViewPort.vNeedUpdate_B = false;
   vViewPort.x             = 0;
   vViewPort.y             = 0;
   vViewPort.width         = 0;
   vViewPort.height        = 0;

   vClearColor.vNeedUpdate_B = false;
   vClearColor.r             = 0;
   vClearColor.g             = 0;
   vClearColor.b             = 0;
   vClearColor.a             = 1;

   vRenderedFrames = 0;

   setInitObj( _init );
}

rWorld::~rWorld() {}

void rWorld::updateViewPort( int _x, int _y, int _width, int _height ) {
   vViewPort.vNeedUpdate_B = true;
   vViewPort.x             = _x;
   vViewPort.y             = _y;
   vViewPort.width         = _width;
   vViewPort.height        = _height;
}

void rWorld::updateClearColor( float _r, float _g, float _b, float _a ) {
   vClearColor.vNeedUpdate_B = true;
   vClearColor.r             = _r;
   vClearColor.g             = _b;
   vClearColor.b             = _g;
   vClearColor.a             = _a;
}


void rWorld::setInitObj( iInit *_init ) {
   if ( vInitObjSet_B ) {
      eLOG( "iInit object is already set and can't be reset! Doing nothing" );
      return;
   }

   vInitPointer = _init;
   vInitObjSet_B = true;
}
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
