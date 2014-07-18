#ifndef CMDANDINIT_H
#define CMDANDINIT_H

#include <engine.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace e_engine;

class cmdANDinit {
   private:
      vector<string> args;
      string         argv0;
      
      bool           vCanUseColor;
      
      bool           vDoFunction;
      unsigned int   vFunctionLoops;
      
      cmdANDinit() {}
      
      void postInit();
      void preInit();
      void usage();
   public:
      cmdANDinit( int argc, char *argv[], bool _color );
      
      bool parseArgsAndInit();
      
      void getFunctionInf( unsigned int &_loops, bool &_doIt ) { _loops = vFunctionLoops; _doIt = vDoFunction; }
      
};

#endif // CMDANDINIT_H
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
