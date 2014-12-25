#ifndef BENCHCLASS_H
#define BENCHCLASS_H

#include <functional>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include "cmdANDinit.hpp"

class BenchBaseVirtual {
   public:
      virtual double funcToCallVirtual( int _a, double _b ) = 0;
      virtual double funcToCallInlineVirtual( int _a, double _b ) = 0;

      virtual ~BenchBaseVirtual() {}
};


class BenchClass : public BenchBaseVirtual {
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
         for( auto i = 0; i < 100; ++i )
            ++_a;
         return _b * _a;
      }

      virtual double funcToCallVirtual( int _a, double _b );
      virtual double funcToCallInlineVirtual( int _a, double _b ) {
         for( auto i = 0; i < 100; ++i )
            ++_a;
         return _b * _a;
      }

      std::mutex bMutex;
      unsigned int vLoopsToDoMutex;

      double funcNormal( int a, double b );
      double funcMutex( int a, double b );
      double funcLockGuard( int a, double b );

      unsigned int vLoopsToDoCast;

      void doFunction();
      void doMutex();

   public:
      BenchClass() = delete;
      BenchClass( cmdANDinit *_cmd );

};

double cFuncToCall( int _a, double _b );
inline double cFuncToCallInline( int _a, double _b ) {
   for( auto i = 0; i < 100; ++i )
      ++_a;
   return _b * _a;
}

#endif // BENCHCLASS_H

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
