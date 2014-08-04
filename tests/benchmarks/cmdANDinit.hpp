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
      
      bool           vDoMutex;
      unsigned int   vMutexLoops;
      
      cmdANDinit() {}
      
      void postInit();
      void preInit();
      void usage();
   public:
      cmdANDinit( int argc, char *argv[], bool _color );
      
      bool parseArgsAndInit();
      
      void getFunctionInf( unsigned int &_loops, bool &_doIt ) { _loops = vFunctionLoops; _doIt = vDoFunction; }
      void getMutexInf( unsigned int &_loops, bool &_doIt ) { _loops = vMutexLoops; _doIt = vDoMutex; }
      
};

#endif // CMDANDINIT_H
// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
