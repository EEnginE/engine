#include <string>
#include <vector>
#include <engine.hpp>
#include "cmdANDinit.hpp"
#include "BenchClass.hpp"

using namespace e_engine;
using namespace std;

#define COLOR true


int main( int argc, char *argv[] ) {
   cmdANDinit initStuff( argc, argv, COLOR );
   
   if ( !initStuff.parseArgsAndInit() ) {
      // help called or to less arguments
      return 1;
   }
   
   BenchClass benchs( &initStuff );

   LOG.stopLogLoop();
   
   return 0;
}
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
