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

#include <engine.hpp>
#include "BenchClass.hpp"
#include "cmdANDinit.hpp"

BenchBaseVirtual::~BenchBaseVirtual() {}

using namespace std;

#define START( __VarName__ )                                                                       \
   std::chrono::system_clock::time_point __VarName__ = std::chrono::system_clock::now();
#define STOP( __VarName__ )                                                                        \
   static_cast<uint64_t>( std::chrono::duration_cast<std::chrono::microseconds>(                   \
                                std::chrono::system_clock::now() - __VarName__ ).count() );

typedef std::chrono::system_clock::duration TIME_DURATION;

BenchClass::BenchClass( cmdANDinit *_cmd )
    : vTheSlot( &BenchClass::funcToCall, this ),
      vTheSlotInline( &BenchClass::funcToCallInline, this ) {


   bool lDoFunctionBench = false;
   bool lDoMutexBench = false;
   _cmd->getFunctionInf( vLoopsToDo, lDoFunctionBench );
   _cmd->getMutexInf( vLoopsToDoMutex, lDoMutexBench );

   if ( lDoFunctionBench ) {
      vTheSignal.connect( &vTheSlot );
      vFunctionPointer = &BenchClass::funcToCall;
      vCFunctionPointer = &cFuncToCall;
      vStdFunc = std::bind(
            &BenchClass::funcToCall, this, std::placeholders::_1, std::placeholders::_2 );

      vTheSignalInline.connect( &vTheSlotInline );
      vFunctionPointerInline = &BenchClass::funcToCallInline;
      vCFunctionPointerInline = &cFuncToCallInline;
      vStdFuncInline = std::bind(
            &BenchClass::funcToCallInline, this, std::placeholders::_1, std::placeholders::_2 );

      doFunction();
   }

   if ( lDoMutexBench )
      doMutex();
}

void BenchClass::doFunction() {
   int a = 3;
   double b = 5.5;

   iLOG( "==== BEGIN FUNCTION BENCHMARK ====" );
   iLOG( "" );
   iLOG( "  - Loops: ", vLoopsToDo );
   iLOG( "  - Args:  ", a, ", ", b );

   // Normal:

   START( signalSlot );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      vTheSignal( a, b );
   }
   uint64_t lSigSlot = STOP( signalSlot );

   START( functionPointer );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      ( *this.*vFunctionPointer )( a, b );
   }
   uint64_t lFunc = STOP( functionPointer );

   START( cFunctionPointer );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      vCFunctionPointer( a, b );
   }
   uint64_t lCFunc = STOP( cFunctionPointer );

   START( normal );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      funcToCall( a, b );
   }
   uint64_t lNormal = STOP( normal );

   START( virt );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      funcToCallVirtual( a, b );
   }
   uint64_t lVirt = STOP( virt );

   START( stdFunc );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      vStdFunc( a, b );
   }
   uint64_t lStdFunc = STOP( stdFunc );


   // Inline:

   START( signalSlotInline );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      vTheSignalInline( a, b );
   }
   uint64_t lSigSlotIn = STOP( signalSlotInline );

   START( functionPointerInline );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      ( *this.*vFunctionPointerInline )( a, b );
   }
   uint64_t lFuncIn = STOP( functionPointerInline );

   START( cFfunctionPointerInline );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      vCFunctionPointerInline( a, b );
   }
   uint64_t lCFuncIn = STOP( cFfunctionPointerInline );

   START( normalInline );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      funcToCallInline( a, b );
   }
   uint64_t lNormalIn = STOP( normalInline );

   START( normalInlineVirt );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      funcToCallInlineVirtual( a, b );
   }
   uint64_t lNormalVirtIn = STOP( normalInlineVirt );

   START( stdFuncInline );
   for ( unsigned int i = 0; i < vLoopsToDo; ++i ) {
      vStdFuncInline( a, b );
   }
   uint64_t lStdFuncIn = STOP( stdFuncInline );

   iLOG( "  - Time: microseconds" );


   iLOG( "  = [NORMAL] Signal Slot:     ", lSigSlot );
   iLOG( "  = [NORMAL] Functionpointer: ", lFunc );
   iLOG( "  = [NORMAL] C F Ptr:         ", lCFunc );
   iLOG( "  = [NORMAL] Normal call:     ", lNormal );
   iLOG( "  = [NORMAL] Virtual call:    ", lVirt );
   iLOG( "  = [NORMAL] Std Function:    ", lStdFunc );

   iLOG( "  = [INLINE] Signal Slot:     ", lSigSlotIn );
   iLOG( "  = [INLINE] Functionpointer: ", lFuncIn );
   iLOG( "  = [INLINE] C F Ptr:         ", lCFuncIn );
   iLOG( "  = [INLINE] Normal call:     ", lNormalIn );
   iLOG( "  = [INLINE] Virtual call:    ", lNormalVirtIn );
   iLOG( "  = [INLINE] Std Function:    ", lStdFuncIn );

   string lSigSlot_str = std::to_string( lSigSlot );
   string lFunc_str = std::to_string( lFunc );
   string lCFunc_str = std::to_string( lCFunc );
   string lNormal_str = std::to_string( lNormal );
   string lVirt_str = std::to_string( lVirt );
   string lStdFunc_str = std::to_string( lStdFunc );

   string lSigSlotIn_str = std::to_string( lSigSlotIn );
   string lFuncIn_str = std::to_string( lFuncIn );
   string lCFuncIn_str = std::to_string( lCFuncIn );
   string lNormalIn_str = std::to_string( lNormalIn );
   string lNormalVirtIn_str = std::to_string( lNormalVirtIn );
   string lStdFuncIn_str = std::to_string( lStdFuncIn );

   lSigSlot_str.resize( 10, ' ' );
   lFunc_str.resize( 10, ' ' );
   lCFunc_str.resize( 10, ' ' );
   lNormal_str.resize( 10, ' ' );
   lVirt_str.resize( 10, ' ' );
   lStdFunc_str.resize( 10, ' ' );

   lSigSlotIn_str.resize( 10, ' ' );
   lFuncIn_str.resize( 10, ' ' );
   lCFuncIn_str.resize( 10, ' ' );
   lNormalIn_str.resize( 10, ' ' );
   lNormalVirtIn_str.resize( 10, ' ' );
   lStdFuncIn_str.resize( 10, ' ' );


   dLOG( "\n   |==================|============|============|"
         "\n   |       Type       |   normal   |   inline   |"
         "\n   |------------------|------------|------------|"
         "\n   | Signal Slot      | ",
         lSigSlot_str,
         " | ",
         lSigSlotIn_str,
         " |"
         "\n   | Function Pointer | ",
         lFunc_str,
         " | ",
         lFuncIn_str,
         " |"
         "\n   | C Func. Pointer  | ",
         lCFunc_str,
         " | ",
         lCFuncIn_str,
         " |"
         "\n   | Normal           | ",
         lNormal_str,
         " | ",
         lNormalIn_str,
         " |"
         "\n   | Virtual          | ",
         lVirt_str,
         " | ",
         lNormalVirtIn_str,
         " |"
         "\n   | STD Function     | ",
         lStdFunc_str,
         " | ",
         lStdFuncIn_str,
         " |"
         "\n   |==================|============|============|" );
}


double BenchClass::funcNormal( int a, double b ) {
   for ( int i = 0; i < 100; ++i ) {
      b *= i + a;
   }
   return b;
}

double BenchClass::funcMutex( int a, double b ) {
   bMutex.lock();
   for ( int i = 0; i < 100; ++i ) {
      b *= i + a;
   }
   bMutex.unlock();
   return b;
}

double BenchClass::funcLockGuard( int a, double b ) {
   std::lock_guard<std::mutex> guard1( bMutex );
   for ( int i = 0; i < 100; ++i ) {
      b *= i + a;
   }
   return b;
}


void BenchClass::doMutex() {
   int a = 3;
   double b = 5.5;
   iLOG( "==== BEGIN FUNCTION BENCHMARK ====" );
   iLOG( "" );
   iLOG( "  - Loops: ", vLoopsToDoMutex );
   iLOG( "  - Args:  ", a, ", ", b );

   START( normal );
   for ( unsigned int i = 0; i < vLoopsToDoMutex; ++i ) {
      b = funcNormal( a, b );
   }
   uint64_t lNormal = STOP( normal );

   START( mutex );
   for ( unsigned int i = 0; i < vLoopsToDoMutex; ++i ) {
      b = funcMutex( a, b );
   }
   uint64_t lMutex = STOP( mutex );

   START( lockGuard );
   for ( unsigned int i = 0; i < vLoopsToDoMutex; ++i ) {
      b = funcLockGuard( a, b );
   }
   uint64_t lLockGuard = STOP( lockGuard );

   iLOG( "  - Time: microseconds" );


   iLOG( "  = Normal:    ", lNormal );
   iLOG( "  = Mutex:     ", lMutex );
   iLOG( "  = LockGuard: ", lLockGuard );
}


// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;
