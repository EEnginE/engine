/*!
 * \file rWorld.hpp
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

#ifndef R_WORLD_H
#define R_WORLD_H

#include "defines.hpp"

#include "uSignalSlot.hpp"
#include "iInit.hpp"

namespace e_engine {

class RENDER_API rWorld {

 private:
   bool vInitObjSet_B;

   std::mutex vRenderLoopMutex_BT;
   std::condition_variable vRenderLoopWait_BT;

   uint64_t vRenderedFrames = 0;

   struct {
      bool vNeedUpdate_B;
      int x;
      int y;
      int width;
      int height;
   } vViewPort;

   struct {
      bool vNeedUpdate_B;
      float r;
      float g;
      float b;
      float a;
   } vClearColor;

   void renderLoop();

   iInit *vInitPointer;
   std::thread vRenderLoop_BT;

 public:
   rWorld( iInit *_init );
   virtual ~rWorld();

   rWorld() = delete;

   void setInitObj( iInit *_init );

   virtual void renderFrame() = 0;

   uint64_t *getRenderedFramesPtr() { return &vRenderedFrames; }

   void updateViewPort( int _x, int _y, int _width, int _height );
   void updateClearColor( float _r, float _g, float _b, float _a );
};
}

#endif // R_WORLD_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
