#ifndef BENCHCLASS_H
#define BENCHCLASS_H

#include <functional>
#include "cmdANDinit.hpp"


class BenchClass {
   private:
      // --- function Bench ---
      unsigned int vLoopsToDo;

      uSignal<double, int, double>           vTheSignal;
      uSlot<double, BenchClass, int, double> vTheSlot;

      uSignal<double, int, double>           vTheSignalInline;
      uSlot<double, BenchClass, int, double> vTheSlotInline;

      double( BenchClass::*vFunctionPointer )( int, double );
      double( BenchClass::*vFunctionPointerInline )( int, double );
      
      double( *vCFunctionPointer )( int, double );
      double( *vCFunctionPointerInline )( int, double );

      boost::function<double( int, double )> vBoostFunc;
      boost::function<double( int, double )> vBoostFuncInline;
      
      std::function<double( int, double )>   vStdFunc;
      std::function<double( int, double )>   vStdFuncInline;

      double funcToCall( int _a, double _b );
      inline double funcToCallInline( int _a, double _b ) {
         for ( auto i = 0; i < 100; ++i )
            ++_a;
         return _b * _a;
      }


      BenchClass() {}


      void doFunction();

   public:
      BenchClass( cmdANDinit *_cmd );

};

double cFuncToCall( int _a, double _b );
inline double cFuncToCallInline( int _a, double _b ) {
   for ( auto i = 0; i < 100; ++i )
      ++_a;
   return _b * _a;
}

#endif // BENCHCLASS_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
