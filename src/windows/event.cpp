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

typedef unsigned       int Uint32;
typedef unsigned short int Uint16;



int eInit::eventLoop() {
   //! \todo Move this in windows_win32
   if ( ! vEventLoopHasFinished_B )
      return -1;

   vEventLoopHasFinished_B = false;

   vWindowsDestroy_B   = false;
   vWindowsNCDestrox_B = false;

   {
      // Make sure lLockWindow_BT will be destroyed
      boost::lock_guard<boost::mutex> lLockWindow_BT( vCreateWindowMutex_BT );
      vCreateWindowReturn_I = createContext();
      makeNOContextCurrent();

      // Context created; continue with init();
      vCreateWindowCondition_BT.notify_one();
   }


   // Now wait until the main event loop is officially "started"

   boost::unique_lock<boost::mutex> lLockEvent_BT( vStartEventMutex_BT );
   while ( !vContinueWithEventLoop_B ) vStartEventCondition_BT.wait( lLockEvent_BT );


   iLOG "Event loop started" END

   MSG msg;

//    while ( ( vMainLoopRunning_B && !vWindowRecreate_B ) || ( vMainLoopRunning_B || ( vWindowRecreate_B && ! ( vWindowsDestroy_B && vWindowsNCDestrox_B ) ) ) ) {
   while ( !( vWindowsDestroy_B && vWindowsNCDestrox_B ) ) {
      if ( !getHaveContext() )
         DestroyWindow( getHWND_win32() );

      if ( vLoopsPaused_B ) {
         boost::unique_lock<boost::mutex> lLock_BT( vEventLoopMutex_BT );
         vEventLoopISPaused_B = true;
         while ( vEventLoopPaused_B ) vEventLoopWait_BT.wait( lLock_BT );
         vEventLoopISPaused_B = false;
      }
      while ( PeekMessageW( &msg, getHWND_win32(), 0, 0, PM_REMOVE ) ) {
         TranslateMessage( &msg );
         DispatchMessageW( &msg );
      }

      B_SLEEP( milliseconds, 5 );
   }

   // Proccess the last messages
   while ( PeekMessageW( &msg, NULL, 0, 0, PM_REMOVE ) ) {
      TranslateMessage( &msg );
      DispatchMessageW( &msg );
   }

   iLOG "Event loop finished!" END
   vEventLoopHasFinished_B = true;
   return 1;
}

namespace windows_win32 {

LRESULT CALLBACK eContext::initialWndProc( HWND _hwnd, UINT _uMsg, WPARAM _wParam, LPARAM _lParam ) {
   if ( _uMsg == WM_NCCREATE ) {
      LPCREATESTRUCT lCreateStruct_win32 = reinterpret_cast<LPCREATESTRUCT>( _lParam );
      void *lCreateParam_win32 = lCreateStruct_win32->lpCreateParams;
      eContext *this__ = reinterpret_cast<eContext *>( lCreateParam_win32 );

      this__->vHWND_Window_win32 = _hwnd;

      SetWindowLongPtrW( _hwnd,
                         GWLP_USERDATA,
                         reinterpret_cast<LONG_PTR>( this__ ) );
      SetWindowLongPtrW( _hwnd,
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
   LONG_PTR lUserData_win32 = GetWindowLongPtrW( _hwnd, GWLP_USERDATA );
   eContext *this__ = reinterpret_cast<eContext *>( lUserData_win32 );
   eWinInfo _tempInfo( e_engine::e_engine_internal::__eInit_Pointer_OBJ.get() );

   if ( ! this__ || _hwnd != this__->vHWND_Window_win32 ) {
      eLOG "Bad Windows callback error" END
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
         _tempInfo.eResize.width = _lParam & 0xFFFF; // Get the low order word as a width
         _tempInfo.eResize.height = _lParam >> 16; // Get the high order word as a height
         vResize_SIG.sendSignal( _tempInfo );
         iLOG "The window was resized to width " ADD _tempInfo.eResize.width ADD " and height " ADD _tempInfo.eResize.height END
         return 0;
      case WM_MOVE://Window was moved
         _tempInfo.eResize.posX = _lParam & 0xFFFF; // Get the low order word as the x-Position
         _tempInfo.eResize.posY = _lParam >> 16; // Get the high order word as the y-Position
         iLOG "The window was moved to x" ADD _tempInfo.eResize.posX ADD " and y" ADD _tempInfo.eResize.posY END
         vResize_SIG.sendSignal( _tempInfo );
         return 0;
      case WM_MOUSEMOVE: //Mouse moved, see http://msdn.microsoft.com/en-us/library/windows/desktop/ms645616%28v=vs.85%29.aspx
         _tempInfo.eMouse.posX = _lParam & 0xFFFF; // Get the low order word as the x-Position
         _tempInfo.eMouse.posY = _lParam >> 16; // Get the high order word as the y-Position
         //_tempInfo.eMouse.state =

         iLOG "The mouse was moved to x" ADD _tempInfo.eMouse.posX ADD " and y" ADD _tempInfo.eMouse.posY END

         //!\todo Check if the coords are right, see the article above

         //!\todo Is a mousestate necessary? Only gives information if mouse is moved while mousebuttons are held
         //May also be solved by logging the individual mousebuttons

         vMouse_SIG.sendSignal( _tempInfo );
         return 0;
      case WM_CLOSE:
         _tempInfo.type = 10;
         vWindowClose_SIG.sendSignal( _tempInfo );
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
      case WM_LBUTTONDBLCLK: //!User Clicked Leftbutton twice \todo Doesnt work
         iLOG "Doubleclicked left button" END
         return 0;
      case WM_RBUTTONDOWN: //Rightbutton clicked
         iLOG "Rightbutton clicked " END
         return 0;
      case WM_RBUTTONUP:   //Rightbutton released
         iLOG "Rightbutton released " END
         return 0;
      case WM_SYSCHAR:
      case WM_CHAR:
         //This returns the actual key the user has typed in
         //It is only used if a key associated to a character, not a function key, was pressed
         //!\note Unicode should be supported by this, as it is a Unicode window, cannot be checked because the visual output (cmd/terminal) does not support unicode
         //!\todo Handle this output for text messages/text inputs


         iLOG "Key translated as: " ADD( unsigned int ) _wParam END
         return 0;
      case WM_UNICHAR:
         iLOG _wParam END
         if ( _wParam == UNICODE_NOCHAR ) {
            return TRUE;
         }
         return FALSE;
      case WM_KEYUP:
         key_state = E_KEY_RELEASED;
         //iLOG "Key pressed: " ADD (char) _wParam END
      case WM_KEYDOWN: //Key pressed
         _tempInfo.eKey.state = key_state;
         iLOG "TESTING: " END
         _tempInfo.eKey.key   = processWindowsKeyInput( _wParam, key_state );
         vKey_SIG.sendSignal( _tempInfo );
         //MapVirtualKey(_wParam, MAPVK_VK_TO_CHAR)
         //The above returns 0 when a function key is pressed, may be useful
         return 0;
      case WM_DESTROY:
         iLOG "Window Destroyed WM_DESTROY" END
         vWindowsDestroy_B   = true;
         break;
      case  WM_NCDESTROY:
         iLOG "Window Destroyed WM_NCDESTROY" END
         vWindowsNCDestrox_B = true;
         break;
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
