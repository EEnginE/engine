/*!
 * \file windows/event.cpp
 * \brief \b Classes: \a eInit
 * \sa e_eInit.cpp e_eInit.hpp
 */

#include "eInit.hpp"
#include <windows.h>
#include "log.hpp"

namespace e_engine {

// namespace windows_win32 {

int eInit::eventLoop() {
   //! \todo Move this in windows_win32
   
   boost::lock_guard<boost::mutex> lLockWindow_BT(vCreateWindowMutex_BT);
   vCreateWindowReturn_I = createContext();
         
   // Context created; continue with init();
   vCreateWindowCondition_BT.notify_one();
   
   iLOG "SEND" END
   
   
   // Now wait until the main event loop is officially "started"
   
   boost::unique_lock<boost::mutex> lLockEvent_BT(vStartEventMutex_BT);
   vStartEventCondition_BT.wait(lLockEvent_BT);
   
   iLOG "Event loop started" END
   
   MSG msg;

   while ( vMainLoopRunning_B ) {
      if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {

         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
      
      B_SLEEP( milliseconds, 10 );
   }

   vEventLoopHasFinished_B = true;
   return 1;
}

// } // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
