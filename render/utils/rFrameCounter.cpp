/*!
 * \file rFrameCounter.cpp
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

#include "rFrameCounter.hpp"
#include "uLog.hpp"

namespace e_engine {

rFrameCounter::~rFrameCounter() { disableFrameCounter( true ); }

rFrameCounter::rFrameCounter( rWorld *_rWorld, bool _enable )
    : vWorld( _rWorld ), vRenderedFrames( _rWorld->getRenderedFramesPtr() ) {

   if ( _enable )
      enableFrameCounter();
}

/*!
 * \brief The loop that outputs the frames per second in the interval defined through vSleepDelay
 */
void rFrameCounter::frameCounterLoop() {
   LOG.nameThread( L"fps" );
   while ( vFrameCounterEnabled ) {
      if ( !vWorld->getIsRenderLoopPaused() ) {
         iLOG( "FPS: ", *vRenderedFrames / vHelper ); // Change this to output the
                                                      // resulting fps in a proper way (and
                                                      // as a double)
         *vRenderedFrames = 0;
      }
      B_SLEEP( milliseconds, vSleepDelay );
   }
}

/*!
 * \brief Enables the frame counter
 */
void rFrameCounter::enableFrameCounter() {
   *vRenderedFrames = 0;
   vFrameCounterEnabled = true;
   frameCounterThread = std::thread( &rFrameCounter::frameCounterLoop, this );
   iLOG( "Frame counter enabled" );
}

/*!
 * \brief Disables the frame counter
 * \param _join Make the current thread join the frameloopthread until it is finished
 */
void rFrameCounter::disableFrameCounter( bool _join ) {
   vFrameCounterEnabled = false;
   if ( _join )
      frameCounterThread.join();
   iLOG( "Frame counter disabled" );
}
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
