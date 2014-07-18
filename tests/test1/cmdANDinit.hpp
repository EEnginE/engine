#ifndef CMDANDINIT_HPP
#define CMDANDINIT_HPP

#include <engine.hpp>
#include <vector>
#include <string>

using namespace std;
using namespace e_engine;

class cmdANDinit {
   private:
      vector<string> args;
      string         argv0;
      string         dataRoot;

      bool           vCanUseColor;

      cmdANDinit() {}

      void postInit();
      void preInit();
      void usage();
   public:
      cmdANDinit( int argc, char *argv[] );

      string getDataRoot() const {return dataRoot;}

      bool parseArgsAndInit();

};

#endif // CMDANDINIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; 
