/*!
 * \file rFrameCounter.cpp
 */

#include "rFrameCounter.hpp"

namespace e_engine {

rFrameCounter::rFrameCounter( rWorld *_rWorld, bool _enable ) :
   vWorld( _rWorld ),
   vRenderedFrames( _rWorld->getRenderedFramesPtr() )  {

   if( _enable )
      enableFrameCounter();

}

/*!
 * \brief The loop that outputs the frames per second in the interval defined through vSleepDelay
 */
void rFrameCounter::frameCounterLoop() {
   LOG.nameThread( L"fps" );
   while( vFrameCounterEnabled ) {
      if( !vWorld->getIsRenderLoopPaused() ) {
         iLOG( "FPS: ", ( int )( *vRenderedFrames / vHelper ) ); // Change this to output the resulting fps in a proper way (and as a double)
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
   if( _join )
      frameCounterThread.join();
   iLOG( "Frame counter disabled" );
}


}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
