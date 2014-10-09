#ifndef CMDANDINIT_HPP
#define CMDANDINIT_HPP

#include <engine.hpp>
#include <vector>
#include <string>

#include "testStarter.hpp"

using namespace std;
using namespace e_engine;

class cmdANDinit {
   private:
      vector<string> args;
      string         argv0;
      string         dataRoot;
      
      vector<string> outputFiles;

      bool           vCanUseColor;

      cmdANDinit() {}

      void usage();
   public:
      cmdANDinit( int argc, char *argv[], testStarter &_starter, bool &_errors );

      string getDataRoot() const {return dataRoot;}

      bool parseArgsAndInit( testStarter &_starter );
      void generate( uJSON_data &_data );
};

#endif // CMDANDINIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on; remove-trailing-spaces on;
