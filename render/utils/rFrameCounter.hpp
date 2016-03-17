/*!
 * \file rFrameCounter.hpp
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

#pragma once

#include "defines.hpp"

#include "rWorld.hpp"

namespace e_engine {

// \todo Add a signal to output the FPS

class RENDER_API rFrameCounter {
 private:
   rWorld *vWorld;

   uint64_t *vRenderedFrames;

   int vSleepDelay = 1000;
   double vHelper  = vSleepDelay / 1000; // vSleepDelay in seconds

   bool vFrameCounterEnabled = false;

   std::thread frameCounterThread;

   void frameCounterLoop();

 public:
   rFrameCounter( rWorld *_rWorld, bool _enable );
   virtual ~rFrameCounter();

   rFrameCounter() = delete;

   void enableFrameCounter();
   void disableFrameCounter( bool _join = false );

   void setSleepDelay( int _newSleepDelay ) {
      vSleepDelay = _newSleepDelay;
      vHelper     = static_cast<double>( vSleepDelay / 1000 );
   }

   bool getIsCounterEnabled() const { return vFrameCounterEnabled; }
};
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on;
