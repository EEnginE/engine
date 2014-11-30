/*!
 * \file rFrameCounter.hpp
 */

 #include "uLog.hpp"

namespace e_engine {

    //\todo Add a signal for FPS output, make the loop pause when the renderloop is paused and fix the weird behavior when iInit restart() is called

class rFrameCounter {
 private:
      iInit               *vInit;

      uint64_t            *vRenderedFrames;

      int    vSleepDelay = 1000;
      double vHelper     = vSleepDelay/1000;    //vSleepDelay in seconds

      bool     vFrameCounterEnabled;

      boost::thread frameCounterThread;

      void frameCounterLoop() {
        while(vFrameCounterEnabled) {
            iLOG("FPS: ", (int) (*vRenderedFrames / vHelper)); // Change this to output the resulting fps in a proper way (and as a double)
            *vRenderedFrames = 0;
            B_SLEEP(milliseconds, vSleepDelay);
        }
      }

      rFrameCounter() {}
 public:
      rFrameCounter( rWorld *_rWorld) :
          vRenderedFrames (_rWorld->getRenderedFramesPtr()),
          vFrameCounterEnabled( true )  {

          frameCounterThread = boost::thread(&rFrameCounter::frameCounterLoop, this);

          iLOG( "Frame counter enabled" );

      }

      void enableFrameCounter()                  {*vRenderedFrames = 0; vFrameCounterEnabled = true;  frameCounterThread = boost::thread(&rFrameCounter::frameCounterLoop, this); iLOG( "Frame counter enabled" );}
      void disableFrameCounter(bool _join)       {                      vFrameCounterEnabled = false; if(_join) frameCounterThread.join();                                        iLOG( "Frame counter disabled" );}

      void setSleepDelay( double _newSleepDelay) {vSleepDelay = _newSleepDelay; vHelper = vSleepDelay/1000;}

      bool getIsCounterEnabled()   const         {return vFrameCounterEnabled;}

};

}
