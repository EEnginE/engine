/*!
 * \file rFrameCounter.hpp
 */

#include "defines.hpp"

#include "uLog.hpp"
#include "rWorld.hpp"

namespace e_engine {

// \todo Add a signal to output the FPS

class rFrameCounter {
 private:
   rWorld *vWorld;

   uint64_t *vRenderedFrames;

   int vSleepDelay = 1000;
   double vHelper = vSleepDelay / 1000; // vSleepDelay in seconds

   bool vFrameCounterEnabled = false;

   std::thread frameCounterThread;

   void frameCounterLoop();

 public:
   rFrameCounter( rWorld *_rWorld, bool _enable );
   virtual ~rFrameCounter() { disableFrameCounter( true ); }

   rFrameCounter() = delete;

   void enableFrameCounter();
   void disableFrameCounter( bool _join = false );

   void setSleepDelay( double _newSleepDelay ) {
      vSleepDelay = _newSleepDelay;
      vHelper = vSleepDelay / 1000;
   }

   bool getIsCounterEnabled() const { return vFrameCounterEnabled; }
};
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on;
