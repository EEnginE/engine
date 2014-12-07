#include "BenchClass.hpp"

// Do it here so that the compiler can't do its inline optimization
// for inline we have another function
double BenchClass::funcToCall( int _a, double _b ) {
   for( auto i = 0; i < 100; ++i )
      ++_a;
   return _b * _a;
}

double BenchClass::funcToCallVirtual( int _a, double _b ) {
   for( auto i = 0; i < 100; ++i )
      ++_a;
   return _b * _a;
}

double cFuncToCall( int _a, double _b ) {
   for( auto i = 0; i < 100; ++i )
      ++_a;
   return _b * _a;
}

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
