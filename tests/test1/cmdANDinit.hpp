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
      string         meshToRender;
      string         vShader;

      uJSON_data     vData_JSON;

      bool           vCanUseColor;

      GLfloat        vNearZ;
      GLfloat        vFarZ;

      cmdANDinit() {}

      void postInit();
      void preInit();
      void usage();
   public:
      cmdANDinit( int argc, char *argv[] );

      string getMesh()     const {return dataRoot + meshToRender;}
      string getShader()   const {return dataRoot + string( "shaders/" ) + vShader;}

      GLfloat getNearZ()   const {return vNearZ;}
      GLfloat getFarZ()    const {return vFarZ;}

      bool parseArgsAndInit();

};

#endif // CMDANDINIT_HPP

// kate: indent-mode cstyle; indent-width 3; replace-tabs on; line-numbers on;remove-trailing-spaces on;
