/*!
 * \file windows/event.cpp
 * \brief \b Classes: \a eInit
 * \sa e_eInit.cpp e_eInit.hpp
 */

#include "eInit.hpp"
#include "context.hpp"
#include <windows.h>
#include "log.hpp"

namespace e_engine {

namespace {

}

int eInit::eventLoop() {
   //! \todo Move this in windows_win32
   vEventLoopHasFinished_B = false;

   do {

      {
         // Make sure lLockWindow_BT will be destroyed
         std::cout << "AAA" << std::endl;
         boost::lock_guard<boost::mutex> lLockWindow_BT( vCreateWindowMutex_BT );
         std::cout << "BBB" << std::endl;
         vCreateWindowReturn_I = createContext();
         std::cout << "CCC" << std::endl;
         makeNOContextCurrent();
         std::cout << "DDD" << std::endl;

         // Context created; continue with init();
         vCreateWindowCondition_BT.notify_one();
      }


      // Now wait until the main event loop is officially "started"

      boost::unique_lock<boost::mutex> lLockEvent_BT( vStartEventMutex_BT );
      vStartEventCondition_BT.wait( lLockEvent_BT );


      iLOG "Event loop started" END

      MSG msg;

      while ( vMainLoopRunning_B ) {
         if ( vLoopsPaused_B ) {
            boost::unique_lock<boost::mutex> lLock_BT( vEventLoopMutex_BT );
            vEventLoopISPaused_B = true;
            while ( vEventLoopPaused_B ) vEventLoopWait_BT.wait( lLock_BT );
            vEventLoopISPaused_B = false;

            // Jump back to create context
            if ( vWindowRecreate_B || ! getHaveContext() )
               break;

         }
         if ( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) ) {

            TranslateMessage( &msg );
            DispatchMessage( &msg );
         }

         B_SLEEP( milliseconds, 10 );
      }
   } while ( vMainLoopRunning_B );

   vEventLoopHasFinished_B = true;
   return 1;
}

namespace windows_win32 {

using namespace e_engine;

LRESULT CALLBACK eContext::initialWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   if ( _uMsg == WM_NCCREATE ) {
      LPCREATESTRUCT lCreateStruct_win32 = reinterpret_cast<LPCREATESTRUCT>( _lParam );
      void *lCreateParam_win32 = lCreateStruct_win32->lpCreateParams;
      eContext *this__ = reinterpret_cast<eContext *>( lCreateParam_win32 );
      

//       if ( this__->vHWND_Window_win32 != 0 ) {
//          // This function was already called -- this should never happen
//          eLOG "Internal Error: eContext::initialWndProc was already called!!" END
//          this__->destroyContext();
//          this__->vWindowsCallbacksError_B = true;
//       }

      this__->vHWND_Window_win32 = _hwnd;
      SetWindowLongPtr( _hwnd,
                        GWLP_USERDATA,
                        reinterpret_cast<LONG_PTR>( this__ ) );
      SetWindowLongPtr( _hwnd,
                        GWLP_WNDPROC,
                        reinterpret_cast<LONG_PTR>( &eContext::staticWndProc ) );
      eWinInfo _tempInfo( e_engine::e_engine_internal::__eInit_Pointer_OBJ.get() );
      return this__->actualWndProc( _uMsg, _wParam, _lParam, _tempInfo );
   }
   // if it isn't WM_NCCREATE, do something sensible and wait until
   //   WM_NCCREATE is sent
   return DefWindowProc( _hwnd, _uMsg, _wParam, _lParam );
}

LRESULT CALLBACK eContext::staticWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   LONG_PTR lUserData_win32 = GetWindowLongPtr( _hwnd, GWLP_USERDATA );
   eContext *this__ = reinterpret_cast<eContext *>( lUserData_win32 );
   eWinInfo _tempInfo( e_engine::e_engine_internal::__eInit_Pointer_OBJ.get() );

   if ( ! this__ || _hwnd != this__->vHWND_Window_win32 ) {
      eLOG "Bad Windows callback error" END
      this__->destroyContext();
      this__->vWindowsCallbacksError_B = true;
   }

   return this__->actualWndProc( _uMsg, _wParam, _lParam, _tempInfo );
}


LRESULT CALLBACK eContext::actualWndProc( UINT _uMsg, WPARAM _wParam, LPARAM _lParam, eWinInfo _tempInfo ) {
   unsigned int key_state = E_KEY_PRESSED;

   if ( _tempInfo.eInitPointer == 0 ) {
      eLOG "eInit-pointer is not yet initialized" END
      return 0;
   }

   switch ( _uMsg ) {
      case WM_SIZE://Window size was changed
         iLOG "Resized " END
         return 0;
      case WM_MOVE://Window was moved
         iLOG "Moved " END
         return 0;
      case WM_CLOSE: //Window is closed TODO: Doesnt work yet, add functionality
         iLOG "Closed " END
         return 0;
      case WM_SETFOCUS: //Window has been focused
         iLOG "Focus Set " END
         return 0;
      case WM_LBUTTONDOWN: //Leftbutton clicked
         iLOG "Leftbutton clicked " END
         return 0;
      case WM_LBUTTONUP: //Leftbutton released
         iLOG "Leftbutton released " END
         return 0;
      case WM_LBUTTONDBLCLK: //User Clicked Leftbutton twice TODO: Doesnt work
         iLOG "Doubleclicked left button" END
         return 0;
      case WM_RBUTTONDOWN: //Rightbutton clicked
         iLOG "Rightbutton clicked " END
         return 0;
      case WM_RBUTTONUP: //Rightbutton released
         iLOG "Rightbutton released " END
         return 0;
      case WM_KEYUP:
         key_state = E_KEY_RELEASED;
      case WM_KEYDOWN: //Key pressed
         iLOG "Key pressed: " ADD( char ) _wParam END
         _tempInfo.eKey.state = key_state;
         _tempInfo.eKey.key   = _wParam; // TODO: Process _wParam first
         vKey_SIG.sendSignal( _tempInfo );
         return 0;
//       case WM_MOUSEMOVE:
//          iLOG "Mouse moved: " END
//          return 0;
      default:
         char lStr_CSTR[6];
         snprintf( lStr_CSTR, 5, "%04x", _uMsg );
         iLOG "Found Event: 0x" ADD lStr_CSTR END
         break;
   }

   return DefWindowProc( vHWND_Window_win32, _uMsg, _wParam, _lParam );
}

} // windows_win32

} // e_engine

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
