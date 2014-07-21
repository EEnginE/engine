#include "BenchClass.hpp"
#include "cmdANDinit.hpp"

#define START( __VarName__ ) boost::chrono::system_clock::time_point __VarName__ = boost::chrono::system_clock::now();
#define STOP( __VarName__ )  boost::chrono::duration_cast<boost::chrono::microseconds>(boost::chrono::system_clock::now() - __VarName__).count();

typedef boost::chrono::system_clock::duration TIME_DURATION;

BenchClass::BenchClass( cmdANDinit *_cmd ) {
   bool lDoFunctionBench;
   _cmd->getFunctionInf( vLoopsToDo, lDoFunctionBench );

   if ( lDoFunctionBench ) {
      vTheSlot.setFunc( &BenchClass::funcToCall, this );
      vTheSignal.connectWith( &vTheSlot );
      vFunctionPointer  = &BenchClass::funcToCall;
      vCFunctionPointer = &cFuncToCall;
      vBoostFunc        = boost::bind( &BenchClass::funcToCall, this, _1, _2 );
      vStdFunc          = std::bind( &BenchClass::funcToCall , this, std::placeholders::_1, std::placeholders::_2 );

      vTheSlotInline.setFunc( &BenchClass::funcToCallInline, this );
      vTheSignalInline.connectWith( &vTheSlotInline );
      vFunctionPointerInline  = &BenchClass::funcToCallInline;
      vCFunctionPointerInline = &cFuncToCallInline;
      vBoostFuncInline        = boost::bind( &BenchClass::funcToCallInline, this, _1, _2 );
      vStdFuncInline          = std::bind( &BenchClass::funcToCallInline, this, std::placeholders::_1, std::placeholders::_2 );

      doFunction();
   }
}


void BenchClass::doFunction() {
   int    a = 3;
   double b = 5.5;

   iLOG "==== BEGIN FUNCTION BENCHMARK ====" END
   iLOG "" END
   iLOG "  - Loops: " ADD vLoopsToDo END
   iLOG "  - Args:  " ADD a ADD ", " ADD b END

   // Normal:

   START( signalSlot );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vTheSignal( a, b );
   }
   uint64_t lSigSlot = STOP( signalSlot );

   START( functionPointer );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      ( *this.*vFunctionPointer )( a, b );
   }
   uint64_t lFunc = STOP( functionPointer );
   
   START( cFunctionPointer );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vCFunctionPointer( a, b );
   }
   uint64_t lCFunc = STOP( cFunctionPointer );

   START( normal );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      funcToCall( a, b );
   }
   uint64_t lNormal = STOP( normal );

   START( boostFunc );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vBoostFunc( a, b );
   }
   uint64_t lBoostFunc = STOP( boostFunc );
   
   START( stdFunc );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vStdFunc( a, b );
   }
   uint64_t lStdFunc = STOP( stdFunc );


   // Inline:

   START( signalSlotInline );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vTheSignalInline( a, b );
   }
   uint64_t lSigSlotIn = STOP( signalSlotInline );

   START( functionPointerInline );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      ( *this.*vFunctionPointerInline )( a, b );
   }
   uint64_t lFuncIn = STOP( functionPointerInline );
   
   START( cFfunctionPointerInline );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vCFunctionPointerInline( a, b );
   }
   uint64_t lCFuncIn = STOP( cFfunctionPointerInline );

   START( normalInline );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      funcToCallInline( a, b );
   }
   uint64_t lNormalIn = STOP( normalInline );

   START( boostFuncInline );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vBoostFuncInline( a, b );
   }
   uint64_t lBoostFuncIn = STOP( boostFuncInline );
   
   START( stdFuncInline );
   for ( auto i = 0; i < vLoopsToDo; ++i ) {
      vStdFuncInline( a, b );
   }
   uint64_t lStdFuncIn = STOP( stdFuncInline );

   iLOG "  - Time: microseconds" END


   iLOG "  = [NORMAL] Signal Slot:     " ADD lSigSlot END
   iLOG "  = [NORMAL] Functionpointer: " ADD lFunc END
   iLOG "  = [NORMAL] C F Ptr:         " ADD lCFunc END
   iLOG "  = [NORMAL] Normal call:     " ADD lNormal END
   iLOG "  = [NORMAL] Boost Function:  " ADD lBoostFunc END
   iLOG "  = [NORMAL] Std Function:    " ADD lStdFunc END

   iLOG "  = [INLINE] Signal Slot:     " ADD lSigSlotIn END
   iLOG "  = [INLINE] Functionpointer: " ADD lFuncIn END
   iLOG "  = [INLINE] C F Ptr:         " ADD lCFuncIn END
   iLOG "  = [INLINE] Normal call:     " ADD lNormalIn END
   iLOG "  = [INLINE] Boost Function:  " ADD lBoostFuncIn END
   iLOG "  = [INLINE] Std Function:    " ADD lStdFuncIn END

   string lSigSlot_str     = boost::lexical_cast<string>( lSigSlot );
   string lFunc_str        = boost::lexical_cast<string>( lFunc );
   string lCFunc_str       = boost::lexical_cast<string>( lCFunc );
   string lNormal_str      = boost::lexical_cast<string>( lNormal );
   string lBoostFunc_str   = boost::lexical_cast<string>( lBoostFunc );
   string lStdFunc_str     = boost::lexical_cast<string>( lStdFunc );

   string lSigSlotIn_str   = boost::lexical_cast<string>( lSigSlotIn );
   string lFuncIn_str      = boost::lexical_cast<string>( lFuncIn );
   string lCFuncIn_str     = boost::lexical_cast<string>( lCFuncIn );
   string lNormalIn_str    = boost::lexical_cast<string>( lNormalIn );
   string lBoostFuncIn_str = boost::lexical_cast<string>( lBoostFuncIn );
   string lStdFuncIn_str   = boost::lexical_cast<string>( lStdFuncIn );

   lSigSlot_str.resize( 10, ' ' );
   lFunc_str.resize( 10, ' ' );
   lCFunc_str.resize( 10, ' ' );
   lNormal_str.resize( 10, ' ' );
   lBoostFunc_str.resize( 10, ' ' );
   lStdFunc_str.resize( 10, ' ' );

   lSigSlotIn_str.resize( 10, ' ' );
   lFuncIn_str.resize( 10, ' ' );
   lCFuncIn_str.resize( 10, ' ' );
   lNormalIn_str.resize( 10, ' ' );
   lBoostFuncIn_str.resize( 10, ' ' );
   lStdFuncIn_str.resize( 10, ' ' );


   dLOG    ""
   NEWLINE "   |==================|============|============|"
   NEWLINE "   |       Type       |   normal   |   inline   |"
   NEWLINE "   |------------------|------------|------------|"
   NEWLINE "   | Signal Slot      | " ADD lSigSlot_str   ADD " | " ADD lSigSlotIn_str   ADD " |"
   NEWLINE "   | Function Pointer | " ADD lFunc_str      ADD " | " ADD lFuncIn_str      ADD " |"
   NEWLINE "   | C Func. Pointer  | " ADD lCFunc_str     ADD " | " ADD lCFuncIn_str     ADD " |"
   NEWLINE "   | Normal           | " ADD lNormal_str    ADD " | " ADD lNormalIn_str    ADD " |"
   NEWLINE "   | Boost Function   | " ADD lBoostFunc_str ADD " | " ADD lBoostFuncIn_str ADD " |"
   NEWLINE "   | STD Function     | " ADD lStdFunc_str   ADD " | " ADD lStdFuncIn_str   ADD " |"
   NEWLINE "   |==================|============|============|"
   NEWLINE
   NEWLINE
   END
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
