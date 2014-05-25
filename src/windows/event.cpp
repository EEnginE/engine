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
   
   vRenderLoopHasFinished_B = false;
   
   
   vCreateWindowMutex_BT.lock();
   vCreateWindowReturn_I = createContext();
   makeNOContextCurrent();
   vCreateWindowMutex_BT.unlock();
   
   
   // Now wait until the main event loop is officially "started"
   
   while( true ) {
      vStartEventMutex_BT.lock();
      if( vMainLoopRunning_B ) {
         vStartEventMutex_BT.unlock();
         break;
      }
      vStartEventMutex_BT.unlock();
      B_SLEEP( milliseconds, 10 );
   }
   
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
