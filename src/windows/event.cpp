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

   MSG msg;

   iLOG "RUNNING" END

   while ( vMainLoopRunning_B ) {
      if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {

         TranslateMessage( &msg );
         DispatchMessage( &msg );
      }
      
      B_SLEEP( milliseconds, 100 );
   }

   return 1;
}

// } // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
